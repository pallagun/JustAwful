#ifndef __GT_RANGE2_H__
#define __GT_RANGE2_H__ 1

#include "Constants.h"
#include "Range1.h"
#include "Point2.h"

typedef struct Range2
{
  Range1 x,y;
} Range2;


void Range2_set3(Range2 * range, const Range1 * const xRange, const Range1 * const yRange);
void Range2_set5(Range2 * range, const gtfloat xMin, const gtfloat xMax, 
		                 const gtfloat yMin, const gtfloat yMax);
void Range2_setLargest(Range2 * range);
void Range2_initialize(Range2 * range);
bool Range2_valid(const Range2 * const range);
void Range2_translate(Range2 * range, const gtfloat delx, const gtfloat dely);

bool Range2_almostEqual3(const Range2 * const A, const Range2 * const B, const gtfloat maxDelta);
bool Range2_almostEqual2(const Range2 * const A, const Range2 * const B);

gtfloat Range2_area(const Range2 * const range);
gtfloat Range2_smallestDimension(const Range2 * const range);

void Range2_encompass(Range2 * accumulator, const Range2 * const addition);
void Range2_encompassPoint(Range2 * accumulator, const Point2 * const pt);
  
#endif
