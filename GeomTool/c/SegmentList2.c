#include "include/SegmentList2.h"

#include <stdlib.h> 		/* malloc, free */
#include <string.h>		/* memcpy */
#include <assert.h> 		/* assert */
#include "include/Collision2.h" /* collision2continaer */
#include "include/Range1Set.h"
#include "include/Containment.h" 

#define GT_SEGLIST_DO_EOMFAIL { puts("EOM\n"); exit(1); }
#define GT_SEGLIST_DO_TERMINAL { puts("SEGLIST ERR\n"); exit(1); }
#define seg(LIST, IDX) SegmentList2_seg((LIST), (IDX))
#define segp(LIST, IDX) ((Segment2*)&seg((LIST), (IDX)))
#define numSegs(LIST) SegmentList2_numSegs((LIST))
#define lastSeg(LIST) SegmentList2_lastSeg(LIST)
#define firstSeg(LIST) seg((LIST), 0)

/* private functions to this file */
bool Internal_calcSpin3(const SegmentList2 * const segs, const Range2 * const trustedRange, short * spin);
void Internal_refreshCacheAfterAppend(SegmentList2 * list);
int Internal_expandSeg(const Segment2 * const src, const gtfloat delta, Segment2 * dest);
bool Internal_calcContinuousClosed(const SegmentList2 * const segs, bool * continuous, bool * closed);
bool Internal_calcZeroArea(const SegmentList2 * const segs, bool * isZeroArea);
bool Internal_calcSpin2(const SegmentList2 * const segs, short * spin);
bool Internal_calcPathRange2(const SegmentList2 * const segs, Range2 * bounds);
bool Internal_calcInteriorPoint(const SegmentList2 * const segs, Point2 * pt);
bool Internal_calcType(const SegmentList2 * const segs, SegmentList2Type * type);
void Internal_invalidateCache(SegmentList2 * list);



void Internal_refreshCacheAfterAppend(SegmentList2 * list)
{
  /* this is the function that is called after you append a segment
     onto the end of a segmentlist and you want to keep the cache 
     updated. -- thanks capt. obvious*/
  Range2 segRange;  
  Point2 testStart, testEnd;
  bool check;
  /* short pastSpin; */

  assert(list->segments.numItems >= 1); /* should be called after an append, so you must have at least one */

  /*  was the cache already valid? */
  if (!list->private_cacheValid)
    {				/* cache wasn't valid, just recache all the parameters */
      SegmentList2_refreshCache(list);
    }
  else /* if (list->private_cacheValid) -- cache WAS valid last time, attempt to update it.*/
    {  
      /* is it really valid? */
      assert((list->private_isClosed && list->private_isContinuous) || (!list->private_isClosed));
      
      /* wipe the spin */
      /* pastSpin = list->private_spin; */

      /* bounds */
/* #define        SegmentList2_numSegs(LIST) ((LIST)->segments.numItems) */
/* #define        SegmentList2_seg(LIST, IDX) ((Segment2 *)((LIST)->segments.item + (IDX) * sizeof(Segment2))) */
      
/*       Seg2_range2( list->segments.item  */

      Seg2_range2(seg(list, numSegs(list)-1), &segRange);
      /* Seg2_range2(list->segments[list->segments.numItems-1], &segRange); */
      Range2_encompass(&(list->private_pathBounds), &segRange);

      if (list->segments.numItems <= 1)
	{			/* can't possibly be closed, but IS continuous */
	  list->private_isContinuous = true;
	  list->private_isClosed = false;
	  list->private_isZeroArea = true;
	  list->private_type = PATH;
	  list->private_interiorPointValid = false;
	  list->private_spin = 0; /* set spin to be invalid  */
	}
      else if (!list->private_isContinuous) 
	{			/* it wasn't continuous last time, so it sure as 
				   heck isn't going to be continuous now.*/
	  list->private_isClosed = false;
	  list->private_isZeroArea = true;
	  list->private_type = PATH;
	  list->private_interiorPointValid = false;
	  list->private_spin = 0; /* set spin to be invalid  */
	}
      else if (list->private_isClosed)
	{ 			/* it WAS continuous last time, and it WAS closed 
				   last time, if you're appending something to this,
				   it's not going to be closed now, that's for sure */
	  list->private_isClosed = false;
	  list->private_isZeroArea = true;
	  list->private_type = PATH;
	  list->private_interiorPointValid = false;
	  list->private_spin = 0; /* set spin to be invalid  */
	}
      else
	{			/* cache was valid, it has segments, it was continuous
				   and it wasn't closed, it could possibly be closed now*/
	  Seg2_endPoint(seg(list, numSegs(list)-2), &testEnd);
	  Seg2_startPoint(seg(list, numSegs(list)-1), &testStart);
	  list->private_isContinuous &= Point2_almostEqual2(&testStart, &testEnd);

	  if (list->private_isContinuous)
	    {		/* it's continuous, check for closed */
	      Seg2_endPoint(seg(list, numSegs(list)-1), &testEnd);
	      Seg2_startPoint(seg(list, 0), &testStart);
	      list->private_isClosed &= Point2_almostEqual2(&testStart, &testEnd);

	      if (list->private_isClosed)
		{ 		/* calculate the spin */
		  check = Internal_calcSpin3(list, &(list->private_pathBounds), &(list->private_spin));
		  assert(check);
		  assert(list->private_spin == 1 || list->private_spin == -1);

		  if (list->private_spin == 1)
		    list->private_type = BOUNDED;
		  else if (list->private_spin == -1)
		    list->private_type = HOLE;
		  else
		    list->private_type = PATH;
		  /* check for zero area. */
		  check = Internal_calcZeroArea(list, &(list->private_isZeroArea));
		  assert(check);
		  assert(list->private_isZeroArea == true || list->private_isZeroArea == false);
		}
	      else
		{		/* not closed */
		  list->private_isZeroArea = false;
		  list->private_type = PATH;
		}
	    }
	  else
	    {			/* not continuous ! */
	      list->private_isClosed = false;
	      list->private_isZeroArea = true;
	      list->private_type = PATH;	      
	    }
	}
      list->private_cacheValid = true;
    }
  assert(list->private_cacheValid);
  GT_SEGLIST_VALID1(list);
}
int Internal_expandSeg(const Segment2 * const src, const gtfloat delta, Segment2 * dest)
{
  /* returns false if the expansion will result in a flip (can only happen to arcs) */
  Vec2 dir;
  assert(delta != 0);
  assert(src != NULL);
  assert(dest != NULL);

  dest->type = src->type;
  if (src->type == LINE)
    {
      dest->s.line = src->s.line;
      Line2_direction(&(src->s.line), &dir);
      Point2_rotate90(&dir, -1);
      Line2_translate(&(dest->s.line), &dir);
      return (true);
    }
  else
    {
      assert(src->type == ARC);
      dest->s.arc = src->s.arc;
      assert(dest->s.arc.angle.rot != 0);
      if (dest->s.arc.angle.rot > 0)
	dest->s.arc.radius += delta; /* "expanding" arc */
      else
	  dest->s.arc.radius -= delta; /* contracting arc */

      if (dest->s.arc.radius <= 0)
	return (false); /* this expansion will result in a point or inversion */
      
      return (true);
    }
}
#ifndef NDEBUG
bool SegmentList2_db_validateCache(const SegmentList2 * const list)
{
  SegmentList2 * temp;
  bool result = true;
  if (list->private_cacheValid)
    {
      
      temp = SegmentList2_create();
      SegmentList2_clone(list, temp);
      
      SegmentList2_refreshCache(temp);
      
      result &= list->private_isClosed == temp->private_isClosed;
      assert(result);
      result &= list->private_isContinuous == temp->private_isContinuous;
      assert(result);
      result &= list->private_spin == temp->private_spin;
      assert(result);
      result &= list->private_isZeroArea == temp->private_isZeroArea;
      assert(result);

      if (numSegs(list) > 0 && temp->private_type != NULL_SET && temp->private_type != PATH)
	result &= Range2_almostEqual2(&(list->private_pathBounds), &(temp->private_pathBounds));

      assert(result);
      result &= list->private_type == temp->private_type;
      assert(result);
    }
  return(result);
}
#else
#define SegmentList2_db_validateCache(A) true
#endif

