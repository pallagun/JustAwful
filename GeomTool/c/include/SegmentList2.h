#ifndef __GT_SEGMENTLIST2_H__
#define __GT_SEGMENTLIST2_H__

#include "Segment2.h"
#include "Range2.h"
#include "BlockVec.h"

#define GT_SEGMENTLIST2_DEFAULT_SIZE 8 /* I don't know. 8? whatever. */
#define GT_SEGMENTLIST2_DEFAULT_OVERSIZE 2 /* if someone tells me to make it 10 big, I'll make it 12 big. NOTE: this might be dumb */
#define GT_SEGMENTLIST2_DEFAULT_GROWTH 4   /* if you need more segments, just tack on this many.. */

#define GT_SEGLIST_VALID1(A) assert(A != NULL && SegmentList2_db_validateCache(A))

#define GT_SEGLISTREF_VALID1(A) assert(SegmentList2_db_validateCache(&A))

typedef enum SegmentList2Type
  {
    PATH = 0,
    UNBOUNDED = 1,
    NULL_SET = 2,
    BOUNDED = 3,
    HOLE = 4,
  } SegmentList2Type;
 
static const char * const SegmentList2TypeText[] = 
  { 
    "PATH",
    "UNBOUNDED",
    "NULL_SET",
    "BOUNDED",
    "HOLE" 
  };

/* when closed and continuous represents a region of 2d space with no holes (or the inverse of that (a hole))
   when it isn't closed it represents a path
   when it isn't continuous it represents a worthless pile of junk */
typedef struct SegmentList2
{
  /* blockvec always first */
  BlockVec segments;
  /* stats */
  bool private_isClosed;
  bool private_isContinuous;
  short private_spin;
  bool private_isZeroArea;
  Point2 private_interiorPoint;
  bool private_interiorPointValid;
  Range2 private_pathBounds;
  SegmentList2Type private_type;
  bool private_cacheValid;

  /* TODO: add something like "boundedCacheValid" */
  struct SegmentList2 * boundedCache;
} SegmentList2;

/* constructors & initializers */
SegmentList2 * SegmentList2_create();
SegmentList2 * SegmentList2_create1(const unsigned int predictedNumberOfSegments);
SegmentList2 * SegmentList2_createType(const SegmentList2Type type);
SegmentList2 * SegmentList2_createClone(const SegmentList2 * const src);
void           SegmentList2_initialize(SegmentList2 * list);
void           SegmentList2_initialize2(SegmentList2 * list, const unsigned int containerSize);
#define        SegmentList2_numSegs(LIST) ((LIST)->segments.numItems)
#define        SegmentList2_seg(LIST, IDX) (((Segment2 *)((LIST)->segments.item))[(IDX)]) /* REFERENCE to the segment (not a pointer) */
#define        SegmentList2_lastSeg(LIST) (SegmentList2_seg((LIST), SegmentList2_numSegs((LIST))-1))
/* dstructors & uninitailizers */
void           SegmentList2_destroy(SegmentList2 * list);
void           SegmentList2_uninitialize(SegmentList2 * list);
/* Basic list operations */
void           SegmentList2_clear(SegmentList2 * list);
void           SegmentList2_reallocate(SegmentList2 * list, const unsigned int predictedNumberOfSegments);
/* TODO: change the order of the arguments in this function */
void           SegmentList2_clone(const SegmentList2 * const source, SegmentList2 * dest);
void           SegmentList2_pop(SegmentList2 * list, const bool refreshCache);
void           SegmentList2_remove(SegmentList2 * list, const unsigned int idx, const bool refreshCache);
void           SegmentList2_removeRange(SegmentList2 * list, const unsigned int startIdx, const unsigned int endIdx, const bool refreshCache);
void           SegmentList2_insertCopy(SegmentList2 * list, Segment2 * segment, const unsigned int idx, const bool refreshCache);
void           SegmentList2_appendCopy(SegmentList2 * list, const Segment2 * const seg, const bool refreshCache);
void           SegmentList2_appendCopyList(SegmentList2 * list, const SegmentList2 * const otherList, const bool refreshCache);
int            SegmentList2_getIdxFromList(const SegmentList2 * const list, const Segment2 * const segment);

/* Cache related functions */
#ifndef NDEBUG
bool SegmentList2_db_validateCache(const SegmentList2 * const list);
#else
#define SegmentList2_db_validateCache(A) GT_TRUE
#endif
void           SegmentList2_refreshCache(SegmentList2 * list);
bool        SegmentList2_cacheValid(const SegmentList2 * const list);
void           SegmentList2_invalidateCache(SegmentList2 * list);
void           SegmentList2_flip1(SegmentList2 * list);
void           SegmentList2_cacheBounded(SegmentList2 * list);


/* modifiers */
bool        SegmentList2_setType(SegmentList2 * list, const SegmentList2Type type);
void           SegmentList2_forceContinuous(SegmentList2 * segList, const bool forceClosed);
void           SegmentList2_translate(SegmentList2 * segList, const Vec2 delta);
void           SegmentList2_scaleOrigin(SegmentList2 * segList, const gtfloat alpha);

/* queries - which could modify cache */
bool          SegmentList2_isContinuous(SegmentList2 * segs);
bool          SegmentList2_isClosed(SegmentList2 * segs);
bool          SegmentList2_isZeroArea(SegmentList2 * segs);
short            SegmentList2_spin(SegmentList2 * segs);
bool          SegmentList2_pathRange2(SegmentList2 * segs, Range2 * range);
bool          SegmentList2_interiorPoint(SegmentList2 * segs, Point2 * point);
SegmentList2Type SegmentList2_type(SegmentList2 * segs);

/* queries - dumb - cache hit if they can, otherwise resort to calculation */
bool        SegmentList2_calcContinuousClosed(const SegmentList2 * const list, bool * continuous, bool * closed);
bool        SegmentList2_calcZeroArea(const SegmentList2 * const list, bool * isZeroArea);
bool        SegmentList2_calcSpin(const SegmentList2 * const list, short * spin);
bool        SegmentList2_calcPathRange(const SegmentList2 * const list, Range2 * bounds);
bool        SegmentList2_calcInteriorPoint(const SegmentList2 * const list, Point2 * pt);
bool        SegmentList2_calcType(const SegmentList2 * const list, SegmentList2Type * type);

/* queries - a bit more complicated */
bool        SegmentList2_almostEqual2(const SegmentList2 * const A, const SegmentList2 * const B, const bool allowOffset);
bool        SegmentList2_pointFarthestAlong(const SegmentList2 * const segs, const Vec2 * const direction, Point2 * pt);
void           SegmentList2_flip2(const SegmentList2 * const source, SegmentList2 * dest);
bool        SegmentList2_simplify(const SegmentList2 * const input, SegmentList2 * output, unsigned int tryLevel, const bool isContinuous, const bool isClosed);
bool        SegmentList2_blindExpansion(const SegmentList2 * const input, const gtfloat growth, SegmentList2 * output);
unsigned int   SegmentList2_getApproximationSize(const SegmentList2 * const input, const gtfloat maxError);
int            SegmentList2_getApproximation(const SegmentList2 * const input, const gtfloat maxError, Point2 * buffer, const unsigned int bufferMaxPoints);


#endif

