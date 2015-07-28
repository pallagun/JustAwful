#include "include/Collision2.h"

#include <stdlib.h>		/* malloc/free */
#include <string.h> 		/* memcpy */

#ifndef NDEBUG
unsigned long Collision2Cont_create_count;
unsigned long Collision2Cont_destroy_count;
unsigned long Collision2Cont_resize_count;
#endif

/* 
   =====REFERENCE======\
   typedef struct Point2IntersectInfo
   {
   const Segment2 * aSegment;
   const Segment2 * bSegment;
   scgfloat aParametric, bParametric;
   Point2 pt;
   } Point2IntersectInfo;

   typedef struct Segment2IntersectInfo
   {
   const Segment2 * aSegment;
   const Segment2 * bSegment;
   Span1 aParametric, bParametric;
   Segment2 iSeg;
   } Segment2IntersectInfo;

   typedef struct Collision2Container
   {
   unsigned int numSegs, numPts, numRanges;
   Point2IntersectInfo *pts;
   Segment2IntersectInfo *segs;
   Range2 *ranges;
   unsigned int private_ptBuffSize, private_segBuffSize, private_rangeBuffSize;
   } Collision2Container;
*/

Collision2Container * Collision2Cont_create()
{
  Collision2Container * temp;
  temp = malloc(sizeof(Collision2Container));

  /* initialze to empty */
  temp->numPts = temp->numSegs = temp->numRanges = 0;

  /* make space for points */
  temp->pts = malloc(sizeof(Point2IntersectInfo)*GT_COLLISION2CONT_DEFAULT_PTS);
  temp->private_ptBuffSize = GT_COLLISION2CONT_DEFAULT_PTS;

  temp->segs = malloc(sizeof(Segment2IntersectInfo)*GT_COLLISION2CONT_DEFAULT_SEGS);
  temp->private_segBuffSize = GT_COLLISION2CONT_DEFAULT_SEGS;

  temp->ranges = malloc(sizeof(Range2)*GT_COLLISION2CONT_DEFAULT_RANGES);
  temp->private_rangeBuffSize = GT_COLLISION2CONT_DEFAULT_RANGES;

#ifndef NDEBUG
  ++Collision2Cont_create_count;
#endif
  
  GT_COLLISION2CONT_VALID(temp);
  return(temp);
}
void Collision2Cont_destroy(Collision2Container * cont)
{
  GT_COLLISION2CONT_VALID(cont);
  free(cont->pts);
  free(cont->segs);
  free(cont->ranges);
  free(cont);
#ifndef NDEBUG
  ++Collision2Cont_destroy_count;
#endif
}
void Collision2Cont_clear(Collision2Container * cont)
{
  GT_COLLISION2CONT_VALID(cont);
  cont->numPts = 0;
  cont->numRanges = 0;
  cont->numSegs = 0;
}
void Collision2Cont_append(Collision2Container * dest, const Collision2Container * const src)
{
  GT_COLLISION2CONT_VALID(dest);
  GT_COLLISION2CONT_VALID(src);
  
  Collision2Cont_ptReserve(dest, src->numPts);
  Collision2Cont_segReserve(dest, src->numSegs);
  Collision2Cont_rangeReserve(dest,src->numRanges);

  memcpy( &(dest->pts[dest->numPts]), src->pts, src->numPts*sizeof(Point2IntersectInfo));
  dest->numPts += src->numPts;

  memcpy( &(dest->segs[dest->numSegs]), src->segs, src->numSegs*sizeof(Segment2IntersectInfo));
  dest->numSegs += src->numSegs; 

  memcpy( &(dest->pts[dest->numRanges]), src->ranges, src->numRanges*sizeof(Range2));
  dest->numRanges += src->numRanges;
}
void Collision2Cont_ptReserve(Collision2Container * cont, const unsigned int numNewPts)
{
  Point2IntersectInfo * temp;
  unsigned int newSize;
  GT_COLLISION2CONT_VALID(cont);
  if (numNewPts > 0 
      && cont->private_ptBuffSize < cont->numPts + numNewPts)
    {
      newSize = cont->numPts + numNewPts + GT_COLLISION2CONT_DEFAULT_PTS;
      
      temp = realloc(cont->pts, newSize * sizeof(Point2IntersectInfo));
      if (temp == NULL)
	{
	  puts("Collision2Cont_ptReserve(): unable to reserve more point space");
	  exit(1);
	}
      cont->pts = temp;
      cont->private_ptBuffSize = newSize;
    }
}
void Collision2Cont_segReserve(Collision2Container * cont, const unsigned int numNewSegs)
{
  Segment2IntersectInfo * temp;
  unsigned int newSize;
  GT_COLLISION2CONT_VALID(cont);
  if (numNewSegs > 0 
      && cont->private_segBuffSize < cont->numSegs + numNewSegs)
    {
      newSize = cont->numSegs + numNewSegs + GT_COLLISION2CONT_DEFAULT_SEGS;
      
      temp = realloc(cont->segs, newSize * sizeof(Segment2IntersectInfo));
      if (temp == NULL)
	{
	  puts("Collision2Cont_segReserve(): unable to reserve more point space");
	  exit(1);
	}
      cont->segs = temp;
      cont->private_segBuffSize = newSize;
    }
}
void Collision2Cont_rangeReserve(Collision2Container * cont, const unsigned int numNewRanges)
{
  Range2 * temp;
  unsigned int newSize;
  GT_COLLISION2CONT_VALID(cont);
  if (numNewRanges > 0 
      && cont->private_rangeBuffSize < cont->numRanges + numNewRanges)
    {
      newSize = cont->numRanges + numNewRanges + GT_COLLISION2CONT_DEFAULT_RANGES;
      
      temp = realloc(cont->ranges, newSize * sizeof(Range2));
      if (temp == NULL)
	{
	  puts("Collision2Cont_ptReserve(): unable to reserve more point space");
	  exit(1);
	}
      cont->ranges = temp;
      cont->private_rangeBuffSize = newSize;
    }
}
void Collision2Cont_decomposeSegsToPoints(Collision2Container * cont)
{
  /* given a container, pull any line in it into their endpoints and ditch the line */
  unsigned int i;
    
  GT_COLLISION2CONT_VALID(cont);

  if (cont->numSegs > 0)
    {
      /* reserve all the space that I might need */
      Collision2Cont_ptReserve(cont, cont->numSegs * 2);
        
      for (i = 0; i < cont->numSegs; ++i)
	{
	  if (cont->segs[i].iSeg.type == LINE)
	    {
	      /* start */
	      cont->pts[cont->numPts].aSegment = cont->segs[i].aSegment;
	      cont->pts[cont->numPts].bSegment = cont->segs[i].bSegment;
	      cont->pts[cont->numPts].aParametric = cont->segs[i].aParametric.start;
	      cont->pts[cont->numPts].bParametric = cont->segs[i].bParametric.start;
	      cont->pts[cont->numPts].pt = cont->segs[i].iSeg.s.line.start;
	      ++(cont->numPts);
	      /* end */
	      cont->pts[cont->numPts].aSegment = cont->segs[i].aSegment;
	      cont->pts[cont->numPts].bSegment = cont->segs[i].bSegment;
	      cont->pts[cont->numPts].aParametric = cont->segs[i].aParametric.end;
	      cont->pts[cont->numPts].bParametric = cont->segs[i].bParametric.end;
	      cont->pts[cont->numPts].pt = cont->segs[i].iSeg.s.line.end;
	      ++(cont->numPts);      
	    }
	  else
	    {
	      assert(cont->segs[i].iSeg.type == ARC);
	      /* start */
	      cont->pts[cont->numPts].aSegment = cont->segs[i].aSegment;
	      cont->pts[cont->numPts].bSegment = cont->segs[i].bSegment;
	      cont->pts[cont->numPts].aParametric = cont->segs[i].aParametric.start;
	      cont->pts[cont->numPts].bParametric = cont->segs[i].bParametric.start;
	      Arc2_startPoint(&(cont->segs[i].iSeg.s.arc), &(cont->pts[cont->numPts].pt));
	      ++(cont->numPts);
	      /* end */
	      cont->pts[cont->numPts].aSegment = cont->segs[i].aSegment;
	      cont->pts[cont->numPts].bSegment = cont->segs[i].bSegment;
	      cont->pts[cont->numPts].aParametric = cont->segs[i].aParametric.end;
	      cont->pts[cont->numPts].bParametric = cont->segs[i].bParametric.end;
	      Arc2_endPoint(&(cont->segs[i].iSeg.s.arc), &(cont->pts[cont->numPts].pt));
	      ++(cont->numPts);
	    }
	}
      
      cont->numSegs = 0;
    }
}

void Collision2Cont_sortByAParam(Collision2Container * cont)
{
  assert(0);
}
int INTERNAL_compareByBParam(const void * A, const void * B)
{
  if (((Point2IntersectInfo*)A)->bParametric > ((Point2IntersectInfo*)B)->bParametric)
    return 1;
  else if (((Point2IntersectInfo*)A)->bParametric < ((Point2IntersectInfo*)B)->bParametric)
    return -1;
  else
    return 0;
}
void Collision2Cont_sortByBParam(Collision2Container * cont)
{
  assert (cont->numSegs == 0 && cont->numRanges == 0);
  /* sort the points from lowest to highest by bParametric value */
  qsort(cont->pts, cont->numPts, sizeof(Point2IntersectInfo), INTERNAL_compareByBParam);
  
}







