#include "include/Set2.h"
#include "include/Containment.h"
#include "include/BlockTree.h"
#include <stdlib.h>
/*
typedef struct Set2
{
  BlockVec regions;
} Set2;
*/

#define GT_SET2_DO_EOMFAIL { puts("EOM"); exit(1); }

#define GT_SET2_DEFAULT_SIZE 1
#define GT_SET2_OVERSIZE 1
#define GT_SET2_DO_EOMFAIL { puts("EOM"); exit(1); }

#define GT_SET2_MALLOC(PTR,SIZE) if (((PTR) = malloc(SIZE)) == NULL) GT_SET2_DO_EOMFAIL

#define region(SET, IDX) Set2_region((SET), (IDX))
#define numRegions(SET) Set2_numRegions((SET))
#define lastRegion(SET) region((SET), numRegions((SET))-1)
#define firstRegion(SET) region((SET), 0)

bool GT_SET2_VALID(const Set2 * const set)
{
  unsigned int i;
  unsigned int numHoles, numUnboundeds, numNulls;
  assert(set != NULL);
  numNulls = numHoles = numUnboundeds = 0;
  

  for (i = 0; i < numRegions(set); ++i)
    {
      GT_REGION_VALID(&region(set,i));

      if (Region2_boundaryType(&region(set,i)) == HOLE)
	++numHoles;
      else if (Region2_boundaryType(&region(set,i)) == UNBOUNDED)
	++numUnboundeds;
      else if (Region2_boundaryType(&region(set,i)) == NULL_SET)
	++numNulls;
    }

  assert(numHoles + numUnboundeds <= 1); /* only one unbounded stle region per set */
  assert(numNulls == 0 || ( numNulls == 1 && numRegions(set) == 1) ); 
  
  return true;
}
unsigned int Set2_numSegments(const Set2 * const set)
{
  unsigned int i, accum;
  accum = 0;

  for (i = 0; i < numRegions(set); ++i)
    accum += Region2_numSegments(&region(set,i));
  
  return accum;
}
void Set2_initialize(Set2 * set);
void Set2_initialize2(Set2 * set, const unsigned int containerSize);

