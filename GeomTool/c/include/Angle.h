#ifndef __GT_ANGLE_H__
#define __GT_ANGLE_H__

#include "Constants.h"
#include "Point2.h"

typedef struct Angle
{
  gtfloat start,end;
  short rot; 			/* +1 = CCW, -1 = CW, else invalid ... TODO: this should be a bool/enum?... */
} Angle;

#define GT_ANGLE_VALID(a) assert((a != NULL) && ((a->rot == 1 && a->end > a->start) || (a->rot == -1 && a->end < a->start)) );

gtfloat Angle_getParametricCoord(const Angle * const angle, const gtfloat radians);
gtfloat Angle_getParametricAngle(const Angle * const angle, const gtfloat parametricCoord);
bool Angle_contains(const Angle * const angle, const gtfloat radians, const bool allowEndPoints);
bool Angle_containsA(const Angle * const A, const Angle * const B, const bool allowEndPoints);
gtfloat Angle_span(const Angle * const angle);
gtfloat Angle_width(const Angle * const angl); /* just abs(span) */
bool Angle_intersect2(const Angle * const A, const Angle * const B);
bool Angle_intersect3(const Angle * const A, const Angle * const B, Angle * output);
gtfloat Angle_bisect(const Angle * const A);

gtfloat Angle_dirDisplacement(const gtfloat radiansA, const gtfloat radiansB, const short rotDir);
gtfloat Angle_displacement(const gtfloat radiansA, const gtfloat radiansB);
gtfloat Angle_distance(const gtfloat radiansA, const gtfloat radiansB); /* ABS of _displacement */

void Angle_invert(Angle * angle);
void Angle_complement(const Angle * const A, Angle * output, const bool flipRotation);
void Angle_ensureValid(Angle * angle);
void Angle_setStartFromVec(Angle * angle, const Vec2 * dir);
void Angle_setEndFromVec(Angle * angle, const Vec2 * dir);

bool Angle_almostEqual2(const Angle * const A, const Angle * const B);
bool Angle_almostEqualDirectional2(const Angle * const A, const Angle * const B);
bool Angle_almostEqualReverseDirectional2(const Angle * const A, const Angle * const B);


#endif

