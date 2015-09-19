#include "include/Collision2.h"
#include "include/Util.h"
/* NOTE: this is my threshold for a collision */
/* #define GT_COLLISION2_THRESHOLD GT_ALMOST_ZERO */
/* GT_COLLISION2_CROSS_PARALLEL_THRESHOLD */

#ifndef MAX
 #define MAX(a,b) ( (a > b) ? (a) : (b) )
#endif

#ifndef MIN
 #define MIN(a,b) ( (a < b) ? (a) : (b) )
#endif

#ifndef ABS
 #define ABS(a) ( (a < 0) ? (-a) : (a) )
#endif

/* and now my adventure into giant macros */
#define GT_COLLISION2CONT_ADD_PARAMS(info, aSeg, bSeg, aParam, bParam, flip) \
  if (flip)								\
    {									\
      info.aSegment = bSeg;						\
      info.bSegment = aSeg;						\
      info.aParametric = bParam;				\
      info.bParametric = aParam;				\
    }									\
  else									\
    {									\
      info.aSegment = aSeg;						\
      info.bSegment = bSeg;						\
      info.aParametric = aParam;				\
      info.bParametric = bParam;				\
    }									


/* Internal_* functions will not clear out the container, ever.  Only append to it. */
/* Collision_* functions will ALWAYS clear out the container. */

/* you can't say " I want two end points" without also saying "I want endpoints" */
#define GT_COLLISION2_VALID_ENDPOINT_INSTRUCTION(includeEndPoints, includeEndToEndPoints) \
  assert( !includeEndToEndPoints || (includeEndToEndPoints && includeEndPoints))

bool Internal_PP(Collision2Container * cont, const Point2 * const A, const Point2 * const B,
		    const bool flip,
		    const Segment2 * const aSeg, const Segment2 * const bSeg,
		    const gtfloat aParametric, const gtfloat bParametric)
{
  assert(A != NULL);
  assert(B != NULL);
  GT_COLLISION2CONT_VALID(cont);
  /* given two points (possibly from two segmetns at two parametric values), determine if they "collide"
     and if so, store that in the container*/
  if (Point2_almostEqual3(A, B, GT_COLLISION2_THRESHOLD))
    {
      /* there was a "collision", get the point */
      Collision2Cont_ptReserve(cont, 1);
      Point2_add3((Point2*)(&(cont->pts[cont->numPts].pt)), A, B);
      Point2_scale2((Point2*)(&(cont->pts[cont->numPts].pt)), 2);
      GT_COLLISION2CONT_ADD_PARAMS(cont->pts[cont->numPts], aSeg, bSeg, aParametric, bParametric, flip);
      ++cont->numPts;
      return true;
    }
  return false;
}
bool Internal_LP(Collision2Container * cont, const Line2 * const A, const Point2 * const B, const bool includeEndPoints,
		    const bool flip,
		    const Segment2 * const aSeg, const Segment2 * const bSeg,
		    const gtfloat bParametric)
{
  Vec2 V, Vp;
  gtfloat parallelDistance, Alength;
  assert(A != NULL);
  assert(B != NULL);
  GT_COLLISION2CONT_VALID(cont);
  Collision2Cont_ptReserve(cont, 1);
 
  Line2_direction(A, &V);	           /* line direction, normalized */
  Point2_sub3(&Vp, B, &(A->start) );       /* C_Point2 Vp = B - A.startpoint(); */
  parallelDistance = Point2_dot(&Vp, &V);  /* cfloat parallelDistance = Vp.dot(V); //dot(Vp,V); */
  Point2_scale2(&V, parallelDistance);	   /* V is now the parallel distance to point B */
  Point2_sub2(&Vp, &V);                    /* Vp is now the orthogonal distance to point B from the line.  Vp = Vp - V*parallelDistance; */

  if (Point2_mag(&Vp) < GT_COLLISION2_THRESHOLD)
    {					   /* point is colinear to line, see if it is in the right place. */
      Alength = Line2_length(A);
      if ( 0 <= parallelDistance && parallelDistance <= Alength ) /* the point exists on this line! */
	{
	  if (includeEndPoints
	      || ( !Point2_almostEqual3( &(A->start), B, GT_COLLISION2_THRESHOLD)
		   && !Point2_almostEqual3( &(A->end  ), B, GT_COLLISION2_THRESHOLD) ) )
	    {			/* you either have to include endpoints or this can't be at an end point */
	      cont->pts[cont->numPts].pt = *B;
	      GT_COLLISION2CONT_ADD_PARAMS(cont->pts[cont->numPts], aSeg, bSeg, parallelDistance/Alength, bParametric, flip);
	      ++cont->numPts;
	      return (true);
	    }
	}
    }
  /* sadly, you didn't find a collision just yet,... but due to rounding issues, I may have missed a 
     collision EXACTLY at the endpoint.  quick check for that. */
  if (includeEndPoints)
    {
      if (Internal_PP(cont, &(A->start), B, flip, aSeg, bSeg, GT_EXACTLY_ZERO, bParametric))
	return(true);
      else if (Internal_PP(cont, &(A->end), B, flip, aSeg, bSeg, GT_EXACTLY_ONE, bParametric))
	return(true);
    }
  
  /* apparently there is no collision :( */
  return(false);
}
bool Internal_AP(Collision2Container * cont, const Arc2 * const A, const Point2 * const B, const bool includeEndPoints,
  		    const bool flip,
		    const Segment2 * const aSeg, const Segment2 * const bSeg,
		    const gtfloat bParametric)
{
  Vec2 displacement;
  Point2 Astart, Aend;
  gtfloat displacementDistance, displacementTheta;

  assert(A != NULL);
  assert(B != NULL);
  GT_COLLISION2CONT_VALID(cont);
  Collision2Cont_ptReserve(cont, 1);
  
  Point2_sub3(&displacement, B, &(A->center));
  displacementDistance = Point2_mag(&displacement);
  
  if (!GT_ALMOST_EQUAL3(displacementDistance, A->radius, GT_COLLISION2_THRESHOLD))
    return (false);  	/* this point is either too far or too close to the center to intersect the arc */

  Arc2_startPoint(A, &Astart);	/* going to need these */
  Arc2_endPoint(A, &Aend);	/* a little later */

  displacementTheta = Point2_theta(&displacement);
  if (Angle_contains( &(A->angle), displacementTheta, true))
    { 				/* this is at the right radius, and it's in the right radial range, it's a hit (probably) */
      if (includeEndPoints
	  || ( !Point2_almostEqual3( &(Astart), B, GT_COLLISION2_THRESHOLD)
	       && !Point2_almostEqual3( &(Aend  ), B, GT_COLLISION2_THRESHOLD) ) )
	{			/* you either have to include endpoints or this can't be at an end point */
	  cont->pts[cont->numPts].pt = *B;
	  GT_COLLISION2CONT_ADD_PARAMS(cont->pts[cont->numPts], aSeg, bSeg, Angle_getParametricCoord( &(A->angle), displacementTheta), bParametric, flip);
	  ++cont->numPts;
	  return (true);
	}
    }

  /* sadly, you didn't find a collision just yet,... but due to rounding issues, I may have missed a 
     collision EXACTLY at the endpoint.  quick check for that. */
  if (includeEndPoints)
    {
      if (Internal_PP(cont, &Astart, B, flip, aSeg, bSeg, GT_EXACTLY_ZERO, bParametric))
	return(true);
      else if (Internal_PP(cont, &Aend, B, flip, aSeg, bSeg, GT_EXACTLY_ONE, bParametric))
	return(true);
    }
  
  /* apparently there is no collision :( */
  return(false);
}
bool Internal_LL_colinear(Collision2Container * cont, const Line2 * const A, const Line2 * const B, 
		    const bool includeEndPoints, const bool includeEndToEndPoints, const bool unbounded,
  		    const bool flip,
		    const Segment2 * const aSeg, const Segment2 * const bSeg)
  
