/*
 * point.c
 */

#include "point.h"

point add_points(point *p, point *q)
{
    /*
     * Return the point created by adding the coordinates of p and q.
     *
     * param p - Pointer to the first operand
     * param q - Pointer to the second operand
     * return - New point that is the result of the point addition
     */
    point new_point = {p->y + q->y, p->x + q->x};
    return new_point;
}

void set_point(point *p, int y, int x)
{
    /*
     * Set a point's y and x at the same time.
     *
     * param p - Pointer to the point that will be directly modified
     * param y - The new y value for p
     * param x - The new x value for p
     */
    p->y = y;
    p->x = x;
}
