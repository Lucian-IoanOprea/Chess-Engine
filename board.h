#ifndef __BOARD_H__
#define __BOARD_H__

#include <signal.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

#define PAWN     0
#define KNIGHT   1
#define BISHOP   2
#define ROOK     3
#define QUEEN    4
#define KING     5
#define OCCUPIED 6
#define EMPTY    7


typedef struct Board {
    uint8_t mailbox[64];
    uint64_t b[2][7];
    uint64_t occupied;
    uint64_t zobrist;
} Board;


extern uint64_t king_moves[64];
extern uint64_t knight_moves[64];

void reset_board(Board *board);
void clear_board(Board *board);
int consistent_board(Board *board);
void generate_movetables(void);
uint64_t rook_moves(Board *board, int tile);
uint64_t bishop_moves(Board *board, int tile);
uint64_t pawn_moves(Board *board, int tile);
int is_threatened(Board *board, int tile);
int king_in_check(Board *board, int color);
int get_pieces(Board *board);
#endif // __BOARD_H__