{
  /* Vec2 temp; */
  Point2 segStart, segEnd;
  Span1 bParam, aParam; 	/* the parametric coordinates of the other line in my coordinate system */
  bool sameDirection, isTouching;
  unsigned int endPointCount;
#ifndef NDEBUG
  Vec2 aVec, bVec; 		/* do it without these, use the parametricCoord, it's fst. */
#endif

  GT_COLLISION2_VALID_ENDPOINT_INSTRUCTION(includeEndPoints, includeEndToEndPoints);
  assert(A != NULL);
  assert(B != NULL);
  GT_COLLISION2CONT_VALID(cont);

#ifndef NDEBUG
  /* make sure these lines are actually "parallel" enough */
  Line2_direction(A, &aVec);
  Line2_direction(B, &bVec);
  assert( Point2_cross(&aVec, &bVec) < GT_COLLISION2_CROSS_PARALLEL_THRESHOLD);
#endif

  if (unbounded)
    { 				/* this is an unbounded collision check on two colinear lines.  this will always
				   result in an intersect, and for some reason that intersect is goign to be all
				   of A and B for historical reasons.  Does this even ever happen?*/
      assert(0); 		/* does this even happen? */
      return(true);
    } /* and now I don't have to worry about unbounded anymore */
  
  aParam = Line2_getParametricSpan(A, &(B->start), &(B->end));
  
  assert(aParam.start != aParam.end);

  if (aParam.end >= aParam.start)
    {
      sameDirection = true; 	/* these lines go in the same direction */
    }
  else
    { 
      sameDirection = false;    /* these lines go in different directions */
      Span1_flip(&aParam);
    }
  /* are they touching in any places? */
  isTouching = (aParam.start <= GT_EXACTLY_ONE && aParam.end >= GT_EXACTLY_ZERO);
  aParam.start = MAX(aParam.start, GT_EXACTLY_ZERO);
  aParam.end = MIN(aParam.end, GT_EXACTLY_ONE);
  
  /* is there any touching part here? */
  if (isTouching)
    { 				/* parts are touching, so lets use that part that's touching */
      Collision2Cont_segReserve(cont,1);
      
      Line2_parametricPoint(A, aParam.start, &segStart);
      Line2_parametricPoint(A, aParam.end, &segEnd);
      if (Point2_almostEqual3(&segStart, &segEnd, GT_COLLISION2_THRESHOLD))
	{
	  /* these two points are so close that they're essentially a point. */
	  Point2_add2(&segStart, &segEnd);
	  Point2_scale2(&segStart, 0.5); /* and now this is the actual intersect point */

	  if (!includeEndToEndPoints || !includeEndPoints)
	    {
	      /* you're not allowing two endpoints to collide or end points at all, 
		 I'll have to make sure it's one end point or less */
	      endPointCount = 0; /* count your end points! */
	      
	      if (Point2_almostEqual3(&segStart, &(A->start), GT_COLLISION2_THRESHOLD)
		  || Point2_almostEqual3(&segStart, &(A->end), GT_COLLISION2_THRESHOLD))
		++endPointCount;

	      if (Point2_almostEqual3(&segStart, &(B->start), GT_COLLISION2_THRESHOLD)
		  || Point2_almostEqual3(&segStart, &(B->end), GT_COLLISION2_THRESHOLD))
		++endPointCount;

	      if (!includeEndToEndPoints && endPointCount >= 2)
		return(false); /* you don't allow end point to end point hits, but it appears you have that situation, NO HITS */
	      else if (!includeEndPoints && endPointCount >= 1)
		return(false); /* you don't allow any end point involvement, but it appears you have some, NO HITS */
	    }

	  /* well then this is a point intersect, but it's a safe one, so lets use it. */
	  cont->pts[cont->numPts].pt = segStart;
	  GT_COLLISION2CONT_ADD_PARAMS(cont->pts[cont->numPts], aSeg, bSeg, Span1_mid(&aParam), Line2_getParametricCoord(B, &segStart), flip);
	  ++cont->numPts;
	  return (true);	  
	} /* end collision being a point case */
      else
	{
	  /* and here we have an honest to god segment that is the intersect area. */
	  if (sameDirection)
	    {
	      bParam = Line2_getParametricSpan(B, &(A->start), &(A->end));
	      bParam.start = MAX(bParam.start, GT_EXACTLY_ZERO);
	      bParam.end = MIN(bParam.end, GT_EXACTLY_ONE);
	    }
	  else
	    {
	      bParam = Line2_getParametricSpan(B, &(A->start), &(A->end));
	      bParam.end = MAX(bParam.end, GT_EXACTLY_ZERO);
	      bParam.start = MIN(bParam.start, GT_EXACTLY_ONE);
	    }
	     
	  assert(sameDirection || (bParam.start > bParam.end)); /* if not same direction, b goes backwards */
	  assert(aParam.start < aParam.end);

	  cont->segs[cont->numSegs].iSeg.type = LINE;
	  cont->segs[cont->numSegs].iSeg.s.line.start = segStart;
	  cont->segs[cont->numSegs].iSeg.s.line.end = segEnd;

	  GT_COLLISION2CONT_ADD_PARAMS(cont->segs[cont->numSegs], aSeg, bSeg, aParam, bParam, flip);
	  ++cont->numSegs;
	  return (true);
	}
    } /* end case where lines are actually touching */
  else if (includeEndToEndPoints)
    { 			/* you allow end point to end point hits, due to round off error, I may have missed that, check them explicitly */
      /* Internal_PP(Collision2Container * cont, const Point2 * const A, const Point2 * const B,
	 const bool flip,
	 const Segment2 * const aSeg, const Segment2 * const bSeg,
	 const gtfloat aParametric, const gtfloat bParametric) */
      if (Internal_PP(cont, &(A->start), &(B->start), flip, aSeg, bSeg, GT_EXACTLY_ZERO, GT_EXACTLY_ZERO))
	return (true); 	/* found your collision, get out */
      
      if (Internal_PP(cont, &(A->start), &(B->end), flip, aSeg, bSeg, GT_EXACTLY_ZERO, GT_EXACTLY_ONE))
	return (true); 	/* foudn your collision, get out */
      
      if (Internal_PP(cont, &(A->end), &(B->start), flip, aSeg, bSeg, GT_EXACTLY_ONE, GT_EXACTLY_ZERO))
	return (true); 	/* found your collision, get out */
      
      if (Internal_PP(cont, &(A->end), &(B->end), flip, aSeg, bSeg, GT_EXACTLY_ONE, GT_EXACTLY_ONE))
	return (true); 	/* foudn your collision, get out */
    } /* end endpoint to endpoint force check case */
  
  /* apparently no intersects :( */
  return false;
}
bool Internal_AA_cocircular(Collision2Container * cont, const Arc2 * const A, const Arc2 * const B, 
		    const bool includeEndPoints, const bool includeEndToEndPoints, const bool unbounded,
  		    const bool flip,
		    const Segment2 * const aSeg, const Segment2 * const bSeg)
{
  assert(A != NULL);
  assert(B != NULL);
  GT_COLLISION2_VALID_ENDPOINT_INSTRUCTION(includeEndPoints, includeEndToEndPoints);
  GT_COLLISION2CONT_VALID(cont);
 
  assert(0);
  return (false);
}
bool Internal_LL(Collision2Container * cont, const Line2 * const A, const Line2 * const B, 
		    const bool includeEndPoints, const bool includeEndToEndPoints, const bool unbounded,
  		    const bool flip,
		    const Segment2 * const aSeg, const Segment2 * const bSeg)
{
  gtfloat aLength, bLength, temp, det, aParam, bParam;
  Point2 aCenter, bCenter, aIntersect, bIntersect, rhs, deltaStart;
  Vec2 aDir, bDir, aDir90, aVec, bVec;
  int endPointCount;
  assert(A != NULL);
  assert(B != NULL);
  GT_COLLISION2_VALID_ENDPOINT_INSTRUCTION(includeEndPoints, includeEndToEndPoints);
  GT_COLLISION2CONT_VALID(cont);
  
  assert(!unbounded); /* do I reall need unbounded */

  aLength = Line2_boxLength(A);
  bLength = Line2_boxLength(B);

  if (aLength < GT_COLLISION2_THRESHOLD)
    { 				/* A isn't a line, it's a point  */
      Point2_add3(&aCenter, &(A->start), &(A->end));
      Point2_scale2(&aCenter, 1/2);
      if (bLength < GT_COLLISION2_THRESHOLD)
	{ 			/* B isn't a line, it's a point, they're both points */
	  Point2_add3(&bCenter, &(B->start), &(B->end));
	  Point2_scale2(&bCenter, 1/2);
	  return Internal_PP(cont, &aCenter, &bCenter, flip, aSeg, bSeg, 1/2, 1/2);
	}
      else
	{ 			/* B is a line, but A isn't :( */
	  return Internal_LP(cont, B, &aCenter, includeEndPoints, !flip, aSeg, bSeg, 1/2);
	}
    }
  else if (bLength < GT_COLLISION2_THRESHOLD)
    { 				/* A isn't a point, but B is. */
      Point2_add3(&bCenter, &(B->start), &(B->end));
      Point2_scale2(&bCenter, 1/2);
      return Internal_LP(cont, A, &bCenter, includeEndPoints, flip, aSeg, bSeg, 1/2);
    }
  
  /* both are lines, run line-line intersect */
  /* Collision2Cont_clear(cont);  /\* clear(); *\/ */

  /* grab some handy variables */
  Line2_vector(A, &aVec);
  Line2_vector(B, &bVec);
  aDir = aVec;
  Point2_normalize(&aDir);
  bDir = bVec;
  Point2_normalize(&bDir);
  
  /* TODO: add in a bounding box check here, that will probably speed things up - NOTE: can't actually do this if it is an unbounded intersect, since the bounding boxes are infinite size.. */
  /* TODO: and do the same to any other intersect(Segment,Segment) function (c_Line & C_Arc) */

  temp = Point2_cross(&aDir, &bDir);

  if (-GT_COLLISION2_CROSS_PARALLEL_THRESHOLD < temp && temp < GT_COLLISION2_CROSS_PARALLEL_THRESHOLD)
    { 				/* these two lines are parallel, see if they're colinear */
      Point2_sub3(&deltaStart, &(B->start), &(A->start));
      aDir90 = aDir;
      Point2_rotate90(&aDir90,1);
      
      temp = Point2_dot(&aDir90, &deltaStart);
      if (-GT_COLLISION2_THRESHOLD < temp && temp < GT_COLLISION2_THRESHOLD)
	{ 			/* these are "colinear" lines..   */
	  return Internal_LL_colinear(cont, A, B, includeEndPoints, includeEndToEndPoints, unbounded, flip, aSeg, bSeg);
	}
      else
	{ 			/* lines are parallel but not colinear, no possible intersect ever */
	  return false;
	}      
    } /* end parallel line check */
  else
    {  /* not parallel, so they DO intersect SOMEPLACE, go find it, get the parametric coords of the intersect point */
       /* vector form: */
       /* ta*Vb + A.A = tb*Vb + B.A */
       /* ta and tb are scalars, others are column vects, you end up with */
       /* [Va, -Vb] [ta over tb] = B.A -A.A */
       /* need inverse of [Va, -Vb] */
       /* A = [ a b ]  A^-1 = 1 / (ad - bc) * [  d -b ] */
       /*     [ c d ]                         [ -c  a ] */
       /* where a = Va.x, b = -Vb.x, c = Va.y, d = -Vb.y */
      det = ( aVec.x * (-bVec.y) - (-bVec.x)*aVec.y ); /* cfloat det = (Va.x*-Vb.y - -Vb.x*Va.y); */
      /* assert(det == test);	// TODO: test was a det.  //TODO: why am I doing this twice? -> you aren't, you had to normalize one of them */
      /*TODO: refactor this to use A_Util::solve2 */
      /* C_Point2 rhs = B.startPoint() - A.startPoint(); //(right hand side) */
      Point2_sub3(&rhs, &(B->start), &(A->start));
      /* cfloat ta = ( -Vb.y * rhs.x + Vb.x * rhs.y) / det; */
      aParam = ( -bVec.y*rhs.x + bVec.x*rhs.y ) / det;
      /* cfloat tb = ( -Va.y * rhs.x + Va.x * rhs.y) / det; */
      bParam = ( -aVec.y*rhs.x + aVec.x*rhs.y ) / det;
      
      /* C_Point2 intersectA = A.startPoint() + Va*ta; */
      aIntersect = aVec;
      Point2_scale2(&aIntersect, aParam);
      Point2_add2(&aIntersect, &(A->start));
      /* C_Point2 intersectB = B.startPoint() + Vb*tb; */
      bIntersect = bVec;
      Point2_scale2(&bIntersect, bParam);
      Point2_add2(&bIntersect, &(B->start));

      /* assert (intersectA.insideBoxDistance(intersectB)); */
      assert(Point2_almostEqual2(&bIntersect, &aIntersect));

      /* the actual intersect is going to be average of these two points, floating point error */
      Point2_add2(&aIntersect, &bIntersect);
      Point2_scale2(&aIntersect, 0.5);


      Collision2Cont_ptReserve(cont, 1);
      
      if (unbounded) 		/* if you're unbounded, you're ALWAYS good (when not parallel) */
	{
	  cont->pts[cont->numPts].pt = aIntersect;
	  GT_COLLISION2CONT_ADD_PARAMS(cont->pts[cont->numPts], aSeg, bSeg, aParam, bParam, flip);
	  ++cont->numPts;
	  return(true);
	} /* end unbounded case */
      else if (0 <= aParam && aParam <= 1 
	       && 0 <= bParam && bParam <= 1)   /*  ok, gotta be in range now */
	{					/* awesome,  */
	  if (!includeEndToEndPoints || !includeEndPoints)
	    {		/* you're not allowing two endpoints to collide or end points at all, I'll have to make sure it's one end point or less */
	      endPointCount = 0; /* count your end points! */

	      if (Point2_almostEqual3(&aIntersect, &(A->start), GT_COLLISION2_THRESHOLD)
		  || Point2_almostEqual3(&aIntersect, &(A->end), GT_COLLISION2_THRESHOLD))
		++endPointCount;

	      if (Point2_almostEqual3(&aIntersect, &(B->start), GT_COLLISION2_THRESHOLD)
		  || Point2_almostEqual3(&aIntersect, &(B->end), GT_COLLISION2_THRESHOLD))
		++endPointCount;

	      if (!includeEndToEndPoints && endPointCount >= 2)
		return(false); /* you don't allow end point to end point hits, but it appears you have that situation, NO HITS */
	      else if (!includeEndPoints && endPointCount >= 1)
		return(false); /* you don't allow any end point involvement, but it appears you have some, NO HITS */
	    }

	  /* you made it here, so this is a valid collision */
	  cont->pts[cont->numPts].pt = aIntersect;
	  GT_COLLISION2CONT_ADD_PARAMS(cont->pts[cont->numPts], aSeg, bSeg, aParam, bParam, flip);
	  ++cont->numPts;
	  return(true);
	} /* end parametric collision case */
      else if (includeEndToEndPoints)
	{ 			/* you allow end point to end point hits, due to round off error, I may have missed that, check them explicitly */
	  /* Internal_PP(Collision2Container * cont, const Point2 * const A, const Point2 * const B,
		    const bool flip,
		    const Segment2 * const aSeg, const Segment2 * const bSeg,
		    const gtfloat aParametric, const gtfloat bParametric) */
 	  if (Internal_PP(cont, &(A->start), &(B->start), flip, aSeg, bSeg, GT_EXACTLY_ZERO, GT_EXACTLY_ZERO))
	    return (true); 	/* found your collision, get out */
	  
	  if (Internal_PP(cont, &(A->start), &(B->end), flip, aSeg, bSeg, GT_EXACTLY_ZERO, GT_EXACTLY_ONE))
	    return (true); 	/* foudn your collision, get out */

	  if (Internal_PP(cont, &(A->end), &(B->start), flip, aSeg, bSeg, GT_EXACTLY_ONE, GT_EXACTLY_ZERO))
	    return (true); 	/* found your collision, get out */
	  
	  if (Internal_PP(cont, &(A->end), &(B->end), flip, aSeg, bSeg, GT_EXACTLY_ONE, GT_EXACTLY_ONE))
	    return (true); 	/* foudn your collision, get out */
	} /* end endpoint to endpoint force check case */
    } /* end non-parallel line collision checks */

  /* apparently, you didn't hit anything.. get outta here */
  return (false);
}
bool Internal_LA(Collision2Container * cont, const Line2 * const A, const Arc2 * const B, 
		    const bool includeEndPoints, const bool includeEndToEndPoints, const bool unbounded,
  		    const bool flip,
		    const Segment2 * const aSeg, const Segment2 * const bSeg)
{
  gtfloat aLength, bLength, a, b, c, underSquare, t1, t2, t1b, t2b;
  int endpointCount, collisionCount;
  Point2 aCenter, bCenter, aVec, intersect1, intersect2, bStart, bEnd;
  assert(A != NULL);
  assert(B != NULL);
  GT_COLLISION2_VALID_ENDPOINT_INSTRUCTION(includeEndPoints, includeEndToEndPoints);
  GT_COLLISION2CONT_VALID(cont);
  
  assert(!unbounded); 		/* really, do I use this? */
 
  aLength = Line2_boxLength(A);
  bLength = Arc2_length(B);
  
  if (aLength < GT_COLLISION2_THRESHOLD)
    { 				/* A isn't a line, it's a point  */
      Point2_add3(&aCenter, &(A->start), &(A->end));
      Point2_scale2(&aCenter, 1/2);
      if (bLength < GT_COLLISION2_THRESHOLD)
	{ 			/* B isn't a line, it's a point, they're both points */
	  Arc2_parametricPoint(B, 1/2, &bCenter);
	  return Internal_PP(cont, &aCenter, &bCenter, flip, aSeg, bSeg, 1/2, 1/2);
	}
      else
	{ 			/* B is a line, but A isn't :( */
	  return Internal_AP(cont, B, &aCenter, includeEndPoints, !flip, aSeg, bSeg, 1/2);
	}
    }
  else if (bLength < GT_COLLISION2_THRESHOLD)
    { 				/* A isn't a point, but B is. */
      Arc2_parametricPoint(B, 1/2, &bCenter);
      return Internal_LP(cont, A, &bCenter, includeEndPoints, flip, aSeg, bSeg, 1/2);
    }

  Line2_vector(A, &aVec);  
  Arc2_startPoint(B, &bStart); 	/* these are only conditionally needed, I could probably skip this in some cases? */
  Arc2_endPoint(B, &bEnd);
  
  /* line(t=0..1) = A->A() + vA*t; */
  /* circle = (x-Cx)^2+(y-Cy)^2 = r^2; */
  
  /*  sooooo */
  /* (A.ax + vAx*t - Cx)^2 + (A.ay + vAy*t - Cy)^2 = r^2 */
  /*  in a*t^2 + b*t + c = 0 form it ends up being */
  a = (aVec.x*aVec.x+aVec.y*aVec.y);
  b = 2*( aVec.x*(A->start.x-B->center.x) + aVec.y*(A->start.y-B->center.y) );
  c = (A->start.x-B->center.x)*(A->start.x-B->center.x) + (A->start.y-B->center.y)*(A->start.y-B->center.y) - B->radius*B->radius;
  /* solve for t, party. */
  
  /* standard ///////-b +/- sqrt(b^2-4*a*c) div 2*a */
  Collision2Cont_ptReserve(cont, 2); /* preemptively doing this :( */
  collisionCount = 0;
  underSquare = b*b-4*a*c;
  if (underSquare >= 0)
    {
      underSquare = sqrt(underSquare);
      t1 = (-b - underSquare) / (2*a);
      t2 = (-b + underSquare) / (2*a);

      Point2_scale3(&intersect1, &aVec, t1);
      Point2_add2(&intersect1, &(A->start));
      t1b = Arc2_getParametricCoord(B, &intersect1);
      
      Point2_scale3(&intersect2, &aVec, t2);	    
      Point2_add2(&intersect2, &(A->start));
      t2b = Arc2_getParametricCoord(B, &intersect2);

      if (unbounded)
	{ 			/* add both points as hits and get out! */
	  cont->pts[cont->numPts].pt = intersect1;
	  GT_COLLISION2CONT_ADD_PARAMS(cont->pts[cont->numPts], aSeg, bSeg, t1, t1b, flip);
	  ++cont->numPts;

	  cont->pts[cont->numPts].pt = intersect2;
	  GT_COLLISION2CONT_ADD_PARAMS(cont->pts[cont->numPts], aSeg, bSeg, t2, t2b, flip);
	  ++cont->numPts;
	  
	  return(true);
	}
       
       /* figure out which of these two intersect points is actually valid */
       if (GT_EXACTLY_ZERO <= t1 && t1 <= GT_EXACTLY_ONE 
	   && GT_EXACTLY_ZERO <= t1b && t1b <= GT_EXACTLY_ONE)
	 { 			/* apparently this point is parametrically valid! */
	   
	   endpointCount = 0;
	   if (!includeEndPoints || !includeEndToEndPoints)
	     { 			/* some amount of end points are not valid, make sure you aren't at those points */
	       if (Point2_almostEqual3(&intersect1, &(A->start), GT_COLLISION2_THRESHOLD)
		   || Point2_almostEqual3(&intersect1, &(A->end), GT_COLLISION2_THRESHOLD))
		 ++endpointCount;
		
	       if (Point2_almostEqual3(&intersect1, &(bStart), GT_COLLISION2_THRESHOLD)
		   || Point2_almostEqual3(&intersect1, &(bEnd), GT_COLLISION2_THRESHOLD))
		 ++endpointCount;
	       
	       if (!includeEndToEndPoints && endpointCount >= 2)
		 endpointCount = 3;
	       else if (!includeEndPoints && endpointCount >= 1)
		 endpointCount = 3;
	     }
	   if (endpointCount != 3)
	     {			/* this is a safe intersect */
	       cont->pts[cont->numPts].pt = intersect1;
	       GT_COLLISION2CONT_ADD_PARAMS(cont->pts[cont->numPts], aSeg, bSeg, t1, t1b, flip);
	       ++cont->numPts;
	       ++collisionCount;
	     }
	 } /* end t1/intersect1 intersect */

       if (GT_EXACTLY_ZERO <= t2 && t2 <= GT_EXACTLY_ONE 
	   && GT_EXACTLY_ZERO <= t2b && t2b <= GT_EXACTLY_ONE)
	 { 			/* apparently this point is parametrically valid! */
	   endpointCount = 0;
	   if (!includeEndPoints || !includeEndToEndPoints)
	     { 			/* some amount of end points are not valid, make sure you aren't at those points */
	       if (Point2_almostEqual3(&intersect2, &(A->start), GT_COLLISION2_THRESHOLD)
		   || Point2_almostEqual3(&intersect2, &(A->end), GT_COLLISION2_THRESHOLD))
		 ++endpointCount;
		
	       if (Point2_almostEqual3(&intersect2, &(bStart), GT_COLLISION2_THRESHOLD)
		   || Point2_almostEqual3(&intersect2, &(bEnd), GT_COLLISION2_THRESHOLD))
		 ++endpointCount;
	       
	       if (!includeEndToEndPoints && endpointCount >= 2)
		 endpointCount = 3;
	       else if (!includeEndPoints && endpointCount >= 1)
		 endpointCount = 3;
	     }
	   if (endpointCount != 3)
	     {			/* this is a safe intersect */
	       cont->pts[cont->numPts].pt = intersect2;
	       GT_COLLISION2CONT_ADD_PARAMS(cont->pts[cont->numPts], aSeg, bSeg, t2, t2b, flip);
	       ++cont->numPts;
	       ++collisionCount;
	     }
	 } /* end t2/intersect2 intersect */
    }	   /* end of the undersquare case (non-end point collisions) */

  if (collisionCount < 2 && includeEndToEndPoints)
    { 				/* you aren't full up on collisions, and you are allowing endpoint to end point
				   collisions I'll go check to see fi you have any of those now.*/

      /* start point to start point?? */
      if (Point2_almostEqual3( &(A->start), &bStart, GT_COLLISION2_THRESHOLD) /* is this a point collision? */
	  && (collisionCount == 0 /* can I just add it? or do I have to worry about duplicating a past intersect */
	      || !Point2_almostEqual3( &(A->start), &(cont->pts[cont->numPts-1].pt), GT_COLLISION2_THRESHOLD) ) ) /* make sure this isn't a previous collision */
	{
	  Point2_add3(&intersect2, &(A->start), &bStart);
	  Point2_scale2(&intersect2, 1/2);
	  cont->pts[cont->numPts].pt = intersect2;
	  GT_COLLISION2CONT_ADD_PARAMS(cont->pts[cont->numPts], aSeg, bSeg, GT_EXACTLY_ZERO, GT_EXACTLY_ZERO, flip);
	  ++cont->numPts;
	  
	  if (collisionCount == 1)
	    return (true); 	/* was one, now is 2 intersects, that's the max, get out */
	  else
	    collisionCount = 1;
	}

      /* start point to end point?? */
      if (Point2_almostEqual3( &(A->start), &bEnd, GT_COLLISION2_THRESHOLD) /* is this a point collision? */
	  && (collisionCount == 0 /* can I just add it? or do I have to worry about duplicating a past intersect */
	      || !Point2_almostEqual3( &(A->start), &(cont->pts[cont->numPts-1].pt), GT_COLLISION2_THRESHOLD) ) ) /* make sure this isn't a previous collision */
	{
	  Point2_add3(&intersect2, &(A->start), &bEnd);
	  Point2_scale2(&intersect2, 1/2);
	  cont->pts[cont->numPts].pt = intersect2;
	  GT_COLLISION2CONT_ADD_PARAMS(cont->pts[cont->numPts], aSeg, bSeg, GT_EXACTLY_ZERO, GT_EXACTLY_ONE, flip);
	  ++cont->numPts;
	  
	  if (collisionCount == 1)
	    return (true); 	/* was one, now is 2 intersects, that's the max, get out */
	  else
	    collisionCount = 1;
	}

      /*  end point to start point?? */
      if (Point2_almostEqual3( &(A->end), &bStart, GT_COLLISION2_THRESHOLD) /* is this a point collision? */
	  && (collisionCount == 0 /* can I just add it? or do I have to worry about duplicating a past intersect */
	      || !Point2_almostEqual3( &(A->end), &(cont->pts[cont->numPts-1].pt), GT_COLLISION2_THRESHOLD) ) ) /* make sure this isn't a previous collision */
	{
	  Point2_add3(&intersect2, &(A->end), &bStart);
	  Point2_scale2(&intersect2, 1/2);
	  cont->pts[cont->numPts].pt = intersect2;
	  GT_COLLISION2CONT_ADD_PARAMS(cont->pts[cont->numPts], aSeg, bSeg, GT_EXACTLY_ONE, GT_EXACTLY_ZERO, flip);
	  ++cont->numPts;
	  
	  if (collisionCount == 1)
	    return (true); 	/* was one, now is 2 intersects, that's the max, get out */
	  else
	    collisionCount = 1;
	}

      /* end point to end point?? */
      if (Point2_almostEqual3( &(A->end), &bEnd, GT_COLLISION2_THRESHOLD) /* is this a point collision? */
	  && (collisionCount == 0 /* can I just add it? or do I have to worry about duplicating a past intersect */
	      || !Point2_almostEqual3( &(A->end), &(cont->pts[cont->numPts-1].pt), GT_COLLISION2_THRESHOLD) ) ) /* make sure this isn't a previous collision */
	{
	  Point2_add3(&intersect2, &(A->end), &bEnd);
	  Point2_scale2(&intersect2, 1/2);
	  cont->pts[cont->numPts].pt = intersect2;
	  GT_COLLISION2CONT_ADD_PARAMS(cont->pts[cont->numPts], aSeg, bSeg, GT_EXACTLY_ONE, GT_EXACTLY_ONE, flip);
	  ++cont->numPts;
	  
	  if (collisionCount == 1)
	    return (true); 	/* was one, now is 2 intersects, that's the max, get out */
	  else
	    collisionCount = 1;
	}      
    } /* end additional endpoint to end point checks */
  
  return (collisionCount > 0);
}
bool Internal_AA(Collision2Container * cont, const Arc2 * const A, const Arc2 * const B, 
		    const bool includeEndPoints, const bool includeEndToEndPoints, const bool unbounded,
  		    const bool flip,
		    const Segment2 * const aSeg, const Segment2 * const bSeg)
{
  gtfloat aLength, bLength, centerDistance, d1, h, theta1, thetaA, thetaB;
  Vec2 centerDisplacement, temp, perp;
  Point2 aCenter, bCenter, intersect1, intersect2, aStart, aEnd, bStart, bEnd;
  unsigned int endpointCount, hitCount; 	/* should this be a short? */

  assert(A != NULL);
  assert(B != NULL);
  GT_COLLISION2_VALID_ENDPOINT_INSTRUCTION(includeEndPoints, includeEndToEndPoints);
  GT_COLLISION2CONT_VALID(cont);
  assert(!unbounded);		/* ugh, don't use this */

  aLength = Arc2_length(A);
  bLength = Arc2_length(B);
  
  if (aLength < GT_ALMOST_ZERO)
    { 				/* A isn't a line, it's a point  */
      Arc2_parametricPoint(A, 1/2, &aCenter);
      if (bLength < GT_ALMOST_ZERO)
	{ 			/* B isn't a line, it's a point, they're both points */
	  Arc2_parametricPoint(B, 1/2, &bCenter);
	  return Internal_PP(cont, &aCenter, &bCenter, flip, aSeg, bSeg, 1/2, 1/2);
	}
      else
	{ 			/* B is a line, but A isn't :( */
	  return Internal_AP(cont, B, &aCenter, includeEndPoints, !flip, aSeg, bSeg, 1/2);
	}
    }
  else if (bLength < GT_ALMOST_ZERO)
    { 				/* A isn't a point, but B is. */
      Arc2_parametricPoint(B, 1/2, &bCenter);
      return Internal_AP(cont, A, &bCenter, includeEndPoints, flip, aSeg, bSeg, 1/2);
    }
  
  /* both are lines, run line-line intersect */
  /* Collision2Cont_clear(cont);  /\* clear(); *\/ */

  Point2_sub3(&centerDisplacement, &(B->center), &(A->center));
  centerDistance = Point2_mag(&centerDisplacement);

  /* first things first, make sure you're even within the right distance. */
  if ((centerDistance - A->radius - B->radius > 0) ||	/* too far appart, not touching */
      (centerDistance + A->radius < B->radius) ||	/* A is entirely inside B */
      (centerDistance + B->radius < A->radius))		/* B is entirely inside A */
    {	/* looks like you're too far apart. */
      return(false);
    }

  if (centerDistance < GT_ALMOST_ZERO)
    {
      assert(0);
      /* return Internal_cocircular(cont, A, B, /\* stuff *\/); */
    }

  /* needed for endpoint checks.. note: Do this more efficienetly */
  Arc2_startPoint(A, &aStart);
  Arc2_endPoint(A, &aEnd);
  assert(!Point2_almostEqual3(&aStart, &aEnd, GT_COLLISION2_THRESHOLD));/* this better not be an almost 2PI radians curve */
  Arc2_startPoint(B, &bStart);
  Arc2_endPoint(B, &bEnd);
  assert(!Point2_almostEqual3(&bStart, &bEnd, GT_COLLISION2_THRESHOLD));/* this better not be an almost 2PI radians curve */
  
  /* ok, so not cocircular, so continuing on...  could possibly be touching.  Find the two  */
  /* points where that could happen.  d1 will be the distance along A.center->B.center that the intersects are. */
  d1 = (B->radius*B->radius - A->radius*A->radius - centerDistance*centerDistance) / (-2*centerDistance);
  /* h will be the distance perpendicular to A.center->B.center that the intersect ... is.... on? */
  theta1 = acos(d1/A->radius);
  h = ABS(sin(theta1)*A->radius);
  Point2_normalize(&centerDisplacement);

  Collision2Cont_ptReserve(cont,2);

  if (h < GT_COLLISION2_THRESHOLD/2)
    {	/* single intersect  either displacement or -displacement direction, these circles touch in exactly one point. */
      Point2_scale3(&intersect1, &centerDisplacement, d1);
      Point2_add2(&intersect1, &(A->center)); /* and that's your intersect */

      thetaA = Point2_theta(&centerDisplacement);
      thetaB = thetaA + M_PI; 	/* TODO: this can be done more efficiently I think... */

      if (unbounded 
	  || ( Angle_contains(&(A->angle), thetaA, true)
	       && Angle_contains(&(B->angle), thetaB, true)))
	{ 			/* well then, this is a possible intersect. */
	  endpointCount = 0;
	  
	  if (!unbounded && (!includeEndPoints || !includeEndToEndPoints))
	    { 			/* you're not unbounded, and you don't like one or more endpoints, ensure you're not at an end point */
	      if (Point2_almostEqual3(&intersect1, &aStart, GT_COLLISION2_THRESHOLD) 
		  || Point2_almostEqual3(&intersect1, &aEnd, GT_COLLISION2_THRESHOLD))
		++endpointCount;
	      
	      if (Point2_almostEqual3(&intersect1, &bStart, GT_COLLISION2_THRESHOLD) 
		  || Point2_almostEqual3(&intersect1, &bEnd, GT_COLLISION2_THRESHOLD))
		++endpointCount;
	      
	      if (!includeEndToEndPoints && endpointCount >= 2)
		endpointCount = 3; 	/* no, invalid intersec t */
	      else if (!includeEndPoints && endpointCount >= 1)
		endpointCount = 3;	/* also invalid intersect */
	    }
	  
	  if (endpointCount != 3)
	    { 			/* this is an OK intersect! */
	      cont->pts[cont->numPts].pt = intersect1;
	      GT_COLLISION2CONT_ADD_PARAMS(cont->pts[cont->numPts], aSeg, bSeg, Angle_getParametricCoord(&(A->angle), thetaA), Angle_getParametricCoord(&(B->angle), thetaB), flip);
	      ++cont->numPts;
	      return (true); /* no other intersects are possible after this. */
	    }
	}     /* end single intersect point was valid case. */
      else
	{     /* check end points for hits */
	  hitCount = 0;
	  if (Point2_almostEqual3(&aStart, &bStart, GT_COLLISION2_THRESHOLD))
	    {
	      Point2_add3(&intersect1, &aStart, &bStart);
	      Point2_scale2(&intersect1, 1/2);
	      cont->pts[cont->numPts].pt = intersect1;
	      GT_COLLISION2CONT_ADD_PARAMS(cont->pts[cont->numPts], aSeg, bSeg, GT_EXACTLY_ZERO, GT_EXACTLY_ZERO, flip);
	      ++cont->numPts;
	      ++hitCount;
	    }
	  if (Point2_almostEqual3(&aStart, &bEnd, GT_COLLISION2_THRESHOLD))
	    {
	      Point2_add3(&intersect1, &aStart, &bEnd);
	      Point2_scale2(&intersect1, 1/2);
	      cont->pts[cont->numPts].pt = intersect1;
	      GT_COLLISION2CONT_ADD_PARAMS(cont->pts[cont->numPts], aSeg, bSeg, GT_EXACTLY_ZERO, GT_EXACTLY_ONE, flip);
	      ++cont->numPts;
	      ++hitCount;
	    }
	  if (hitCount < 2 && Point2_almostEqual3(&aEnd, &bStart, GT_COLLISION2_THRESHOLD))
	    {
	      Point2_add3(&intersect1, &aEnd, &bStart);
	      Point2_scale2(&intersect1, 1/2);
	      cont->pts[cont->numPts].pt = intersect1;
	      GT_COLLISION2CONT_ADD_PARAMS(cont->pts[cont->numPts], aSeg, bSeg, GT_EXACTLY_ONE, GT_EXACTLY_ZERO, flip);
	      ++cont->numPts;
	      ++hitCount;
	      
	      if (hitCount < 2 && Point2_almostEqual3(&aEnd, &bEnd, GT_COLLISION2_THRESHOLD))
		{
		  Point2_add3(&intersect1, &aEnd, &bEnd);
		  Point2_scale2(&intersect1, 1/2);
		  cont->pts[cont->numPts].pt = intersect1;
		  GT_COLLISION2CONT_ADD_PARAMS(cont->pts[cont->numPts], aSeg, bSeg, GT_EXACTLY_ONE, GT_EXACTLY_ONE, flip);
		  ++cont->numPts;
		  ++hitCount;
		}
	    }
	  return (hitCount > 0);
	} /* end end-point checks */
      return (false); 	/* only had one chance for a collision and you missed it */
    } /* end single intersect case */
  else
    { 				/* potentially two intersects case */
      /* this is the case where the arcs could intersect at TWO points */
      /* // think Circle(center(1,1),radius(2)) and Circle(center(3,1),radius(2))       */
      hitCount = 0;
      perp = centerDisplacement;
      Point2_rotate90(&perp, 1);
      Point2_scale2(&perp, h);
      
      Point2_scale2(&centerDisplacement, d1);
      Point2_add3(&intersect1, &(A->center), &centerDisplacement);
      intersect2 = intersect1;
      Point2_add2(&intersect2, &perp);
      Point2_invert1(&perp);
      Point2_add2(&intersect1, &perp);

      {				/* check out intersect1 */
	Point2_sub3(&temp, &intersect1, &(A->center));
	thetaA = Point2_theta(&temp);
	Point2_sub3(&temp, &intersect1, &(B->center));
	thetaB = Point2_theta(&temp);
	
	/* ensure this isn't on an end point if it's not supposed to be */
	endpointCount = 0;
	
	if (!unbounded && (!includeEndPoints || !includeEndToEndPoints))
	  { 			/* you're not unbounded, and you don't like one or more endpoints, ensure you're not at an end point */
	    if (Point2_almostEqual3(&intersect1, &aStart, GT_COLLISION2_THRESHOLD) 
		|| Point2_almostEqual3(&intersect1, &aEnd, GT_COLLISION2_THRESHOLD))
	      ++endpointCount;
	    
	    if (Point2_almostEqual3(&intersect1, &bStart, GT_COLLISION2_THRESHOLD) 
		|| Point2_almostEqual3(&intersect1, &bEnd, GT_COLLISION2_THRESHOLD))
	      ++endpointCount;
	    
	    if (!includeEndToEndPoints && endpointCount >= 2)
	      endpointCount = 3; 	/* no, invalid intersec t */
	    else if (!includeEndPoints && endpointCount >= 1)
	      endpointCount = 3;	/* also invalid intersect */
	  }
      
	if (endpointCount != 3 
	    && Angle_contains(&(A->angle), thetaA, true)
	    && Angle_contains(&(B->angle), thetaB, true))
	  { 			/* this is an OK intersect! */
	    cont->pts[cont->numPts].pt = intersect1;
	    /* TODO: it's stupid to call Angle_contains, then call angle_getParametricCoord... */
	    GT_COLLISION2CONT_ADD_PARAMS(cont->pts[cont->numPts], aSeg, bSeg, Angle_getParametricCoord(&(A->angle), thetaA), Angle_getParametricCoord(&(B->angle), thetaB), flip);
	    ++cont->numPts;
	    ++hitCount;
	  }
      }
      
      {				/* check out intersect2 */
	Point2_sub3(&temp, &intersect2, &(A->center));
	thetaA = Point2_theta(&temp);
	Point2_sub3(&temp, &intersect2, &(B->center));
	thetaB = Point2_theta(&temp);
	
	/* ensure this isn't on an end point if it's not supposed to be */
	endpointCount = 0;
	
	if (!unbounded && (!includeEndPoints || !includeEndToEndPoints))
	  { 			/* you're not unbounded, and you don't like one or more endpoints, ensure you're not at an end point */
	    if (Point2_almostEqual3(&intersect2, &aStart, GT_COLLISION2_THRESHOLD) 
		|| Point2_almostEqual3(&intersect2, &aEnd, GT_COLLISION2_THRESHOLD))
	      ++endpointCount;
	    
	    if (Point2_almostEqual3(&intersect2, &bStart, GT_COLLISION2_THRESHOLD) 
		|| Point2_almostEqual3(&intersect2, &bEnd, GT_COLLISION2_THRESHOLD))
	      ++endpointCount;
	    
	    if (!includeEndToEndPoints && endpointCount >= 2)
	      endpointCount = 3; 	/* no, invalid intersec t */
	    else if (!includeEndPoints && endpointCount >= 1)
	      endpointCount = 3;	/* also invalid intersect */
	  }
      
	if (endpointCount != 3 
	    && Angle_contains(&(A->angle), thetaA, true)
	    && Angle_contains(&(B->angle), thetaB, true))
	  { 			/* this is an OK intersect! */
	    cont->pts[cont->numPts].pt = intersect2;
	    /* TODO: it's stupid to call Angle_contains, then call angle_getParametricCoord... */
	    GT_COLLISION2CONT_ADD_PARAMS(cont->pts[cont->numPts], aSeg, bSeg, Angle_getParametricCoord(&(A->angle), thetaA), Angle_getParametricCoord(&(B->angle), thetaB), flip);
	    ++cont->numPts;
	    ++hitCount;
	  }
      }

      if (hitCount != 2 && includeEndToEndPoints)
	{ 			/* technically speaking, I could have endpoint to end point collisions that I didn't detect, I should find those. */
	  endpointCount = 0;
	  if (Point2_almostEqual3(&aStart, &bStart, GT_COLLISION2_THRESHOLD) ||
	      Point2_almostEqual3(&aStart, &bEnd, GT_COLLISION2_THRESHOLD))
	    {
	      endpointCount = 1;
	    }
	  if (Point2_almostEqual3(&aEnd, &bStart, GT_COLLISION2_THRESHOLD) ||
	      Point2_almostEqual3(&aEnd, &bEnd, GT_COLLISION2_THRESHOLD))
	    {
	      ++endpointCount;
	    }
	  if (endpointCount > 0)
	    {
	      assert(1 == 0); 	/* well looks like you really do have some intersects, better fix this code! */
	      hitCount = endpointCount;
	    }
	}
      
      return (hitCount > 0);
    }
}
bool Internal_SLL(Collision2Container * cont, const SegmentList2 * const A, const Line2 * const B,
		     bool includeEndPoints, bool includeEndToEndPoints, const Segment2 * const bSeg)
{
  unsigned int i;
  bool hasIntersect;
  assert(A != NULL);
  assert(B != NULL);
  GT_COLLISION2_VALID_ENDPOINT_INSTRUCTION(includeEndPoints, includeEndToEndPoints);
  GT_COLLISION2CONT_VALID(cont);

  hasIntersect = false;

  for (i = 0; i < SegmentList2_numSegs(A); ++i)
    {
      assert(SegmentList2_seg(A,i).type == LINE || SegmentList2_seg(A,i).type == ARC); /* no other types for now. */
      if (SegmentList2_seg(A,i).type == LINE)
	{
	  hasIntersect |= Internal_LL(cont, &(SegmentList2_seg(A,i).s.line), B, 
				      includeEndPoints, includeEndToEndPoints, 
				      false, false, &(SegmentList2_seg(A,i)), bSeg);
	}
      else
	{			/* also a flip :( */
	  hasIntersect |= Internal_LA(cont, B, &(SegmentList2_seg(A,i).s.arc),
				      includeEndPoints, includeEndToEndPoints,
				      false, true, bSeg, &(SegmentList2_seg(A,i)));
	}
    }
  GT_COLLISION2CONT_VALID(cont);
  return (hasIntersect);
}
bool Internal_SLA(Collision2Container * cont, const SegmentList2 * const A, const Arc2 * const B,
		     bool includeEndPoints, bool includeEndToEndPoints, const Segment2 * const bSeg)
{
  unsigned int i;
  bool hasIntersect;
  assert(A != NULL);
  assert(B != NULL);
  GT_COLLISION2_VALID_ENDPOINT_INSTRUCTION(includeEndPoints, includeEndToEndPoints);
  GT_COLLISION2CONT_VALID(cont);

  hasIntersect = false;

  for (i = 0; i < SegmentList2_numSegs(A); ++i)
    {
      assert(SegmentList2_seg(A,i).type == LINE || SegmentList2_seg(A,i).type == ARC); /* no other types for now. */
      if (SegmentList2_seg(A,i).type == LINE)
	{
	  hasIntersect |= Internal_LA(cont, &(SegmentList2_seg(A,i).s.line), B, 
				      includeEndPoints, includeEndToEndPoints, 
				      false, false, &(SegmentList2_seg(A,i)), bSeg);
	}
      else
	{
	  hasIntersect |= Internal_AA(cont, &(SegmentList2_seg(A,i).s.arc) , B,
				      includeEndPoints, includeEndToEndPoints,
				      false, false, &(SegmentList2_seg(A,i)), bSeg);
	}
    }
  GT_COLLISION2CONT_VALID(cont);
  return (hasIntersect);
}		    


