#ifndef __GT_SET2_H__
#define __GT_SET2_H__ 1

#include "BlockVec.h"
#include "Region2.h"

typedef struct Set2
{
  BlockVec regions;
} Set2;

/* #define GT_SET2_VALID(SET) GT_BLOCKVEC_VALID((BlockVec*)(SET)) */
bool GT_SET2_VALID(const Set2 * const set);

Set2 *  Set2_create();
Set2 *  Set2_create1(const unsigned int numRegions);
void    Set2_destroy(Set2 * set);

#define Set2_numRegions(SET) ((SET)->regions.numItems)
#define Set2_region(SET, IDX) (((Region2 *)((SET)->regions.item))[(IDX)])

unsigned int Set2_appendEmpty(Set2 * set);
Region2 * Set2_appendEmptyP(Set2 * set);
unsigned int Set2_appendCopy(Set2 * set, const Region2 * const input);
unsigned int Set2_appendCopyAsRegion(Set2 * set, const SegmentList2 * const input);
Region2 * Set2_appendCopyAsRegionP(Set2 * set, const SegmentList2 * const input);
Region2 * Set2_appendTypeAsRegionP(Set2 * set, const SegmentList2Type type);
SegmentList2Type  Set2_boundaryType(const Set2 * const set);

void    Set2_appendCopySet(Set2 * set, const Set2 * const input);
void	Set2_cloneAllSegsToList(const Set2 * const set, SegmentList2 * accum, bool refreshCache);
bool Set2_almostEqual2ordered(const Set2 * const A, const Set2 * const B);
bool Set2_almostEqual2unordered(const Set2 * const A, const Set2 * const B);
unsigned int Set2_numSegments(const Set2 * const set);
void    Set2_clear(Set2 * set);

void    Set2_translate(Set2 * set, const Vec2 delta);
bool Set2_invertR(const Region2 * const region, Set2 * output);
bool	Set2_invert(const Set2 * const set, Set2 * output);
bool Set2_blindExpandToList(const Set2 * const input, const gtfloat growth, SegmentList2 * accum);

bool Set2_expandSL(const SegmentList2 * const segs, const gtfloat expansion, Set2 * output);
bool Set2_expandR(const Region2 * const region, const gtfloat expansion, Set2 * output);
bool Set2_expandS(const Set2 * const set, const gtfloat expansion, Set2 * output);

bool Set2_unionSLSL(const SegmentList2 * const A, const SegmentList2 * const B, Set2 * output);
bool Set2_unionRSL(const Region2 * const A, const SegmentList2 * const B, Set2 * output);
bool Set2_unionRR(const Region2* const A, const Region2 * const B, Set2 * output);
bool Set2_unionSSL(const Set2 * const A, const SegmentList2 * const B, Set2 * output);
bool Set2_unionSR(const Set2 * const A, const Region2 * const B, Set2 * output);
bool Set2_unionSS(const Set2 * const A, const Set2 * const B, Set2 * output);

bool Set2_intersectSLSL(const SegmentList2 * const A, const SegmentList2 * const B, Set2 * output);
bool Set2_intersectRSL(const Region2 * const A, const SegmentList2 * const B, Set2 * output);
bool Set2_intersectRR(const Region2 * const A, const Region2 * const B, Set2 * output);
bool Set2_intersectSSL(const Set2 * const A, const SegmentList2 * const B, Set2 * output);
bool Set2_intersectSR(const Set2 * const A, const Region2 * const B, Set2 * output);
bool Set2_intersectSS(const Set2 * const A, const Set2 * const B, Set2 * output);




#endif

