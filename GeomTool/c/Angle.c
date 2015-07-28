#include "include/Angle.h"

#include <stdlib.h>  /* for NULL */
#include <assert.h>
/* should I be using a fmod here instead of these while loops? */

gtfloat Angle_span(const Angle * const angle)
{
  /* TODO: .. do I really need while loops to do this? really? */
  gtfloat tempEnd;
  GT_ANGLE_VALID(angle);
  assert(angle->rot == 1 || angle->rot == -1);

  tempEnd = angle->end; 
  if (angle->rot >= 0)
    {	/* spin CCW, _radEnd must be > _radStart */
      while (tempEnd < angle->start)
	tempEnd += 2 * M_PI;
    }
  else /* if (_rotDir < 0) */
    {	 /* spin CW, _radEnd must be < _radStart */
      while (tempEnd > angle->start)
	tempEnd -= 2 * M_PI;
    }
  
  return (tempEnd - angle->start);
}
gtfloat Angle_bisect(const Angle * const A)
{
  gtfloat angle;
  angle = Angle_dirDisplacement(A->start, A->end, A->rot)/2 + A->start;
  normalizeZeroTo2Pi(angle);
  return angle;
}
gtfloat Angle_width(const Angle * const angle)
{				/* just the abs(span) */
  gtfloat span = Angle_span(angle);
  
  GT_ANGLE_VALID(angle);
  assert(angle->rot == 1 || angle->rot == -1);
  
  if (span < 0)
    span = -span;

  return(span);
}
gtfloat Angle_getParametricCoord(const Angle * const angle, const gtfloat radians)
{
  /* TODO: figure out how to do this without a while loop? */
  gtfloat temp;
  GT_ANGLE_VALID(angle);
  assert(radians <= 2*M_PI && radians >= -2*M_PI); /* this one might be optimistic... */
  assert(angle->rot == 1 || angle->rot == -1);
  
  temp = radians - angle->start;
  if (angle->rot == 1)
    {  /* ensure temp is between 0 and 2*M_PI */
      while (temp < 0) temp += 2*M_PI;
      while (temp > 2*M_PI) temp -= 2*M_PI;
    }
  else /* if(angle->rot == -1) */
    {  /* ensure temp is between 0 and -2*M_PI */
      while (temp < -2*M_PI) temp += 2*M_PI;
      while (temp > 0) temp -= 2*M_PI;
    }
  
  return (temp / Angle_span(angle));
}
bool Angle_intersect2(const Angle * const A, const Angle * const B)
{
  /* TODO: make this a stripped down version of _intersect3() */
  Angle trash;
  return Angle_intersect3(A, B, &trash);
}
bool Angle_intersect3(const Angle * const A, const Angle * const B, Angle * output)
{
  /* if A and B overlap at all, return that radial range with the rotation of angle A */
  bool bEndInA, bStartInA, hasIntersect;
  
  GT_ANGLE_VALID(A);
  GT_ANGLE_VALID(B);
  assert(output != NULL);

  /* check to see if the B start or B end are inside the A range 
     if so, I can use them as my boundary.  If none of Bs angles are
     in A, then I need to check and see if B encompases A entirely */

  bEndInA = Angle_contains(A, B->start, true);
  bStartInA = Angle_contains(A, B->end, true);
  output->rot = A->rot; 	/* requirement of the function */
  hasIntersect = false; 	/* assume false */

  if (bEndInA && bStartInA)
    {
      /* A contains B entirely - just need to resolve the direction now */
      /* starting from A->start see which you hit first [bstart,bend] by going in A->rot */
      /* | Angle(As,Bs) < Angle(As,Be) | A->rot | use B order | */
      /* |-----------------------------+--------+-------------| */
      /* |                           0 |      1 | no          | */
      /* |                           0 |     -1 | yes         | */
      /* |                           1 |      1 | yes         | */
      /* |                           1 |     -1 | no          | */
      hasIntersect = true;
      if ((Angle_dirDisplacement(A->start, B->start, A->rot)
	   < Angle_dirDisplacement(A->start, B->end, A->rot))
	  == (A->rot == 1))
	{
	  output->start = B->start;
	  output->end = B->end;
	}
      else
	{
	  output->start = B->end;
	  output->end = B->start;
	}
    }
  else if (bEndInA)
    { 				/* only the end of B is in A, use that and A->end */
      hasIntersect = true;
      if (A->rot == B->rot)
	{
	  output->start = A->start;
	  output->end = B->end;
	}
      else
	{
	  output->start = B->end;
	  output->end = A->end;
	}
    }
  else if (bStartInA)
    {				/* only the start of B is in A */
      hasIntersect = true;
      if (A->rot == B->rot)
	{
	  output->start = B->start;
	  output->end = A->end;
	}
      else
	{
	  output->start = A->start;
	  output->end = B->start;
	}
    }
  else
    {
      assert(!bEndInA && !bStartInA);
      /* so , now the only options are A is entirely in B, or not at all
	 so I really only need to test one point */
      if (Angle_contains(B, Angle_bisect(A), true));
      {				/* A is entirely inside A */
	output->start = A->start;
	output->end = A->end;
	hasIntersect = true;
      }
    }
  return hasIntersect;
}
bool Angle_contains(const Angle * const angle, const gtfloat radians, const bool allowEndPoints)
{
  gtfloat temp;
  GT_ANGLE_VALID(angle);
  
  temp = radians;
  
  if (angle->rot == 1)
    {
      while (temp < angle->start) temp += 2*M_PI;
      while (temp > angle->start+2*M_PI) temp -= 2*M_PI;
      
      if (allowEndPoints)
	return (angle->start <= temp && temp <= angle->end);
      else
	return (angle->start < temp && temp < angle->end);
    }
  else /* if(angle->rot == -1) */
    {
      while (temp > angle->start) temp -= 2*M_PI;
      while (temp < angle->start-2*M_PI) temp += 2*M_PI;

      if (allowEndPoints)
	return (angle->start >= temp && temp >= angle->end);
      else
	return (angle->start > temp && temp > angle->end);
    }
}
bool Angle_containsA(const Angle * const A, const Angle * const B, const bool allowEndPoints)
{
  /* does the angle A contain the angle B */
  GT_ANGLE_VALID(A);
  GT_ANGLE_VALID(B);
  Angle cA, cB;

  /* format A and B so they're ready to be compared lineraly */
  cA = *A;
  if (A->rot < 0)
    Angle_invert(&cA);
  /* if (A->rot == 1) */
  /*   cA = *A; */
  /* else */
  /*   Angle_complement(A, &cA, true); */

  assert(! (cA.end < cA.start - (2 * M_PI)) );
  if (cA.end < cA.start)
    cA.end += 2 * M_PI;

  cB = *B;
  if (B->rot < 0)
    Angle_invert(&cB);
    
  /* if (B->rot == 1) */
  /*   cB = *B; */
  /* else */
  /*   Angle_complement(B, &cB, true); */

  assert(! (cB.end < cB.start - (2 * M_PI)) );
  if (cB.end < cB.start)
    cB.end += 2 * M_PI;

  assert(cA.start <= cA.end);
  assert(cB.start <= cB.end);
  
  /* now, I'm either going to sweep B up or down, sliding it past A, looking for a hit */
  if (cB.end < cA.start) 	/* going to slide B up */
    {				/* B is way below A, no possible containment here, rotate up to next stripe */
      cB.start += 2*M_PI;
      cB.end += 2*M_PI;
      while (cB.start < cA.end)	/* while you didn't go past  */
	{
	  if ((cA.start < cB.start && cB.end < cA.end)
	      || (allowEndPoints && (cA.start <= cB.start && cB.end <= cA.end)))
	    return true;	/* contained!!! */

	  cB.start += 2*M_PI;
	  cB.end += 2*M_PI;
	}
    }
  else
    {
      assert (cB.end >= cA.start);
      /* ok, the Bend is above the Astart, rotate it down stripes to check for containment */
      while (cB.end > cA.start)
	{
	  if ((cA.start < cB.start && cB.end < cA.end)
	      || (allowEndPoints && (cA.start <= cB.start && cB.end <= cA.end)))
	    return true;	/* contained!!! */
	  
	  cB.start -= 2*M_PI;
	  cB.end -= 2*M_PI;
	}
    }
  /* well, no containment found.. */
  return false;
}
void Angle_ensureValid(Angle * angle)
{
  /* get the angle in good shape.
     1) make sure the start point is between -2PI and 2PI
     2) if rot == 1 make sure end > start
     3) if rot == -1 make sure end < start */
  assert(angle != NULL);

  while (angle->start > 2*M_PI)
    angle->start -= 2*M_PI;
  while (angle->start <= -2*M_PI)
    angle->start += 2*M_PI;

  if (angle->rot == 1)
    {
      while (angle->end < angle->start)
	angle->end += 2*M_PI;
      while (angle->end > angle->start + 2*M_PI)
	angle->end -= 2*M_PI;
    }
  else
    {
      assert(angle->rot == -1);
      while (angle->end > angle->start)
	angle->end -= 2*M_PI;
      while (angle->end < angle->start - 2*M_PI)
	angle->end += 2*M_PI;
    }
}
void Angle_complement(const Angle * const A, Angle * output, const bool flipRotation)
{
  if (flipRotation)
    {
      output->start = A->start;
      output->end = A->end;
      output->rot = -1 * A->rot;
    }
  else
    {
      output->start = A->end;
      output->end = A->start;
      output->rot = A->rot;
    }
}
void Angle_invert(Angle * angle)
{
  gtfloat temp;
  GT_ANGLE_VALID(angle);
  
  temp = angle->start;
  angle->start = angle->end;
  angle->end = temp;
  angle->rot *= -1;
}
void Angle_setStartFromVec(Angle * angle, const Vec2 * const dir)
{
  angle->start = Point2_theta(dir);
}
void Angle_setEndFromVec(Angle * angle, const Vec2 * const dir)
{
  angle->end = Point2_theta(dir);
}
bool Angle_almostEqual2(const Angle * const A, const Angle * const B)
{
  return Angle_almostEqualDirectional2(A,B) || Angle_almostEqualReverseDirectional2(A,B);
}
bool Angle_almostEqualDirectional2(const Angle * const A, const Angle * const B)
{
  GT_ANGLE_VALID(A);
  GT_ANGLE_VALID(B);

  return ( A->rot == B->rot
	   && GT_ALMOST_EQUAL2(A->start, B->start)
	   && GT_ALMOST_EQUAL2(A->end, B->end));
}
bool Angle_almostEqualReverseDirectional2(const Angle * const A, const Angle * const B)
{
  GT_ANGLE_VALID(A);
  GT_ANGLE_VALID(B);

  return ( A->rot == -B->rot
	   && GT_ALMOST_EQUAL2(A->start, B->end)
	   && GT_ALMOST_EQUAL2(A->end, B->start));
}
gtfloat Angle_dirDisplacement(const gtfloat radiansA, const gtfloat radiansB, const short rotDir)
{
  /* distance (in rads) from A to B but going in the dir of rotDir */
  gtfloat temp;
  assert(rotDir == 1 || rotDir == -1);
  if (rotDir == 1)
    {
      temp = radiansB - radiansA;
      while (temp < 0)
	temp += 2*M_PI;
    }
  else
    {
      assert(rotDir == -1);
      temp = radiansA - radiansB;
      while (temp > 0)
	temp -= 2*M_PI;
    }
  return temp;
}
gtfloat Angle_displacement(const gtfloat radiansA, const gtfloat radiansB)
{
  /* TODO: this possible without a while loop? */
  gtfloat val = radiansA - radiansB;
  while (val < -M_PI)
    val += M_PI;
  while (val > M_PI)
    val -= M_PI;
  
  return(val);
}
gtfloat Angle_distance(const gtfloat radiansA, const gtfloat radiansB)
{
  gtfloat val = Angle_displacement(radiansA, radiansB);
  if (val < 0)
    return -val;
  else
    return val;
}
gtfloat Angle_getParametricAngle(const Angle * const A, const gtfloat parametricCoord)
{
  GT_ANGLE_VALID(A);
  return A->start + parametricCoord * Angle_span(A);
}
