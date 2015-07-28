#include "include/Range2.h"

#include "include/Range1.h"
#include <assert.h>
#include <stdlib.h> 		/* for NULL */


void Range2_set3(Range2 * range, const Range1 * const xRange, const Range1 * const yRange)
{
  assert(range != NULL);
  assert(xRange != NULL);
  assert(yRange != NULL);

  range->x = *xRange;
  range->y = *yRange;
}
void Range2_set5(Range2 * range, const gtfloat xMin, const gtfloat xMax,
		                 const gtfloat yMin, const gtfloat yMax)
{
  assert(range != NULL);
  assert(xMin <= xMax);
  assert(yMin <= yMax);
  range->x.Min = xMin;
  range->x.Max = xMax;
  range->y.Min = yMin;
  range->y.Max = yMax;
}
void Range2_setLargest(Range2 * range)
{
  assert(range != NULL);
  range->y.Min = range->x.Min = -1 * (GT_UNREASONABLY_LARGE_VALUE-1);
  range->y.Max = range->x.Max = (GT_UNREASONABLY_LARGE_VALUE-1);
}
void Range2_initialize(Range2 * range)
{
  assert(range != NULL);
  range->x.Min = range->y.Min = GT_UNREASONABLY_LARGE_VALUE;
  range->x.Max = range->y.Max = -GT_UNREASONABLY_LARGE_VALUE;
}
bool  Range2_valid(const Range2 * const range)
{
  assert(range != NULL);
  return Range1_valid(&(range->x)) && Range1_valid(&(range->y));
}
void Range2_translate(Range2 * range, const gtfloat delx, const gtfloat dely)
{
  assert(range != NULL);
  Range1_translate(&(range->x), delx);
  Range1_translate(&(range->y), dely);
}
bool Range2_almostEqual3(const Range2 * const A, const Range2 * const B, const gtfloat maxDelta)
{
  assert(A != NULL);
  assert(B != NULL);
  assert(maxDelta >= 0);
  return Range1_almostEqual3(&(A->x), &(B->x), maxDelta) 
    && Range1_almostEqual3(&(A->y), &(B->y), maxDelta);
}
bool Range2_almostEqual2(const Range2 * const A, const Range2 * const B)
{
  assert(A != NULL);
  assert(B != NULL);
  return Range2_almostEqual3(A,B,GT_ALMOST_ZERO);
}
gtfloat Range2_area(const Range2 * const range)
{
  assert(range != NULL);
  return Range1_width(&(range->x)) * Range1_width(&(range->y));
}
gtfloat Range2_smallestDimension(const Range2 * const range)
{
  gtfloat x,y;
  assert(range != NULL);
  x = Range1_width(&(range->x));
  y = Range1_width(&(range->y));

  return ( (x < y) ? (x) : (y) );
}
void Range2_encompass(Range2 * accumulator, const Range2 * const addition)
{
  assert(accumulator != NULL);
  assert(addition != NULL);
  Range1_encompass(&(accumulator->x), &(addition->x));
  Range1_encompass(&(accumulator->y), &(addition->y));
}
void Range2_encompassPoint(Range2 * accumulator, const Point2 * const pt)
{
  assert(accumulator != NULL);
  assert(pt != NULL);
  Range1_encompassValue(&(accumulator->x), pt->x);
  Range1_encompassValue(&(accumulator->y), pt->y);
}
