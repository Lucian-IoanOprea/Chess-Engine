#include "Hashtable.h"
#include "utils.h"
#include "move.h"

uint64_t zobrist[8][64];
hash_table_entry hashtable[HT_SIZE];

void set_key(hash_table_entry *h, uint64_t key) {
    h->key = key;
}
void set_depth(hash_table_entry *h, uint8_t depth) {
    h->depth = depth;
}
void set_type(hash_table_entry *h, uint8_t type) {
    h->type = type;
}
void set_move(hash_table_entry *h, MoveScore ms) {
    h->move = ms;
}
void set_color(hash_table_entry *h, int color) {
    h->color = color;
}

void init_zobrist_table() {
    hashtable[0].key = 1;

    for(int i = 0; i < 8; ++i) {
        for(int j = 0; j < 64; ++j) {
            for(int k = 0; k < 8; ++k) {
                zobrist[i][j] = (zobrist[i][j] << 8) | (random() & 0xff);
            }
        }
    }
}

void put(uint64_t key, uint8_t depth, uint8_t type, MoveScore move, int color) {
    if (color == BLACK) {
        type == 1 ? type = 2 : type == 2 ? type = 1 : type = 0; 
        move.score = -move.score;
    }

    int index = (key % HT_SIZE);

    set_key(&hashtable[index], key);
    set_depth(&hashtable[index], depth);
    set_move(&hashtable[index], move);
    set_type(&hashtable[index], type);
    set_color(&hashtable[index], color);
}

MoveScore get(uint64_t key, uint8_t depth, int alpha, int beta, int color) {
    int index = (key % HT_SIZE);
    hash_table_entry entry = hashtable[index];

    if (color == BLACK) {
        entry.type == 1 ? entry.type = 2 : entry.type == 2 ? entry.type = 1 : entry.type = 0; 
        entry.move.score = -entry.move.score;
    }
    
    MoveScore fail;
    fail.move.begin = 64;
    fail.move.end = 64;

    if (entry.color != color || entry.key != key || entry.depth < depth) {
        return fail;
    }

    if (entry.type == 0 || (entry.type == 1 && entry.move.score >= beta) ||
        (entry.type == 2 && entry.move.score <= alpha)) {
        return entry.move;
    }

    return fail;
}
