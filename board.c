#include "board.h"

int board_to_file(board * b, char * name)
{
    if (access(name, F_OK) != -1) return 1;
    FILE * file;
    file = fopen(name, "w");
    if (file == NULL) return 1;
    char * value = "%s\n";

    for(int i = 0; i < b->y_max; i++)
    {
        char * links = line_links_to_string(b, i);
        if (i != 0) fprintf(file, value, links);
        free(links);
        if (i == (b->y_max - 1)) value = "%s";
        char * tiles = line_tiles_to_string(b, i, NULL, NULL, 0, NULL, 0, 0);
        fprintf(file, value, tiles);
        free(tiles);
    }
    fclose(file);
    return 0;
}

line * copy_line(coord * start, coord * end)
{
    // TODO: Check if mallocs worked
    line * l = NULL;
    l = (line *) malloc(sizeof(line));
    l->start = (coord *) malloc(sizeof(coord));
    l->end = (coord *) malloc(sizeof(coord));
    l->start->x = start->x;
    l->start->y = start->y;
    l->end->x = end->x;
    l->end->y = end->y;
    return l;
}

void copy_tile(tile from, tile to)
{
    for(int i = 0; i < 3; i++)
        for(int j = 0; j < 3; j++)
            to[i][j] = from[i][j];
}

board * extend_board(board * b, stack * undo_list, stack * redo_list)
{
    coord * vector = (coord *) malloc(sizeof(coord *));
    vector->x = 0;
    vector->y = 0;

    // Top
    if (has_point(b, b->x_max, 0, 1) == 1)
    {
        b = extend_line(b);
        tile * line_tempo = b->tiles[b->y_max - 1];
        for (int i = b->y_max - 2; i >= 0; i--)
            b->tiles[i + 1] = b->tiles[i];
        b->tiles[0] = line_tempo;
        vector->y = 1;
    }

    // Bottom
    if (has_point(b, b->x_max, b->y_max - 1, 1) == 1)
        b = extend_line(b);

    // Left
    if (has_point(b, b->y_max, 0, 0) == 1)
    {
        b = extend_column(b);
        for (int i = 0; i < b->y_max; i++)
        {
            tile tile_tempo = {0};
            copy_tile(b->tiles[i][b->x_max - 1], tile_tempo);
            for (int j = b->x_max - 2; j >= 0; j--)
                copy_tile(b->tiles[i][j], b->tiles[i][j + 1]);
            copy_tile(tile_tempo, b->tiles[i][0]);
        }
        vector->x = 1;
    }

    // Right
    if (has_point(b, b->y_max, b->x_max - 1, 0) == 1)
        b = extend_column(b);

    // Update lists
    if (vector->x != 0 || vector->y != 0)
    {
        update_stack(undo_list, vector);
        update_stack(redo_list, vector);
    }

    free(vector);

    return b;
}

board * extend_column(board * b)
{
    (b->x_max)++;
    for (int i = 0; i < b->y_max; i++)
        if (realloc_tiles_line(b, b->x_max - 1, i) == 1) return NULL;
    return b;
}

board * extend_line(board * b)
{
    (b->y_max)++;

    if (realloc_tiles(b, b->y_max - 1) == 1)
        return NULL;

    free(b->tiles[b->y_max - 1]);
    b->tiles[b->y_max - 1] = malloc(b->x_max * sizeof(tile));

    for(int i = 0; i < b->x_max; i++)
    {
        init_tile(b->tiles, i, b->y_max - 1, 0);
    }
    return b;
}

