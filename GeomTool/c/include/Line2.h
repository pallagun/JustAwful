
#ifndef __GT_LINE2_H__
#define __GT_LINE2_H__

#include "Constants.h"
#include "Point2.h"
#include "Range2.h"

#ifndef NDEBUG
 #include <stdio.h>
#endif

typedef struct Line2
{
  Point2 start,end;
} Line2;

#ifndef NDEBUG
void     Line2_debug(const Line2 * const line);
#endif

void Line2_set3(Line2 * line, const Point2 * const start, const Point2 * const end);
void Line2_set5(Line2 * line, const gtfloat startX, const gtfloat startY, const gtfloat endX, const gtfloat endY);
void Line2_setParametric(Line2 * line, const Line2 * const source, const gtfloat min_t, const gtfloat max_t);

void Line2_range2(const Line2 * const line, Range2 * range);
void Line2_direction(const Line2 * const line, Vec2 * dir);
void Line2_vector(const Line2 * const line, Vec2 * characteristicVector);
gtfloat Line2_length(const Line2 * const line);
gtfloat Line2_sqLength(const Line2 * const line);
gtfloat Line2_boxLength(const Line2 * const line);
void Line2_pointFarthestAlong(const Line2 * const line, const Vec2 * const dir, Point2 * output);

gtfloat Line2_getParametricCoord(const Line2 * const line, const Point2 * const pt);
Span1 Line2_getParametricSpan(const Line2 * const line, const Point2 * const startPt, const Point2 * const endPt);
void Line2_parametricPoint(const Line2 * const line, const gtfloat coordinate, Point2 * pt);

bool Line2_almostEqual2(const Line2 * const A, const Line2 * const B);
bool Line2_almostEqualDirectional2(const Line2 * const A, const Line2 * const B);
bool Line2_almostEqualReverseDirectional2(const Line2 * const A, const Line2 * const B);

void Line2_translate(Line2 * line, const Vec2 * const delta);
void Line2_scaleOrigin(Line2 * line, const gtfloat alpha);
void Line2_flip1(Line2 * line);
void Line2_flip2(const Line2 * const src, Line2 * dest); /* I think this needs to have argument order flipped */
void Line2_truncateToParametric(Line2 * line, const gtfloat min_t, const gtfloat max_t);
void Line2_blindExpand(Line2 * line, const gtfloat delta, Vec2 * expandDir);





#endif

