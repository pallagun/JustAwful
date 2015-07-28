#ifndef __GT_SEGMENTLIST2SET_H__
#define __GT_SEGMENTLIST2SET_H__ 1

#include "SegmentList2.h"
#include "BlockVec.h"

#define GT_REGION2_DEFAULT_SIZE 8
#define GT_REGION2_DEFAULT_GROWTH 8
#define GT_REGION2_DEFAULT_OVERSIZE 2

/* region2 represents a continuous region of 2d space */

typedef struct Region2
{
  BlockVec lists;
} Region2;
typedef Region2 SegmentList2Set;


#define GT_REGION2_VALID(A) assert(A != NULL)

/* constructors */
Region2 *         Region2_create();
Region2 *         Region2_create1(const unsigned int containerSize);
Region2 *         Region2_createClone(const Region2 * const input);
void              Region2_initialize(Region2 * set);
void              Region2_initialize2(Region2 * set, const unsigned int containerSize);
void              Region2_reallocate(Region2 * set, const unsigned int containerSize);
/* destructors */
void              Region2_destroy(Region2 * set);
void              Region2_uninitialize(Region2 * set);
/* clear/clone/realloc */
void              Region2_clear(Region2 * set);
/* TODO: change the order of the args in this function */
void              Region2_clone(const Region2 * const source, Region2 * dest);
/* basic stuff */
/* #define GT_REGION_VALID(REGION) assert((REGION) != NULL); GT_BLOCKVEC_VALID( (BlockVec*)(REGION) ) */
bool           GT_REGION_VALID(const Region2 * const source);
void              Region2_remove(Region2 * set, const unsigned int target);
void              Region2_removeRange(Region2 * set, const unsigned int startIdx, const unsigned int endIdx);
void              Region2_pop(Region2 * set);
void              SegmentList2Set_pop(SegmentList2Set * listSet);
void              Region2_insertCopy(Region2 * set, const SegmentList2 * const list, const unsigned int idx);
void              Region2_appendCopy(Region2 * set, const SegmentList2 * const list);
unsigned int      Region2_appendEmpty(Region2 * set);
SegmentList2 *    Region2_appendEmptyP(Region2 * set);
void              Region2_appendCopySet(Region2 * list, const SegmentList2Set * const otherList);
#define           Region2_numLists(REGION) ((REGION)->lists.numItems)
#define           Region2_list(REGION, IDX) (((SegmentList2 *)((REGION)->lists.item))[(IDX)])
#define           Region2_listp(REGION, IDX) (&Region2_list((REGION),(IDX)))
void              Region2_refreshCache(Region2 * reg);
SegmentList2Type  Region2_boundaryType(const Region2 * const reg);
bool           Region2_almostEqual(const Region2 * const A, const Region2 * const B, bool allowListOutOfOrder, bool allowSegmentsOutOfOrder);
void              Region2_translate(Region2 * reg, const Vec2 delta);
/*void              Region2_invert(X) - nope, can't do this, may not be a region when you're done */


unsigned int      Region2_numSegments(const Region2 * const set);
unsigned int      SegmentList2Set_numSegments(const SegmentList2Set * const set);
void		  Region2_cloneAllSegsToList(const Region2 * const region, SegmentList2 * accum, bool refreshCache);
bool           Region2_blindExpandToList(const Region2 * const input, const gtfloat growth, SegmentList2 * accum);


/* more complicated crap */

bool SegmentList2Set_makeUnique_destructive(SegmentList2 * inputAll, SegmentList2Set * output, bool removeOpenPaths);
bool SegmentList2Set_makeUnique(const SegmentList2Set * const input, SegmentList2Set * output);

#endif



