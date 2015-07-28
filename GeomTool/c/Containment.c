#include "include/Containment.h"
#include <math.h>
/* typedef enum AngleIntersectType */
/*   { */
/*     AIT_None = 0x00, */
/*     AIT_intersection = 0x01, */
/*     AIT_A_contains_B = 0x02, */
/*     AIT_B_contains_A = 0x04, */
/*     AIT_Error = 0x08 */
/*   } AIType; */



/* containment tests */
bool Containment_R1F(const Range1 * const A, const gtfloat B, const bool allowContact)
{
  GT_RANGE1P_VALID(A);

  return ( (allowContact && (A->Min <= B && B <= A->Max))
	   || (A->Min < B && B < A->Max));
}
bool Distinct_R1F(const Range1 * const A, const gtfloat B, const bool allowContact)
{
  GT_RANGE1P_VALID(A);

  return ( (allowContact && (B <= A->Min || A->Max <= B))
	   || (B < A->Min || A->Max < B));
}
bool Distinct_R1R1(const Range1 * const A, const Range1 * const B, const bool allowContact)
{
  GT_RANGE1P_VALID(A);
  GT_RANGE1P_VALID(B);
  
  return ((allowContact && (A->Max <= B->Min || B->Max <= A->Min))
	  ||
	  (A->Max <= B->Min || B->Max <= A->Min));
}
bool Containment_R1R1(const Range1 * const A, const Range1 * const B, const bool allowContact)
{
  GT_RANGE1P_VALID(A);
  GT_RANGE1P_VALID(B);
  
  return ( 
	  (allowContact &&
	   (A->Min <= B->Min && B->Max <= A->Max))
	  ||
	  (A->Min < B->Min && B->Max < A->Max));
}
bool Containment_R2P(const Range2 * const A, const Point2 * const B, const bool allowContact)
{
  assert(A != NULL);
  assert(B != NULL);
  return (Containment_R1F(&(A->x), B->x, allowContact)
	  && Containment_R1F(&(A->y), B->y, allowContact));
}
bool Distinct_R2P(const Range2 * const A, const Point2 * const B, const bool allowContact)
{
  assert(A != NULL);
  assert(B != NULL);
  
  return (Distinct_R1F(&(A->x), B->x, allowContact)
	  || Distinct_R1F(&(A->y), B->y, allowContact));
}
bool Distinct_R2R2(const Range2 * const A, const Range2 * const B, const bool allowContact)
{
  assert(A != NULL);
  assert(B != NULL);
  return ( Distinct_R1R1(&(A->x), &(B->x), allowContact)
	   && Distinct_R1R1(&(A->y), &(B->y), allowContact));  
}
bool Containment_R2R2(const Range2 * const A, const Range2 * const B, const bool allowContact)
{
  assert(A != NULL);
  assert(B != NULL);

  return ( Containment_R1R1(&(A->x), &(B->x), allowContact)
	   && Containment_R1R1(&(A->y), &(B->y), allowContact));  
}
int Internal_getWindingNumber(const SegmentList2 * const container, const Line2 * const ray, const Vec2 * dotFilter)
{
  /* grab the winding number of a point inside of a segmentlist (container) */
  /* the ray->start is the point being tested, the ray->end is some point WAY OUTSIDE OF THE container */
  /* the dotFilter is used in dot(crossDir, dotFilter) to add or subtract winding */
  Collision2Container * cont;
  int winding;
  unsigned int i;
  Vec2 testDir;

  winding = 0;
  cont = Collision2Cont_create();
  if (Collision2_SLL(cont, container, ray, true, true) == true)
    {
      /* collisions someplace, testLine1 goes straight up! */
      /* TODO: I'm ignoring the segment hits here, not sure if I should or not */
      for (i = 0; i < cont->numPts; ++i)
	{			/* check the dot(collision->dirA, LEFT) > 0 */
	  Seg2p_parametricDirection(cont->pts[i].aSegment, cont->pts[i].aParametric, &testDir);
	  winding += sign( Point2_dot(&testDir, dotFilter) );
	}
    }
  Collision2Cont_destroy(cont);
  return winding;
}
bool Containment_SLP(const SegmentList2 * const A, const Point2 * const B, const bool allowContact)
{
  SegmentList2Type type;
  Range2 pathBounds;
  Line2 testLine; 	/* TODO: i probably don't need two tests here, this is more for my own insecurity */
  int winding1, winding2;
  Vec2 windingCheck;

  GT_SEGLIST_VALID1(A);
  assert(B != NULL);
  
  /* basic ray casting with a test on the bounds if it exists first */
  type = PATH; 		      /* initialize to something invalid */
  if (SegmentList2_cacheValid(A))
    {				/* you have a valid bound and this point isn't in it, no possible containment */
      SegmentList2_calcType(A, &type);
      if (type == UNBOUNDED)
	return true;
      else if (type == NULL_SET || type == PATH)
	return false;
      else
	{
	  assert(type == HOLE || type == BOUNDED);
	  SegmentList2_calcPathRange(A, &pathBounds);
	  if (type == BOUNDED && !Containment_R2P(&pathBounds, B, allowContact))
	    return false; 	/* not even in the boundeds bounding box */
	  else if (type == HOLE && Distinct_R2P(&pathBounds, B, allowContact))
	    return true;	/* not even near the hole, clears in the hole's coverd region */
	}
    }

  /* resort to ray casting - first one is straight up */
  testLine.start = *B;
  Point2_set3(&(testLine.end), B->x, pathBounds.y.Max + GT_REASONABLE_VALUE); /* straight up */
  Point2_set3(&windingCheck, -1.0, 0.0); /* winding check against left */
  winding1 = Internal_getWindingNumber(A, &testLine, &windingCheck);

  /* second, test going to the right */
  Point2_set3(&(testLine.end), pathBounds.x.Max + GT_REASONABLE_VALUE, B->y); /* right */
  Point2_set3(&windingCheck, 0.0, 1.0); /* winding check against up */
  winding2 = Internal_getWindingNumber(A, &testLine, &windingCheck);

  assert(-1 <= winding1 && winding1 <= 1);
  assert(-1 <= winding2 && winding2 <= 1);
  if (winding1 == winding2)
    {
      /* awesome, accurate reading */
      if (winding1 == 1)
	return true;
      else if (winding1 == -1)
	return false;
      else
	{
	  assert(winding1 == 0); /* no hit, if it's a hole, I'm in it.  If its bounded I'm out of it */
	  if (type == PATH)
	    {
	      /* apparently I didn't get your type before, but I need it now */
	      SegmentList2_calcType(A, &type);
	    }

	  if (type == BOUNDED || type == NULL_SET || type == PATH)
	    return false;
	  else
	    {
	      assert(type == HOLE || type == UNBOUNDED);
	      return true;
	    }
	}
    }
  else
    {
      /* winding numbers are off, something is screwed up, this probably isnt a closed loop
	 so I can't contain anything*/
      return false;
    }
}
bool Distinct_SLP(const SegmentList2 * const A, const Point2 * const B, const bool allowContact)
{
  return (!Containment_SLP(A, B, !allowContact));
}
/* quick enum for where a collision takes place along a segment */
enum HCC_loc {
  start = 0,
  middle,
  end };
