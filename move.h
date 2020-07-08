#ifndef __MOVE_H__
#define __MOVE_H__

#include "board.h"
#include "game.h"

typedef struct Move {
    uint8_t begin;
    uint8_t end;
    uint8_t promote;
} Move;

typedef struct MoveScore {
    Move move;
    int score;
} MoveScore;

char *xboard_move(Move m);
int is_xboard_move(const char *move);
Move get_xboard_move(const char *move);
void apply_move(Game *game, Move m,bool isendgame);
void generate_movelist(Game *game, Move *moves, int *nmoves);
uint64_t generate_moves(Game *game, int tile);
int is_valid_move(Game game, Move m, int print,bool endgame);
int piece_square_score(int piece, int square, int color,bool endgame);


#endif // __MOVE_H__
