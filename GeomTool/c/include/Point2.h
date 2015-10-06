#ifndef __GT_POINT2_H__
#define __GT_POINT2_H__ 1

#include <math.h>
#include <stdbool.h>
#include <assert.h>

#include "Constants.h"

#ifndef NDEBUG
 #include <stdio.h>
#endif

struct Point2
{
  gtfloat x,y;
};

typedef struct Point2 Point2;
typedef Point2 Vec2; 		/* also this. */


#ifndef NDEBUG
void Point2_debug(const Point2 * const pt);
#endif

void Point2_set3(Point2 * pt, const gtfloat x, const gtfloat y);
void Point2_add3(Point2 * accumulator, const Point2 * const A, const Point2 * const B);
void Point2_add2(Point2 * accumulator, const Point2 * const addition);
void Point2_sub3(Point2 * accumulator, const Point2 * const A, const Point2 * const B);
void Point2_sub2(Point2 * accumulator, const Point2 * const subtraction);
void Point2_scale2(Point2 * pt, const gtfloat scale);
void Point2_scale3(Point2 * pt, const Point2 * const input, const gtfloat scale);
void Point2_invert1(Point2 * pt);
void Point2_invert2(Point2 * output, const Point2 * const input);

gtfloat Point2_theta(const Point2 * const pt);
void Point2_rotate90(Point2 * pt, const int zSign);

gtfloat Point2_cross(const Point2 * const A, const Point2 * const B);
gtfloat Point2_dot(const Point2 * const A, const Point2 * const B);

gtfloat Point2_magSquared(const Point2 * const pt);
gtfloat Point2_mag(const Point2 * const pt);
gtfloat Point2_distance(const Point2 * const A, const Point2 * const B);
void Point2_normalize(Point2 * pt);

bool Point2_almostEqual3(const Point2 * const A, const Point2 * const B, const gtfloat maxDelta);
bool Point2_almostEqual2(const Point2 * const A, const Point2 * const B);


#endif

