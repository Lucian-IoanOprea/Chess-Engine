
#include "bit_utils.h"
#include "Hashtable.h"
#include "move.h"
#include "board.h"
#include "game.h"

char *xboard_move(Move m) {
    static char move[6];
    int index = 0;
    move[index] = 'a' + (m.begin % 8);
    index++;
    move[index] = '1' + (m.begin / 8);
    index++;
    move[index] = 'a' + (m.end % 8);
    index++;
    move[index] = '1' + (m.end / 8);
    index++;

    if(m.promote > 0 && m.promote < 5) {
        move[index] = ".nbrq"[m.promote];
        index++;
    }

    move[index++] = '\0';

    return move;
}

int is_xboard_move(const char *move) {
	 if(move[0] < 'a' || move[0] > 'h' || move[1] < '1'|| move[1] > '8' || move[2] < 'a' || move[2] > 'h' || move[3] < '1'|| move[3] > '8'  || strlen(move) < 4 || strlen(move) > 5 || (move[4] && !strchr("nbrq",move[4])) ) {
        return 0;
    }
    return 1;
}

Move get_xboard_move(const char *move) {
    Move action;
    action.end = (move[2] - 'a') + ((move[3] - '1') * 8);
    action.begin = (move[0] - 'a') + ((move[1] - '1') * 8);
    
    action.promote = 0;
    if(move[4] == 'n'){
    	action.promote = KNIGHT;
    }else if(move[4] == 'b'){
    	action.promote = BISHOP;
    }else if(move[4] == 'r'){
    	action.promote = ROOK;
    }else if(move[4] == 'q'){
    	action.promote = QUEEN;
    }

    return action;
}

void apply_move(Game *game, Move m,bool isendgame) {
    int beginpiece;
    int endpiece;
    int begincolor;
    int endcolor;
    Move m2;
    uint64_t beginbit, endbit;
    uint64_t epbit;
    int eptile;

    beginpiece = (&game->board)->mailbox[m.begin],endpiece = (&game->board)->mailbox[m.end], beginbit = 1ull << m.begin,endbit = 1ull << m.end;
   ((&game->board)->occupied & endbit != 0) ? game->quiet_moves = 0 : game->quiet_moves++;
    begincolor = !((&game->board)->b[WHITE][OCCUPIED] & beginbit),endcolor = !((&game->board)->b[WHITE][OCCUPIED] & endbit);

    if(beginpiece == PAWN && ((m.end / 8) == (5 - begincolor * 3)) && ((m.end % 8) == game->ep)) {

        eptile = (4 - begincolor) * 8 + game->ep;
        game->eval += piece_square_score(PAWN, eptile, !game->turn,isendgame);
        epbit = 1ull << eptile;
        (&game->board)->mailbox[eptile] = EMPTY;
        (&game->board)->occupied ^= epbit;
        (&game->board)->b[!begincolor][OCCUPIED] ^= epbit;
        (&game->board)->b[!begincolor][PAWN] ^= epbit;
    }
       beginpiece == PAWN && abs(m.begin - m.end) == 16 ? game->ep = m.begin % 8 : game->ep = 9;

    game->eval -= piece_square_score(beginpiece, m.begin, game->turn,isendgame);
    (&game->board)->mailbox[m.begin] = EMPTY;
    (&game->board)->occupied ^= beginbit;
    (&game->board)->zobrist ^= zobrist[beginpiece][m.begin];
    (&game->board)->zobrist ^= zobrist[EMPTY][m.begin];
    (&game->board)->b[begincolor][beginpiece] ^= beginbit;
    (&game->board)->b[begincolor][OCCUPIED] ^= beginbit;
    

    if(endpiece != EMPTY) {
    	(&game->board)->b[endcolor][endpiece] ^= endbit;
        (&game->board)->b[endcolor][OCCUPIED] ^= endbit;
        game->eval += piece_square_score(endpiece, m.end, !game->turn,isendgame);
        
    }
    (&game->board)->zobrist ^= zobrist[endpiece][m.end];
    if(m.promote) {
        beginpiece = m.promote;
    }
    game->eval += piece_square_score(beginpiece, m.end, game->turn,isendgame);
    (&game->board)->mailbox[m.end] = beginpiece;
    (&game->board)->occupied |= endbit;
    (&game->board)->b[begincolor][beginpiece] |= endbit;
    (&game->board)->b[begincolor][OCCUPIED] |= endbit;
    (&game->board)->zobrist ^= zobrist[beginpiece][m.end];

    if(endpiece == ROOK) {
        if((m.end - 0 == 0 && endcolor == WHITE)
                || (m.end - 56 == 0 && endcolor == BLACK)){
            game->can_castle[endcolor][QUEENSIDE] = false;
    }
        if((m.end - 7 == 0 && endcolor == WHITE)
                || (m.end - 63 == 0 && endcolor == BLACK)){
            game->can_castle[endcolor][KINGSIDE] = false;
    }
    }
    
    if(beginpiece == ROOK) {
        if((m.begin == 0 && begincolor == WHITE)
                || (m.begin - 56 == 0 && begincolor == BLACK)){

            game->can_castle[begincolor][QUEENSIDE] = false;
    }
        if((m.begin - 7 == 0 && begincolor == WHITE)
                || (m.begin - 63 == 0 && begincolor == BLACK)){
            game->can_castle[begincolor][KINGSIDE] = false;
    }
    }
    

    if(beginpiece == KING) {
        game->can_castle[begincolor][QUEENSIDE] = false;
        game->can_castle[begincolor][KINGSIDE] = false;
        if(abs(m.begin - m.end) == 2) {
            if(m.begin <= m.end) {
                m2.begin = m.begin + 3;
                m2.end = m.end - 1;
                m2.promote = 0;
                apply_move(game, m2,isendgame);
                game->turn = !game->turn;
                game->eval = -game->eval;
            }
            else {
                m2.begin = m.begin - 4;
                m2.end = m.end + 1;
                m2.promote = 0;
                apply_move(game, m2,isendgame);
                game->turn = !game->turn;
                game->eval = -game->eval;
            }
        }
    }
    game->eval = -game->eval;
    game->turn = !game->turn;

}

