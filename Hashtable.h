#ifndef __HASHTABLE_H__
#define __HASHTABLE_H__

#include <signal.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include "utils.h"

void init_zobrist_table();
void set_key(hash_table_entry *h, uint64_t key);
void set_depth(hash_table_entry *h, uint8_t depth);
void set_type(hash_table_entry *h, uint8_t type);
void set_move(hash_table_entry *h, MoveScore ms);
void set_color(hash_table_entry *h, int color);

void put(uint64_t key, uint8_t depth, uint8_t type, MoveScore move, int color);
MoveScore get(uint64_t key, uint8_t depth, int alpha, int beta, int color);

#endif // __HASHTABLE_H__