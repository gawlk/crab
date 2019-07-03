#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include "board.h"
#include "history.h"
#include "structs.h"

/*
 * ---
 * HEADER:
 * This header contains the definition of all the functions that will be used to handle
 * all the inputs and outputs of the program
 * ---
 */

/*
 * Clear the terminal screen
 */
void clear_screen();

/*
 * Wait for 'Enter' key to continue
 */
void enter_to_continue();

/*
 * Return the key pressed by the player
 */
char get_input();

/*
 * Convert a line of links from the board to a string
 * Return the string
 */
char * line_links_to_string(board * b, int i);

/*
 * Convert a line of tiles from the board to a string
 * Return the string
 */
char * line_tiles_to_string(board * b, int i, coord * cursor, valid_coords * vcs, int show_vcs, line ** possible_lines, int line_choice, int show_colors);

/*
 * Print the board
 */
void print_board(board * b, coord * cursor, valid_coords * vcs, int show_vcs, line ** possible_lines, int line_choice);

/*
 * Print a game over message
 */
void print_game_over();

/*
 * Print a help message for the execution of the game
 */
void print_help_exec();

/*
 * Print a help message in game
 */
void print_help_game();

/*
 * Calculate the score by counting all the played points and print it
 */
void print_score(stack * undo_list);

/*
 * Print a welcome message
 */
void print_welcome();

/*
 * Check the passed arguments to the program, which will either:
 * - Load a specific board
 * - Create a random board
 * - Print a help message and quit
 * Returns the address of the board
 */
board * process_args(int argc, char ** argv);

/*
 * Check the input, which will either:
 * - Move the cursor
 * - Play a move
 * - Toggle a help message
 * - Undo a move
 * - Redo a move
 * - Toggle the display of possible points
 * - Save current board to file
 * - Quit the game
 */
coord * process_input(board * b, stack ** undo_list, stack ** redo_list, valid_coords * vcs, valid_coord ** vc, int * line_choice, coord * cursor, char c, int * show_possible, int * show_help);

/*
 * Save the board to a file with a name given by the player
 */
void save(board * b);

#endif // INTERFACE_H_