void generate_movelist(Game *game, Move *movelist, int *nmoves) {
    uint64_t pieces = game->board.b[game->turn][OCCUPIED];
    Move m;
    int nmove = 0;
    do {
        int piece = lsb_index(pieces);
        pieces ^= 1ull << piece;
        m.begin = piece;
        uint64_t moves = generate_moves(game, piece);

        while(moves) {
            int move = lsb_index(moves);
            moves ^= 1ull << move;
            m.end = move;

            if(game->board.mailbox[piece] == PAWN && (m.end / 8 == 0 || m.end / 8 == 7)) {
            	m.promote = QUEEN;
                movelist[nmove++] = m;
            } else {
                m.promote = 0;
                movelist[nmove++] = m;
            }
        }
    } while(pieces);

    *nmoves = nmove;
}
uint64_t generate_moves(Game *game, int tile) {
    int color = !((&game->board)->b[WHITE][OCCUPIED] & (1ull << tile));
    int type = (&game->board)->mailbox[tile];
    uint64_t moves = 0;
    uint64_t blockers;

    if(type == PAWN){
    	moves = pawn_moves(&game->board, tile);
        int x = tile % 8, y = tile / 8;
        if((game->ep == x + 1 || game->ep == x - 1) && (y == 4 - color)){
            moves |= 1ull << ((5 - color * 3) * 8 + game->ep);
        }
    }else if(type == KNIGHT){
    	moves = knight_moves[tile];
    }else if(type == BISHOP){
    	moves = bishop_moves(&game->board, tile);
    }else if(type == ROOK){
    	moves = rook_moves(&game->board, tile);
    }else if(type == KING){
    	moves = king_moves[tile];
        if(game->can_castle[color][QUEENSIDE]) {
            blockers = ((1ull << (tile - 1)) | (1ull << (tile - 2))
                | (1ull << (tile - 3))) & (&game->board)->occupied;
            if(!blockers && !is_threatened(&game->board, tile)
                    && !is_threatened(&game->board, tile - 1)
                    && !is_threatened(&game->board, tile - 2))
                moves |= 1ull << (tile - 2);
        }
        if(game->can_castle[color][KINGSIDE]) {
            blockers = ((1ull << (tile + 1)) | (1ull << (tile + 2)))
                & (&game->board)->occupied;
            if(!blockers && !is_threatened(&game->board, tile)
                    && !is_threatened(&game->board, tile + 1)
                    && !is_threatened(&game->board, tile + 2))
                moves |= 1ull << (tile + 2);
        }
    }else if(type == QUEEN){
    	moves = bishop_moves(&game->board, tile) | rook_moves(&game->board, tile);
    }

    return moves & ~(&game->board)->b[color][OCCUPIED];
}

int is_valid_move(Game game, Move m, int print,bool isendgame) {
    Game game2 = game;
    uint64_t beginbit = 1ull << m.begin;
    
    if(!(((&game.board))->b[game.turn][OCCUPIED] & beginbit)) {
        
        return 0;
    }
    uint64_t endbit = 1ull << m.end;
    if(!(generate_moves(&game, m.begin) & endbit)) {
        
        return 0;
    }

    if(((&game.board))->mailbox[m.begin] == PAWN && !m.promote && ((m.end / 8) == 0 || (m.end / 8) == 7)) {
        return 0;
    }


    if(((&game.board)->mailbox[m.begin] != PAWN || ((m.end / 8) != 0 && (m.end / 8) != 7)) && m.promote) {
 
        return 0;
    }
    apply_move(&game2, m,isendgame);
    if(king_in_check((&game2.board), game.turn)) {
        return 0;
    }
    return 1;
}

static int piece_score[6] = {100, 320, 330, 500, 900, 0};

int piece_square_score(int piece, int square, int color,bool isendgame) {
    if(piece == EMPTY){
        return 0;
    }
    if( isendgame == false){ 
    square = (square % 8) + (7 - (square / 8)) * 8;
    if(color == BLACK){
        return piece_score[piece] + piece_square[piece][63 - square];
    }
    if(color == WHITE){
        return piece_score[piece] + piece_square[piece][square];
    }
    }
    else{
        square = (square % 8) + (7 - (square / 8)) * 8;
    if(color == BLACK){
        return piece_score[piece] + piece_square1[piece][63 - square];
    }
    if(color == WHITE){
        return piece_score[piece] + piece_square1[piece][square];
    }

    }
    return 0;
}
