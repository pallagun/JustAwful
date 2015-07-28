#include "include/Collision1.h"	/*  */

#include <assert.h>
#include <stdlib.h>  /* NULL */
/* typedef struct Collision1Container */
/* { */
/*   unsigned int numPts, numRanges; */
/*   gtfloat pts[GT_COLLISION1CONT_MAX_PTS]; */
/*   Range1 ranges[GT_COLLISION1CONT_MAX_RANGES]; */

/* } Collision1Container; */
#ifndef MIN
#define MIN(a,b) ( (a < b) ? (a) : (b) )
#endif
#ifndef MAX
#define MAX(a,b) ( (a > b) ? (a) : (b) )
#endif


bool Collision1_RF(Collision1Container * cont, const Range1 * const A, const gtfloat B, bool includeEndPoints)
{
  assert(cont != NULL);
  assert(A != NULL);
  Collision1Cont_clear(cont);
  if ( (includeEndPoints && (A->Min <= B && B <= A->Max))
       || (!includeEndPoints && (A->Min < B && B < A->Max)) )
    {
      cont->pts[0] = B;
      cont->numPts = 1;
      return true;
    }
  return false;
}
bool Collision1_RR(Collision1Container * cont, const Range1 * const A, const Range1 * const B, bool includeEndPoints)
{
  gtfloat min,max;
  assert(cont != NULL);
  assert(A != NULL);
  assert(B != NULL);
  Collision1Cont_clear(cont);
  min = MAX(A->Min, B->Min);
  max = MIN(A->Max, B->Max);
  
  if ( (includeEndPoints && min <= max)
       || (!includeEndPoints && min < max) )
  {
    cont->ranges[0].Min = min;
    cont->ranges[0].Max = max;
    cont->numRanges = 1;
    return true;
  }
  return false;
}
bool Collision1_AT(Collision1Container * cont, const Angle * const A, const gtfloat theta, bool includeEndPoints)
{
  assert(cont != NULL);
  assert(A != NULL);
  Collision1Cont_clear(cont);
  if ( Angle_contains(A, theta, includeEndPoints) )
    {
      cont->pts[0] = theta;
      cont->numPts = 1;
      return true;
    }
  return false;
}
bool Collision1_AA(Collision1Container * cont, const Angle * const A, const Angle * const B, bool includeEndPoints)
{
  gtfloat Astart,Aend,Bstart,Bend;
  gtfloat fStart,fEnd;
  bool retVal;

  assert(cont != NULL);
  assert(A != NULL);
  assert(B != NULL);
  Collision1Cont_clear(cont);
  /* TODO: this should probably be able to happen without a zillion while loops... */
  /* untested code from old C_Collision1 here. */
  Astart = A->start, Aend = A->end, Bstart = B->start, Bend = B->end;
  
  while (Aend < Astart)
    Aend += 2*M_PI;
  
  while (Aend <  Astart)
    Aend += 2*M_PI;
  while (Aend > Astart + 2*M_PI)
    Aend -= 2*M_PI;
  
  while (Bstart > Astart + 2*M_PI)
    Bstart -=2*M_PI;
  while (Bstart < Astart)
    Bstart += 2*M_PI;
  
  while (Bend <  Bstart)
    Bend += 2*M_PI;
  while (Bend > Bstart + 2*M_PI)
    Bend -= 2*M_PI;
  
  
  fStart = MAX(Astart,Bstart);
  fEnd = MIN(Aend,Bend);
  
  retVal = false;
  if (fStart < fEnd)
    {
      cont->ranges[0].Min = fStart;
      cont->ranges[0].Max = fEnd;
      cont->numRanges = 1;
      retVal = true;
    }
  else if (includeEndPoints && fStart == fEnd) /* really friggin unlikely... */
    {
      cont->pts[0] = fStart;
      cont->numPts = 1;
      retVal = true;
    }
  
  Astart += 2*M_PI;
  Aend += 2*M_PI;
  fStart = MAX(Astart,Bstart);
  fEnd = MIN(Aend,Bend);
  
  
  if (fStart < fEnd)
    {
      cont->ranges[cont->numRanges].Min = fStart;
      cont->ranges[cont->numRanges].Max = fEnd;
      ++(cont->numRanges);
      retVal = true;
    }
  else if (includeEndPoints && fStart == fEnd)
    {
      cont->pts[cont->numPts] = fStart;
      ++(cont->numPts);
      retVal = true;
    }
  return(retVal);
}
