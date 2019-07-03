#include "history.h"

void empty_stack(stack ** s)
{
    while ((* s))
    {
        stack * tmp = * s;
        * s = (* s)->previous;
        free_stack(tmp);
    }
}

void free_stack(stack * s)
{
    if (s)
    {
        free(s->c);
        free_line(s->l);
        free(s);
    }
}

int get_stack_size(stack * undo_list)
{
    int size = 0;

    while (undo_list != NULL)
    {
        undo_list = undo_list->previous;
        size++;
    }
    return size;
}

void play(board * b, coord * c, line * l, stack ** undo_list, stack ** redo_list)
{
    _play(b, c, l, undo_list);
    empty_stack(redo_list);
}

void redo(board * b, stack ** undo_list, stack ** redo_list)
{
    stack * tmp;
    if ((tmp = stack_pop(redo_list))) _play(b, tmp->c, tmp->l, undo_list);
    free_stack(tmp);
}

stack * stack_pop(stack ** s)
{
    if (! (* s)) return NULL;
    stack * tmp = (* s);
    (* s) = (* s)->previous;
    tmp->previous = NULL;
    return tmp;
}

int stack_push(stack ** s, coord * c, line * l)
{
    stack * tmp = (stack *) malloc(sizeof(stack));
    tmp->c = (coord *) malloc(sizeof(coord));
    tmp->l = copy_line(l->start, l->end);
    if (! tmp || ! tmp->c || ! tmp->l) return 1;

    tmp->c->x = c->x;
    tmp->c->y = c->y;
    tmp->previous = * s;
    (* s) = tmp;
    return 0;
}

void undo(board * b, stack ** undo_list, stack ** redo_list)
{
    stack * tmp;
    if ((tmp = stack_pop(undo_list)))
    {
        // TODO: Check if the removed points has other links, it shouldn't though
        b->tiles[tmp->c->y][tmp->c->x][1][1] = 0;
        set_board_line(b, tmp->l, 0);
        stack_push(redo_list, tmp->c, tmp->l);
    }
    free_stack(tmp);
}

void update_stack(stack * s, coord * vector)
{
    while(s)
    {
        s->c->x += vector->x;
        s->c->y += vector->y;
        s->l->start->x += vector->x;
        s->l->start->y += vector->y;
        s->l->end->x += vector->x;
        s->l->end->y += vector->y;
        s = s->previous;
    }
}

void _play(board * b, coord * c, line * l, stack ** undo_list)
{
    b->tiles[c->y][c->x][1][1] = 1;
    set_board_line(b, l, 1);
    stack_push(undo_list, c, l);
}