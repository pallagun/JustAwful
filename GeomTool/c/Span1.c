#include "include/Span1.h"

#include <stdlib.h> 		/* to get NULL */
#include <assert.h>

void Span1_set(Span1 * sp, const gtfloat start, const gtfloat end)
{
  assert(sp != NULL);
  sp->start = start;
  sp->end = end;
}
void Span1_correctOrder(Span1 * sp)
{
  assert(sp != NULL);

  if (sp->start > sp->end)
    Span1_flip(sp);
}
void Span1_flip(Span1 * sp)
{
  gtfloat temp;

  assert(sp != NULL);
  temp = sp->start;
  sp->start = sp->end;
  sp->end = temp;  
}
/* TODO: should probably be a #define? */
gtfloat Span1_mid(const Span1 * const sp)
{
  assert(sp != NULL);
  return ( (sp->start + sp->end) / 2 );
}
/* TODO: should probably be a #define? */
gtfloat Span1_width(const Span1 * const sp)
{
  assert(sp != NULL);
  return ( (sp->end - sp->start) );
}
