#ifndef __GT_ARC2_H__
#define __GT_ARC2_H__

#include "Constants.h"
#include "Point2.h"
#include "Range2.h"
#include "Angle.h"

#ifndef NDEBUG
#include <stdio.h>
#endif

typedef struct Arc2
{
  Point2 center;
  gtfloat radius;
  Angle angle;
} Arc2;

#ifndef NDEBUG
void     Arc2_debug(const Arc2 * const line);
#endif

/* the last part about < M_PI - not a strict requirement, but in my code it's probably going to be true and if it isn't it should raise an eyebrow */
#define GT_ARC_VALID(A) assert(Arc2_ensureValid(A));
void Arc2_set4(Arc2 * arc, const Point2 * const center, const gtfloat radius, const Angle * const angle);
void Arc2_set6(Arc2 * arc, const Point2 * const center, const gtfloat radius, const gtfloat startRadians, const gtfloat endRadians, const short rotation);
void Arc2_setParametric(Arc2 * arc, const Arc2 * const src, const gtfloat min_t, const gtfloat max_t);

void Arc2_range2(const Arc2 * const arc, Range2 * range);
gtfloat Arc2_length(const Arc2 * const arc);
void Arc2_pointFarthestAlong(const Arc2 * const arc, const Vec2 * const dir, Point2 * output);
gtfloat Arc2_getParametricCoord(const Arc2 * const arc, const Point2 * const pt);
unsigned int Arc2_getApproximationSize(const Arc2 * const arc, const gtfloat maxError);
bool Arc2_getApproximation(const Arc2 * const arc, Point2 * buffer, const unsigned int bufferSize);

void Arc2_startPoint(const Arc2 * const arc, Point2 * start);
void Arc2_endPoint(const Arc2 * const arc, Point2 * end);
void Arc2_parametricPoint(const Arc2 * const arc, const gtfloat coordinate, Point2 * pt);

void Arc2_startDirection(const Arc2 * const arc, Vec2 * dir);
void Arc2_endDirection(const Arc2 * const arc, Vec2 * dir);
void Arc2_parametricDirection(const Arc2 * const arc, const gtfloat coordinate, Vec2 * dir);

void Arc2_startDelDirection(const Arc2 * const arc, Vec2 * dir);
void Arc2_endDelDirection(const Arc2 * const arc, Vec2 * dir);
void Arc2_parametricDelDirection(const Arc2 * const arc, const gtfloat coordinate, Vec2 * delDir);

bool Arc2_almostEqual2(const Arc2 * const A, const Arc2 * const B);
bool Arc2_almostEqualDirectional2(const Arc2 * const A, const Arc2 * const B);
bool Arc2_almostEqualReverseDirectional2(const Arc2 * const A, const Arc2 * const B);

void Arc2_translate(Arc2 * arc, const Vec2 * const delta);
void Arc2_scaleOrigin(Arc2 * arc, const gtfloat alpha);
void Arc2_flip1(Arc2 * arc);
void Arc2_flip2(const Arc2 * const src, Arc2 * dest);
void Arc2_truncateToParametric(Arc2 * arc, const gtfloat min_t, const gtfloat max_t);
bool Arc2_blindExpand(Arc2 * arc, const gtfloat delta);

bool Arc2_ensureValid(const Arc2 * const arc);

#endif