board * file_to_board(char * name)
{
    FILE * file;
    file = fopen(name, "r");
    if (file == NULL) return NULL;

    board * b = (board *) malloc(sizeof(board));
    b->tiles = (tile **) malloc(sizeof(tile *));
    b->tiles[0] = (tile *) calloc(1, sizeof(tile));

    if (b && b->tiles && b->tiles[0])
    {
        int c = 0;
        int c_old = 0;
        int x = 0;
        int x_old = 0;
        int x_board = 0;
        int y = 0;
        int y_board = 0;
        int e = 0; // Error

        while ((c = getc(file)) != EOF)
        {
            printf("%c", c);
            switch(c)
            {
                case '.' :
                case '*' :
                    if ((x != 0 && (c_old != ' ' && c_old != '-')) || y % 2 != 0)
                        e = 1;
                    else if (x != 0 && realloc_tiles_line(b, x_board, y_board) == 1)
                        return NULL;
                    break;

                case 'o' :
                    if ((x != 0 && (c_old != ' ' && c_old != '-')) || y % 2 != 0)
                        e = 1;
                    else if (x != 0 && realloc_tiles_line(b, x_board, y_board) == 1)
                        return NULL;
                    else
                    {
                        b->tiles[y_board][x_board][1][1] = 1;
                        get_status_of_neighbors(b->tiles, x_board, y_board, x_board == x_old);
                    }
                    break;

                case ' ' :
                    if (y % 2 == 0 && c_old != '.' && c_old != 'o' && c_old != '*')
                        e = 1;
                    break;

                case '-' :
                    if (c_old != 'o' || is_point(b->tiles, x_board, y_board) != 1)
                        e = 1;
                    else
                        b->tiles[y_board][x_board][1][2] = 1;
                    break;

                case '|' :
                    if (x % 2 == 1 || is_point(b->tiles, x_board, y_board) != 1)
                        e = 1;
                    else
                        b->tiles[y_board][x_board][2][1] = 1;
                    break;

                case '/' :
                    if (x % 2 == 0 || is_point(b->tiles, x_board + 1, y_board) != 1)
                        e = 1;
                    else
                        b->tiles[y_board][x_board + 1][2][0] = 1;
                    break;

                case '\\' :
                    if (x % 2 == 0 || is_point(b->tiles, x_board, y_board) != 1)
                        e = 1;
                    else
                        b->tiles[y_board][x_board][2][2] = 1;
                    break;

                case 'X' :
                    if (x % 2 == 0 || is_point(b->tiles, x_board, y_board) != 1 || is_point(b->tiles, x_board + 1, y_board) != 1)
                        e = 1;
                    else
                    {
                        b->tiles[y_board][x_board][2][2] = 1;
                        b->tiles[y_board][x_board + 1][2][0] = 1;
                    }
                    break;

                case '\n' :
                    if (y % 2 == 0 && x_board + 1 < LINE_SIZE)
                    {
                        free_board(b, y_board);
                        return NULL;
                    }

                    if (y == 0) x_old = x_board;

                    if ((y % 2 == 0 && (x % 2 == 0 || x_board != x_old)) || (y % 2 == 1 && x_board > x_old)) {
                        e = 1;
                        break;
                    }

                    y++;
                    y_board = y / 2;

                    if (y % 2 == 0 && realloc_tiles(b, y_board) == 1)
                        return NULL;

                    x = -1; // Not 0 because of the x++ at the end
                    break;

                default :
                    e = 1;
            }

            x++;
            x_board = x / 2;
            c_old = c;

            if (e == 1)
            {
                free_board(b, y_board);
                return NULL;
            }
        }
        if (c_old != '.' && c_old != 'o') return NULL;

        // TODO: Add a checker to see if lines have LINE_SIZE points

        if (y_board + 1 < LINE_SIZE)
        {
            free_board(b, y_board);
            return NULL;
        }

        fclose(file);

        b->x_max = x_board + 1;
        b->y_max = y_board + 1;

        return b;
    }
    else
    {
        free_board(b, 1);
        return NULL;
    }
}

void free_board(board * b, int y)
{
    int i;
    for (i = 0; i < y; ++i)
    {
        free(b->tiles[i]);
    }
    free(b->tiles);
    free(b);
}

void free_line(line * l)
{
    free(l->start);
    free(l->end);
    free(l);
}

void free_lines(lines * l)
{
    int i;
    for (i = 0; i < l->len; i++)
    {
        free_line(l->list[i]);
    }
    if(l->len != 0) free(l->list);
    free(l);
}

void free_valid_coord(valid_coord * vc)
{
    free_lines(vc->lines);
    free(vc->c);
    free(vc);
}

void free_valid_coords(valid_coords * vcs)
{
    int i;
    for (i = 0; i < vcs->len; ++i)
    {
        free_valid_coord(vcs->list[i]);
    }
    if(vcs->len != 0) free(vcs->list);
    free(vcs);
}

