#include "utils.h"
#include "Hashtable.h"
#include "game.h"
#include "move.h"
#include "board.h"
#include <bits/stdc++.h>

void reset_game(Game *game) {
    reset_board(&(game->board));
    for(int i = 0; i < 2; ++i) {
        for(int j = 0; j < 2; ++j) {
            game->can_castle[i][j] = 1;
        }
    }

    game->ep = 9;
    game->eval = 0;
    game->quiet_moves = 0;
    game->turn = WHITE;
    game->engine = BLACK;
}

int main(int argc, char *argv[]) {
    Game game;
    char *line = NULL;
    size_t len = 0;
    bool endgame = false;
    signal(SIGINT, SIG_IGN);
    setvbuf(stdin, NULL, _IOLBF, 0);
    setvbuf(stdout, NULL, _IOLBF, 0);
    init_zobrist_table();
    generate_movetables();
    reset_game(&game);

    printf("feature done=1\n");
    srand(time(NULL));
    while (getline(&line, &len, stdin) != -1) {
        if (line[strlen(line) - 1] == '\n')
            line[strlen(line) - 1] = '\0';
        if (strcmp(line, "xboard") == 0) {
            continue;
        } else if (strcmp(line, "new") == 0) {
            reset_game(&game);
        } else if (strcmp(line, "force") == 0) {
            game.engine = FORCE;
        } else if (strcmp(line, "go") == 0) {
            game.engine = game.turn;
        } else if (strcmp(line, "resign") == 0) {
            printf("resign\n");
        } else if (strcmp(line, "quit") == 0) {
            exit(EXIT_SUCCESS);
        } else if (is_xboard_move(line)) {
            Move m = get_xboard_move(line);

            if(is_valid_move(game, m, 1,endgame)) {
                apply_move(&game, m,endgame);
            }
        }

        if (game.turn == game.engine) {
            Move m = best_move(game,endgame);
            if (m.begin != 64) {
                apply_move(&game, m,endgame);
                int res = get_pieces(&(game.board));
                if( res <= 9 ){
                    endgame = true;
                }
                printf("move %s\n", xboard_move(m));
            }
        }
    }

    return 0;
}
