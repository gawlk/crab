#include "interface.h"

void clear_screen()
{
    printf("\e[1;1H\e[2J");
}

void enter_to_continue()
{
    unsigned char c;
    printf("    Press 'Enter' to continue");
    while ((c = get_input()) != '\n');
}

char get_input()
{
    // TODO: Check if creation of structs during each during creates a memory leak
    struct termios old_tio;
    struct termios new_tio;
    unsigned char c;

    /* Get the terminal */
    tcgetattr(STDIN_FILENO, & old_tio);

    new_tio = old_tio;

    /* Black magic */
    new_tio.c_lflag &= (~ICANON & ~ECHO);

    /* Apply new settings */
    tcsetattr(STDIN_FILENO, TCSANOW, & new_tio);

    c = getchar();

    /* Restore the former settings */
    tcsetattr(STDIN_FILENO, TCSANOW, & old_tio);

    return c;
}

char * line_links_to_string(board * b, int i)
{
    char * str = (char *) malloc(b->x_max * 2); // - 1 + 1 for \0

    for(int j = 0; j < b->x_max; j++)
    {
        if (j != 0)
        {
            if (b->tiles[i][j - 1][0][2] == 1) strcat(str, "/");
            else strcat(str, " ");
        }
        else strcpy(str, "");

        if (is_point(b->tiles, j, i) == 1)
        {
            if (b->tiles[i][j - 1][0][2] == 1 && b->tiles[i][j][0][0] == 1) str[strlen(str) - 1] = 'X';
            else if (b->tiles[i][j][0][0] == 1) str[strlen(str) - 1] = '\\';

            if (b->tiles[i][j][0][1] == 1) strcat(str, "|");
            else strcat(str, " ");
        }
        else strcat(str, " ");
    }

    return str;
}

char * line_tiles_to_string(board * b, int i, coord * cursor, valid_coords * vcs, int show_vcs, line ** possible_lines, int line_choice, int show_colors)
{
    // 512 Because of various additional colors
    char * str = (char *) malloc(512);

    for(int j = 0; j < b->x_max; j++)
    {
        if (j != 0)
        {
            if (b->tiles[i][j][1][0] == 1) strcat(str, "-");
            else strcat(str, " ");
        }
        else strcpy(str, "");

        if ((show_colors == 1 && cursor && cursor->x == j && cursor->y == i) || (possible_lines && line_contains_xy(possible_lines[line_choice], j, i) == 1)) strcat(str, "\e[47;30m");

        if (is_point(b->tiles, j, i) == 1) strcat(str, "o");
        else
        {
            valid_coord * vc = NULL;
            if (vcs) vc = is_valid_coord(vcs, j, i);

            if (show_vcs == 1 && vc) strcat(str, "*");
            else
            {
                if (show_colors == 1) strcat(str, "\e[90m");
                strcat(str, ".");
            }
        }

        if (show_colors == 1) strcat(str, "\e[0m");
    }
    return str;
}

void print_board(board * b, coord * cursor, valid_coords * vcs, int show_vcs, line ** possible_lines, int line_choice)
{
    printf("\e[90m.");
    for(int i = 0; i < b->x_max * 2 + 1; i++) printf("-");
    printf(".\n");
    for(int i = 0; i < b->y_max; i++)
    {
        printf("| \e[0m");
        if (i != 0)
        {
            char * links = line_links_to_string(b, i);
            printf("%s", links);
            free(links);
            printf("\e[90m |\n| \e[0m");
        }
        char * tiles = line_tiles_to_string(b, i, cursor, vcs, show_vcs, possible_lines, line_choice, 1);
        printf("%s", tiles);
        free(tiles);
        printf("\e[90m |\n");
    }
    printf("'");
    for(int i = 0; i < b->x_max * 2 + 1; i++) printf("-");
    printf("'\n");
    printf("\e[0m\n");
}

void print_game_over()
{
    printf("    - Game over -\n");
    printf("\n");
}

// TODO: Translate to english
void print_help_exec()
{
    printf("\n");
    printf("crab\n");
    printf("\n");
    printf("\"main\"         - Launch game with a random board\n");
    printf("\"main -r FILE\" - Launch game with a specific board\n");
    printf("\"main -h\"      - See this message\n");
    printf("\n");
}

