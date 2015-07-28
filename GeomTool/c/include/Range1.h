#ifndef __GT_RANGE1_H__
#define __GT_RANGE1_H__ 1

#include <stdbool.h>
#include "Constants.h"
#include "Span1.h"

typedef struct Range1
{
  gtfloat Min,Max;
} Range1;

#define GT_RANGE1P_VALID(RANGE) assert(RANGE != NULL && RANGE->Min <= RANGE->Max);

void Range1_set3(Range1 * range, const gtfloat Min, const gtfloat Max);
void Range1_set2(Range1 * range, const Span1 * const span);
void Range1_coallesce3(Range1 * range, const gtfloat a, const gtfloat b);
void Range1_translate(Range1 * range, const gtfloat delta);

gtfloat Range1_mid(const Range1 * const range);
gtfloat Range1_width(const Range1 * const range);
void Range1_encompass(Range1 * accumulator, const Range1 * const addition);
void Range1_encompassValue(Range1 * accumulator, const gtfloat addition);

bool Range1_almostEqual3(const Range1 * const A, const Range1 * const B, const gtfloat maxDelta);
bool Range1_almostEqual2(const Range1 * const A, const Range1 * const B);
bool Range1_valid(const Range1 * const range);

#endif
