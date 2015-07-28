#include "include/Collision1.h"

#include <assert.h>  /*  */
#include <stdlib.h>  /* malloc/free */
#include <string.h>  /* memcpy */

Collision1Container * Collision1Cont_create()
{
  Collision1Container * temp;
  temp = malloc(sizeof(Collision1Container));
  return(temp);
}
void Collision1Cont_destroy(Collision1Container * cont)
{
  assert(cont != NULL);
  free(cont);
}
void Collision1Cont_clear(Collision1Container * cont)
{
  assert(cont != NULL);
  cont->numPts = 0;
  cont->numRanges = 0;  
}
void Collision1Cont_append(Collision1Container * dest, const Collision1Container * const src)
{
  assert(src != NULL);
  assert(dest != NULL);

  assert(dest->numPts + src->numPts <= GT_COLLISION1CONT_MAX_PTS);
  assert(dest->numRanges + src->numRanges <= GT_COLLISION1CONT_MAX_RANGES);

  memcpy( &(dest->pts[dest->numPts]), src->pts, src->numPts*sizeof(gtfloat));
  dest->numPts += src->numPts;

  memcpy( &(dest->pts[dest->numRanges]), src->pts, src->numRanges*sizeof(Range1));
  dest->numRanges += src->numRanges;
}
