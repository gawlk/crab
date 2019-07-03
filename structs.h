#ifndef STRUCTS_H_
#define STRUCTS_H_

/*
 * ---
 * HEADER:
 * This header contains the definition of all the different structures that are used by other functions
 * ---
 */

/*
 * `tile` is a 3*3 matrix of 1-s and 0-s
 * Each element of the matrix (except the one on the center that makes of the tile a point or an empty tile) represents a neighbor tile on the board
 * If an element equals to 1, it means that the point is linked to a neighbor tile (where there must also be a point)
 * Otherwise it's just a meaningless tile for the point
 */
typedef int tile[3][3];

typedef struct {
    int len;
    tile ** list;
} tiles;

/*
 * A board is a structure containing tiles and a vertical and horizontal size
 */
typedef struct {
    int x_max;
    int y_max;
    tile ** tiles;
} board;

/*
 * `coord` is a structure containing two int-s
 * Combined, they represent a position on the board
 */
typedef struct {
    int x;
    int y;
} coord;

/*
 * `line` is a structure that has two coordinates
 * The first one is for its start and the second one is for its end
 */
typedef struct {
    coord * start;
    coord * end;
} line;

typedef struct {
    int len;
    line ** list;
} lines;

/*
 * `valid_coord` is a structure that contains coordinates
 * and a list containing all the possible lines it can create
 */
typedef struct {
    coord * c;
    lines * lines;
} valid_coord;

typedef struct {
    int len;
    valid_coord ** list;
} valid_coords;

/*
 * Define the structure for the redo list and undo list
 */

typedef struct base_stack stack;
struct base_stack
{
    coord * c;
    line * l;
    stack * previous;
};

#endif // STRUCTS_H_