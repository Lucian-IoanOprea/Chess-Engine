#include "utils.h"
#include <time.h>
#include <bits/stdc++.h>
#include "Hashtable.h"

#define SEARCHDEPTH 6
#define MOVES_MAX 120

static void sort_moves(Move *moves, int nmoves, Game *game) {
    int first_captured = 0;
    int first_not_captured = 0;
    while(1) {
        while(first_not_captured < nmoves && (game->board.occupied & (1ull << moves[first_not_captured].end)))
            ++first_not_captured;

        if(first_not_captured >= nmoves){
            break;
        }
        while(first_captured < nmoves && !(game->board.occupied & (1ull << moves[first_captured].end))){
            ++first_captured;
    	}

        if(first_captured >= nmoves){
            break;
        }
        Move tmp;
        tmp = moves[first_captured];
        moves[first_captured] = moves[first_not_captured];
        moves[first_not_captured] = tmp;
        ++first_not_captured;
    }
}

 MoveScore alphabeta(Game game, int alpha, int beta, int depth,bool isendgame) {
    Move moves[MOVES_MAX];
    int nmoves, move;
    Move m;
    MoveScore best, new_move;
    Game orig_game;
    int legal_move = 0,  hashtype = 2;

    
    orig_game = game;

    new_move = get(orig_game.board.zobrist, depth, alpha, beta,
            orig_game.turn);
    if (new_move.move.begin != 64) {
        return new_move;
    }

    depth < SEARCHDEPTH - 1 ? best.score = alpha : best.score = -inf;
    if(depth == 0) {
        best.move.begin = 64;
        best.score = game.eval;
        put(orig_game.board.zobrist, depth, 0, best,
                orig_game.turn);
        return best;
    }

    generate_movelist(&game, moves, &nmoves);
    sort_moves(moves, nmoves, &game);

    for(move = 0; move < nmoves; move++) {
        m = moves[move];
        game = orig_game;
        apply_move(&game, m,isendgame);

        if(king_in_check(&(game.board), !game.turn)) {
            continue;
        }

        if(!legal_move) {
            best.move = m;
            legal_move = 1;
        }

        depth == SEARCHDEPTH ? new_move = alphabeta(game, -inf, inf, depth - 1,isendgame) : new_move = alphabeta(game, -beta, -best.score, depth - 1,isendgame);
        new_move.score = -new_move.score;
        
        if(new_move.score >= beta) {
            best.move = m;
            best.score = beta;
            put(orig_game.board.zobrist, depth, 1, best, orig_game.turn);
            return best;
        }

        if(new_move.score > best.score) {
            best.move = m;
            best.score = new_move.score;
            hashtype = 0;
        }
    }

    game = orig_game;
    
    if(legal_move) {
        put(orig_game.board.zobrist, depth, hashtype, best, orig_game.turn);
    }else{
         if(king_in_check(&(game.board), game.turn)){
            best.score = -inf; 
            best.score += (depth - SEARCHDEPTH);
        }else{
            best.score = 0;
        }
        best.move.begin = 64;
    }

    return best;
}

MoveScore iterative_deepening(Game game,bool isendgame) {
    int i = 1;
    MoveScore best;

    while(i <= SEARCHDEPTH) {
        best = alphabeta(game, -inf, inf, i,isendgame);
        if(best.move.begin == 64 || best.score == inf){
             return best;
         }
         i++;

    }

    return best;
}

Move best_move(Game game, bool isendgame) {
    
    return iterative_deepening(game,isendgame).move;
}