Set2 * Set2_create()
{
  return Set2_create1(GT_SET2_DEFAULT_SIZE);
}
void Set2_destroy(Set2 * set)
{
  GT_SET2_VALID(set);
  BlockVec_uninitialize((BlockVec*)set);
  free(set);
}
Set2 * Set2_create1(const unsigned int numRegions)
{
  Set2 * temp;
  
  assert(numRegions < 0xFFFFFFFF);
  
  if ((temp = malloc(sizeof(Set2))) == NULL)
    GT_SET2_DO_EOMFAIL;

  Set2_initialize2(temp, numRegions);

  GT_SET2_VALID(temp);
  return (temp);
}
void Set2_initialize(Set2 * set)
{
  Set2_initialize2(set, GT_SET2_DEFAULT_SIZE);
}
void Set2_initialize2(Set2 * set, const unsigned int containerSize)
{
  assert( containerSize > 0);
  assert( set != NULL );

  BlockVec_initialize((BlockVec*)set, sizeof(Region2));
  if (containerSize > 0)
    BlockVec_resize((BlockVec*)set, containerSize, GT_SET2_OVERSIZE);

  assert(Set2_numRegions(set) == 0);
  
  GT_SET2_VALID(set);
}
void Set2_reallocate(Set2 * set, const unsigned int containerSize)
{
  BlockVec_resize((BlockVec*)set, containerSize, GT_SET2_OVERSIZE);
  GT_SET2_VALID(set);
}
void Set2_clear(Set2 * set)
{
  unsigned int i;
  for (i = 0; i < numRegions(set); ++i)
    Region2_destroy(&(region(set,i)));

  BlockVec_clear((BlockVec*)set);
}
unsigned int Set2_appendEmpty(Set2 * set)
{
  GT_SET2_VALID(set);
  BlockVec_appendNull((BlockVec*)set);
  Region2_initialize(&(lastRegion(set)));
  return Set2_numRegions(set)-1;
}
Region2 * Set2_appendEmptyP(Set2 * set)
{
  GT_SET2_VALID(set);
  unsigned int res;
  res = Set2_appendEmpty(set);
  return &(region(set, res));
}
unsigned int Set2_appendCopy(Set2 * set, const Region2 * const input)
{
  unsigned int setIdx;
  GT_SET2_VALID(set);
  GT_REGION_VALID(input);
  
  setIdx = Set2_appendEmpty(set);
  Region2_clone(input, &region(set, setIdx));
  return setIdx;
}
unsigned int Set2_appendCopyAsRegionNoWrap(Set2 * set, const SegmentList2 * const input)
{
  unsigned int setIdx;
  GT_SET2_VALID(set);
  GT_SEGLIST_VALID1(input);

  setIdx = Set2_appendEmpty(set);
  Region2_appendCopy(&region(set, setIdx), input);
  return setIdx;
}
unsigned int Set2_appendCopyAsRegion(Set2 * set, const SegmentList2 * const input)
{
  unsigned int setIdx;
  SegmentList2 * temp;
  SegmentList2Type type;
  GT_SET2_VALID(set);
  GT_SEGLIST_VALID1(input);
  
  setIdx = Set2_appendEmpty(set);
  if (SegmentList2_calcType(input, &type) && type == HOLE)
    {
      temp = SegmentList2_createType(UNBOUNDED);
      Region2_appendCopy(&region(set, setIdx), temp);
      Region2_appendCopy(&region(set, setIdx), input);
      SegmentList2_destroy(temp);
    }
  else
    {
      Region2_appendCopy(&region(set, setIdx), input);
    }
  GT_REGION_VALID(&region(set,setIdx));
  return setIdx;
}
Region2 * Set2_appendCopyAsRegionP(Set2 * set, const SegmentList2 * const input)
{
  unsigned int setIdx;
  setIdx = Set2_appendCopyAsRegion(set, input);
  return &region(set, setIdx);
}
Region2 * Set2_appendTypeAsRegionP(Set2 * set, const SegmentList2Type type)
{
  Region2 * region;
  SegmentList2 * shell;
  region = Set2_appendEmptyP(set);
  shell = Region2_appendEmptyP(region);
  SegmentList2_setType(shell, type);
  return region;  
}
void Set2_appendCopySet(Set2 * set, const Set2 * const input)
{
  Region2 * newReg;
  int i;
  GT_SET2_VALID(set);
  GT_SET2_VALID(input);

  for (i = 0; i < Set2_numRegions(input); ++i)
    {
      newReg = Set2_appendEmptyP(set);
      Region2_clone(&Set2_region(input,i), newReg);  
    }
}
void Set2_translate(Set2 * set, const Vec2 delta)
{
  int i;
  GT_SET2_VALID(set);

  for (i = 0; i < Set2_numRegions(set); ++i)
    Region2_translate(&Set2_region(set,i), delta);
}
SegmentList2Type  Set2_boundaryType(const Set2 * const set)
{
  SegmentList2Type type;
  unsigned int i;
  GT_SET2_VALID(set);

  type = NULL_SET;
  for (i = 0; i < numRegions(set); ++i)
    {
      type = Region2_boundaryType(&region(set,i));
      if (type == UNBOUNDED || type == HOLE)
	  break;
    }
  return type;
}
void Set2_cloneAllSegsToList(const Set2 * const set, SegmentList2 * accum, bool refreshCache)
{
  unsigned int i;
  /* unsigned int numHoles, numUnboundeds, numNulls; */
  GT_SET2_VALID(set);
  GT_SEGLIST_VALID1(accum);

  for (i = 0; i < numRegions(set); ++i)
    Region2_cloneAllSegsToList(&region(set,i), accum, false);

  if (refreshCache)
    SegmentList2_refreshCache(accum);
}
BlockTree * Internal_containmentTree(SegmentList2Set * uniqueList)
{
  BlockTree * head;
  unsigned int i, j;
  BlockTree * tempNode, * otherNode;
  SegmentList2 * myList, *otherList;
  bool placed, rerun;

  head = BlockTree_create();
  /* BlockTree_initialize(&head); */

  /* run through and cache everybody as a bounded for ease */
  for (i = 0; i < Region2_numLists(uniqueList); ++i)
    {
      assert(SegmentList2_cacheValid(Region2_listp(uniqueList,i)));
      assert(SegmentList2_type(Region2_listp(uniqueList,i)) == HOLE ||
  	     SegmentList2_type(Region2_listp(uniqueList,i)) == BOUNDED);
      SegmentList2_cacheBounded(Region2_listp(uniqueList,i));
      assert(Region2_listp(uniqueList,i)->boundedCache);
      assert(SegmentList2_type(Region2_listp(uniqueList,i)->boundedCache) == BOUNDED);
    }
  
  for (i = 0; i < Region2_numLists(uniqueList); ++i)
    {
      myList = Region2_listp(uniqueList,i)->boundedCache;
      assert(SegmentList2_type(myList) == BOUNDED);
      tempNode = head;
      placed = false;

      while (!placed)
	{
	  /* find out if I fit inside a child of this node or not */
	  rerun = false;
	  for (j = 0; j < BlockTree_numChildren(tempNode); ++j)
	    {
	      otherNode = BlockTree_child(tempNode, j);
	      assert(otherNode != NULL);
	      assert(otherNode->item != NULL);
	      GT_SEGLIST_VALID1(otherNode->item);
	      otherList = ((SegmentList2*)otherNode->item)->boundedCache;
	      /* otherList = ((SegmentList2*)otherNode->item); */
	      assert(SegmentList2_type(otherList) == BOUNDED);
	      
	      if (Containment_SLSL(otherList, myList, true))
		{		/* I belong in THIS child! */
		  tempNode = otherNode;
		  rerun = true;
		  break;
		}
	      else if (Containment_SLSL(myList, otherList, true))
		{		/* this child belongs in ME */
		  tempNode = BlockTree_subsumeChild(tempNode, j);
		  tempNode->item = Region2_listp(uniqueList,i);
		  placed = true;
		  break;
		}
	     else
	       {
		 /* this is my sibling??? */
		 assert(Distinct_SLSL(myList, otherList, true));
		 BlockTree_addChildData(tempNode, Region2_listp(uniqueList,i));
		 placed = true;
		 break;
	       }
	      Containment_SLSL(otherList, myList, true);
	      assert(0); 	/* non-distinct boundeds detected, how did this happen, input error */
	    }
	  if (!placed && !rerun)
	    {			/* didn't place the list yet, also don't belong 
				   in anything at this level, therefore these are
				   my sibling */
	      BlockTree_addChildData(tempNode, Region2_listp(uniqueList,i));
	      placed = true; /* placed! All done */
	    }
	}
    }

  return head;
}
bool Internal_levelSelect_populate(const int levelSelect, BlockTree * currentNode, Set2 * accumulator, Region2 * currentRegion, const int currentLevel)
{
  SegmentList2 * list;
  SegmentList2Type type;
  unsigned int i;
  char levelDelta;
  
  GT_SET2_VALID(accumulator);
  assert(currentNode != NULL);
  
  list = (SegmentList2 *)(currentNode->item);
  assert(SegmentList2_cacheValid(list));
  type = SegmentList2_type(list);

  assert(type != PATH); 	/* deal with this later */
  
  levelDelta = 0; 		/* NULL_SET */
  if (type == BOUNDED || type == UNBOUNDED)
    levelDelta = 1;
  else if (type == HOLE)
    levelDelta = -1;

  /* Am I accumulating a region now? */
  if (currentRegion)
    {
      GT_REGION_VALID(currentRegion);
      assert(type != UNBOUNDED); /* this would be double extra wrong */
      /* assert(type != NULL_SET);	 /\* this would be pointless - but allowable*\/ */
      if (levelDelta == -1 && currentLevel == levelSelect)
	{
	  /* I am a hole and I should be punching myself into the currentRegion */
	  assert(type == BOUNDED || (type == HOLE && Region2_numLists(currentRegion) > 0)); /* don't let a hole be the first thing you add */
	  Region2_appendCopy(currentRegion, list);
	  GT_REGION_VALID(currentRegion);
	  /* wipe the currentRegion reference, my children (if I have any) won't belong in this region accumulator */
	  currentRegion = NULL;
	}
    }
  else 				/* if (!currentRegion) */
    {
      if (levelDelta == 1 && levelDelta+currentLevel == levelSelect)
	{
	  /* time to make a region! */
	  assert(type != HOLE); 	/* don't let the first thing you add be a hole */
	  currentRegion = Set2_appendCopyAsRegionP(accumulator, list);
	  GT_REGION_VALID(currentRegion);
	}
    }

  /* now run through all my children */
  for (i = 0; i < BlockTree_numChildren(currentNode); ++i)
    {
      if (!Internal_levelSelect_populate(levelSelect, BlockTree_child(currentNode, i), accumulator, currentRegion, currentLevel+levelDelta))
	return false;
    }
  return true;
}
bool Internal_levelSelect(const int levelSelect, SegmentList2Set * uniqueList, Set2 * output, const int unboundedLevel)/* bool frameInUnbounded) */
{
  /* given a unique set, containment tree it and pic out the shapes at the proper containtment level */
  bool res;
  BlockTree * head;
  Region2 * initRegion;

  head = Internal_containmentTree(uniqueList);
  assert(head->item == NULL);
  head->item = SegmentList2_createType(NULL_SET);
  
  /* TODO: probably don't need this.... */
  /* if (frameInUnbounded) */
  if (unboundedLevel >= levelSelect && unboundedLevel > 0)
      initRegion = Set2_appendTypeAsRegionP(output, UNBOUNDED);
  else
      initRegion = NULL;


  res = Internal_levelSelect_populate(levelSelect, head, output, initRegion, unboundedLevel);

  GT_SET2_VALID(output);
  SegmentList2_destroy(head->item);
  BlockTree_destroy(head, false);
  
  return res;
}
bool Set2_unionSLSL(const SegmentList2 * const A, const SegmentList2 * const B, Set2 * output)
{
  SegmentList2Type typeA, typeB;
  /* Region2 * outputRegion; */
  SegmentList2Set * unique;
  SegmentList2 * all;
  bool res;

  GT_SEGLIST_VALID1(A);
  GT_SEGLIST_VALID1(B);
  GT_SET2_VALID(output);
  assert(SegmentList2_cacheValid(A));
  assert(SegmentList2_cacheValid(B));

  Set2_clear(output);
  
  if (SegmentList2_calcType(A, &typeA) == false)
    return false;
  
  if (SegmentList2_calcType(B, &typeB) == false)
    return false;
  
  assert(typeA != PATH && typeB != PATH); /* I don't quite know what the proper behaviro here is, probaly just refuse */
  if (typeA == PATH || typeB == PATH)
    return false;

  res = true; 		/* assume success */
  if (typeA == NULL_SET || typeB == UNBOUNDED)
    {				/* A is null, whatever B is, return it (or be is unbounded, so return it)*/
      Set2_appendCopyAsRegion(output, B);
    }
  else if (typeB == NULL_SET || typeA == UNBOUNDED)
    {
      Set2_appendCopyAsRegion(output, A);
    }
  /* TODO: possibly comment this distict check out? the other approach can handle it- not sure at what cost though */
  else if (Distinct_SLSL(A, B, true))
    {
      /* these two shapes are distinct, so their union is just them added together */
      Set2_appendCopyAsRegion(output, A);
      GT_SET2_VALID(output);
      Set2_appendCopyAsRegion(output, B);
    }
  else
    {
      /* not distinct, not easy, have to do hard work on this case */
      all = SegmentList2_createClone(A);
      SegmentList2_appendCopyList(all, B, false); /* don't update cache */

      unique = Region2_create();
      if (!SegmentList2Set_makeUnique_destructive(all, unique, true) ||  /* don't return open paths */
	  /*       Region2_refreshCache(unique); -- might need this, not sure */
	  !Internal_levelSelect(1, unique, output, (typeA == HOLE) + (typeB == HOLE))) /* don't frame it in an unbounded.... unless one of them is a hole */
	{
	  res = false; 	/* well, apparently that failed */
	}
      /* cleanup the workspace */
      SegmentList2_destroy(all);
      Region2_destroy(unique);
    }
  GT_SET2_VALID(output);
  return res;
}
bool Set2_intersectSLSL(const SegmentList2 * const A, const SegmentList2 * const B, Set2 * output)
{
  SegmentList2Type typeA, typeB;
  SegmentList2 * all;
  Region2 * unique;
  bool res = true;

  GT_SEGLIST_VALID1(A);
  GT_SEGLIST_VALID1(B);
  GT_SET2_VALID(output);
  assert(SegmentList2_cacheValid(A));
  assert(SegmentList2_cacheValid(B));

  Set2_clear(output);
  
  if (SegmentList2_calcType(A, &typeA) == false)
    return false;
  
  if (SegmentList2_calcType(B, &typeB) == false)
    return false;
  
  assert(typeA != PATH && typeB != PATH); /* I don't quite know what the proper behaviro here is, probaly just refuse */
  if (typeA == PATH || typeB == PATH)
    return false;

  if (typeB == UNBOUNDED || typeA == NULL_SET)
    {				/* A is null, whatever B is, return it (or be is unbounded, so return it)*/
      Set2_appendCopyAsRegion(output, A);
    }
  else if (typeA == UNBOUNDED || typeB == NULL_SET)
    {
      Set2_appendCopyAsRegion(output, B);
    }
  /* TODO: possibly comment this distict check out and make Internal_unionSLSL** handle distinct bounding thingies?? */
  else if (Distinct_SLSL(A, B, true))
    {
      /* these two shapes are distinct, so their intersect is jack squat */
      /* all = SegmentList2_createType(NULL_SET); */
      /* Set2_appendCopyAsRegion(output, all); */
      /* SegmentList2_destroy(all); */
      return true;
    }
  else
    {
      /* not distinct, not easy, have to do hard work on this case */
      all = SegmentList2_createClone(A);
      SegmentList2_appendCopyList(all, B, false); /* don't update cache */
      unique = Region2_create();
      if (!SegmentList2Set_makeUnique_destructive(all, unique, true) ||  /* don't return open paths */
	  /*       Region2_refreshCache(unique); -- might need this, not sure */
	  !Internal_levelSelect(2, unique, output, (typeA == HOLE) + (typeB == HOLE))) /* frame it in an unbounded if one of them is a hole */
	{
	  res = false; 	/* well, apparently that failed */
	}
      /* cleanup the workspace */
      SegmentList2_destroy(all);
      Region2_destroy(unique);
    }
  GT_SET2_VALID(output);
  return res;
}
bool Set2_unionRSL(const Region2 * const A, const SegmentList2 * const B, Set2 * output)
{
  SegmentList2Type typeA, typeB;
  bool res;
  SegmentList2Set * unique;
  SegmentList2 * all;
  
  GT_REGION_VALID(A);
  GT_SEGLIST_VALID1(B);
  GT_SET2_VALID(output);
  assert(SegmentList2_cacheValid(B));
  
  Set2_clear(output);
  if (SegmentList2_calcType(B, &typeB) == false)
    return false;

  /* quick exit case, return B if A is NULL_SET or B is UNBOUNDED */
  res = true;
  typeA = Region2_boundaryType(A);
  assert(typeA != PATH && typeB != PATH);
  
  if (typeA == NULL_SET || typeB == UNBOUNDED)
    {
      Set2_appendCopyAsRegion(output, B);
    }
  else if (typeA == UNBOUNDED || typeB == NULL_SET)
    {
      Set2_appendCopy(output, A);
    }
  else if (Distinct_RSL(A, B, true))
    {
      Set2_appendCopy(output, A);
      Set2_appendCopyAsRegion(output, B);
    }
  else
    {
      /* not distinct, not easy, have to do hard work on this case */
      all = SegmentList2_createClone(B);
      Region2_cloneAllSegsToList(A, all, false); /* don't update cache */
      unique = Region2_create();
      if (!SegmentList2Set_makeUnique_destructive(all, unique, true) ||  /* don't return open paths */
	  /*       Region2_refreshCache(unique); -- might need this, not sure */
	  !Internal_levelSelect(1, unique, output, (typeA == HOLE) + (typeB == HOLE))) /* don't frame it in an unbounded.... unless one of them is a hole */
	{
	  res = false; 	/* well, apparently that failed */
	}
      /* cleanup the workspace */
      SegmentList2_destroy(all);
      Region2_destroy(unique);
    }
  GT_SET2_VALID(output);
  return res;
}
bool Set2_intersectRSL(const Region2 * const A, const SegmentList2 * const B, Set2 * output)
{
  SegmentList2Type typeA, typeB;
  bool res;
  SegmentList2Set * unique;
  SegmentList2 * all;
  
  GT_REGION_VALID(A);
  GT_SEGLIST_VALID1(B);
  GT_SET2_VALID(output);
  assert(SegmentList2_cacheValid(B));
  assert(SegmentList2_type(&Region2_list(A,0)) != HOLE);

  Set2_clear(output);
  if (SegmentList2_calcType(B, &typeB) == false)
    return false;

  typeA = SegmentList2_type(&Region2_list(A, 0));

  assert(typeA != PATH && typeB != PATH);
  /* quick exit case, return B if A is NULL_SET or B is UNBOUNDED */
  res = true;
  if (typeA == NULL_SET || typeB == NULL_SET)
    {
      /* nothing - get outta here */
    }
  else if (typeB == UNBOUNDED)
    {
      Set2_appendCopy(output, A);
    }
  else if (typeA == UNBOUNDED)
    {
      Set2_appendCopyAsRegion(output, B);
    }
  else if (Distinct_RSL(A, B, true))
    {
      /* nothing! */
    }
  else
    {
      /* not distinct, not easy, have to do hard work on this case */
      all = SegmentList2_createClone(B);
      Region2_cloneAllSegsToList(A, all, false); /* don't update cache */

      unique = Region2_create();
      if (!SegmentList2Set_makeUnique_destructive(all, unique, true) ||  /* don't return open paths */
	  /*       Region2_refreshCache(unique); -- might need this, not sure */
	  !Internal_levelSelect(2, unique, output, (typeA == HOLE) + (typeB == HOLE))) /* don't frame it in an unbounded.... unless one of them is a hole */
	{
	  res = false; 	/* well, apparently that failed */
	}
      /* cleanup the workspace */
      SegmentList2_destroy(all);
      Region2_destroy(unique);
    }
  GT_SET2_VALID(output);
  return res;
}
bool Set2_unionRR(const Region2* const A, const Region2 * const B, Set2 * output)
{
  SegmentList2Type typeA, typeB;
  bool res;
  SegmentList2 * all;
  SegmentList2Set * unique;
  
  GT_REGION_VALID(A);
  GT_REGION_VALID(B);
  GT_SET2_VALID(output);

  assert(SegmentList2_cacheValid(&Region2_list(A, 0)));
  assert(SegmentList2_cacheValid(&Region2_list(B, 0)));

  Set2_clear(output);
  typeA = Region2_boundaryType(A);
  typeB = Region2_boundaryType(B);

  assert(typeA != PATH && typeB != PATH);

  res = true;
  if (typeA == NULL_SET)
    {
      if (typeB != NULL_SET)
	{
	  Set2_appendCopy(output, B);
	}
    }
  else if (typeB == NULL_SET)
    {
      Set2_appendCopy(output, A);
    }
  else if (typeA == UNBOUNDED)
    {
      Set2_appendCopy(output,A);
    }
  else if (typeB == UNBOUNDED)
    {
      Set2_appendCopy(output,B);
    }
  else if (Distinct_RR(A,B,true))
    {
      Set2_appendCopy(output, A);
      Set2_appendCopy(output, B);
    }
  else
    {
      all = SegmentList2_create();
      Region2_cloneAllSegsToList(A, all, false); /* don't update cache */
      Region2_cloneAllSegsToList(B, all, false);
      
      unique = Region2_create();
      if (!SegmentList2Set_makeUnique_destructive(all, unique, true) ||  /* don't return open paths */
	  /*       Region2_refreshCache(unique); -- might need this, not sure */
	  !Internal_levelSelect(1, unique, output, (typeA == UNBOUNDED) + (typeB == HOLE))) /* don't frame it in an unbounded.... unless one of them is a hole */
	{
	  res = false; 	/* well, apparently that failed */
	}
      /* cleanup the workspace */
      SegmentList2_destroy(all);
      Region2_destroy(unique);
    }
  GT_SET2_VALID(output);
  return res;
}
bool Set2_intersectRR(const Region2* const A, const Region2 * const B, Set2 * output)
{
  SegmentList2Type typeA, typeB;
  bool res;
  SegmentList2 * all;
  SegmentList2Set * unique;
  
  GT_REGION_VALID(A);
  GT_REGION_VALID(B);
  GT_SET2_VALID(output);

  assert(SegmentList2_cacheValid(&Region2_list(A, 0)));
  assert(SegmentList2_cacheValid(&Region2_list(B, 0)));

  Set2_clear(output);
  typeA = Region2_boundaryType(A);
  typeB = Region2_boundaryType(B);

  assert(typeA != PATH && typeB != PATH);

  res = true;
  if (typeA == NULL_SET || typeB == NULL_SET)
    {
      /* nothing */
    }
  else if (typeB == UNBOUNDED)
    {
      Set2_appendCopy(output, A);
    }
  else if (typeA == UNBOUNDED)
    {
      Set2_appendCopy(output, B);
    }
  else if (Distinct_RR(A,B,true))
    {
      /* nothing! */
    }
  else
    {
      all = SegmentList2_create();
      Region2_cloneAllSegsToList(A, all, false); /* don't update cache */
      Region2_cloneAllSegsToList(B, all, false);
      
      unique = Region2_create();
      if (!SegmentList2Set_makeUnique_destructive(all, unique, true) ||  /* don't return open paths */
	  /*       Region2_refreshCache(unique); -- might need this, not sure */
	  !Internal_levelSelect(1, unique, output, (typeA == UNBOUNDED) + (typeB == HOLE))) /* don't frame it in an unbounded.... unless one of them is a hole */
	{
	  res = false; 	/* well, apparently that failed */
	}
      /* cleanup the workspace */
      SegmentList2_destroy(all);
      Region2_destroy(unique);
    }
  GT_SET2_VALID(output);
  return res;
}
bool Set2_unionSSL(const Set2 * const A, const SegmentList2 * const B, Set2 * output)
{
  SegmentList2Type typeA, typeB;
  bool res;
  SegmentList2Set * unique;
  SegmentList2 * all;
  
  GT_SET2_VALID(A);
  GT_SEGLIST_VALID1(B);
  GT_SET2_VALID(output);
  assert(SegmentList2_cacheValid(B));
  
  Set2_clear(output);
  if (SegmentList2_calcType(B, &typeB) == false)
    return false;

  /* quick exit case, return B if A is NULL_SET or B is UNBOUNDED */
  res = true;
  typeA = Set2_boundaryType(A);
  assert(typeA != PATH && typeB != PATH);
  
  if (typeA == NULL_SET || typeB == UNBOUNDED)
    {
      Set2_appendCopyAsRegion(output, B);
    }
  else if (typeA == UNBOUNDED || typeB == NULL_SET)
    {
      Set2_appendCopySet(output, A);
    }
  else if (Distinct_SSL(A, B, true))
    {
      Set2_appendCopySet(output, A);
      Set2_appendCopyAsRegion(output, B);
    }
  else
    {
      /* not distinct, not easy, have to do hard work on this case */
      all = SegmentList2_createClone(B);
      Set2_cloneAllSegsToList(A, all, false); /* don't update cache */
      unique = Region2_create();
      if (!SegmentList2Set_makeUnique_destructive(all, unique, true) ||  /* don't return open paths */
	  /*       Region2_refreshCache(unique); -- might need this, not sure */
	  !Internal_levelSelect(1, unique, output, (typeA == HOLE) + (typeB == HOLE))) /* don't frame it in an unbounded.... unless one of them is a hole */
	{
	  res = false; 	/* well, apparently that failed */
	}
      /* cleanup the workspace */
      SegmentList2_destroy(all);
      Region2_destroy(unique);
    }
  GT_SET2_VALID(output);
  return res;
}
bool Set2_intersectSSL(const Set2 * const A, const SegmentList2 * const B, Set2 * output)
{
  SegmentList2Type typeA, typeB;
  bool res;
  SegmentList2Set * unique;
  SegmentList2 * all;
  
  GT_SET2_VALID(A);
  GT_SEGLIST_VALID1(B);
  GT_SET2_VALID(output);
  assert(SegmentList2_cacheValid(B));

  Set2_clear(output);
  if (SegmentList2_calcType(B, &typeB) == false)
    return false;

  typeA = Set2_boundaryType(A);

  assert(typeA != PATH && typeB != PATH);
  /* quick exit case, return B if A is NULL_SET or B is UNBOUNDED */
  res = true;
  if (typeA == NULL_SET || typeB == NULL_SET)
    {
      /* nothing - get outta here */
    }
  else if (typeB == UNBOUNDED)
    {
      Set2_appendCopySet(output, A);
    }
  else if (typeA == UNBOUNDED)
    {
      Set2_appendCopyAsRegion(output, B);
    }
  else if (Distinct_SSL(A, B, true))
    {
      /* nothing! */
    }
  else
    {
      /* not distinct, not easy, have to do hard work on this case */
      all = SegmentList2_createClone(B);
      Set2_cloneAllSegsToList(A, all, false); /* don't update cache */

      unique = Region2_create();
      if (!SegmentList2Set_makeUnique_destructive(all, unique, true) ||  /* don't return open paths */
	  /*       Region2_refreshCache(unique); -- might need this, not sure */
	  !Internal_levelSelect(2, unique, output, (typeA == HOLE) + (typeB == HOLE))) /* don't frame it in an unbounded.... unless one of them is a hole */
	{
	  res = false; 	/* well, apparently that failed */
	}
      /* cleanup the workspace */
      SegmentList2_destroy(all);
      Region2_destroy(unique);
    }
  GT_SET2_VALID(output);
  return res;
}
bool Set2_unionSR(const Set2 * const A, const Region2 * const B, Set2 * output)
{
  SegmentList2Type typeA, typeB;
  bool res;
  SegmentList2Set * unique;
  SegmentList2 * all;
  
  GT_SET2_VALID(A);
  GT_REGION_VALID(B);
  GT_SET2_VALID(output);

  Set2_clear(output);

  res = true;
  typeA = Set2_boundaryType(A);
  typeB = Region2_boundaryType(B);
  assert(typeA != PATH && typeB != PATH);
  
  if (typeA == NULL_SET || typeB == UNBOUNDED)
    {
      Set2_appendCopy(output, B);
    }
  else if (typeA == UNBOUNDED || typeB == NULL_SET)
    {
      Set2_appendCopySet(output, A);
    }
  else if (Distinct_SR(A, B, true))
    {
      Set2_appendCopySet(output, A);
      Set2_appendCopy(output, B);
    }
  else
    {
      /* not distinct, not easy, have to do hard work on this case */
      all = SegmentList2_create1(Region2_numSegments(B)+Set2_numSegments(A));
      Region2_cloneAllSegsToList(B, all, false);
      Set2_cloneAllSegsToList(A, all, false); 
      unique = Region2_create();
      if (!SegmentList2Set_makeUnique_destructive(all, unique, true) ||  /* don't return open paths */
	  /*       Region2_refreshCache(unique); -- might need this, not sure */
	  !Internal_levelSelect(1, unique, output, (typeA == HOLE) + (typeB == HOLE))) /* don't frame it in an unbounded.... unless one of them is a hole */
	{
	  res = false; 	/* well, apparently that failed */
	}
      /* cleanup the workspace */
      SegmentList2_destroy(all);
      Region2_destroy(unique);
    }
  GT_SET2_VALID(output);
  return res;
}
bool Set2_intersectSR(const Set2 * const A, const Region2 * const B, Set2 * output)
{
  SegmentList2Type typeA, typeB;
  bool res;
  SegmentList2Set * unique;
  SegmentList2 * all;
  
  GT_SET2_VALID(A);
  GT_REGION_VALID(B);
  GT_SET2_VALID(output);

  Set2_clear(output);

  typeA = Set2_boundaryType(A);
  typeB = Region2_boundaryType(B);

  assert(typeA != PATH && typeB != PATH);
  /* quick exit case, return B if A is NULL_SET or B is UNBOUNDED */
  res = true;
  if (typeA == NULL_SET || typeB == NULL_SET)
    {
      /* nothing - get outta here */
    }
  else if (typeB == UNBOUNDED)
    {
      Set2_appendCopySet(output, A);
    }
  else if (typeA == UNBOUNDED)
    {
      Set2_appendCopy(output, B);
    }
  else if (Distinct_SR(A, B, true))
    {
      /* nothing! */
    }
  else
    {
      /* not distinct, not easy, have to do hard work on this case */
      all = SegmentList2_create1(Region2_numSegments(B) + Set2_numSegments(A));
      Region2_cloneAllSegsToList(B, all, false);
      Set2_cloneAllSegsToList(A, all, false); /* don't update cache */

      unique = Region2_create();
      if (!SegmentList2Set_makeUnique_destructive(all, unique, true) ||  /* don't return open paths */
	  /*       Region2_refreshCache(unique); -- might need this, not sure */
	  !Internal_levelSelect(2, unique, output, (typeA == HOLE) + (typeB == HOLE))) /* don't frame it in an unbounded.... unless one of them is a hole */
	{
	  res = false; 	/* well, apparently that failed */
	}
      /* cleanup the workspace */
      SegmentList2_destroy(all);
      Region2_destroy(unique);
    }
  GT_SET2_VALID(output);
  return res;
}
bool Set2_unionSS(const Set2 * const A, const Set2 * const B, Set2 * output)
{
  SegmentList2Type typeA, typeB;
  bool res;
  SegmentList2Set * unique;
  SegmentList2 * all;
  
  GT_SET2_VALID(A);
  GT_SET2_VALID(B);
  GT_SET2_VALID(output);

  Set2_clear(output);

  res = true;
  typeA = Set2_boundaryType(A);
  typeB = Set2_boundaryType(B);
  assert(typeA != PATH && typeB != PATH);
  
  if (typeA == NULL_SET || typeB == UNBOUNDED)
    {
      Set2_appendCopySet(output, B);
    }
  else if (typeA == UNBOUNDED || typeB == NULL_SET)
    {
      Set2_appendCopySet(output, A);
    }
  else if (Distinct_SS(A, B, true))
    {
      Set2_appendCopySet(output, A);
      Set2_appendCopySet(output, B);
    }
  else
    {
      /* not distinct, not easy, have to do hard work on this case */
      all = SegmentList2_create1(Set2_numSegments(B)+Set2_numSegments(A));
      Set2_cloneAllSegsToList(B, all, false);
      Set2_cloneAllSegsToList(A, all, false); 
      unique = Region2_create();
      if (!SegmentList2Set_makeUnique_destructive(all, unique, true) ||  /* don't return open paths */
	  /*       Region2_refreshCache(unique); -- might need this, not sure */
	  !Internal_levelSelect(1, unique, output, (typeA == HOLE) + (typeB == HOLE))) /* don't frame it in an unbounded.... unless one of them is a hole */
	{
	  res = false; 	/* well, apparently that failed */
	}
      /* cleanup the workspace */
      SegmentList2_destroy(all);
      Region2_destroy(unique);
    }
  GT_SET2_VALID(output);
  return res;
}
bool Set2_intersectSS(const Set2 * const A, const Set2 * const B, Set2 * output)
{
  SegmentList2Type typeA, typeB;
  bool res;
  SegmentList2Set * unique;
  SegmentList2 * all;
  
  GT_SET2_VALID(A);
  GT_SET2_VALID(B);
  GT_SET2_VALID(output);

  Set2_clear(output);

  typeA = Set2_boundaryType(A);
  typeB = Set2_boundaryType(B);

  assert(typeA != PATH && typeB != PATH);
  /* quick exit case, return B if A is NULL_SET or B is UNBOUNDED */
  res = true;
  if (typeA == NULL_SET || typeB == NULL_SET)
    {
      /* nothing - get outta here */
    }
  else if (typeB == UNBOUNDED)
    {
      Set2_appendCopySet(output, A);
    }
  else if (typeA == UNBOUNDED)
    {
      Set2_appendCopySet(output, B);
    }
  else if (Distinct_SS(A, B, true))
    {
      /* nothing! */
    }
  else
    {
      /* not distinct, not easy, have to do hard work on this case */
      all = SegmentList2_create1(Set2_numSegments(B) + Set2_numSegments(A));
      Set2_cloneAllSegsToList(B, all, false);
      Set2_cloneAllSegsToList(A, all, false); /* don't update cache */

      unique = Region2_create();
      if (!SegmentList2Set_makeUnique_destructive(all, unique, true) ||  /* don't return open paths */
	  /*       Region2_refreshCache(unique); -- might need this, not sure */
	  !Internal_levelSelect(2, unique, output, (typeA == HOLE) + (typeB == HOLE))) /* don't frame it in an unbounded.... unless one of them is a hole */
	{
	  res = false; 	/* well, apparently that failed */
	}
      /* cleanup the workspace */
      SegmentList2_destroy(all);
      Region2_destroy(unique);
    }
  GT_SET2_VALID(output);
  return res; 
}
bool Set2_invertR(const Region2 * const region, Set2 * output)
{
  Set2 * temp;
  bool res;
  temp = Set2_create();
  Set2_appendCopy(temp, region);

  res = Set2_invert(temp, output);
  Set2_destroy(temp);
  return res;  
}
bool Set2_invert(const Set2 * const set, Set2 * output)
{
  /* to do this, for each region in the set, flip all the segment lists to get a reversed region */
  /* then return the intersection of all the regions with a level select of numRegions */
  SegmentList2Set * uniqueList;
  Region2 * tempRegion;
  unsigned int i, j, unboundedCount;
  SegmentList2 * tempList;
  SegmentList2Type myType;
  bool res;
  GT_SET2_VALID(set);
  GT_SET2_VALID(output);

  res = true;
  unboundedCount = 0;
  uniqueList = Region2_create();

  /* boundedCount = 0; */
  for (i = 0; i < numRegions(set) && res; ++i)
    {
      tempRegion = &region(set,i);
      GT_REGION_VALID(tempRegion);

      for (j = 0; j < Region2_numLists(tempRegion); ++j)
	{
	  if (!SegmentList2_calcType(&Region2_list(tempRegion, j), &myType))
	    {
	      res = false;
	      break;
	    }

	  if (j == 0 && myType == BOUNDED) /* after inversion, these are unbounded */
	    ++unboundedCount;

	  assert((myType == BOUNDED || myType == HOLE) || (j == 0 && myType == UNBOUNDED));
	  if (myType == BOUNDED || myType == HOLE)
	    {
	      tempList = Region2_appendEmptyP(uniqueList);
	      SegmentList2_flip2(&Region2_list(tempRegion,j), tempList);
	    }
	}
    }
  
  if (res && !Internal_levelSelect(numRegions(set), uniqueList, output, unboundedCount))
    res = false;

  Region2_destroy(uniqueList);
  return res;
}

