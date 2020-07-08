#include "board.h"
#include "utils.h"
#include "bit_utils.h"
#include "board_utils.h"

uint64_t king_moves[64];
uint64_t knight_moves[64];

int get_pieces(Board *board) {
    int x, y;
    uint8_t piece;
    int no_pieces = 0;
    for(y = 7; y >= 0; y--) {
        for(x = 0; x < 8; x++) {
            piece = board->mailbox[y * 8 + x];
            if (board->occupied & (1ull << (y * 8 + x))){
                ++no_pieces;
            }
        }
    }

    return no_pieces;
}


void reset_board(Board *board) {
    for(int i = 16; i < 56; ++i) {
        board->mailbox[i] = EMPTY;
    }

    for(int i = 0; i < 2; ++i) {
        uint8_t *rank = board->mailbox + i * 56;
        rank[0] = ROOK;
        rank[1] = KNIGHT;
        rank[2] = BISHOP;
        rank[3] = QUEEN;
        rank[4] = KING;
        rank[5] = BISHOP;
        rank[6] = KNIGHT;
        rank[7] = ROOK;

        rank = board->mailbox + 8 + i * 40;
        for(int j = 0; j < 8; ++j) {
            rank[j] = PAWN;
        }
    }

    board->b[WHITE][PAWN]     = 0x000000000000ff00ull;
    board->b[WHITE][KNIGHT]   = 0x0000000000000042ull;
    board->b[WHITE][BISHOP]   = 0x0000000000000024ull;
    board->b[WHITE][ROOK]     = 0x0000000000000081ull;
    board->b[WHITE][QUEEN]    = 0x0000000000000008ull;
    board->b[WHITE][KING]     = 0x0000000000000010ull;
    board->b[WHITE][OCCUPIED] = 0x000000000000ffffull;

    board->b[BLACK][PAWN]     = 0x00ff000000000000ull;
    board->b[BLACK][KNIGHT]   = 0x4200000000000000ull;
    board->b[BLACK][BISHOP]   = 0x2400000000000000ull;
    board->b[BLACK][ROOK]     = 0x8100000000000000ull;
    board->b[BLACK][QUEEN]    = 0x0800000000000000ull;
    board->b[BLACK][KING]     = 0x1000000000000000ull;
    board->b[BLACK][OCCUPIED] = 0xffff000000000000ull;

    board->occupied = 0xffff00000000ffffull;
    board->zobrist = 0;
}

uint64_t operation(int x, int tile, uint64_t moves){
    moves |= 1ull << (tile + x);
    return moves;
}

void generate_king_moves(void) {
    int tile = 0, x, y;
    uint64_t moves;
    while (tile < 64) {
        x = tile % 8,y = tile / 8,moves = 0;
        if(y != 7) moves = operation(8,tile,moves);
        if(y != 0) moves = operation(-8,tile,moves);
        if(x != 7) moves = operation(1,tile,moves);
        if(x != 0)  moves = operation(-1,tile,moves);
        if(x != 0 && y != 7) moves = operation(7,tile,moves);
        if(x != 7 && y != 7) moves = operation(9,tile,moves);
        if(x != 0 && y != 0) moves = operation(-9,tile,moves);
        if(x != 7 && y != 0) moves = operation(-7,tile,moves);
        king_moves[tile] = moves;
        tile++;
    }
}

void generate_knight_moves(void) {
    int tile;
    int x, y;
    uint64_t moves;

    for(tile = 0; tile < 64; tile++) {
        x = tile % 8, y = tile / 8,moves = 0;
        if(y < 6 && x > 0) moves = operation(15,tile,moves);
        if(y < 7 && x > 1) moves = operation(6,tile,moves);
        if(y < 6 && x < 7) moves = operation(17,tile,moves);
        if(y < 7 && x < 6) moves = operation(10,tile,moves);
        if(y > 1 && x > 0) moves = operation(-17,tile,moves);
        if(y > 0 && x > 1) moves = operation(-10,tile,moves);
        if(y > 1 && x < 7) moves = operation(-15,tile,moves);
        if(y > 0 && x < 6) moves = operation(-6,tile,moves);
        knight_moves[tile] = moves;
    }
}

void generate_rays() {
    int offset[8] = { 9, 7, -7, -9, 8, 1, -8, -1 }, xoffset[8] = { 1, -1, 1, -1, 0, 1, 0, -1 }, x, idx;
    for(int dir = 0; dir < 8; ++dir) {
        ray[dir][64] = 0;
        for(int tile = 0; tile < 64; ++tile) {
            ray[dir][tile] = 0, idx = tile, x = tile % 8;
            while(1) {
                idx += offset[dir], x += xoffset[dir];
                if(x < 0 || x > 7 || idx < 0 || idx > 63) {
                    break;
                }
                ray[dir][tile] |= 1ull << idx;
            }
        }
    }
}

void generate_movetables() {
    generate_rays();
    generate_king_moves();
    generate_knight_moves();
}



int king_in_check(Board *board, int color) {
    return is_threatened(board, lsb_index(board->b[color][KING]));
}
