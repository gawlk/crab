#ifndef HISTORY_H_
#define HISTORY_H_

#include "structs.h"
#include "board.h"

/*
 * ---
 * HEADER:
 * This header contains the definition of all the functions that the user will have to manipulate the board
 * ---
 */

/*
 * Remove all elements from a stack pile
 */
void empty_stack(stack ** s);

/*
 * Free the first stack from a stack pile
 */
void free_stack(stack * s);

/*
 * Get a number of elements in a stack pile
 * Return the number
 */
int get_stack_size(stack * undo_list);

/*
 * `play` calls `_play` and empty the redo stack pile
 */
void play(board * b, coord * c, line * l, stack ** undo_list, stack ** redo_list);

/*
 * Check if a undone play can be played if so play it and remove it from the undo stack pile
 */
void redo(board * b, stack ** undo_list, stack ** redo_list);

/*
 * Remove the first element of a stack pile and return it
 */
stack * stack_pop(stack ** s);

/*
 * Add an element to the stack pile
 * Return 1 if a problem has been encountered
 * Return 0 otherwise
 */
int stack_push(stack ** s, coord * c, line * l);

/*
 * Undo the last play and add it to the undo stack pile
 */
void undo(board * b, stack ** undo_list, stack ** redo_list);

/*
 * Update all the coordinates and lines in stack by moving their x and y coordinates using a vector
 */
void update_stack(stack * s, coord * vector);

/*
 * Add a point to the board and create a new line
 * Add the move to the undo stack pile
 */
void _play(board * b, coord * c, line * l, stack ** undo_list);

#endif // HISTORY_H_