/* outward facing functions */
bool Collision2_AA(Collision2Container * cont, const Arc2 * const A, const Arc2 * const B, bool includeEndPoints, bool includeEndToEndPoints)
{
  assert(A != NULL);
  assert(B != NULL);
  GT_COLLISION2_VALID_ENDPOINT_INSTRUCTION(includeEndPoints, includeEndToEndPoints);
  GT_COLLISION2CONT_VALID(cont);
  
  Collision2Cont_clear(cont); 	/* public function, clear the container before you use this. */
  return Internal_AA(cont, A, B, includeEndPoints, includeEndToEndPoints, false, false, NULL, NULL);
}
bool Collision2_LL(Collision2Container * cont, const Line2 * const A, const Line2 * const B, bool includeEndPoints, bool includeEndToEndPoints)
{
  assert(A != NULL);
  assert(B != NULL);
  GT_COLLISION2_VALID_ENDPOINT_INSTRUCTION(includeEndPoints, includeEndToEndPoints);
  GT_COLLISION2CONT_VALID(cont);
  
  Collision2Cont_clear(cont); 	/* public function, clear the container before you use this. */
  return Internal_LL(cont, A, B, includeEndPoints, includeEndToEndPoints, false, false, NULL, NULL);
}
bool Collision2_LA(Collision2Container * cont, const Line2 * const A, const Arc2 * const B, bool includeEndPoints, bool includeEndToEndPoints)
{
  assert(A != NULL);
  assert(B != NULL);
  GT_COLLISION2_VALID_ENDPOINT_INSTRUCTION(includeEndPoints, includeEndToEndPoints);
  GT_COLLISION2CONT_VALID(cont);

  Collision2Cont_clear(cont); 	/* public function, clear the container before you use this. */
  return Internal_LA(cont, A, B, includeEndPoints, includeEndToEndPoints, false, false, NULL, NULL);
}
bool Collision2_SL(Collision2Container * cont, const Segment2 * const A, const Line2 * const B, bool includeEndPoints, bool includeEndToEndPoints)
{
  assert(A != NULL);
  assert(B != NULL);
  GT_COLLISION2_VALID_ENDPOINT_INSTRUCTION(includeEndPoints, includeEndToEndPoints);
  GT_COLLISION2CONT_VALID(cont);
  assert(A->type == LINE || A->type == ARC);

  Collision2Cont_clear(cont); 	/* public function, clear the container before you use this. */
  
  if (A->type == LINE)
    { /* LINE LINE */
      return Internal_LL(cont, &(A->s.line), B, includeEndPoints, includeEndToEndPoints, false, false, A, NULL);
    }
  else
    { /* ARC LINE -> flip to be LINE ARC */
      return Internal_LA(cont, B, &(A->s.arc) , includeEndPoints, includeEndToEndPoints, false, true , NULL, A); 
    }
}
bool Collision2_SS(Collision2Container * cont, const Segment2 * const A, const Segment2 * const B, bool includeEndPoints, bool includeEndToEndPoints)
{
  assert(A != NULL);
  assert(B != NULL);
  GT_COLLISION2_VALID_ENDPOINT_INSTRUCTION(includeEndPoints, includeEndToEndPoints);
  GT_COLLISION2CONT_VALID(cont);
  assert(A->type == LINE || A->type == ARC);
  assert(B->type == LINE || B->type == ARC); /* no other valid types right now */
  Collision2Cont_clear(cont); 	/* public function, clear the container before you use this. */
  
  if (A->type == LINE)
    {
      if (B->type == LINE) /* LINE LINE */
	return Internal_LL(cont, &(A->s.line), &(B->s.line), includeEndPoints, includeEndToEndPoints, false, false, A, B);
      else                 /* LINE ARC */
	return Internal_LA(cont, &(A->s.line), &(B->s.arc) , includeEndPoints, includeEndToEndPoints, false, false, A, B);
    }
  else
    {
      if (B->type == LINE) /* ARC LINE -> flip to be LINE ARC */
	return Internal_LA(cont, &(B->s.line), &(A->s.arc) , includeEndPoints, includeEndToEndPoints, false, true , B, A); 
      else                 /* ARC ARC */
	return Internal_AA(cont, &(A->s.arc) , &(B->s.arc) , includeEndPoints, includeEndToEndPoints, false, false, A, B);      
    }
}
bool Collision2_SLL(Collision2Container * cont, const SegmentList2 * const A, const Line2 * const B, bool includeEndPoints, bool includeEndToEndPoints)
{
  GT_SEGLIST_VALID1(A);
  assert(B != NULL);

  GT_COLLISION2_VALID_ENDPOINT_INSTRUCTION(includeEndPoints, includeEndToEndPoints);
  GT_COLLISION2CONT_VALID(cont);
  Collision2Cont_clear(cont); 	/* public function, clear the container before you use this. */

  return Internal_SLL(cont, A, B, includeEndPoints, includeEndToEndPoints, NULL);
}
bool Collision2_SLS(Collision2Container * cont, const SegmentList2 * const A, const Segment2 * const B, bool includeEndPoints, bool includeEndToEndPoints)
{
  GT_SEGLIST_VALID1(A);
  assert(B != NULL);
  GT_COLLISION2_VALID_ENDPOINT_INSTRUCTION(includeEndPoints, includeEndToEndPoints);
  GT_COLLISION2CONT_VALID(cont);
  assert(B->type == LINE || B->type == ARC); /* for now */
  Collision2Cont_clear(cont); 	/* public function, clear the container before you use this. */

  if (B->type == LINE)
    {
      return Internal_SLL(cont, A, &(B->s.line), includeEndPoints, includeEndToEndPoints, B);
    }
  else
    {
      return Internal_SLA(cont, A, &(B->s.arc) , includeEndPoints, includeEndToEndPoints, B);
    }
}
bool Collision2_SLSL(Collision2Container * cont, const SegmentList2 * const A, const SegmentList2 * const B, bool includeEndPoints, bool includeEndToEndPoints)
{
  unsigned int i;
  bool hasIntersect;
  GT_SEGLIST_VALID1(A);
  GT_SEGLIST_VALID1(B);
  GT_COLLISION2_VALID_ENDPOINT_INSTRUCTION(includeEndPoints, includeEndToEndPoints);
  GT_COLLISION2CONT_VALID(cont);
  Collision2Cont_clear(cont); 	/* public function, clear the container before you use this. */

  /* for each seg in B, iterate through A on it. */
  hasIntersect = false;
  for (i = 0; i < SegmentList2_numSegs(B); ++i)
    {
      assert(SegmentList2_seg(B,i).type == LINE || SegmentList2_seg(B,i).type == ARC); /* no other types for now. */
      if (SegmentList2_seg(B,i).type == LINE)
	{
	  hasIntersect |= Internal_SLL(cont, A, &(SegmentList2_seg(B,i).s.line), 
				       includeEndPoints, includeEndToEndPoints, &(SegmentList2_seg(B,i)));
	}
      else
	{
	  hasIntersect |= Internal_SLA(cont, A, &(SegmentList2_seg(B,i).s.arc) ,
				       includeEndPoints, includeEndToEndPoints, &(SegmentList2_seg(B,i)));
	}
    }
  return (hasIntersect);
}
		    
