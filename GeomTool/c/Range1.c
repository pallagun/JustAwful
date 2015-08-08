#include "include/Range1.h"

#include <assert.h>
#include <stdlib.h>

void Range1_set3(Range1 * range, const gtfloat Min, const gtfloat Max)
{
  assert(range != NULL);
  assert(Min <= Max);
  range->Min = Min;
  range->Max = Max;
}
void Range1_set2(Range1 * range, const Span1 * const span)
{
  assert(range != NULL);
  Range1_coallesce3(range, span->start, span->end);
}
bool Range1_valid(const Range1 * const range)
{
  return range->Min <= range->Max;
}
void Range1_coallesce3(Range1 * range, const gtfloat a, const gtfloat b)
{
  assert(range != NULL);
  if (a < b)
    {
      range->Min = a;
      range->Max = b;
    }
  else
    {
      range->Min = b;
      range->Max = a;
    }
}
void Range1_translate(Range1 * range, const gtfloat delta)
{
  GT_RANGE1P_VALID(range);
  range->Min += delta;
  range->Max += delta;
}
void Range1_encompass(Range1 * accumulator, const Range1 * const addition)
{
  assert(accumulator != NULL);	/* this one can be invalid */
  GT_RANGE1P_VALID(addition);	/* this one can't */

  if (accumulator->Max < addition->Max)
    accumulator->Max = addition->Max;

  if (accumulator->Min > addition->Min)
    accumulator->Min = addition->Min;
}
void Range1_encompassValue(Range1 * accumulator, const gtfloat addition)
{
  assert(accumulator != NULL);
  /* can't assert is valid here, could have been "initialized" */
  /* GT_RANGE1P_VALID(accumulator); */

  if (accumulator->Max < addition)
    accumulator->Max = addition;

  if (accumulator->Min > addition)
    accumulator->Min = addition;
}
gtfloat Range1_mid(const Range1 * const range)
{
  GT_RANGE1P_VALID(range);
  
  return (range->Min + range->Max) / 2;
}
gtfloat Range1_width(const Range1 * const range)
{
  GT_RANGE1P_VALID(range);

  return (range->Max - range->Min);
}
bool Range1_almostEqual3(const Range1 * const A, const Range1 * const B, const gtfloat maxDelta)
{
  GT_RANGE1P_VALID(A);
  GT_RANGE1P_VALID(B);
  assert(maxDelta > 0);
  
  return GT_ALMOST_EQUAL3(A->Min, B->Min, maxDelta) && GT_ALMOST_EQUAL3(A->Max, B->Max, maxDelta);
}
bool Range1_almostEqual2(const Range1 * const A, const Range1 * const B)
{
  GT_RANGE1P_VALID(A);
  GT_RANGE1P_VALID(B);

  return Range1_almostEqual3(A, B, GT_ALMOST_ZERO);
}
