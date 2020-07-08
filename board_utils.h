#ifndef __BOARD_UTILS_H__
#define __BOARD_UTILS_H__

#include "board.h"
#include "utils.h"
#include "bit_utils.h"
#include "board_utils.h"

#define NW 0
#define NE 1
#define SW 2
#define SE 3
#define NORTH 4
#define EAST  5
#define SOUTH 6
#define WEST  7

static uint64_t ray[8][65];

uint64_t negative_ray(Board *board, int tile, int dir) {
    uint64_t tiles = ray[dir][tile];
    uint64_t blockers = tiles & board->occupied;

    return tiles &= ~ray[dir][msb_index(blockers)];
}


uint64_t positive_ray(Board *board, int tile, int dir) {
    uint64_t tiles = ray[dir][tile];
    uint64_t blockers = tiles & board->occupied;
    
    return tiles &= ~ray[dir][lsb_index(blockers)];
}

uint64_t pawn_moves(Board *board, int tile) {
    int color = !(board->b[WHITE][OCCUPIED] & (1ull << tile));
    uint64_t move, moves = 0;
    int x = tile % 8, y = tile / 8;
    int target;
    if (x > 0) {
        color == WHITE ? target = tile + 7 : target = tile - 9;
        move = 1ull << target;
        
        if(board->b[!color][OCCUPIED] & move) {
            moves |= move;
        }
    }

    if (x < 7) {
        color == WHITE ? target = tile + 9 : target = tile - 7;
        move = 1ull << target;
        
        if(board->b[!color][OCCUPIED] & move) {
            moves |= move;
        }
    }
    
    color == WHITE ? target = tile + 8 : target = tile - 8;
    move = 1ull << target;
    if (!(board->occupied & move)) {
        moves |= move;
    }
    
    if (((y == 1 && color == WHITE) || (y == 6 && color == BLACK))
            && (moves & move)) {

        color == WHITE ? target = tile + 16: target = tile - 16;
        move = 1ull << target;
        if (!(board->occupied & move)) {
            moves |= move;
        }
    }

    return moves;
}

uint64_t rook_moves(Board *board, int tile) {
    uint64_t result = positive_ray(board, tile, NORTH) | positive_ray(board, tile, EAST) |
        negative_ray(board, tile, SOUTH) | negative_ray(board, tile, WEST);
    return result;
}

uint64_t bishop_moves(Board *board, int tile) {
    return positive_ray(board, tile, NW) | positive_ray(board, tile, NE) |
        negative_ray(board, tile, SW) | negative_ray(board, tile, SE);
}

int is_threatened(Board *board, int tile) {
    if (!(board->occupied & (1ull << tile))) {
        return 0;
    }

    int pawn_tile;
    int color = !(board->b[WHITE][OCCUPIED] & (1ull << tile));

    if ((board->b[!color][ROOK] | board->b[!color][QUEEN]) & (rook_moves(board, tile))
            || (bishop_moves(board, tile) & (board->b[!color][BISHOP] | board->b[!color][QUEEN]))
            || (knight_moves[tile] & board->b[!color][KNIGHT])
            || (king_moves[tile] & board->b[!color][KING])) {
        return 1;
    }

    int x = tile % 8;
    if (x > 0) {
        color == WHITE ? pawn_tile = tile + 7 : pawn_tile = tile - 9;
        
        if(board->b[!color][PAWN] & (1ull << pawn_tile)) {
            return 1; 
        }
    }

    if (x < 7) {
        color == WHITE ? pawn_tile = tile + 9 : pawn_tile = tile - 7;

        if(board->b[!color][PAWN] & (1ull << pawn_tile)) {
            return 1;
        }
    }

    return 0;
}



#endif // __BOARD_UTILS_H__