/* constructors & initializers */
SegmentList2 * SegmentList2_create()
{
  return SegmentList2_create1(GT_SEGMENTLIST2_DEFAULT_SIZE-GT_SEGMENTLIST2_DEFAULT_OVERSIZE);
}
SegmentList2 * SegmentList2_create1(const unsigned int containerSize)
{
  SegmentList2 * list;

  list = malloc(sizeof(SegmentList2));
  if (list == NULL)
    GT_SEGLIST_DO_EOMFAIL;

  SegmentList2_initialize2(list, containerSize);

  return list;
}
SegmentList2 * SegmentList2_createType(const SegmentList2Type type)
{
  SegmentList2 * list;

  if (type == BOUNDED || type == HOLE || type == PATH)
    {				/* can't go make one of these from nothing */
      list = NULL;
    }
  else
    {
      assert (type == NULL_SET || type == UNBOUNDED);
      
      list = SegmentList2_create1(0);
      assert(list->private_type == NULL_SET && list->private_cacheValid);

      list->private_type = type; /* I can just flip this */
      list->private_isClosed = false;
      list->private_isContinuous = false;
    }
  return list;  
}
SegmentList2 * SegmentList2_createClone(const SegmentList2 * const src)
{
  SegmentList2 * list;

  list = malloc(sizeof(SegmentList2));
  if (list == NULL)
    GT_SEGLIST_DO_EOMFAIL;

  BlockVec_initialize(&(list->segments), sizeof(Segment2));
  if (numSegs(src) > 0)
    BlockVec_resize(&(list->segments), numSegs(src), 0);

  /* TODO: could probably do this faster with a memcpy */
  list->private_isClosed = src->private_isClosed;
  list->private_isContinuous = src->private_isContinuous;
  list->private_spin = src->private_spin;
  list->private_isZeroArea = src->private_isZeroArea;
  list->private_interiorPoint = src->private_interiorPoint;
  list->private_interiorPointValid = src->private_interiorPointValid;
  list->private_pathBounds = src->private_pathBounds;
  list->private_type = src->private_type;
  list->private_cacheValid = src->private_cacheValid;
  list->boundedCache = NULL;
  /* oh yea, now copy the actual segments */
  BlockVec_copy((BlockVec*)list, (BlockVec*)src);

  return list;
}
void SegmentList2_initialize(SegmentList2 * list)
{
  SegmentList2_initialize2(list, GT_SEGMENTLIST2_DEFAULT_SIZE);
}
void SegmentList2_initialize2(SegmentList2 * list, const unsigned int containerSize)
{
  assert(list != NULL);
  
  BlockVec_initialize(&(list->segments), sizeof(Segment2));
  if (containerSize > 0)
    BlockVec_resize(&(list->segments), containerSize, GT_SEGMENTLIST2_DEFAULT_OVERSIZE);

  list->private_isClosed = false;
  list->private_isContinuous = false; /* well, you're not NOT continuous... sooo.... */
  list->private_spin = 0; 	/* invalid */
  list->private_isZeroArea = true;
  list->private_interiorPointValid = false;
  Range2_initialize(&(list->private_pathBounds));
  list->private_type = NULL_SET;
  list->private_cacheValid = true;
  list->boundedCache = NULL;

  /* GT_SEGLIST_VALID1(list); -- can't call this here, infinite recursion*/ 
}
/* destructors & uninitializers */
void SegmentList2_destroy(SegmentList2 * list)
{
  GT_SEGLIST_VALID1(list);
  SegmentList2_uninitialize(list);
  free(list);
}
void SegmentList2_uninitialize(SegmentList2 * list)
{
  GT_SEGLIST_VALID1(list);
  BlockVec_uninitialize((BlockVec *)list);
  
  list->segments.private_bufferSize = 0;
  list->private_cacheValid = false;
  list->private_interiorPointValid = false;
  list->private_type = PATH;

  if (list->boundedCache != NULL && list->boundedCache != list) /* and here's where I hope I don't recurse infinitely */
    {
      GT_SEGLIST_VALID1(list->boundedCache);
      SegmentList2_uninitialize(list->boundedCache);
      free(list->boundedCache);
      list->boundedCache = NULL;
    }
}
/* basic list operations */
void SegmentList2_clear(SegmentList2 * list)
{
  GT_SEGLIST_VALID1(list);
  numSegs(list) = 0;
  list->private_type = NULL_SET;
  Internal_invalidateCache(list);
}
void SegmentList2_reallocate(SegmentList2 * list, const unsigned int containerSize)
{
  BlockVec_resize((BlockVec*)list, containerSize, GT_SEGMENTLIST2_DEFAULT_OVERSIZE);

  GT_SEGLIST_VALID1(list);
}
void SegmentList2_clone(const SegmentList2 * const source, SegmentList2 * dest)
{
  assert(source != NULL);
  assert(dest != NULL);

  if (dest->segments.private_bufferSize < source->segments.numItems)
    SegmentList2_reallocate(dest, source->segments.numItems);

  assert(dest->segments.private_bufferSize >= source->segments.numItems);

  /* copy over the segments */
  BlockVec_copy((BlockVec*)dest, (BlockVec*)source);

  /* and copy over the rest of the stuff */
  dest->private_isClosed = source->private_isClosed;
  dest->private_isContinuous = source->private_isContinuous;
  dest->private_spin = source->private_spin;
  dest->private_isZeroArea = source->private_isZeroArea;
  dest->private_interiorPoint = source->private_interiorPoint;
  dest->private_interiorPointValid = source->private_interiorPointValid;
  dest->private_pathBounds = source->private_pathBounds;
  dest->private_cacheValid = source->private_cacheValid;
  dest->private_type = source->private_type;

  /* GT_SEGLIST_VALID1(source); -- can't call this here, recursion*/
  /* GT_SEGLIST_VALID1(dest); */
}
/* elementwise operations. */
void SegmentList2_remove(SegmentList2 * list, const unsigned int idx, const bool refreshCache)
{
  GT_SEGLIST_VALID1(list);
  assert(idx <= numSegs(list));
  assert(refreshCache == true || refreshCache == false);
  BlockVec_remove(&(list->segments), idx);
  GT_SEGLIST_VALID1(list);

  if (refreshCache)
    Internal_refreshCacheAfterAppend(list);
  else
    Internal_invalidateCache(list);

  GT_SEGLIST_VALID1(list);  
}
void SegmentList2_removeRange(SegmentList2 * list, const unsigned int startIdx, const unsigned int endIdx, const bool refreshCache)
{
  /* remove all segments between startIdx and end index INCLUSIVE! */
  GT_SEGLIST_VALID1(list);
  assert(refreshCache == true || refreshCache == false);
  BlockVec_removeRange(&(list->segments), startIdx, endIdx);
  GT_SEGLIST_VALID1(list);

  if (refreshCache)
    Internal_refreshCacheAfterAppend(list);
  else
    Internal_invalidateCache(list);
}
void SegmentList2_pop(SegmentList2 * list, const bool refreshCache)
{
  /* GT_SEGLIST_VALID1(list); -- can't call this here, recursion */
  assert(refreshCache == true || refreshCache == false);

  if (numSegs(list) > 0)
    {
      numSegs(list)--;
      Internal_invalidateCache(list);
    }
      
  if (refreshCache)
    SegmentList2_refreshCache(list);

}
void SegmentList2_insertCopy(SegmentList2 * list, Segment2 * segment, const unsigned int idx, const bool refreshCache)
{
  assert(0);
}
void SegmentList2_appendCopy(SegmentList2 * list, const Segment2 * const seg, const bool refreshCache)
{
  assert(seg != NULL);
  GT_SEGLIST_VALID1(list);
  /* is there room for one more segment on here? */
  if (list->segments.private_bufferSize <= numSegs(list)) /* need to make more room here :( */
    SegmentList2_reallocate(list, list->segments.private_bufferSize + GT_SEGMENTLIST2_DEFAULT_GROWTH);

  memcpy(&(seg(list, numSegs(list))), seg, sizeof(Segment2));
  ++numSegs(list);

  if (refreshCache)
    Internal_refreshCacheAfterAppend(list);
  else
    Internal_invalidateCache(list);
  
  GT_SEGLIST_VALID1(list);
}
void SegmentList2_appendCopyList(SegmentList2 * list, const SegmentList2 * const otherList, const bool refreshCache)
{
  GT_SEGLIST_VALID1(list);
  GT_SEGLIST_VALID1(otherList);

  if (BlockVec_appendCopyList(&(list->segments), &(otherList->segments)) == BlockVec_error)
      GT_SEGLIST_DO_TERMINAL;
			 
  if (refreshCache)
    Internal_refreshCacheAfterAppend(list);
  else
    Internal_invalidateCache(list);
  
  GT_SEGLIST_VALID1(list);
}
int SegmentList2_getIdxFromList(const SegmentList2 * const list, const Segment2 * const segment)
{
  int i;
  GT_SEGLIST_VALID1(list);
  assert(segment != NULL);

  for (i = 0; i < numSegs(list); ++i)
    {
      if (segment == &seg(list, i))
	  return i;
    }
  
  return -1;
}
/* Cache related funtions */
void SegmentList2_refreshCache(SegmentList2 * list)
{
  bool check;
  assert(list != NULL);

  check = Internal_calcPathRange2(list, &(list->private_pathBounds));
  assert(check);
  
  check = Internal_calcContinuousClosed(list, &(list->private_isContinuous), &(list->private_isClosed));
  assert(check);

  if (numSegs(list) > 0)
    list->private_type = PATH;
  else
    {
      if (list->private_type != UNBOUNDED) /* if it's unbounded, leave it like that, otherwise, null set */
	list->private_type = NULL_SET;
    }
  
  if (list->private_isClosed)
    {
      check = Internal_calcZeroArea(list, &(list->private_isZeroArea));
      assert(check);
      if (!list->private_isZeroArea)
	{
	  check = Internal_calcSpin3(list, &(list->private_pathBounds), &(list->private_spin));
	  assert(check);
	  /* potential shape! */
	  if (list->private_spin != 0)
	    {
	      list->private_type = ( (list->private_spin > 0) ? (BOUNDED) : (HOLE) );
	    }
	}
      else
	{
	  list->private_spin = 0;
	}
    }
  else
    {
      list->private_isZeroArea = true;
      list->private_spin = 0;
    }
  list->private_cacheValid = true;
  list->private_interiorPointValid = false;

  /* GT_SEGLIST_VALID1(list); */
}
bool SegmentList2_cacheValid(const SegmentList2 * const list)
{
  GT_SEGLIST_VALID1(list);
  return list->private_cacheValid;
}
void SegmentList2_invalidateCache(SegmentList2 * list)
{
  GT_SEGLIST_VALID1(list);
  Internal_invalidateCache(list); /* wait... why are there two of these? */
}
/* modifiers */
bool SegmentList2_setType(SegmentList2 * list, const SegmentList2Type type)
{
  if (type == BOUNDED || type == HOLE || type == PATH)
    { 				/* these are the types you can't set to, they 
				   have to be derived and verified from segment data */
      return false;
    }
  else
    {
      assert(type == UNBOUNDED || type == NULL_SET);
      /* you want all space or no space, either way 
	 you shouldn't have any segments */
      SegmentList2_clear(list);
      list->private_type = type;
      list->private_isContinuous = false;
      list->private_isClosed = false;
      list->private_isZeroArea = true;
      list->private_spin = 0;
      list->private_cacheValid = true;
      if (type == UNBOUNDED)
	{ 			/* unbounded, so any point is interior, pick 0,0 */
	  list->private_interiorPointValid = true;
	  list->private_interiorPoint.x = list->private_interiorPoint.y = 0;
	}
      else
	{ 			/* null_set so there is neven an interior point */
	  assert(type == NULL_SET);
	  list->private_interiorPointValid = false;
	}
      return true;
    }
}
void SegmentList2_translate(SegmentList2 * list, const Vec2 delta)
{
  unsigned int i;
  GT_SEGLIST_VALID1(list);

  for (i = 0; i < numSegs(list); ++i)
    Segment2_translate(segp(list,i), &delta);

  if (list->private_cacheValid)
    {
      Range2_translate(&(list->private_pathBounds), delta.x, delta.y);
      if (list->private_interiorPointValid)
	Point2_add2(&(list->private_interiorPoint), &delta);
    }

  assert(SegmentList2_db_validateCache(list));

}
void SegmentList2_scaleOrigin(SegmentList2 * list, const gtfloat alpha)
{
  unsigned int i;
  GT_SEGLIST_VALID1(list);
  
  for (i = 0; i < numSegs(list); ++i)
    Segment2_scaleOrigin(segp(list,i), alpha);

  assert(0); 			/* TODO: scale the range in the cache too, then you don't have to invalidate it */
  SegmentList2_invalidateCache(list);
  
  /* and now technically, the only part of the cache that I *need* to invalidate in the interior point (if there is one) */
  list->private_interiorPointValid = false;
}
void SegmentList2_forceContinuous(SegmentList2 * list, const bool forceClosed)
{
   /* algorithm:  get a segment: */
   /*   it's a line */
   /*    the next segment is a line? */
   /*     line endpoint and next line start point = (line.end + next.start)/2 */
   /*    the next segment is an arc? */
   /*     adjust next.startRad() to get it as close to line.end as possible. */
   /*   it's an arc */
   /*    the next segment is a line? */
   /*     adjust the arc.endRad to get it as close to next.startPoint as possible */
   /*    the next segment is an arc? */
   /*     f it, just give up.  (really, get them as close as possible. */
   /*   --- if at any step you fail to get seg.end close enough to next.start, then return false and die. */

  unsigned int i, j;
  Vec2 bendTarget;
#ifndef NDEBUG
  bool flipTest;
#endif

  GT_SEGLIST_VALID1(list);
  assert(numSegs(list) > 1);

  for (i = (forceClosed ? 0 : 1); i < numSegs(list); ++i)
    {
      j = ( (i == 0) ? (numSegs(list) - 1) : (i - 1) );

      if (seg(list, i).type == ARC)
	{
	  if (seg(list, j).type == ARC)
	    { 			/* I am an arc, and the previous segment was also an arc...  CRASH */
	      assert(0); 	/* in reality, I should modify these arcs to get as close as possible. */
	    }
	  else
	    { 			/* I am an arc, and the previous segment was a line, bend my start to the line end. */
	      assert(seg(list, j).type == LINE);
	      Point2_sub3(&bendTarget, &(seg(list, j).s.line.end), &(seg(list, i).s.arc.center));
#ifndef NDEBUG
	      flipTest = Angle_span( &(seg(list, i).s.arc.angle) ) >= 0;
#endif
	      seg(list, i).s.arc.angle.start = Point2_theta(&bendTarget);
	      /* ensure you didn't accidentally mess up this arc. */
	      assert(flipTest == (Angle_span( &(seg(list, i).s.arc.angle) ) >= 0) ); 
	    }
	}
      else 			/* if (seg(list, i).type == LINE) */
	{
	  assert(seg(list, i).type == LINE);
	  if (seg(list, j).type == ARC)
	    {			/* YOU are a line, but the previous segment was an arc, bend it to you */
	      Point2_sub3(&bendTarget, &(seg(list, i).s.line.start), &(seg(list, i).s.arc.center));
#ifndef NDEBUG
	      flipTest = Angle_span( &(seg(list, i).s.arc.angle) ) >= 0;
#endif
	      seg(list, j).s.arc.angle.end = Point2_theta(&bendTarget);
	      /* ensure you didn't accidentally mess up this arc. */
	      assert(flipTest == (Angle_span( &(seg(list, i).s.arc.angle) ) >= 0) ); 
	    }
	  else
	    {			/* YOU Are a line and the previous segment is a line, merge your points. */
	      assert(seg(list, j).type == LINE);
	      Point2_add3(&bendTarget, &(seg(list, i).s.line.start), &(seg(list, j).s.line.end));
	      Point2_scale2(&bendTarget, 0.5);
	      seg(list, i).s.line.start = bendTarget;
	      seg(list, j).s.line.end   = bendTarget;
	    }
	}
    }
  SegmentList2_refreshCache(list);
}
/* queries - which could modify cache */
bool SegmentList2_isContinuous(SegmentList2 * segs)
{
  GT_SEGLIST_VALID1(segs);
  if (!segs->private_cacheValid)
    {
      SegmentList2_refreshCache(segs);
    }
  GT_SEGLIST_VALID1(segs);
  return segs->private_isContinuous;
}
bool SegmentList2_isClosed(SegmentList2 * segs)
{
  GT_SEGLIST_VALID1(segs);
  if (!segs->private_cacheValid)
    {
      SegmentList2_refreshCache(segs);
    }
  GT_SEGLIST_VALID1(segs);
  return segs->private_isClosed;
}
bool SegmentList2_isZeroArea(SegmentList2 * segs)
{
  GT_SEGLIST_VALID1(segs);
  if (!segs->private_cacheValid)
    {
      SegmentList2_refreshCache(segs);
    }
  GT_SEGLIST_VALID1(segs);
  return segs->private_isZeroArea;
}
short SegmentList2_spin(SegmentList2 * segs)
{
  GT_SEGLIST_VALID1(segs);
  if (!segs->private_cacheValid)
    {
      SegmentList2_refreshCache(segs);
    }
  GT_SEGLIST_VALID1(segs);
  return segs->private_spin;
}
bool SegmentList2_pathRange2(SegmentList2 * segs, Range2 * range)
{
  GT_SEGLIST_VALID1(segs);
  if (!segs->private_cacheValid)
    {
      SegmentList2_refreshCache(segs);
    }
  GT_SEGLIST_VALID1(segs);
  if (segs->segments.numItems > 0)
    {
      *range = segs->private_pathBounds;
      return (true);
    }
  return (false);
}
bool SegmentList2_interiorPoint(SegmentList2 * segs, Point2 * point)
{
  GT_SEGLIST_VALID1(segs);
  if (!segs->private_cacheValid)
    {
      SegmentList2_refreshCache(segs);
    }
  GT_SEGLIST_VALID1(segs);
  if (!segs->private_interiorPointValid)
    {
      Internal_calcInteriorPoint(segs, &(segs->private_interiorPoint));
    }
  if (segs->private_interiorPointValid)
    {
      *point = segs->private_interiorPoint;
      return (true);
    }
  return(false);
}
SegmentList2Type SegmentList2_type(SegmentList2 * segs)
{
  GT_SEGLIST_VALID1(segs);
  if (!segs->private_cacheValid)
      SegmentList2_refreshCache(segs);

  GT_SEGLIST_VALID1(segs);
  return segs->private_type;
}
/* queries - dumb - no cachce hit, no cache modify */
bool SegmentList2_calcContinuousClosed(const SegmentList2 * const list, bool * continuous, bool * closed)
{
  if (SegmentList2_cacheValid(list))
    {
      *continuous = list->private_isContinuous;
      *closed = list->private_isClosed;
      return true;
    }
  else
    return Internal_calcContinuousClosed(list, continuous, closed);
}
bool Internal_calcContinuousClosed(const SegmentList2 * const list, bool * continuous, bool * closed)
{
  unsigned int i;
  Point2 firstStart, myStart, lastEnd;

  assert(list != NULL);
  assert(continuous != NULL);
  assert(closed != NULL);

  if (numSegs(list) == 0)
    {
      *continuous = false;
      *closed = false;
    }
  else if (numSegs(list) <= 1)
    {
      *continuous = true;
      *closed = false;
    }
  else
    {
      *continuous = true; 	/* assume it's continuous, we'll prove it's false later */
      /* pickup the ending point of the first line. */

      if (seg(list,0).type == LINE)
	{
	  lastEnd = seg(list,0).s.line.end;
	  firstStart = seg(list,0).s.line.start;
	}
      else
	{
	  assert(seg(list,0).type == ARC);
	  Arc2_endPoint( &(seg(list,0).s.arc), &lastEnd);	    
	  Arc2_startPoint( &(seg(list,0).s.arc), &firstStart);
	}
      
      for (i = 1; i < numSegs(list); ++i)
	{ 			/* check that this segments start is near the last segment's end. */
				/* then store this segments end point as "lastEnd" */	  
	  if (seg(list,i).type == LINE) /* line case */
	    {
	      if (!Point2_almostEqual2(&lastEnd, &(seg(list,i).s.line.start)))
		{
		  *continuous = false;
		  break;
		}
	      lastEnd = seg(list,i).s.line.end;
	    }
	  else			/* arc case. */
	    {
	      assert(seg(list,i).type == ARC);
	      Arc2_startPoint(&(seg(list,i).s.arc), &myStart);
	      if (!Point2_almostEqual2(&lastEnd, &myStart))
		{
		  *continuous = false;
		  break;
		}

	      Arc2_endPoint( &(seg(list,i).s.arc), &lastEnd);
	    }
	}
      if (!(*continuous))
	{
	  *closed = false;
	}
      else 			/* it was continuous, so there is a chance it could also be closed. */
	{
	  *closed = Point2_almostEqual2(&lastEnd, &firstStart); /* could be closed */
	}
    }
  return(true);
}
bool SegmentList2_calcZeroArea(const SegmentList2 * const list, bool * isZeroArea)
{
   if (SegmentList2_cacheValid(list))
    {
      *isZeroArea = list->private_isZeroArea;
      return true;
    }
  else
    return Internal_calcZeroArea(list, isZeroArea);
}
bool Internal_calcZeroArea(const SegmentList2 * const segs, bool * isZeroArea)
{
 #ifndef NDEBUG
  bool testClosed, testContinuous;
#endif
  
  Collision2Container * cont;
  SegmentList2 * remaining;
  Segment2 seg;
  Range1Set * coverage;
  Range1 tempRange;
  unsigned int i;
  bool foundOutstanding;
  
#ifndef NDEBUG
  assert(Internal_calcContinuousClosed(segs, &testClosed, &testContinuous));
  assert(testClosed && testContinuous);
#endif
  assert(segs != NULL);
  assert(isZeroArea != NULL);

  foundOutstanding = false;
  coverage = Range1Set_create();
  cont = Collision2Cont_create();
  remaining = SegmentList2_create();

				/* All segments minus the first one */
  SegmentList2_clone(segs, remaining); /* grab everything you gave me */;
 
  while (remaining->segments.numItems > 0)
    {
      seg = lastSeg(remaining); /* grab the last segment on the list */
      SegmentList2_pop(remaining, false); /* and knock off the last segment off the list*/
            
      if (Collision2_SLS(cont, remaining, &seg, false, false)
  	  && cont->numSegs > 0)
  	{
	  Range1Set_clear(coverage);
	  for (i = 0; i < cont->numSegs; ++i)
	    {
	      Range1_set2(&tempRange, &(cont->segs[i].bParametric));
	      Range1Set_add(coverage, &tempRange);
	    }
	  /* and now, if the range is 0->1, I can throw this segment away, and keep on going */
	  /* however, if the range is NOT 0->1, I can stop now because I know I have a piece of a segment that is exclusive of all other segments */
	  if (coverage->numRanges == 1
	      && coverage->range[0].Min <= GT_ALMOST_ZERO
	      && coverage->range[0].Max >= GT_EXACTLY_ONE-GT_ALMOST_ZERO)
	    {			/* this segment is entirely covered, time to move on to the next one. */
	      /* KEEP GOING */
	    }
	  else
	    { 			/* this segment is NOT entirely covered!  this means you're NOT zero area */
	      foundOutstanding = true;
	      break;
	    }
	}
    }

  Range1Set_destroy(coverage);
  Collision2Cont_destroy(cont);
  SegmentList2_destroy(remaining);

  *isZeroArea = foundOutstanding;
  return(true);
}
bool Internal_calcSpin3(const SegmentList2 * const segs, const Range2 * const trustedRange, short * spin)
{
#ifndef NDEBUG
  bool testClosed, testContinuous;
#endif

  gtfloat midY, lowestParametric;
  Line2 testLine;
  Vec2 segDir;
  Collision2Container * cont;
  unsigned int i,j;
  short output;

#ifndef NDEBUG
  assert(Internal_calcContinuousClosed(segs, &testClosed, &testContinuous));
  assert(testClosed && testContinuous);
#endif

  assert(segs != NULL);
  assert(trustedRange != NULL);
  assert(spin != NULL);

  /* shoot a line from left to right at middle y level */
  midY = Range1_mid(&(trustedRange->y));
  Line2_set5(&testLine, trustedRange->x.Min-GT_REASONABLE_VALUE, midY, trustedRange->x.Max+GT_REASONABLE_VALUE, midY);
  cont = Collision2Cont_create();
  output = 0;
  lowestParametric = GT_EXACTLY_ONE * 2;

  /* this is the way to di it without referencing segmentlist level collisions */
  {
    for (j = 0; j < numSegs(segs); ++j)
      {
	if (Collision2_SL(cont, &seg(segs,j), &testLine, true,true))
	  {
	    Collision2Cont_decomposeSegsToPoints(cont);
	    for (i = 0; i < cont->numPts; ++i)
	      {
		assert(cont->pts[i].aSegment->type == LINE || cont->pts[i].aSegment->type == ARC);
		
		if (cont->pts[i].bParametric > lowestParametric)
		  continue; 		/* nope, don't care about this one. */
	    
		Seg2p_parametricDirection(cont->pts[i].aSegment, cont->pts[i].aParametric, &segDir);
	    
		if (cont->pts[i].bParametric < lowestParametric)
		  {
		    lowestParametric = cont->pts[i].bParametric;
		    output = ( (segDir.y <= 0) ? ( 1 ) : ( -1 ) );
		  }
		else if (cont->pts[i].bParametric == lowestParametric)
		  {
		    if (segDir.y <= 0 && output == 1)
		      output = 1;
		    else if (segDir.y > 0 && output == -1)
		      output = -1;
		    else
		      output = 0; 	/* two at the same point, going different directions? uhg... no. */
		  }
	      }
	  }
      }
  }
  /* this is the way to do it and reference collision2 */
  /* { */
  /*   if (!Collision2_SLL(cont, segs, &testLine, true, true)) */
  /*     { */
  /* 	Collision2Cont_destroy(cont); */
  /* 	*spin = 0; */
  /* 	return (false); */
  /*     } */
  /*   Collision2Cont_decomposeSegsToPoints(cont);  */
    
    
  /*   for (i = 0; i < cont->numPts; ++i) */
  /*   {				/\* at this point, figure out if the segment from segs is going "up" or "down" *\/ */
  /*     assert(cont->pts[i].aSegment->type == LINE || cont->pts[i].aSegment->type == ARC); */
      
  /*     if (cont->pts[i].bParametric > lowestParametric) */
  /* 	continue; 		/\* nope, don't care about this one. *\/ */
      
  /*     Seg2p_parametricDirection(cont->pts[i].aSegment, cont->pts[i].aParametric, &segDir); */
      
  /*     if (cont->pts[i].bParametric < lowestParametric) */
  /* 	{ */
  /* 	  lowestParametric = cont->pts[i].bParametric; */
  /* 	  output = ( (segDir.y <= 0) ? ( 1 ) : ( -1 ) ); */
  /* 	} */
  /*     else if (cont->pts[i].bParametric == lowestParametric) */
  /* 	{ */
  /* 	  if (segDir.y <= 0 && output == 1) */
  /* 	    output = 1; */
  /* 	  else if (segDir.y > 0 && output == -1) */
  /* 	    output = -1; */
  /* 	  else */
  /* 	    output = 0; 	/\* two at the same point, going different directions? uhg... no. *\/ */
  /* 	} */
  /*   } */
  /* } */
  Collision2Cont_destroy(cont);
  *spin = output;

  return (true);
}
bool SegmentList2_calcSpin(const SegmentList2 * const list, short * spin)
{
  if (SegmentList2_cacheValid(list))
    {
      *spin = list->private_spin;
      return true;
    }
  else
    return Internal_calcSpin2(list, spin);
}
bool Internal_calcSpin2(const SegmentList2 * const segs, short * spin)
{
  Range2 range;
#ifndef NDEBUG
  bool testClosed, testContinuous;
#endif
  assert(segs != NULL);
  assert(spin != NULL);
#ifndef NDEBUG
  assert(Internal_calcContinuousClosed(segs, &testClosed, &testContinuous));
  assert(testClosed && testContinuous);
#endif
  
  Internal_calcPathRange2(segs, &range);
  return Internal_calcSpin3(segs, &range, spin);
}
bool SegmentList2_calcPathRange(const SegmentList2 * const list, Range2 * bounds)
{
  if (SegmentList2_cacheValid(list))
    {
      *bounds = list->private_pathBounds;
      return true;
    }
  else
    return Internal_calcPathRange2(list, bounds);
}
bool Internal_calcPathRange2(const SegmentList2 * const list, Range2 * bounds)
{
  Range2 temp;
  unsigned int i;
  assert(list != NULL);
  assert(bounds != NULL);
      
  Range2_initialize(bounds);

  for (i = 0; i < list->segments.numItems; ++i)
    {
      /* void     Line2_range2(const Line2 * const line, Range2 * range); */
      /* void     Arc2_range2(const Arc2 * const arc, Range2 * range); */
      assert(seg(list, i).type == LINE || seg(list, i).type == ARC);
      if (seg(list, i).type == LINE)
	Line2_range2(&(seg(list, i).s.line), &temp);
      else
	Arc2_range2( &(seg(list, i).s.arc), &temp);

      Range2_encompass(bounds, &temp);
    }
  return (true);
}
bool SegmentList2_calcInteriorPoint(const SegmentList2 * const list, Point2 * pt)
{
  if (SegmentList2_cacheValid(list) && list->private_interiorPointValid)
    {
      *pt = list->private_interiorPoint;
      return true;
    }
  else
    return Internal_calcInteriorPoint(list, pt);
}
bool Internal_calcInteriorPoint(const SegmentList2 * const list, Point2 * pt)
{
  Range2 bounds;
  SegmentList2Type type;
  Collision2Container * cont;
  gtfloat correctParametric;
  Line2 scanline;
  GT_SEGLIST_VALID1(list);
  assert(pt != NULL);

  /* if the list is HOLE - then I should be returning a point IN THE AREA - not inside the bounding path bounds */
  /* algorithm, start on the left of the bounds, end on the right of the bounds with a Line2
     do a collision check against A.  Your interior point is going to be somewhere between the intersects */

  if (SegmentList2_cacheValid(list))
    {
      
      if (list->private_type == NULL_SET || list->private_type == PATH)
	return false; 	/* nope! */
      
      if (list->private_type == UNBOUNDED)
	{
	  pt->x = pt->y = 0;
	  return true;
	}
      
      type = list->private_type;
      bounds = list->private_pathBounds;
    }
  else
    {
      if (Internal_calcType(list, &type) != true)
	return false; 	/* I don't know what type this is */

      if (type == NULL_SET || type == PATH)
	return false; 	/* nope! */
      
      if (type == UNBOUNDED)
	{
	  pt->x = pt->y = 0;
	  return true;
	}
      
      if (Internal_calcPathRange2(list, &bounds) != true)
	return false; 	/* need this! */
    }

  assert(type == HOLE || type == BOUNDED);

  if (type == HOLE)
    {
      pt->x = bounds.x.Max + GT_REASONABLE_VALUE;
      pt->y = bounds.y.Max + GT_REASONABLE_VALUE;
      return true;
    }
  else
    {				/* time to find an inner point! */
      scanline.start.x = bounds.x.Min - GT_REASONABLE_VALUE;
      scanline.end.x = bounds.x.Max + GT_REASONABLE_VALUE;
      scanline.start.y = scanline.end.y = Range1_mid(&(bounds.y));
      cont = Collision2Cont_create();

      Collision2_SLL(cont, list, &scanline, true, false);
      
      assert(cont->numPts >= 2);
      if (cont->numPts < 2)
	return false;

      cont->numSegs = 0;	/* don't care about segments for now */
      Collision2Cont_sortByBParam(cont);
      
      correctParametric = (cont->pts[0].bParametric + cont->pts[1].bParametric) / 2;
      Line2_parametricPoint(&scanline, correctParametric, pt);

      assert(Containment_SLP(list, pt, false) == true);
      return true;
    }
}
bool SegmentList2_calcType(const SegmentList2 * const list, SegmentList2Type * type)
{
  if (SegmentList2_cacheValid(list))
    {
      *type = list->private_type;
      return true;
    }
  else
    return Internal_calcType(list, type);
}
bool Internal_calcType(const SegmentList2 * const list, SegmentList2Type * type)
{
  assert(false);
  return (false);
}
/* queries - a bit more complicated */
bool SegmentList2_almostEqual2(const SegmentList2 * const A, const SegmentList2 * const B, bool allowOffset)
{
  unsigned int i,j;

  if (numSegs(A) != numSegs(B))
      return false;
  
  if (allowOffset == false)
    {				/* most basic comparison */
      for (i = 0; i < numSegs(A); ++i)
	{
	  if (Segment2_almostEqual2(&seg(A, i), &seg(B,i)) == false)
	    return false;
	}
    }
  else
    {				/* compare, allow wrapping */
      for (j = 0; j < numSegs(A); ++j)
	{
	  if (Segment2_almostEqual2(&seg(A, 0), &seg(B, j)) == true)
	    {
	      /* found the offset! (A:0 @ B:j) */
	      for (i = 0; i < numSegs(A); ++i, ++j)
		{
		  if (j == numSegs(A))
		    j = 0;

		  if (Segment2_almostEqual2(&seg(A, i), &seg(B,j)) == false)
		    return false;
		}
	      return true; 	/* well, ok, I guess you passed */
	    }
	}
      return false; 	/* never found a segment that was a match start */
    }

  return true;
}
bool SegmentList2_pointFarthestAlong(const SegmentList2 * const list, const Vec2 * const direction, Point2 * pt)
{
  Point2 temp;
  Vec2 delta;
  unsigned int i;
  GT_SEGLIST_VALID1(list);
  assert(direction != NULL);
  assert(pt != NULL);
  
  if (list->segments.numItems <= 0)
    return false;
  /* void     Line2_pointFarthestAlong(const Line2 * const line, const Vec2 * const dir, Point2 * output);
     void     Arc2_pointFarthestAlong(const Arc2 * const arc, const Vec2 * const dir, Point2 * output); */
  
  
  assert(firstSeg(list).type == LINE || firstSeg(list).type == ARC);
  if (firstSeg(list).type == LINE)
    Line2_pointFarthestAlong(&(firstSeg(list).s.line), direction, pt);
  else
    Arc2_pointFarthestAlong( &(firstSeg(list).s.arc),  direction, pt);
  
  for (i = 1; i < numSegs(list); ++i)
    {
      assert(seg(list,i).type == LINE || seg(list,i).type == ARC);
      if (seg(list,i).type == LINE)
	Line2_pointFarthestAlong(&(seg(list,i).s.line), direction, &temp);
      else
	Arc2_pointFarthestAlong( &(seg(list,i).s.arc),  direction, &temp);
      
      Point2_sub3(&delta, &temp, pt);
      if (Point2_dot(&delta, direction) > 0)
	  *pt = temp;		/* new point is better */
    }
  
  return (true);  
}
SegmentList2Type Internal_getOppositeType(const SegmentList2Type type)
{
  if (type == BOUNDED)
    return HOLE;
  else if (type == NULL_SET)
    return UNBOUNDED;
  else if (type == HOLE)
    return BOUNDED;
  else if (type == UNBOUNDED)
    return NULL_SET;
  else
    return PATH;
}  
void SegmentList2_flip1(SegmentList2 * list)
{
  unsigned int i,j;
  Segment2 temp;
  GT_SEGLIST_VALID1(list);
  for (i = 0, j = numSegs(list)-1; i <= j; ++i, --j)
    {
      /* i goes into temp (but flipped) */
      if (i == j)
	{
	  Seg2_flip1(seg(list,i));
	}
      else
	{
	  Seg2_flip2(seg(list,i), temp);
	  Seg2_flip2(seg(list,j), seg(list,i));
	  seg(list,j) = temp;
	}
    }

  list->private_type = Internal_getOppositeType(list->private_type);
  list->private_spin = -1 * list->private_spin;
  list->private_interiorPointValid = false;
  GT_SEGLIST_VALID1(list);
}
void SegmentList2_flip2(const SegmentList2 * const source, SegmentList2 * dest)
{
  unsigned int i,j;
  GT_SEGLIST_VALID1(source);
  assert(dest != NULL);
  
  SegmentList2_clear(dest);
  SegmentList2_reallocate(dest, numSegs(source));
  assert(dest->segments.private_bufferSize >= numSegs(source));
  /* WARNING: this loop is cheating, I'm just writing to memory locations like an idiot */
  for (j = 0, i = numSegs(source)-1; j < numSegs(source); --i, ++j)
    {
      assert(i < numSegs(source));
      assert(j < numSegs(source));
      Seg2_flip2(seg(source,i), seg(dest,j));
    }
  
  ((BlockVec *)dest)->numItems = numSegs(source);
  dest->private_cacheValid = source->private_cacheValid;
  if (dest->private_cacheValid)
    {
      dest->private_pathBounds = source->private_pathBounds;
      dest->private_isContinuous = source->private_isContinuous;
      dest->private_isClosed = source->private_isClosed;
      dest->private_isZeroArea = source->private_isZeroArea;
      dest->private_spin = -1 * source->private_spin;
      dest->private_interiorPointValid = false;
      dest->private_type = Internal_getOppositeType(source->private_type);
    }
  GT_SEGLIST_VALID1(source);
  GT_SEGLIST_VALID1(dest);
}
bool SegmentList2_simplify(const SegmentList2 * const input, SegmentList2 * output, unsigned int tryLevel, const bool isContinuous, const bool isClosed)
{
  Point2 end, start;
  bool didWork;
  unsigned int i, j;

  assert(tryLevel == 0);	 /* if not, I don't know what to do. */
  assert(input->segments.numItems > 0);
  assert( (isClosed && isContinuous) || !isClosed );
  didWork = false;
  
  if (tryLevel == 0)
    { 				/* quick try at simplification */
      SegmentList2_clear(output);
      SegmentList2_reallocate(output, numSegs(input));

      for (i = 0; i < numSegs(input); ++i)
	{
	  j = i + 1;
	  if (j >= numSegs(input))
	    {
	      if (isContinuous)
		j = 0;		/* continuous! */
	      else
		break;		/* not continuous, dont try to simplyfy segment 0 and segment numSegs-1 */
	    }
	      
	  if (seg(input,i).type != seg(input,j).type)
	    {
	      SegmentList2_appendCopy(output, &(seg(input,i)), false);
	      continue; 		/* cant stick an arc and line together in any way. */
	    }

	  if (seg(input,i).type == LINE)
	    {			/* two lines. */
	      if (!isContinuous 
		  && !Point2_almostEqual2(&(seg(input,i).s.line.end),
					  &(seg(input,j).s.line.start)))
		{
		  SegmentList2_appendCopy(output, &(seg(input,i)), false);
		  continue; 	/* these two lines aren't continuous and they're too far apart. */
		}
	      
	      Line2_direction(&(seg(input,i).s.line), &end);
	      Line2_direction(&(seg(input,j).s.line), &start);
	      if (!Point2_almostEqual2(&start, &end))
		{
		  SegmentList2_appendCopy(output, &(seg(input,i)), false);
		  continue; 	/* These two lines aren't pointing in the same direction, no getting together */
		}
	      /* you can merge these two lines together. */
	      if (j == 0)	/* this is the wrapping case! */
		{
		  seg(output, 0).s.line.start = seg(input,i).s.line.start;
		}
	      else
		{
		  assert(numSegs(output) < ((BlockVec*)output)->private_bufferSize);
		  seg(output, numSegs(output)).type = LINE;
		  seg(output, numSegs(output)).s.line.start = seg(input,i).s.line.start;
		  seg(output, numSegs(output)).s.line.end = seg(input,i+1).s.line.end;
		  ++( ((BlockVec*)output)->numItems );
		}
	      didWork = true;
	      ++i; 		/* I absorbed this (i) and the next one (i+1), so skip ahead */
	    }
	  
	  if (seg(input,i).type == ARC)
	    { 			/* two arcs */
	      /* TODO: if they have the same center and radius and rot direction and the end of the first is the start of the next... */
	      SegmentList2_appendCopy(output, &(seg(input,i)), false); /* ahhhhhh, crap */
	      continue; 	/* These two lines aren't pointing in the same direction, no getting together */
	    }
	}
    }
  output->private_cacheValid = input->private_cacheValid;
  if (output->private_cacheValid)
    {
      output->private_isClosed = input->private_isClosed;
      output->private_isContinuous = input->private_isContinuous;
      output->private_spin = input->private_spin;
      output->private_isZeroArea = input->private_isZeroArea;
      output->private_interiorPoint = input->private_interiorPoint;
      output->private_interiorPointValid = input->private_interiorPointValid;
      output->private_pathBounds = input->private_pathBounds;
      output->private_type = input->private_type;
    }
  return (didWork);
}
bool SegmentList2_blindExpansion(const SegmentList2 * const input, const gtfloat growth, SegmentList2 * output)
{
  unsigned int i, k, next_i;	/* i = input segment counter, k = output segment counter */
  Vec2 myEndExpandDir, nextStartExpandDir;
  Point2 myEnd;
  gtfloat dirCross, absGrowth;

#ifndef NDEBUG
  bool isCont, isClosed;
  assert( Internal_calcContinuousClosed(input, &isCont, &isClosed) );
  assert( isCont && isClosed );
#endif

  GT_SEGLIST_VALID1(input);
  assert(growth != 0);
  GT_SEGLIST_VALID1(output);
  assert(SegmentList2_db_validateCache(input));

  if (growth == 0)
    return false;

  absGrowth = ( (growth < 0) ? (-growth) : (growth) );
  
  SegmentList2_reallocate(output, input->segments.numItems*2); /* at most double input's size (entirely covex shape) */
  SegmentList2_clear(output); 
  k = 0;

  for (i = 0; i < input->segments.numItems; ++i)
    {
      seg(output,k) = seg(input,i); /* copy over the segment to start */
      
      /* SegmentList2_appendCopy(output, &(input->segments[i]), false); */
      assert(seg(output,k).type == seg(input,i).type);
      if (seg(output,k).type == LINE)
	{
	  Line2_blindExpand(&(seg(output,k).s.line), growth, &myEndExpandDir);
	  myEnd = seg(input,i).s.line.end;
	}
      else
	{
      	  assert(seg(output,k).type == ARC);
	  assert(seg(output,k).s.arc.angle.rot != 0);
	  if (Arc2_blindExpand(&(seg(output,k).s.arc), growth))
	    {
	      Arc2_endDirection(&(seg(output,k).s.arc), &myEndExpandDir); /* expansion was valid, collect end direction */
	      Point2_rotate90(&myEndExpandDir, -1); /* probably not the most efficient way to get this */
	      Arc2_endPoint(&(seg(input,i).s.arc), &myEnd);
	    }
	  else
	    --k;		/* expansion was not valid */
	}
      
      /* and now, check if you need to stitch to the next line */
      next_i = i + 1;
      if (next_i >= numSegs(input))
	next_i = 0; 		/* WRAP AROUND! */
      
      Seg2_startDirection(seg(input,next_i), &nextStartExpandDir);
      Point2_rotate90(&nextStartExpandDir, -1);

      dirCross = Point2_cross(&myEndExpandDir, &nextStartExpandDir) ;
      if (dirCross != 0 && 
	  (growth > 0) == (dirCross > 0))
	{			/* confusing, but I think correct */
	  /* center will be at input->segments[i] end point */
	  /* start angle is defined by myEndExpandDir, end angle defined by nextStartExpandDir */
	  /* the radius will be the growth */
	  if (growth < 0)
	    {			/* if your shrinking, you need to remember to flip these */
	      Point2_invert1(&myEndExpandDir);
	      Point2_invert1(&nextStartExpandDir);
	    }

	  seg(output,++k).type = ARC;
	  Arc2_set6(&(seg(output,k).s.arc), 
		    &myEnd,
		    absGrowth,
		    Point2_theta(&myEndExpandDir),
		    Point2_theta(&nextStartExpandDir),
		    ( (growth > 0) ? ( 1 ) : ( -1 ) ));		    
	}
      ++k;
    }
  
  ((BlockVec*)output)->numItems = k;
  return(true);
}
unsigned int SegmentList2_getApproximationSize(const SegmentList2 * const input, const gtfloat maxError)
{
  /* BUG: if you call this on a segmentlist that isn't continuous, it will spit out garbage */
  GT_SEGLIST_VALID1(input);
  
  unsigned int size;
  unsigned int i;

  size = 1;		/* and one more point to repeat the first point! */
  for (i = 0; i < numSegs(input); ++i)
    {
      if (seg(input,i).type == LINE)
	++size;
      else
	size += Arc2_getApproximationSize(&(seg(input,i).s.arc), maxError);
    }

  return size;
}
int SegmentList2_getApproximation(const SegmentList2 * const input, const gtfloat maxError, Point2 * buffer, const unsigned int bufferMaxPoints)
{
  /* grab an approximation of this segmentlist, return the number of vertices in the approximation */
  /* TODO: terribly inefficient, calls getApproximationSize on each arc (probably again) */
  /* BUG: if you call this on a segmentlist that isn't continuous, it will spit out garbage */
  GT_SEGLIST_VALID1(input);
  unsigned int seg_i, pt_i, arcSize;
  
  if (numSegs(input) > 0 &&  bufferMaxPoints >= 1)
    {
      /* lay down the startpoint first */
      Seg2_startPoint(seg(input,0), &(buffer[0]));
      pt_i = 1;
      for (seg_i = 0; seg_i < numSegs(input) && pt_i < bufferMaxPoints; ++seg_i)
	{
	  if (seg(input,seg_i).type == LINE)
	    {
	      buffer[pt_i] = seg(input,seg_i).s.line.end;
	      ++pt_i;
	    }
	  else
	    {
	      assert(seg(input,seg_i).type == ARC);
	      arcSize = Arc2_getApproximationSize(&(seg(input,seg_i).s.arc), maxError);
	      if (!Arc2_getApproximation(&(seg(input,seg_i).s.arc), &(buffer[pt_i-1]),arcSize))
		{
		  /* TODO: well that threw up, handle this error properly some day */
		  assert(0);
		  return -2;
		}
	      pt_i += arcSize-1;
	    }
	}
      return pt_i;
    }
  return -1;		/* why!? */
}
void SegmentList2_cacheBounded(SegmentList2 * list)
{
  GT_SEGLIST_VALID1(list);
  assert(SegmentList2_cacheValid(list));
  assert(SegmentList2_type(list) == BOUNDED ||
	 SegmentList2_type(list) == HOLE);

  if (list->private_type == BOUNDED)
    list->boundedCache = list;
  else
    {
      list->boundedCache = SegmentList2_create();
      SegmentList2_flip2(list, list->boundedCache);
      assert(SegmentList2_cacheValid(list->boundedCache));
      assert(list->boundedCache->private_type == BOUNDED);
    }
}
void Internal_invalidateCache(SegmentList2 * list)
{
  list->private_cacheValid = false;
  list->private_interiorPointValid = false;
  if (list->boundedCache != NULL)
    {
      if (list->boundedCache == list)
	list->boundedCache = NULL;
      else
	{
	  SegmentList2_uninitialize(list->boundedCache);
	  free(list->boundedCache);
	}
    }
}