void Internal_getIntersectCollisionLocations(const Point2IntersectInfo * const collide, enum HCC_loc * locationA, enum HCC_loc * locationB)
{
  /* find where the intersect is along the segment (segment hit locations locA, locB) */
  /* is this intersect at the beginning of the segments? end? somewhere in the middle?? */
  Point2 tempPoint;
  
  *locationA = middle;
  Seg2p_startPoint(collide->aSegment, &tempPoint);
  if (Point2_almostEqual2(&(collide->pt), &tempPoint))
    {
      *locationA = start;
    }
  else
    {
      Seg2p_endPoint(collide->aSegment, &tempPoint);
      if (Point2_almostEqual2(&(collide->pt), &tempPoint))
	*locationA = end;
    }
  
  *locationB = middle;
  Seg2p_startPoint(collide->bSegment, &tempPoint);
  if (Point2_almostEqual2(&(collide->pt), &tempPoint))
    {
      *locationB = start;
    }
  else
    {
      Seg2p_endPoint(collide->bSegment, &tempPoint);
      if (Point2_almostEqual2(&(collide->pt), &tempPoint))
	*locationB = end;
    }
}
void Internal_getIntersectContainmentAngle(const Segment2 * segment, const gtfloat segmentParametric, const enum HCC_loc location, const SegmentList2 * const origin, Angle * containmentVertex)
{
  /* given a segment from a segmentlist(origin), a parametric point identifiying a vertex on that segment and a HHC_loc classifying that location
   find the contianment angle that describes the "inside" of the shape */
  const Segment2 * next;
  const Segment2 * prev;
  int segIdx;
  Point2 tempVec;
  
  if (location == middle)
    {
      /* setup the containment angle at this vertex, its going to be 180 degrees covering the whole left side of the segment */
      containmentVertex->rot = -1;
      Seg2p_parametricDirection(segment, segmentParametric, &tempVec);
      Angle_setEndFromVec(containmentVertex, &tempVec);
      containmentVertex->start = containmentVertex->end + M_PI;
    }
  else
    {
      if (location == start)
	{
	  next = segment;
	  segIdx = SegmentList2_getIdxFromList(origin, next);
	  assert(segIdx >= 0);
	  segIdx = (segIdx + SegmentList2_numSegs(origin) - 1) % SegmentList2_numSegs(origin);
	  prev = &SegmentList2_seg(origin, segIdx);
	}
      else 		/* END! */
	{
	  assert(location == end);
	  prev = segment;
	  segIdx = SegmentList2_getIdxFromList(origin, prev);
	  assert(segIdx >= 0);
	  segIdx = (segIdx + 1) % SegmentList2_numSegs(origin);
	  next = &SegmentList2_seg(origin, segIdx);
	}
      
      /* setup the containment angle at this vertex */
      /* assert(Point2_almostEqual2(prev->s.line.end,next->s.line.start)); */
      containmentVertex->rot = -1;
      Seg2p_endDirection(prev, &tempVec);
      Point2_invert1(&tempVec);
      Angle_setStartFromVec(containmentVertex, &tempVec);
      Seg2p_startDirection(next, &tempVec);
      Angle_setEndFromVec(containmentVertex, &tempVec);

      if (containmentVertex->start < containmentVertex->end) /* we have a rot of -1 so end must be less than start */
	containmentVertex->end -= 2*M_PI;
    }
}
bool Internal_confirmAdistinctB(const SegmentList2 * const A, const SegmentList2 * const B, Collision2Container * cont)
{
  /* probably going to be the same as internal_confirmedAcontainsB, but use !Angle_intersect instead of Angle_contains */
  /* go through each collision, determine what type of collision it is */
  unsigned int i;
  const Point2IntersectInfo * collide;
  Angle vertA, vertB;
  enum HCC_loc locA, locB;
  
  /* I'm only working with points here, any segment intersects aren't really on any side per se */
  Collision2Cont_decomposeSegsToPoints(cont);
  for (i = 0; i < cont->numPts; ++i)
    {
      /* is this collision on the start, middle or end of the A segment (and same for the B segment) */
      collide = &cont->pts[i];
      
      /* find where the intersect is along the segment (segment hit locations locA, locB) */
      Internal_getIntersectCollisionLocations(collide, &locA, &locB);

      /* now, a quick easy out case */
      /* if (locA == middle && locB == middle) return false; */  /* NO! could be arcs! */

      /* if A requires two segments, gather both segments, if not, model it as two segments anways */
      Internal_getIntersectContainmentAngle(collide->aSegment, collide->aParametric, locA, A, &vertA);
      Internal_getIntersectContainmentAngle(collide->bSegment, collide->bParametric, locB, B, &vertB);
      
      /* ok, angle hit time! */
      if (Angle_intersect2(&vertA, &vertB))
	return false; 	/* these angle overlap somewhere, so the shapes do at this point also - NOT DISTINCT */
    }
  return true;
}
bool Internal_confirmAcontainsB(const SegmentList2 * const A, const SegmentList2 * const B, Collision2Container * cont)
{
  /* go through each collision, determine what type of collision it is */
  unsigned int i;
  Point2IntersectInfo * collide;
  Angle vertA, vertB;
  enum HCC_loc locA, locB;

  /* I'm only working with points here, any segment intersects aren't really on any side per se */
  Collision2Cont_decomposeSegsToPoints(cont);
  for (i = 0; i < cont->numPts; ++i)
    {
      /* is this collision on the start, middle or end of the A segment (and same for the B segment) */
      collide = &cont->pts[i];

      /* find where the intersect is along the segment (segment hit locations locA, locB) */
      Internal_getIntersectCollisionLocations(collide, &locA, &locB);

      /* now, a quick easy out case */
      /* if (locA == middle && locB == middle) return false; */  /* NO! could be arcs! */

      /* if A requires two segments, gather both segments, if not, model it as two segments anways */
      Internal_getIntersectContainmentAngle(collide->aSegment, collide->aParametric, locA, A, &vertA);
      Internal_getIntersectContainmentAngle(collide->bSegment, collide->bParametric, locB, B, &vertB);
      assert( (vertA.rot == 1 && vertA.end > vertA.start) || (vertA.rot == -1 && vertA.end < vertA.start) );
      assert( (vertB.rot == 1 && vertB.end > vertB.start) || (vertB.rot == -1 && vertB.end < vertB.start) );
      /* ok, angle hit time! */
      if (!Angle_containsA(&vertA, &vertB, true))
	return false; 	/* A angle dons NOT contain B, so the shape A does not contain the shape B at this point */      
    }
  return true;
}
bool Containment_SLSL(const SegmentList2 * const A, const SegmentList2 * const B, const bool allowContact)
{
  /* does the shape defined by A contain the shape contained by B? */
  SegmentList2Type aType, bType;
  Range2 aRange, bRange;
  Point2 bInterior;
  Collision2Container * cont;
  bool res;
  
  GT_SEGLIST_VALID1(A);
  GT_SEGLIST_VALID1(B);
  assert(SegmentList2_cacheValid(A));
  assert(SegmentList2_cacheValid(B));
  SegmentList2_calcType(A, &aType);
  SegmentList2_calcType(B, &bType);

  assert (aType == PATH || aType == NULL_SET || aType == BOUNDED || aType == HOLE || aType == UNBOUNDED);
  assert (bType == PATH || bType == NULL_SET || bType == BOUNDED || bType == HOLE || bType == UNBOUNDED);

  /* quick exit cases - remove PATH, NULL_SET and a=UNBOUNDED from the possible scenarios */
  {
    if (aType == PATH || bType == PATH)
      {
	assert(0); 			/* write this case - no idea what the right behavior is here*/
	return false;
      }
    else if (aType == NULL_SET || bType == NULL_SET)
      {
	assert(0); 		/* is this really how I want to do this? maybe it is */
	return false;
      }
    else if (aType == UNBOUNDED)
      {
	return (bType != UNBOUNDED || allowContact);
      }
  }

  /* ok - done with the easy cases -now some early filters that need ranges */
  SegmentList2_calcPathRange(A, &aRange);
  SegmentList2_calcPathRange(B, &bRange);
  
  /* filters based on ranges */
  {
    if (aType == HOLE)
      {
	if (bType == UNBOUNDED)
	  {
	    return false;
	  }
	/* A = hole, B = hole */
	else if (bType == HOLE)
	  {
	    if (!Containment_R2R2(&bRange, &aRange, allowContact)) /* this becomes irritatingly backwards */
	      return false;
	  }
	/* A = hole, B = bounded */
	else
	  {
	    assert(bType == BOUNDED);
	    if (Distinct_R2R2(&aRange, &bRange, allowContact)) /* completely distinct paths - contained */
	      return true;
	  }      
      }
    else if (aType == BOUNDED)
      {
	if (bType == HOLE || bType == UNBOUNDED)
	  {
	    return false;
	  }
	else
	  {
	    assert(bType == BOUNDED);
	    if (!Containment_R2R2(&aRange, &bRange, allowContact)) /* the a range doesn't even contain the b range */
	      return false;
	  }
      }
  }

  /* if you got here, A and B have  to be holes or a boundeds and NOT (A bounde B hole) */
  {
    assert(aType == HOLE || aType == BOUNDED);
    assert(bType == HOLE || bType == BOUNDED);
    assert(!(aType == BOUNDED && bType == HOLE));
  }
  
  /* now check for collisions  */
  cont = Collision2Cont_create();
  if (Collision2_SLSL(cont, A, B, true, true))
    {				/* apparently there are hits - now I need to figure out if the collisions are crossing or not */
      if (!allowContact)
	{
	  res = false;
	}
      else
	{			/* have to evaluate the collisions now */
	  return Internal_confirmAcontainsB(A, B, cont);
	}
    }
  else
    {				/* there are no collisions, so check interior points */
      SegmentList2_calcInteriorPoint(B, &bInterior);
      res = Containment_SLP(A, &bInterior, allowContact);
    }

  Collision2Cont_destroy(cont); /* done with this */
  return res;
}
bool Distinct_SLSL(const SegmentList2 * const A, const SegmentList2 * const B, const bool allowContact)
{
  /* does the shape defined by A contain the shape contained by B? */
  SegmentList2Type aType, bType;
  Range2 aRange, bRange;
  Point2 interiorPoint;
  Collision2Container * cont;
  bool res;
  
  GT_SEGLIST_VALID1(A);
  GT_SEGLIST_VALID1(B);
  assert(SegmentList2_cacheValid(A));
  assert(SegmentList2_cacheValid(B));
  SegmentList2_calcType(A, &aType);
  SegmentList2_calcType(B, &bType);

  assert (aType == PATH || aType == NULL_SET || aType == BOUNDED || aType == HOLE || aType == UNBOUNDED);
  assert (bType == PATH || bType == NULL_SET || bType == BOUNDED || bType == HOLE || bType == UNBOUNDED);

  /* quick exit cases - remove PATH, NULL_SET, UNBOUNDED and double HOLE from the possible scenarios */
  {
    if (aType == PATH || bType == PATH)
      {
	assert(0); 			/* write this case - no idea what the right behavior is here*/
	return false;
      }
    else if (aType == NULL_SET || bType == NULL_SET)
      {
	assert(0); 		/* is this really how I want to do this? maybe it is */
	return false;
      }
    /* only things left are unbounded, hole and bounded */
    else if (aType == UNBOUNDED || bType == UNBOUNDED )
      {
	return false;
      }
    else if (aType == HOLE && bType == HOLE)
      {
	return false; 	/* at infinity, these are going to contact */
      }
  }

  /* ok - done with the easy cases -now some early filters that need ranges */
  SegmentList2_calcPathRange(A, &aRange);
  SegmentList2_calcPathRange(B, &bRange);
  
  /* filters based on ranges, all that remains is BOUNDED/HOLE and BOUNDED/BOUNDED*/
  {
    if (aType == HOLE || bType == HOLE)
      {
	assert (aType == BOUNDED || bType == BOUNDED); /* the other one had better be bounded */
	/* if the holePath range and bounded range are entirely distinct then I know to return false
	   as the bounded exists outside of the HOLE*/
	if (Distinct_R2R2(&aRange, &bRange, allowContact)) /* completely distinct paths - contained */
	  return false;
      }
    else
      {
	assert(aType == BOUNDED && bType == BOUNDED);
	/* double bounded */
	if (Distinct_R2R2(&aRange, &bRange, allowContact))
	  return true;
      }
  }

  /* now check for collisions  */
  cont = Collision2Cont_create();
  if (Collision2_SLSL(cont, A, B, true, true))
    {				/* apparently there are hits - now I need to figure out if the collisions are crossing or not */
      if (!allowContact)
	{
	  res = false;
	}
      else
	{			/* have to evaluate the collisions now */
	  return Internal_confirmAdistinctB(A, B, cont);
	}
    }
  else
    {				/* there are no collisions, so check interior points */
      /* dont calculate an interior point, just use loop point[0] */
      /* if A and B are bounded this will work just fine */
      /* if one is a hole and one is a bounded then the bounded one
	 must contribute the point and the HOLE must be passed to distinct_SLP.   */
      /* A and B both can't be bounded, I checked for that previousl */

      if (aType == BOUNDED && bType == HOLE)
	{			/* swap A and B */
	  Seg2_startPoint(SegmentList2_seg(A,0), &interiorPoint)
	  res = Distinct_SLP(B, &interiorPoint, allowContact);
	}
      else
	{
	  Seg2_startPoint(SegmentList2_seg(B,0), &interiorPoint)
	  res = Distinct_SLP(A, &interiorPoint, allowContact);
	}
    }

  Collision2Cont_destroy(cont); /* done with this */
  return res;
}
bool Containment_RSL(const Region2 * const A, const SegmentList2 * const B, const bool allowContact)
{
  unsigned int i;
  GT_SEGLIST_VALID1(B);
  GT_REGION_VALID(A);

  for (i = 0; i < Region2_numLists(A); ++i)
    {
      if (!Containment_SLSL(Region2_listp(A,i), B, allowContact))
	return false;
    }
  
  return true; 		/* welp, I guess everyone contains you, so you're contained */
}
bool Distinct_RSL(const Region2 * const A, const SegmentList2 * const B, const bool allowContact)
{
  unsigned int i;
  GT_SEGLIST_VALID1(B);
  GT_REGION_VALID(A);

  for (i = 0; i < Region2_numLists(A); ++i)
    {
      if (Distinct_SLSL(Region2_listp(A,i), B, allowContact))
	return true;
    }
  
  return false; 		/* welp, I guess everyone contains you, so you're contained */
}
bool Containment_RR(const Region2 * const A, const Region2 * const B, const bool allowContact)
{
  unsigned int iA, iB;
  bool oneContained;
  GT_REGION_VALID(A);
  GT_REGION_VALID(B);

  /* check shells first */
  if (Containment_SLSL(Region2_listp(A,0), Region2_listp(B,0), allowContact))
    {
      /* now check the holes */
      for (iA = 1; iA < Region2_numLists(A); ++iA)
	{
	  /* there is a hole in the proposed container, make sure  */
	  oneContained = false;
	  for (iB = 1; iB < Region2_numLists(B) && !oneContained; ++iB)
	      oneContained |= Containment_SLSL(Region2_listp(A,iA), Region2_listp(B,iB), allowContact);

	  if (!oneContained)
	    return false;
	}

      return true; 		/* shell contained and all holes contained */
    }
  return false;
}
bool Distinct_RR(const Region2 * const A, const Region2 * const B, const bool allowContact)
{
  unsigned int i;
  GT_REGION_VALID(A);
  GT_REGION_VALID(B);

  for (i = 0; i < Region2_numLists(A); ++i)
    {
      if (Distinct_RSL(B, Region2_listp(A,i), allowContact))
	return true;
    }

  return false;  
}
bool Containment_SSL(const Set2 * const A, const SegmentList2 * const B, const bool allowContact)
{
  unsigned int i;
  GT_SET2_VALID(A);
  GT_SEGLIST_VALID1(B);

  for (i = 0; i < Set2_numRegions(A); ++i)
    {
      if (Containment_RSL(&Set2_region(A,i), B, allowContact))
	return true;
    }
  return false;
}
bool Distinct_SSL(const Set2 * const A, const SegmentList2 * const B, const bool allowContact)
{
  unsigned int i;
  GT_SET2_VALID(A);
  GT_SEGLIST_VALID1(B);
  
  for (i = 0; i < Set2_numRegions(A); ++i)
    {
      if (!Distinct_RSL(&Set2_region(A,i), B, allowContact))
	return false;
    }
  return true;
}
bool Containment_SR(const Set2 * const A, const Region2 * const B, const bool allowContact)
{
  unsigned int i;
  GT_SET2_VALID(A);
  GT_REGION_VALID(B);

  for (i = 0; i < Set2_numSegments(A); ++i)
    {
      if (Containment_RR(&Set2_region(A,i), B, allowContact))
	return true;
    }
  return false;
}
bool Distinct_SR(const Set2 * const A, const Region2 * const B, const bool allowContact)
{
  unsigned int i;
  GT_SET2_VALID(A);
  GT_REGION_VALID(B);

  for (i = 0; i < Set2_numSegments(A); ++i)
    {
      if (!Distinct_RR(&Set2_region(A,i), B, allowContact))
	return false;
    }
  return true;
}
bool Containment_SS(const Set2 * const A, const Set2 * const B, const bool allowContact)
{
  unsigned int i,j;
  bool rContained;
  GT_SET2_VALID(A);
  GT_SET2_VALID(B);

  for (j = 0; j < Set2_numRegions(B); ++j)
    {
      rContained = false;
      for (i = 0; i < Set2_numRegions(A) && !rContained; ++i)
	rContained |= Containment_RR(&Set2_region(A,i), &Set2_region(B,j), allowContact);

      if (!rContained)
	return false;
    }

  return true; 		/* every region(B) was contained by some region(A) */
}
bool Distinct_SS(const Set2 * const A, const Set2 * const B, const bool allowContact)
{
  unsigned int i,j;
  bool rDistinct;
  GT_SET2_VALID(A);
  GT_SET2_VALID(B);

  for (j = 0; j < Set2_numRegions(B); ++j)
    {
      rDistinct = true;
      for (i = 0; i < Set2_numRegions(A) && rDistinct; ++i)
	rDistinct &= Distinct_RR(&Set2_region(A,i), &Set2_region(B,j), allowContact);

      if (!rDistinct)
	return false;
    }

  return true; 		/* every region(B) was distinct from every region(A) */
}