void print_help_game()
{
    printf("    ------------\n");
    printf("\n");
    printf("    How to play:\n");
    printf("\n");
    printf("wasd/zqsd/Arrows - Move the cursor\n");
    printf("Enter - Play a move (if possible)\n");
    printf("h - Toggle this message\n");
    printf("u - Undo a move\n");
    printf("r - Redo a move\n");
    printf("p - Toggle the display of possible points\n");
    printf("S - Save current board to file\n");
    printf("Q - Quit the game\n");
    printf("\n");
    printf("    ------------\n");
    printf("\n");
}

void print_score(stack * undo_list)
{
    printf("    Score: %d\n", get_stack_size(undo_list));
    printf("\n");
}

void print_welcome()
{
    // TODO: Find a better name
    printf("\n");
    printf("  ______     ______     ______     ______    \n");
    printf(" /\\  ___\\   /\\  == \\   /\\  __ \\   /\\  == \\   \n");
    printf(" \\ \\ \\____  \\ \\  __<   \\ \\  __ \\  \\ \\  __<   \n");
    printf("  \\ \\_____\\  \\ \\_\\ \\_\\  \\ \\_\\ \\_\\  \\ \\_____\\ \n");
    printf("   \\/_____/   \\/_/ /_/   \\/_/\\/_/   \\/_____/ \n");
    printf("\n");
    printf("     \e[90m.\e[0m o-o-o-o-o \e[90m. .\e[0m\n");
    printf("\n");
    printf("      > Welcome, !\n");
    printf("    To this wonderful solo tic-tac-toe.\n");
    printf("\n");
    printf("    During the game, you can press:\n");
    printf("     'h' - To toggle a help message\n");
    printf("\n");
}

board * process_args(int argc, char ** argv)
{
    // TODO: Add some print when return NULL
    if (argc == 1)
    {
        return random_board();
    }
    else if (strcmp(argv[1], "-h") == 0)
    {
        print_help_exec();
    }
    else if (argc > 2 && strcmp(argv[1], "-r") == 0)
    {
        return file_to_board(argv[2]);
    }
    return NULL;
}

coord * process_input(board * b, stack ** undo_list, stack ** redo_list, valid_coords * vcs, valid_coord ** vc, int * line_choice, coord * cursor, char c, int * show_possible, int * show_help)
{
    switch(c)
    {
        case 'h':
            * show_help = abs((* show_help) - 1);
            break;
        case 'r':
            if (! (* vc)) redo(b, undo_list, redo_list);
            break;
        case 'u':
            if (! (* vc)) undo(b, undo_list, redo_list);
            break;
        case 'p':
            * show_possible = abs((* show_possible) - 1);
            break;
        case 'w':
        case 'z':
        case 'A':
            if (! (* vc))
            {
                if (cursor->y - 1 >= 0) cursor->y--;
                else cursor->y = b->y_max - 1;
            }
            break;
        case 's':
        case 'B':
            if (! (* vc))
            {
                if (cursor->y + 1 < b->y_max) cursor->y++;
                else cursor->y = 0;
            }
            break;
        case 'd':
        case 'C':
            if (! (* vc))
            {
                if (cursor->x + 1 < b->x_max) cursor->x++;
                else cursor->x = 0;
            }
            else
            {
                (* line_choice)++;
                if ((* line_choice) == (* vc)->lines->len) (* line_choice) = 0;
            }
            break;
        case 'a':
        case 'q':
        case 'D':
            if (! (* vc))
            {
                if (cursor->x - 1 >= 0) cursor->x--;
                else cursor->x = b->x_max - 1;
            }
            else
            {
                (* line_choice)--;
                if ((* line_choice) < 0) (* line_choice) = (* vc)->lines->len - 1;
            }
            break;
        case 'S':
            save(b);
            break;
        case '\n':
            if (! * vc)
            {
                (* vc) = is_valid_coord(vcs, cursor->x, cursor->y);

                if (* vc && (* vc)->lines->len == 1)
                {
                    play(b, (* vc)->c, (* vc)->lines->list[0], undo_list, redo_list);
                    (* vc) = NULL;
                }
            }
            else
            {
                play(b, (* vc)->c, (* vc)->lines->list[* line_choice], undo_list, redo_list);
                (* line_choice) = 0;
                (* vc) = NULL;
            }
            break;
        case 'Q':
        case ' ':
            break;
    }

    return cursor;
}

void save(board * b)
{
    // TODO: Change to malloc
    char name[32];
    int error = 0;

    printf("    Enter file name: ");
    fgets(name, sizeof(name), stdin);
    name[strlen(name) - 1] = '\0';

    error = board_to_file(b, name);
    if (error == 1) printf("    Something went wrong...\n");
    else printf("    Success !\n");
    printf("\n");

    enter_to_continue();
}