valid_coords * get_valid_coords(board * b)
{
    valid_coords * vcs = (valid_coords *) malloc(sizeof(valid_coords));
    vcs->len = 0;

    for(int i = 0; i < b->y_max; i++)
    {
        for(int j = 0; j < b->x_max; j++)
        {
            if (is_point(b->tiles, j, i) != 1)
            {
                valid_coord * vc = (valid_coord *) malloc(sizeof(valid_coord));
                vc->c = (coord *) malloc(sizeof(coord));
                vc->c->x = j;
                vc->c->y = i;
                vc->lines = (lines *) malloc(sizeof(lines));
                vc->lines->len = 0;

                update_valid_coord_lines(b, vc, -1, 0);
                update_valid_coord_lines(b, vc, -1, -1);
                update_valid_coord_lines(b, vc, 0, -1);
                update_valid_coord_lines(b, vc, 1, -1);

                if (vc->lines->len == 0)
                {
                    free_valid_coord(vc);
                }
                else
                {
                    // TODO: Check if realloc worked
                    (vcs->len)++;

                    if (vcs->len == 1)
                        vcs->list = (valid_coord **) malloc(sizeof(valid_coord *));
                    else
                        vcs->list = (valid_coord **) realloc(vcs->list, vcs->len * sizeof(valid_coord *));

                    vcs->list[vcs->len - 1] = vc;
                }
            }
        }
    }

    return vcs;
}

void get_status_of_neighbors(tile ** tiles, int x, int y, int is_at_right_border)
{
    if (y != 0)
    {
        if (x != 0 && is_point(tiles, x - 1, y - 1) == 1 && tiles[y - 1][x - 1][2][2] == 1)
            tiles[y][x][0][0] = 1;
        if (is_point(tiles, x, y - 1) == 1 && tiles[y - 1][x][2][1] == 1)
            tiles[y][x][0][1] = 1;
        if (is_at_right_border != 1 && is_point(tiles, x + 1, y - 1) == 1 && tiles[y - 1][x + 1][2][0] == 1)
            tiles[y][x][0][2] = 1;
    }
    if (x != 0 && is_point(tiles, x - 1, y) == 1 && tiles[y][x - 1][1][2] == 1)
        tiles[y][x][1][0] = 1;
}

int has_point(board * b, int i_max, int x, int line)
{
    for (int i = 0; i < i_max; i++)
    {
        if (line == 1 && is_point(b->tiles, i, x) == 1) return 1;
        else if (line == 0 && is_point(b->tiles, x, i) == 1) return 1;
    }
    return 0;
}

line * init_line(int x, int y)
{
    // TODO: Check if mallocs worked
    line * l = NULL;
    l = (line *) malloc(sizeof(line));
    l->start = (coord *) malloc(sizeof(coord));
    l->end = (coord *) malloc(sizeof(coord));
    l->start->x = x;
    l->start->y = y;
    l->end->x = x;
    l->end->y = y;
    return l;
}

void init_tile(tile ** tiles, int x, int y, int status)
{
    for(int i = 0; i < 3; i++)
    {
        for(int j = 0; j < 3; j++)
        {
            if (i == 1 && j == 1) tiles[y][x][1][1] = status;
            else tiles[y][x][i][j] = 0;
        }
    }
}

int is_game_over(valid_coords * vcs)
{
    return vcs->len == 0;
}

int is_point(tile ** tiles, int x, int y)
{
    if (tiles[y][x][1][1] == 1) return 1;
    return 0;
}

valid_coord * is_valid_coord(valid_coords * vcs, int x, int y)
{
    for(int i = 0; i < vcs->len; i++)
    {
        valid_coord * vc = vcs->list[i];
        if (vc->c->x == x && vc->c->y == y) return vcs->list[i];
    }
    return NULL;
}

// TODO: Duplicate of set_board_line()
int line_contains_xy(line * l, int x_point, int y_point)
{
    int x;
    int y;
    int x_dir = (l->end->x - l->start->x) / (LINE_SIZE - 1);
    int y_dir = (l->end->y - l->start->y) / (LINE_SIZE - 1);

    for (int i = 0; i < LINE_SIZE; i++)
    {
        x = l->start->x + (x_dir * i);
        y = l->start->y + (y_dir * i);
        if (x == x_point && y == y_point) return 1;
    }
    return 0;
}

board * random_board()
{
    // TODO: Handle malloc errors and return NULL
    srand(time(NULL));

    int i;
    int j;

    const int max_rand = 15;
    int x = rand() % max_rand + LINE_SIZE + 1;
    int y = rand() % max_rand + LINE_SIZE + 1;

    valid_coords * vcs = NULL;
    valid_coord * vc = NULL;

    board * b = (board *) malloc(sizeof(board));
    b->x_max = x;
    b->y_max = y;
    b->tiles = (tile **) malloc(y * sizeof(tile *));

    for(i = 0; i < y; i++)
    {
        b->tiles[i] = malloc(x * sizeof(tile));
    }

    do
    {
        if (vcs) free_valid_coords(vcs);

        for(i = 0; i < y; i++)
        {
            for(j = 0; j < x; j++)
            {
                init_tile(b->tiles, j, i, 0);
            }
        }

        for(i = 0; i < y; i++)
        {
            for(j = 0; j < x; j++)
            {
                vcs = get_valid_coords(b);
                vc = is_valid_coord(vcs, j, i);
                if (vc == NULL)
                {
                    if (rand() % 3 == 1) b->tiles[i][j][1][1] = 1;
                }
                else if (rand() % 2 == 1)
                {
                    b->tiles[i][j][1][1] = 1;
                    set_board_line(b, vc->lines->list[0], 1);
                }
                free_valid_coords(vcs);
            }
        }
        vcs = get_valid_coords(b);
    }
    while (vcs->len == 0);

    free_valid_coords(vcs);

    return b;
}

