#include "board.h"
#include "history.h"
#include "interface.h"

int main(int argc, char ** argv)
{
    board * board = NULL;
    stack * undo_list = NULL;
    stack * redo_list = NULL;
    valid_coords * vcs = NULL;
    valid_coord * vc = NULL;
    coord * cursor = NULL;

    unsigned char c = ' ';
    int line_choice = 0;
    int show_possible = 0;
    int show_help = 0;
    int status = 0;

    cursor = (coord *) malloc(sizeof(coord));
    cursor->x = 0;
    cursor->y = 0;

    board = process_args(argc, argv);

    if (board)
    {
        while (status != 3)
        {
            switch (status)
            {
                case 0:
                    clear_screen();
                    print_welcome();
                    enter_to_continue();
                    status++;
                    break;
                case 1:
                    board = extend_board(board, undo_list, redo_list);
                    vcs = get_valid_coords(board);

                    while (c != 'Q' && is_game_over(vcs) == 0)
                    {
                        clear_screen();
                        if (! vc) print_board(board, cursor, vcs, show_possible, NULL, 0);
                        else print_board(board, NULL, vcs, show_possible, vc->lines->list, line_choice);
                        print_score(undo_list);
                        if (show_help == 1) print_help_game();
                        c = get_input();
                        cursor = process_input(board, & undo_list, & redo_list, vcs, & vc, & line_choice, cursor, c, & show_possible, & show_help);
                        if (! vc)
                        {
                            free_valid_coords(vcs);
                            board = extend_board(board, undo_list, redo_list);
                            vcs = get_valid_coords(board);
                        }
                    }
                    free_valid_coords(vcs);
                    status++;
                    break;
                case 2:
                    // TODO: Add a way to replay
                    clear_screen();
                    print_board(board, NULL, NULL, 0, NULL, 0);
                    print_game_over();
                    print_score(undo_list);
                    enter_to_continue();
                    if (board) free_board(board, board->y_max);
                    empty_stack(& undo_list);
                    empty_stack(& redo_list);
                    status++;
                    break;
            }
        }
        clear_screen();
    }

    free(cursor);

    return 0;
}
