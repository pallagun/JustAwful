#ifndef __GT_SPAN1_H__
#define __GT_SPAN1_H__ 1

#include "Constants.h"

/* struct to represent a set of end points */
/* start < end OR end < start, either way is acceptable. */
typedef struct Span1
{
  gtfloat start,end;
} Span1;


void     Span1_set(Span1 * sp, const gtfloat start, const gtfloat end);
void     Span1_correctOrder(Span1 * sp);
void     Span1_flip(Span1 * sp);

gtfloat Span1_mid(const Span1 * const sp);
gtfloat Span1_width(const Span1 * const sp);



#endif
