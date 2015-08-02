#include "include/Region2.h"

#include "include/Collision2.h"
#include "stdlib.h" 		/* malloc */

/*
  BlockVec lists
  } Region2; */


/* #define DEBUG_REG2_UNMERGE 1 */
#define GT_REGION_OVERSIZE 4
#define GT_REGION_DO_EOMFAIL { puts("EOM"); exit(1); }

#define list(REGION, IDX) Region2_list((REGION), (IDX))
#define numLists(REGION) Region2_numLists((REGION))
#define lastList(REGION) list((REGION), numLists((REGION))-1)
#define firstList(REGION) list((REGION), 0)



bool INTERNAL_Region2_unmerge(const Region2 * const input, Region2 * output, bool removeOpenPaths);

/* TODO: turn this into a SegmentList2Set_isValid function */
bool GT_SEGLISTSET_VALID( const SegmentList2Set * const region)
{
  if (region == NULL)
    {
      assert(0);
      return false;
    }

  GT_BLOCKVEC_VALID( (BlockVec*)(region) );

  return true;
}
/* TODO: turn this into a Region2_isValid() function */
bool GT_REGION_VALID( const Region2 * const region)
{
  unsigned int i;
  SegmentList2Type mytype;
  bool res;
  if (region == NULL)
    return false;

  GT_BLOCKVEC_VALID( (BlockVec*)(region) );

  for (i = 0; i < numLists(region); ++i)
    {
      res = SegmentList2_calcType(&list(region, i), &mytype);
      if (res != true)
	{
	  assert(0);
	  return false;
	}

      if (i == 0)
	{
	  if (mytype == PATH || mytype == HOLE)
	    {
	      assert(0);
	      return false;
	    }	      
	}
      else
	{
	  if (mytype == BOUNDED || mytype == PATH)
	    {
	      assert(0);
	      return false;
	    }
	}
    }
  
  return true;
}
Region2 * Region2_create()
{
  return Region2_create1(GT_REGION2_DEFAULT_SIZE);
}
Region2 * Region2_create1(const unsigned int numLists)
{
  Region2 * temp;
  
  assert(numLists < 0xFFFFFFFF);
  
  if ((temp = malloc(sizeof(Region2))) == NULL)
    GT_REGION_DO_EOMFAIL;

  Region2_initialize2(temp, numLists);

  GT_REGION_VALID(temp);
  return (temp);
}
Region2 * Region2_createClone(const Region2 * const input)
{
  Region2 * temp;
  GT_REGION_VALID(input);
  
  if ((temp = malloc(sizeof(Region2))) == NULL)
    GT_REGION_DO_EOMFAIL;

  Region2_initialize2(temp, Region2_numLists(input));
  
  GT_REGION_VALID(temp);
  Region2_clone(input, temp);
  GT_REGION_VALID(temp);
  return (temp);
}
void Region2_initialize(Region2 * region)
{
  Region2_initialize2(region, GT_REGION2_DEFAULT_SIZE);
}
void Region2_initialize2(Region2 * region, const unsigned int containerSize)
{
  /* assert( containerSize > 0); - technically, this should be ok */
  assert(region != NULL);

  BlockVec_initialize((BlockVec*)region, sizeof(SegmentList2));
  if (containerSize > 0)
    BlockVec_resize((BlockVec*)region, containerSize, GT_REGION2_DEFAULT_OVERSIZE);

  assert(Region2_numLists(region) == 0);
  
  GT_REGION_VALID(region);
}
void Region2_reallocate(Region2 * region, const unsigned int containerSize)
{
  BlockVec_resize((BlockVec*)region, containerSize, GT_REGION2_DEFAULT_OVERSIZE);
  GT_REGION_VALID(region);
}
void Region2_remove(Region2 * region, const unsigned int target)
{
  GT_REGION_VALID(region);
  assert(target > 0 && target < numLists(region));
  assert(0);
}
void Region2_appendCopy(Region2 * region, const SegmentList2 * const list)
{
  SegmentList2 * newList;
  GT_SEGLISTSET_VALID(region);
  GT_SEGLIST_VALID1(list);

  newList = Region2_appendEmptyP(region);
  SegmentList2_clone(list, newList);
}
void Region2_insertCopy(Region2 * region, const SegmentList2 * const list, const unsigned int idx)
{
  GT_REGION_VALID(region);
  GT_SEGLIST_VALID1(list);
  assert(idx < numLists(region));
  assert(0);
}
unsigned int Region2_appendEmpty(Region2 * region)
{
  GT_SEGLISTSET_VALID(region);
  
  BlockVec_appendNull((BlockVec*)region);
  
  SegmentList2_initialize(&(lastList(region)));

  return numLists(region)-1;  
}
SegmentList2 * SegmentList2Set_appendEmptyP(SegmentList2Set * set)
{
  unsigned int res;

  GT_SEGLISTSET_VALID(set);
  
  res = Region2_appendEmpty(set);
  return &(list(set, res));
}
SegmentList2 * Region2_appendEmptyP(Region2 * region)
{
    GT_REGION_VALID(region);
    return SegmentList2Set_appendEmptyP(region);
}
void Region2_appendCopySet(Region2 * region, const Region2 * const otherRegion)
{
  unsigned int i;

  GT_REGION_VALID(region);
  GT_REGION_VALID(otherRegion);
 
  Region2_reallocate(region, numLists(region) + numLists(otherRegion));
  for ( i = 0; i < numLists(otherRegion); ++i)
    Region2_appendCopy(region, &(list(otherRegion,i)));
  
}
void Region2_destroy(Region2 * region)
{
  GT_SEGLISTSET_VALID(region);
  BlockVec_uninitialize((BlockVec*)region);
  free(region);
}
void Region2_clear(Region2 * region)
{
  GT_REGION_VALID(region);
  BlockVec_clear((BlockVec*)region);
}
void Region2_clone(const Region2 * const src, Region2 * dest)
{
  GT_REGION_VALID(src);
  GT_REGION_VALID(dest);

  Region2_reallocate(dest, numLists(src));
  Region2_appendCopySet(dest, src);
}
void SegmentList2Set_pop(SegmentList2Set * listSet)
{
  GT_SEGLISTSET_VALID( listSet);
  BlockVec_pop((BlockVec*)listSet);
}
void Region2_pop(Region2 * region)
{
  GT_REGION_VALID(region);
  BlockVec_pop((BlockVec*)region);
}
void Region2_refreshCache(Region2 * reg)
{
  unsigned int i;
  GT_SEGLISTSET_VALID(reg);

  for (i = 0; i < numLists(reg); ++i)
    {
      if (!SegmentList2_cacheValid( & list(reg, i)))
	SegmentList2_refreshCache(& list(reg, i));
    }
}
bool Region2_almostEqual(const Region2 * const A, const Region2 * const B, bool allowListOutOfOrder, bool allowSegmentsOutOfOrder)
{
  unsigned int i, j, offset_j;
  if (Region2_numLists(A) != Region2_numLists(B))
    return false;

  offset_j = 0;
  if (allowListOutOfOrder)
    {
      for (offset_j = 0; offset_j < Region2_numLists(B); ++offset_j)
	{
	  if (SegmentList2_almostEqual2(&Region2_list(A,0), &Region2_list(B,offset_j), allowSegmentsOutOfOrder) == true)
	      break;
	}

      if (offset_j >= Region2_numLists(B))
	return false;	/* no match found */
    }

  for (i = 0, j = 0; i < Region2_numLists(A); ++i, ++j)
    {
      if (SegmentList2_almostEqual2(&Region2_list(A, i), &Region2_list(B, (j+offset_j)%Region2_numLists(B)), allowSegmentsOutOfOrder) == false)
	return false;
    }
  return true;
}
void Region2_translate(Region2 * region, const Vec2 delta)
{
  GT_REGION_VALID(region);
  unsigned int i;

  for (i = 0; i < numLists(region); ++i)
    SegmentList2_translate(&(list(region,i)), delta);
}
unsigned int Region2_numSegments(const Region2 * const region)
{
  GT_REGION_VALID(region);
  unsigned int accum, i;

  for (i = 0; i < numLists(region); ++i)
    accum += SegmentList2_numSegs(&(list(region,i)));

  return accum;  
}
unsigned int SegmentList2Set_numSegments(const SegmentList2Set * const region)
{
  GT_SEGLISTSET_VALID(region);
  unsigned int accum, i;

  for (i = 0; i < numLists(region); ++i)
    accum += SegmentList2_numSegs(&(list(region,i)));

  return accum;  
}
void Region2_cloneAllSegsToList(const Region2 * const region, SegmentList2 * accum, bool refreshCache)
{
  unsigned int i;
  GT_REGION_VALID(region);
  GT_SEGLIST_VALID1(accum);

  for (i = 0; i < numLists(region); ++i)
    SegmentList2_appendCopyList(accum, Region2_listp(region,i), false);

  if (refreshCache)
    SegmentList2_refreshCache(accum);
}
bool Region2_blindExpandToList(const Region2 * const region, const gtfloat growth, SegmentList2 * accum)
{
  GT_REGION_VALID(region);
  GT_SEGLIST_VALID1(accum);
  unsigned int i;
  SegmentList2 * temp;
  bool res;

  if ((temp = SegmentList2_create()) == NULL)
    return false;

  res = true;
  
  for (i = 0; i < numLists(region) && res; ++i)
    {
      if (SegmentList2_blindExpansion(&list(region,i), growth, temp) != true)
	{
	  res = false;
	  SegmentList2_appendCopyList(accum, temp, false);
	}
    }

  SegmentList2_destroy(temp);

  return res;      
}
SegmentList2Type Region2_boundaryType(const Region2 * const reg)
{
  SegmentList2Type type;
  GT_REGION_VALID(reg);
  if (numLists(reg) <= 0)
    return NULL_SET;
  else
    {
      type = SegmentList2_type(Region2_listp(reg,0));
      if (type == UNBOUNDED && numLists(reg) > 1)
	type = HOLE;		/* it's actually more of a "hole" type thing.... */

      return type;
    }  
}
void Region2_union(const SegmentList2 * const A, const SegmentList2 * const B, Region2 * output)
{
  assert(0); 			/* does this even get called?? */
  Region2 * input;
  
  GT_SEGLIST_VALID1(A);
  GT_SEGLIST_VALID1(B);
  GT_REGION_VALID(output);

  if ( (input = Region2_create1(2)) == NULL )
    GT_REGION_DO_EOMFAIL;
  
  Region2_appendCopy(input, A);
  Region2_appendCopy(input, B);
  
  INTERNAL_Region2_unmerge(input, output, true);

  /* uuuuhhhhh, I'm done now? */  
}
bool SegmentList2Set_makeUnique(const SegmentList2Set * const input, SegmentList2Set * output)
{
  return INTERNAL_Region2_unmerge(input, output, true);
}

