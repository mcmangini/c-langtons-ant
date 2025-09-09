#ifndef POINT_H
#define POINT_H

/*
 * point.h
*/

typedef struct {
    int y, x;
} point;

point add_points(point *p, point *q);
void set_point(point *p, int y, int x);

#endif