int realloc_tiles(board * b, int y)
{
    tile ** tiles_tempo = (tile **) realloc(b->tiles, (y + 1) * sizeof(tile *));
    if (! tiles_tempo)
    {
        free_board(b, y);
        return 1;
    }
    else
    {
        b->tiles = tiles_tempo;
    }

    b->tiles[y] = (tile *) calloc(1, sizeof(tile));
    if (! b->tiles[y])
    {
        free_board(b, y + 1);
        return 1;
    }
    return 0;
}

int realloc_tiles_line(board * b, int x, int y)
{
    tile * tiles_line_tempo = (tile *) realloc(b->tiles[y], (x + 1) * sizeof(tile));
    if (! tiles_line_tempo)
    {
        free_board(b, y);
        return 1;
    }
    else
    {
        b->tiles[y] = tiles_line_tempo;
        init_tile(b->tiles, x, y, 0);
        return 0;
    }
}

void set_board_line(board * b, line * l, int status)
{
    int x;
    int y;
    int x_dir = (l->end->x - l->start->x) / (LINE_SIZE - 1);
    int y_dir = (l->end->y - l->start->y) / (LINE_SIZE - 1);

    for (int i = 0; i < LINE_SIZE; i++)
    {
        x = l->start->x + (x_dir * i);
        y = l->start->y + (y_dir * i);
        if (i != 0) b->tiles[y][x][1 - y_dir][1 - x_dir] = status;
        if (i != LINE_SIZE - 1) b->tiles[y][x][1 + y_dir][1 + x_dir] = status;
    }
}

void solver()
{
    // TODO: Make a tree and compare results
}

void update_valid_coord_lines(board * b, valid_coord * vc, int x_dir, int y_dir)
{
    int counter = 0;
    int dir_counter = 0;
    int state = 0;
    int x = vc->c->x;
    int y = vc->c->y;
    coord * tmp_start = NULL;
    coord * tmp_end = NULL;

    line * l = init_line(x, y);

    while(state != 3)
    {
        x += x_dir;
        y += y_dir;

        if (x < 0 || y < 0 || x >= b->x_max || y >= b->y_max || is_point(b->tiles, x, y) == 0)
        {
            state++;
        }
        else
        {
            dir_counter++;

            if (dir_counter == LINE_SIZE)
            {
                state++;
            }
            else
            {
                counter++;

                if (state == 0)
                {
                    l->start->x = x;
                    l->start->y = y;
                }
                else
                {
                    l->end->x = x;
                    l->end->y = y;
                }

                if (counter >= LINE_SIZE - 1)
                {
                    // TODO: Check if realloc worked
                    (vc->lines->len)++;

                    if (vc->lines->len == 1)
                        vc->lines->list = (line **) malloc(sizeof(line *));
                    else
                        vc->lines->list = (line **) realloc(vc->lines->list, vc->lines->len * sizeof(line *));

                    vc->lines->list[vc->lines->len - 1] = copy_line(l->start, l->end);

                    free_line(l);
                    l = init_line(x, y);

                    tmp_start = vc->lines->list[vc->lines->len - 1]->start;
                    tmp_end = vc->lines->list[vc->lines->len - 1]->end;

                    int x_line_dir = (tmp_end->x - tmp_start->x) / (LINE_SIZE - 1);
                    int y_line_dir = (tmp_end->y - tmp_start->y) / (LINE_SIZE - 1);

                    l->start->x = tmp_start->x + x_line_dir;
                    l->start->y = tmp_start->y + y_line_dir;
                }

                if (b->tiles[y][x][1 + y_dir][1 + x_dir] == 1)
                {
                    state++;
                }
            }
        }
        
        if (state == 1)
        {
            dir_counter = 0;
            state++;

            if (abs(x_dir) == 1) x_dir = - x_dir;
            if (abs(y_dir) == 1) y_dir = - y_dir;

            x = vc->c->x;
            y = vc->c->y;
        }
    }

    free_line(l);
}