/* unmerge and friends */
typedef enum unmergeGetBestSegResult
  {
    UGBSR_Invalid = 0,
    UGBSR_Continuation,
    UGBSR_Reset
  } unmergeGetBestSegResult;
bool INTERNAL_isRight(const gtfloat startCross, const gtfloat startDot, const gtfloat endCross, const gtfloat endDot)
{
  if (startCross >= 0 && endCross >= 0)
    {				/* both are on the left */
      return (endDot > startDot);
    }
  else if (startCross < 0 && endCross < 0)
    {				/* both are on the right */
      return (endDot < startDot);
    }
  else 				/* one on one side, one on the other side */
    {
      return (endCross < startCross);
    }
}
unmergeGetBestSegResult INTERNALRegion2_unmerge_getBestSegment(const Collision2Container * const cont, unsigned int * bestIdx)
{
  unsigned int i;
  gtfloat lowParametric, lowCross, lowDot, myCross, myDot;
  Vec2 bDir, aDir;
  unmergeGetBestSegResult res;

  res = UGBSR_Invalid;
  lowParametric = 2;		/* anything over 1 */
  lowCross =  0;		/* anything over 1 */
  lowDot   = -2;
  *bestIdx = cont->numPts+10;      /* anything invalid */
  assert(cont->numPts < 9999999);  /* make sure that last line didn't cause an overflow */
  
  for (i = 0; i < cont->numPts; ++i)
    {
      if ( GT_ALMOST_EQUAL2(cont->pts[i].aParametric, GT_EXACTLY_ONE) )
	continue; 		/* even if I picked this segment, I couldn't go anywhere with it */

      if ( GT_ALMOST_EQUAL2(cont->pts[i].bParametric, GT_EXACTLY_ZERO) )
	continue;		/* TODO: validate this: this would be more of a restart than anything else */

      Seg2p_parametricDirection(cont->pts[i].aSegment, cont->pts[i].aParametric, &aDir);
      Seg2p_parametricDirection(cont->pts[i].bSegment, cont->pts[i].bParametric, &bDir);
      myCross = Point2_cross(&bDir, &aDir);
      myDot = Point2_dot(&bDir, &aDir);
      
      if ( cont->pts[i].bParametric <= lowParametric) /* do I even care about this particular hit? */
	{
	  if ( GT_ALMOST_EQUAL2(cont->pts[i].bParametric, GT_EXACTLY_ONE) )
	    {			/* this is a hit at the end of the segment (allow cross > 0) */
	      if ( INTERNAL_isRight(lowCross, lowDot, myCross, myDot) )
		{		/* this looks like the best so far */
		  lowParametric = cont->pts[i].bParametric;
		  lowCross = myCross;
		  lowDot = myDot;
		  *bestIdx = i;
		  res = UGBSR_Continuation;
		}
	    }
	  else if ( myCross < 0 && res != UGBSR_Reset )
	    {		/* going in the right direction and I haven't found a reset yet */
	      if ( cont->pts[i].bParametric < lowParametric /* must be earlier */
		   || INTERNAL_isRight(lowCross, lowDot, myCross, myDot)) /* or "more right" */
		{
		  lowParametric = cont->pts[i].bParametric;
		  lowCross = myCross;
		  lowDot = myDot;
		  *bestIdx = i;
		  res = UGBSR_Continuation;
		}
	    }
	  else if ( myCross > 0 )
	    {			/* going into me */
	      if ( cont->pts[i].bParametric < lowParametric /* must be earlier */
		   || INTERNAL_isRight(myCross, myDot, lowCross, lowDot)) /* more left, harder cut in */
		{
		  lowParametric = cont->pts[i].bParametric;
		  lowCross = myCross;
		  lowDot = myDot;
		  *bestIdx = i;
		  res = UGBSR_Reset;
		}
	    }
	}
    }

  return res;
}
unsigned int INTERNAL_segListFind(const SegmentList2 * const list, const Segment2 * seg)
{
  unsigned int j;
  for (j = 0; j < SegmentList2_numSegs(list); ++j)
    {
      if (&(SegmentList2_seg(list,j)) == seg)
	  return j;
    }
  assert(0);		/* this is how I fix it for now, terrible idea. */
  return 0xFFFFFFFF;		/* 32 bit big, TODO: this is dumb, fix that */
}

