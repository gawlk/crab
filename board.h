#ifndef BOARD_H_
#define BOARD_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "interface.h"
#include "structs.h"

/*
 * ---
 * HEADER:
 * This header contains the definition of all the functions that will have an impact on the board itself
 * ---
 */

#define LINE_SIZE 5

/*
 * Save to current board into a file
 * The name of the file must be given by the player
 * Return 1 if a problem is encountered
 * (if a file with such a name already exists for example)
 * Return 0 otherwise
 */
int board_to_file(board * b, char * name);

/*
 * Copy the content of a line to another
 */
line * copy_line(coord * start, coord * end);

/*
 * Copy the content of a tile to another
 */
void copy_tile(tile from, tile to);

/*
 * Check the ends of the board and add a line or a column if a point is found
 * The goal is to never have a point on the ends of the board
 * Returns the board
 */
board * extend_board(board * b, stack * undo_list, stack * redo_list);

/*
 * Annex function of extend_board
 * Add a column to the board
 */
board * extend_column(board * b);

/*
 * Annex function of extend_board
 * Add a line to the board
 */
board * extend_line(board * b);

/*
 * Read a file and use its content to initiate the board
 * Return NULL if a problem is encountered
 * Return the new address of the board otherwise
 */
board * file_to_board(char * name);

/*
 * Free a struct board from the memory
 * `y` is the number of tiles lines the board has
 */
void free_board(board * b, int y);

/*
 * Free a struct line from the memory
 */
void free_line(line * l);

/*
 * Free a struct lines from the memory
 */
void free_lines(lines * l);

/*
 * Free a struct valid_coord from the memory
 */
void free_valid_coord(valid_coord * vc);

/*
 * Free a struct valid_coords from the memory
 */
void free_valid_coords(valid_coords * vcs);

/*
 * Return a list of all the possible coordinates where a player can put a point
 * The list is in a struct `valid_coords`
 */
valid_coords * get_valid_coords(board * b);

/*
 * Get the status of all the neighbors of a point that were already generated during the creation of the board
 * By getting their status we can update the status of the point and get its links to other points
 */
void get_status_of_neighbors(tile ** tiles, int x, int y, int is_at_right_border);

/*
 * Check if a line or a column has a point on it (used by extend_board)
 * Return 1 if it's the case
 * Return 0 otherwise
 */
int has_point(board * b, int i_max, int x, int line);

/*
 * Initiate a line and it's values
 */
line * init_line(int x, int y);

/*
 * Initiate a tile of the board
 * The tile will be set as a point or an empty tile depending of the status value
 */
void init_tile(tile ** tiles, int x, int y, int status);

/*
 * Check if there are any coordinates where to player can put a point
 * Return 1 it's the case
 * Return 0 otherwise
 */
int is_game_over(valid_coords * vcs);

/*
 * Check if a tile on the board is a point or an empty tile
 * Return 1 if it's a point
 * Return 0 otherwise
 */
int is_point(tile ** tiles, int x, int y);

/*
 * Check if an an array of coordinates contains particular coordinates
 * Return 1 if coordinates are found
 * Return 0 otherwise
 */
valid_coord * is_valid_coord(valid_coords * vcs, int x, int y);

/*
 * Check if a line contains a certain point (using x and y)
 * Return 1 if it's the case
 * Return 0 otherwise
 */
int line_contains_xy(line * l, int x_point, int y_point);

/*
 * Create a board with a random size from X to Y
 */
board * random_board();

/*
 * Rellocate the area of memory were the tiles are, to a bigger area
 * and allocate the enough memory for the first tile of the new line
 * Return 1 if something went wrong
 * Return 0 otherwise
 */
int realloc_tiles(board * b, int y);

/*
 * Rellocate the area of memory where one line of tiles from the board is, to a bigger area
 * Return 1 if something went wrong
 * Return 0 otherwise
 */
int realloc_tiles_line(board * b, int x, int y);

/*
 * Set all the connections of a line on the board to 1 or 0
 * Depending of the `status` parameter
 */
void set_board_line(board * b, line * l, int status);

/*
 * Solve the game with best possible score
 */
void solver();

/*
 * Update all the lines that can be created in a direction of a valid_coord
 */
void update_valid_coord_lines(board * b, valid_coord * vc, int x_dir, int y_dir);

#endif // BOARD_H_