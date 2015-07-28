#ifndef __GT_COLLISION1_H__
#define __GT_COLLISION1_H__ 1

#include "Constants.h"
#include "Range1.h"
#include "Angle.h"


#define GT_COLLISION1CONT_MAX_PTS 10
#define GT_COLLISION1CONT_MAX_RANGES 10


typedef struct Collision1Container
{
  unsigned int numPts, numRanges;
  gtfloat pts[GT_COLLISION1CONT_MAX_PTS];
  Range1 ranges[GT_COLLISION1CONT_MAX_RANGES];

} Collision1Container;

Collision1Container * Collision1Cont_create();
void Collision1Cont_destroy(Collision1Container * cont);
void Collision1Cont_clear(Collision1Container * cont);
void Collision1Cont_append(Collision1Container * dest, const Collision1Container * const src);

bool Collision1_RF(Collision1Container * cont, const Range1 * const A, const gtfloat B, bool includeEndPoints);
bool Collision1_RR(Collision1Container * cont, const Range1 * const A, const Range1 * const B, bool includeEndPoints);
bool Collision1_AT(Collision1Container * cont, const Angle * const A, const gtfloat theta, bool includeEndPoints);
bool Collision1_AA(Collision1Container * cont, const Angle * const A, const Angle * const B, bool includeEndPoints);

#endif