/* #define DEBUG_REG2_UNMERGE 1 */

#ifdef DEBUG_REG2_UNMERGE
#include <stdio.h> 		/* printf */
#include "include/Serializer.h"
#endif


bool INTERNAL_Region2_unmerge(const SegmentList2Set * const input, Region2 * output, bool removeOpenPaths)
{
  /* find any self collisions in the input and make them into unique or point-contact-only paths */
  /* so, probably get all the segments into one big list and go from there */

#ifdef DEBUG_REG2_UNMERGE
  /* char charBuff[256]; */
#endif
  
  SegmentList2 * all;
  unsigned int i;
  bool res;

  all = SegmentList2_create1(SegmentList2Set_numSegments(input)+2);
  for (i = 0; i < numLists(input); ++i)
    SegmentList2_appendCopyList(all, &(list(input,i)), false);

  res =SegmentList2Set_makeUnique_destructive(all, output, removeOpenPaths);
  SegmentList2_destroy(all);
  return res;
}

bool SegmentList2Set_makeUnique_destructive(SegmentList2 * inputAll, SegmentList2Set * output, bool removeOpenPaths)
{
  /* find any self collisions in the input and make them into unique or point-contact-only paths */
  /* so, probably get all the segments into one big list and go from there */

#ifdef DEBUG_REG2_UNMERGE
  char charBuff[256];
  int i;
#endif
  
  SegmentList2 * accum;
  Segment2 seg, tempSeg;
  Collision2Container * contOther, * contSelf;
  unmergeGetBestSegResult typeOther, typeSelf;
  unsigned int idxOther, idxSelf, collision_i;
  int j;
  Point2 tempStart, tempEnd;
  bool done;

  done = false;
  Region2_clear(output);

  contOther = Collision2Cont_create();
  contSelf = Collision2Cont_create();

  accum = SegmentList2Set_appendEmptyP(output);
  seg = SegmentList2_lastSeg(inputAll);	/* steal the last segment from the end of the list */
  SegmentList2_pop(inputAll,false);
    
  while (!done || SegmentList2_numSegs(inputAll) > 0) /* (SegmentList2_numSegs(all) > 0) */ /* for (i = all->numSegments - 1; i >= 0; --i) */
    {
  
#ifdef DEBUG_REG2_UNMERGE
      Serialize_Seg(SEXP, &seg, charBuff, 256);
      printf("\nIR2U: loop start\nIR2U: work seg = %s\n", charBuff);
      for (i = 0; i < (int)SegmentList2_numSegs(accum); ++i)
	{
	  Serialize_Seg(SEXP, &SegmentList2_seg(accum,i), charBuff, 256);
	  printf("IR2U: accum %d = %s\n", i, charBuff);
	}
      if (SegmentList2_numSegs(accum) == 0)
	printf("IR2U: accum EMPTY!\n");
            
      for (i = 0; i < (int)SegmentList2_numSegs(inputAll); ++i)
	{
	  Serialize_Seg(SEXP, &SegmentList2_seg(inputAll,i), charBuff, 256);
	  printf("IR2U: all seg %d = %s\n", i, charBuff);
	}
      printf("\n");
#endif

      /* for all these collisions, B is the seg, A is a member of the target set */
      if (SegmentList2_numSegs(inputAll) <= 0)
	done = true;	/* this is your last run through */
      
      Collision2_SLS(contOther, inputAll, &seg, true, true);
      Collision2_SLS( contSelf,    accum, &seg, true, true);

      Collision2Cont_decomposeSegsToPoints(contSelf);
      Collision2Cont_decomposeSegsToPoints(contOther);
      
      typeOther = INTERNALRegion2_unmerge_getBestSegment(contOther, &idxOther);
      typeSelf  = INTERNALRegion2_unmerge_getBestSegment(contSelf, &idxSelf);
#ifdef DEBUG_REG2_UNMERGE
      if (typeOther == UGBSR_Invalid)
	printf("IR2U: typeOther=UGBSR_Invalid\n");
      else if (typeOther == UGBSR_Continuation)
	printf("IR2U: typeOther=UGBSR_Continuation (bParametric = %.3f)\n", contOther->pts[idxOther].bParametric);
      else
	printf("IR2U: typeOther=UGBSR_Reset (bParametric = %.3f)\n", contOther->pts[idxOther].bParametric);

      if (typeSelf == UGBSR_Invalid)
	printf("IR2U: typeSelf=UGBSR_Invalid\n");
      else if (typeSelf == UGBSR_Continuation)
	printf("IR2U: typeSelf=UGBSR_Continuation (bParametric = %.3f)\n", contSelf->pts[idxSelf].bParametric);
      else
	printf("IR2U: typeSelf=UGBSR_Reset (bParametric = %.3f)\n", contSelf->pts[idxSelf].bParametric);
#endif
      
      /* hit another segment out there? */
      if (typeOther != UGBSR_Invalid 
	  && (typeSelf == UGBSR_Invalid 
	      || contOther->pts[idxOther].bParametric < contSelf->pts[idxSelf].bParametric))
	{
	  /* the "other" intersect comes earlier, use other */
	  collision_i = INTERNAL_segListFind(inputAll, contOther->pts[idxOther].aSegment);
#ifdef DEBUG_REG2_UNMERGE
	  Serialize_Seg(SEXP, contOther->pts[idxOther].aSegment, charBuff, 256);
	  printf("IR2U: Othr: collision pt %.2f, %.2f\n", contOther->pts[idxOther].pt.x, contOther->pts[idxOther].pt.y);
	  printf("IR2U: Othr: collision A param(%.2f), %s\n", contOther->pts[idxOther].aParametric, charBuff);
	  Serialize_Seg(SEXP, contOther->pts[idxOther].bSegment, charBuff, 256);
	  printf("IR2U: Othr: collision B param(%.2f), %s\n", contOther->pts[idxOther].bParametric, charBuff);
#endif
	  
	  if (typeOther == UGBSR_Reset)
	    {
#ifdef DEBUG_REG2_UNMERGE
	      printf("IR2U: Hit on all seg = RESET\n");
#endif
	      /* clean out the accumulator if needed */
	      if (SegmentList2_numSegs(accum) > 0)
		{
#ifdef DEBUG_REG2_UNMERGE
	      printf("IR2U: flushing accumulator to all\n");
#endif
		  SegmentList2_appendCopyList(inputAll, accum, false);
		  SegmentList2_clear(accum);
		}

	      /* the one you hit becomes the first seg on the accumulator, just the first part of it */	      
	      if (!GT_ALMOST_EQUAL2(contOther->pts[idxOther].aParametric, GT_EXACTLY_ONE))
		{
		  Seg2p_setParametric(&tempSeg, &(SegmentList2_seg(inputAll,collision_i)), contOther->pts[idxOther].aParametric, GT_EXACTLY_ONE);
#ifdef DEBUG_REG2_UNMERGE
		  Serialize_Seg(SEXP, &tempSeg, charBuff, 256);
		  printf("IR2U: splitA trash: %s\n", charBuff);
#endif
		  SegmentList2_appendCopy(inputAll, &tempSeg, false);
		  Seg2p_truncateToParametric(&(SegmentList2_seg(inputAll,collision_i)), GT_EXACTLY_ZERO, contOther->pts[idxOther].aParametric);
#ifdef DEBUG_REG2_UNMERGE
		  Serialize_Seg(SEXP, &SegmentList2_seg(inputAll, collision_i), charBuff, 256);
		  printf("IR2U: splitA keepr: %s\n", charBuff);
#endif
		}

	      SegmentList2_appendCopy(accum, &(SegmentList2_seg(inputAll,collision_i)), false);
	      SegmentList2_remove(inputAll, collision_i, false);

	      /* now the segment you were testing with, I just need the last part of that to remain the segment */
	      if (!GT_ALMOST_EQUAL2(contOther->pts[idxOther].bParametric, GT_EXACTLY_ZERO))
		{
		  Seg2p_setParametric(&tempSeg, &seg, GT_EXACTLY_ZERO, contOther->pts[idxOther].bParametric);
		  SegmentList2_appendCopy(inputAll, &tempSeg, false);
#ifdef DEBUG_REG2_UNMERGE
		  Serialize_Seg(SEXP, &tempSeg, charBuff, 256);
		  printf("IR2U: splitWork trash: %s\n", charBuff);
#endif
		  Seg2p_truncateToParametric(&seg, contOther->pts[idxOther].bParametric, GT_EXACTLY_ONE);
#ifdef DEBUG_REG2_UNMERGE
		  Serialize_Seg(SEXP, &seg, charBuff, 256);
		  printf("IR2U: splitWork keepr: %s\n", charBuff);
#endif
		}
	    }

	  else /* (typeOther == UGBSR_Continuation) */
	    {
#ifdef DEBUG_REG2_UNMERGE
	      printf("IR2U: Hit on all seg = CONTINUATION\n");
#endif
	      assert(typeOther == UGBSR_Continuation);
	      /* split the seg if needed, then stack it on the end of the accum */
	      if (!GT_ALMOST_EQUAL2(contOther->pts[idxOther].bParametric, GT_EXACTLY_ONE))
		{
		  Seg2p_setParametric(&tempSeg, &seg, contOther->pts[idxOther].bParametric, GT_EXACTLY_ONE);
		  SegmentList2_appendCopy(inputAll, &tempSeg, false);
#ifdef DEBUG_REG2_UNMERGE
		  Serialize_Seg(SEXP, &tempSeg, charBuff, 256);
		  printf("IR2U: splitWork trash: %s\n", charBuff);
#endif
		  Seg2p_truncateToParametric(&seg, GT_EXACTLY_ZERO, contOther->pts[idxOther].bParametric);
#ifdef DEBUG_REG2_UNMERGE
		  Serialize_Seg(SEXP, &seg, charBuff, 256);
		  printf("IR2U: splitWork keepr: %s\n", charBuff);
#endif
		}
	      SegmentList2_appendCopy(accum, &seg, false);

	      /* split the hit if needed */
	      if (!GT_ALMOST_EQUAL2(contOther->pts[idxOther].aParametric, GT_EXACTLY_ZERO))
		{
		  Seg2p_setParametric(&tempSeg, &(SegmentList2_seg(inputAll,collision_i)), GT_EXACTLY_ZERO, contOther->pts[idxOther].aParametric);
		  SegmentList2_appendCopy(inputAll, &tempSeg, false);
#ifdef DEBUG_REG2_UNMERGE
		  Serialize_Seg(SEXP, &tempSeg, charBuff, 256);
		  printf("IR2U: splitWork trash: %s\n", charBuff);
#endif
		  Seg2p_truncateToParametric(&(SegmentList2_seg(inputAll,collision_i)), contOther->pts[idxOther].aParametric, GT_EXACTLY_ONE);
#ifdef DEBUG_REG2_UNMERGE
		  Serialize_Seg(SEXP, &seg, charBuff, 256);
		  printf("IR2U: splitWork keepr: %s\n", charBuff);
#endif
		}
	      
	      seg = SegmentList2_seg(inputAll,collision_i);
	      SegmentList2_remove(inputAll, collision_i, false);
	    }
	}
      /* hit a segment on your current accumulator? */
      else if (typeSelf != UGBSR_Invalid
	       && (typeOther == UGBSR_Invalid
		   || contSelf->pts[idxSelf].bParametric <= contOther->pts[idxOther].bParametric))
	{
	  /* the "self" intersect comes earlier, use self, close the loop */
	  collision_i = INTERNAL_segListFind(accum, contSelf->pts[idxSelf].aSegment);
#ifdef DEBUG_REG2_UNMERGE
	  Serialize_Seg(SEXP, contSelf->pts[idxSelf].aSegment, charBuff, 256);
	  printf("IR2U: Self: collision pt %.2f, %.2f\n", contSelf->pts[idxSelf].pt.x, contSelf->pts[idxSelf].pt.y);
	  printf("IR2U: Self: collision A param(%.2f), %s\n", contSelf->pts[idxSelf].aParametric, charBuff);
	  Serialize_Seg(SEXP, contSelf->pts[idxSelf].bSegment, charBuff, 256);
	  printf("IR2U: Self: collision B param(%.2f), %s\n", contSelf->pts[idxSelf].bParametric, charBuff);
#endif


	  if  (typeSelf == UGBSR_Reset)
	    {
#ifdef DEBUG_REG2_UNMERGE
	      printf("IR2U: Hit on SELF = Reset\n");
#endif
	      /* the one you hit becomes the first seg on the accumulator, just the first part of it */	      
	      if (!GT_ALMOST_EQUAL2(contSelf->pts[idxSelf].aParametric, GT_EXACTLY_ONE))
		{
		  Seg2p_setParametric(&tempSeg, &(SegmentList2_seg(accum,collision_i)), contSelf->pts[idxSelf].aParametric, GT_EXACTLY_ONE);
		  SegmentList2_appendCopy(inputAll, &tempSeg, false);
		  Seg2p_truncateToParametric(&(SegmentList2_seg(accum,collision_i)), GT_EXACTLY_ZERO, contSelf->pts[idxSelf].aParametric);
		}
	      
	      /* steal a copy of the hit then remove it from the accumulator */
	      tempSeg = SegmentList2_seg(accum,collision_i);
	      SegmentList2_remove(accum, collision_i, false);

	      /* clean out the accumulator */
	      SegmentList2_appendCopyList(inputAll, accum, false);
	      SegmentList2_clear(accum);
	      
	      SegmentList2_appendCopy(accum, &(tempSeg), false);
	      SegmentList2_remove(inputAll, collision_i, false);

	      /* now the segment you were testing with, I just need the last part of that to remain the segment */
	      if (!GT_ALMOST_EQUAL2(contOther->pts[idxOther].bParametric, GT_EXACTLY_ZERO))
		{
		  Seg2p_setParametric(&tempSeg, &seg, GT_EXACTLY_ZERO, contOther->pts[idxOther].bParametric);
		  SegmentList2_appendCopy(inputAll, &tempSeg, false);
		  Seg2p_truncateToParametric(&seg, contOther->pts[idxOther].bParametric, GT_EXACTLY_ONE);
		}
	    }

	  else	/* (typeSelf == UGBSR_Continuation) */
	    {
#ifdef DEBUG_REG2_UNMERGE
	      printf("IR2U: Hit on SELF = Continuation\n");
#endif
	      assert(typeSelf == UGBSR_Continuation);
	      /* trim whatever piece that you need to off the seg */
	      if (!GT_ALMOST_EQUAL2(contSelf->pts[idxSelf].bParametric, GT_EXACTLY_ONE))
		{		/* trim off the end of seg and throw it back */
		  Seg2p_setParametric(&tempSeg, &seg, contOther->pts[idxOther].bParametric, GT_EXACTLY_ONE);
		  SegmentList2_appendCopy(inputAll, &tempSeg, false);
		  Seg2p_truncateToParametric(&seg, GT_EXACTLY_ZERO, contOther->pts[idxOther].bParametric);
		}

	      /* now figure out if you need to trim anything from accum */
	      if (!GT_ALMOST_EQUAL2(contSelf->pts[idxSelf].aParametric, GT_EXACTLY_ZERO))
		{
		  Seg2p_setParametric(&tempSeg, &(SegmentList2_seg(accum,collision_i)), GT_EXACTLY_ZERO, contSelf->pts[idxSelf].aParametric);
		  SegmentList2_appendCopy(inputAll, &tempSeg, false);
		  Seg2p_truncateToParametric(&(SegmentList2_seg(accum,collision_i)), contSelf->pts[idxSelf].aParametric, GT_EXACTLY_ONE);
		}
	      if (collision_i != 0)
		{		/* you didn't hit at the first segment, going to have to ditch your formers */
		  for (j = 0; j < collision_i; ++j)
		    SegmentList2_appendCopy(inputAll, &(SegmentList2_seg(accum,j)), false);
		  
		  SegmentList2_removeRange(accum, 0, collision_i - 1, false);
		}

	      /* and now you have a closed loop! horray, leave your good accumulator and get a new one */
	      SegmentList2_appendCopy(accum, &seg, true); /* refresh cash here, this is an output */

#ifdef DEBUG_REG2_UNMERGE
	      for (i = 0; i < (int)SegmentList2_numSegs(accum); ++i)
		{
		  Serialize_Seg(SEXP, &SegmentList2_seg(accum,i), charBuff, 256);
		  printf("IR2U: output[%d] %d = %s\n", Region2_numLists(output)-1, i, charBuff);
		}
#endif

	      /* start a new list and seg */
	      if (SegmentList2_numSegs(inputAll) <= 0)
		break;		/* you just finished up a loop and your all out of segments, bail out */

	      accum = SegmentList2Set_appendEmptyP(output);
	      seg = SegmentList2_seg(inputAll,SegmentList2_numSegs(inputAll)-1);
	      SegmentList2_pop(inputAll,false);
 	    }
	}
      /* didn't hit a damn thing */
      else
	{
#ifdef DEBUG_REG2_UNMERGE
	  printf("IR2U: Loop end, NO HITS???\n");
#endif
	  /* No good hits? just end the list then */
	  if (removeOpenPaths)
	    {			/* throw it away! */
	      Seg2_endPoint(seg, &tempEnd);
	      if (SegmentList2_numSegs(accum) > 0)
		Seg2_startPoint(SegmentList2_seg(accum,0), &tempStart);

	      if (SegmentList2_numSegs(accum) <= 0 || !Point2_almostEqual2(&tempEnd, &tempStart))
		{		/* this path isn't closed throw it away and continue*/
		  SegmentList2_clear(accum);
		  if (SegmentList2_numSegs(inputAll) <= 0)
		    {
		      /* this accumulator is crap and you have no more segments, ditch accum and break */
		      SegmentList2Set_pop(output); /* throw away accum */
		      break;			   /* get out */
		    }

		  seg = SegmentList2_seg(inputAll, SegmentList2_numSegs(inputAll)-1);	/* steal the last segment from the end of the list */
		  SegmentList2_pop(inputAll,false);
		  continue;
		}
	    }

#ifdef DEBUG_REG2_UNMERGE
	  printf("IR2U: appending accumulator to output\n");
#endif

	  SegmentList2_appendCopy(accum, &seg, false);

	  /* start a new list and seg if possible */
	  if (SegmentList2_numSegs(inputAll) <= 0)
	    break; 		/* nothing left and you just finished up an accum, get out */

	  accum = SegmentList2Set_appendEmptyP(output);
	  seg = SegmentList2_seg(inputAll, SegmentList2_numSegs(inputAll)-1);	/* steal the last segment from the end of the list */
	  SegmentList2_pop(inputAll,false);
	}
    }

  /* uuuhhhhhh, I'm done now?? */
  return (numLists(output) > 0);
}


