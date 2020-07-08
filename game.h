#ifndef __GAME_H__
#define __GAME_H__

#include <signal.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "board.h"

#define WHITE 0
#define BLACK 1
#define FORCE 2

typedef struct Game {
    Board board;
    bool can_castle[2][2];
    uint8_t quiet_moves;
    uint8_t turn;
    uint8_t engine;
    uint8_t ep;
    int eval;
} Game;

#endif // __GAME_H__