bool Set2_expandSL(const SegmentList2 * const segs, const gtfloat expansion, Set2 * output)
{
  /* blind expand, the unique it, then ... something.... it...
     step 1
     bool        SegmentList2_blindExpansion(const SegmentList2 * const input, const gtfloat growth, SegmentList2 * output);
     
     step 2
     bool SegmentList2Set_makeUnique_destructive(SegmentList2 * inputAll, SegmentList2Set * output, bool removeOpenPaths);
     bool SegmentList2Set_makeUnique(const SegmentList2Set * const input, SegmentList2Set * output);

     step 3
     bool Internal_levelSelect(const int levelSelect, SegmentList2Set * uniqueList, Set2 * output, const int unboundedLevel)
     unbounded level == (inputType == HOLE)
     levelSelect = 1
  */
  SegmentList2 *blindExpand;
  SegmentList2Set * uniqueSet;
  SegmentList2Type myType;
  bool res;

  GT_SEGLIST_VALID1(segs);
  assert(expansion != 0);
  res = false;
  
  Set2_clear(output);
  if (SegmentList2_calcType(segs, &myType) == false)
    return false;

  if (myType == UNBOUNDED || myType == NULL_SET)
    {
      assert(0); 		/* program this, technically valid */
      res = false;
    }
  else if (myType == BOUNDED || myType == HOLE)
    {
      blindExpand = SegmentList2_create();
      if (SegmentList2_blindExpansion(segs, expansion, blindExpand))
	{
	  uniqueSet = Region2_create();
	  if (SegmentList2Set_makeUnique_destructive(blindExpand, uniqueSet, true))
	    {
	      res = Internal_levelSelect(1, uniqueSet, output, (myType == HOLE));
	    }
	  Region2_destroy(uniqueSet);
	}
      SegmentList2_destroy(blindExpand);
    }
  else
    {
      assert(0); 		/* why are you calling me with this non-normal type?? */
      res = false;
    }

  GT_SET2_VALID(output);
  return res;
}
bool Set2_expandR(const Region2 * const region, const gtfloat expansion, Set2 * output)
{
  SegmentList2 * blindExpand;
  SegmentList2Set * uniqueSet;
  SegmentList2Type myType;
  bool res;

  GT_REGION_VALID(region);
  assert(expansion != 0);
  res = false;
  
  Set2_clear(output);
  myType = Region2_boundaryType(region);

  assert(myType == BOUNDED || myType == UNBOUNDED);

  blindExpand = SegmentList2_create();
  if (Region2_blindExpandToList(region, expansion, blindExpand) == false)
    {
      uniqueSet = Region2_create();
      if (SegmentList2Set_makeUnique_destructive(blindExpand, uniqueSet, true))
	{  
	  res = Internal_levelSelect(1, uniqueSet, output, (myType == HOLE));
	}
      Region2_destroy(uniqueSet);
    }
  SegmentList2_destroy(blindExpand);
  GT_SET2_VALID(output);
  return res;
}
bool Set2_expandS(const Set2 * const set, const gtfloat expansion, Set2 * output)
{
  SegmentList2 * blindExpand;
  SegmentList2Set * uniqueSet;
  SegmentList2Type myType;
  bool res;

  GT_SET2_VALID(set);
  assert(expansion != 0);
  res = false;
  
  Set2_clear(output);
  myType = Set2_boundaryType(set);
  
  assert(myType == BOUNDED || myType == UNBOUNDED);
  
  blindExpand = SegmentList2_create();
  if (Set2_blindExpandToList(set, expansion, blindExpand) == false)
    {
      uniqueSet = Region2_create();
      if (SegmentList2Set_makeUnique_destructive(blindExpand, uniqueSet, true))
	{  
	  res = Internal_levelSelect(1, uniqueSet, output, (myType == HOLE));
	}
      Region2_destroy(uniqueSet);
    }
  SegmentList2_destroy(blindExpand);

  GT_SET2_VALID(output);
  return res;
}
bool Set2_blindExpandToList(const Set2 * const input, const gtfloat growth, SegmentList2 * accum)
{
  bool res;
  unsigned int i;
  
  GT_SET2_VALID(input);
  assert(growth != 0);
  GT_SEGLIST_VALID1(accum);
  
  res = true;

  for (i = 0; i < numRegions(input) && res; ++i)
      res = Region2_blindExpandToList(&region(input, i), growth, accum);

  return res;
}
