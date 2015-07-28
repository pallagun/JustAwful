#ifndef __GT_RANGE1SET_H__
#define __GT_RANGE1SET_H__ 1

#include "Range1.h"

/* a set of ranges that DONT overlap and are in order */

#define GT_RANGE1SET_DEFAULT_SIZE 4

#define GT_RANGE1SET_VALID(SET) assert(SET != NULL && SET->range != NULL && SET->private_allocRanges > 0)

typedef struct Range1Set
{
  unsigned int numRanges;
  Range1 * range;
  unsigned int private_allocRanges;
} Range1Set;

Range1Set * Range1Set_create();
void        Range1Set_destroy(Range1Set * set);
void        Range1Set_clear(Range1Set * set);
void        Range1Set_add(Range1Set * set, const Range1 * const add);
void        Range1Set_subtract(Range1Set * set, const Range1 * const sub);

#ifndef NDEBUG
bool     Range1Set_valid(const Range1Set * const set);
#endif

#endif

