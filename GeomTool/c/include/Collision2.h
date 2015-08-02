#ifndef __GT_COLLISION2_H__
#define __GT_COLLISION2_H__ 1

#include "Constants.h"
#include "Point2.h"
#include "Segment2.h"
#include "Span1.h"
#include "Range2.h"
#include "SegmentList2.h"

#define GT_COLLISION2CONT_DEFAULT_PTS 10
#define GT_COLLISION2CONT_DEFAULT_SEGS 10
#define GT_COLLISION2CONT_DEFAULT_RANGES 4
#define GT_COLLISION2_THRESHOLD GT_ALMOST_ZERO
#define GT_COLLISION2_CROSS_PARALLEL_THRESHOLD GT_ALMOST_ZERO

#define GT_COLLISION2CONT_VALID(x) assert(x != NULL && x->pts != NULL && x->segs != NULL && x->ranges != NULL && x->private_ptBuffSize > 0 && x->private_segBuffSize > 0 && x->private_rangeBuffSize > 0)

typedef struct Point2IntersectInfo
{
  const Segment2 * aSegment;
  const Segment2 * bSegment;
  gtfloat aParametric, bParametric;
  /* TODO: add in list indexes in here, they'll probably be more usefull/safer than a memory location */
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

Collision2Container * Collision2Cont_create();
void Collision2Cont_destroy(Collision2Container * cont);
void Collision2Cont_clear(Collision2Container * cont);
void Collision2Cont_append(Collision2Container * dest, const Collision2Container * const src);
void Collision2Cont_decomposeSegsToPoints(Collision2Container * container);
void Collision2Cont_ptReserve(Collision2Container * cont, const unsigned int numNewPts);
void Collision2Cont_segReserve(Collision2Container * cont, const unsigned int numNewSegs);
void Collision2Cont_rangeReserve(Collision2Container * cont, const unsigned int numNewRanges);
void Collision2Cont_sortByAParam(Collision2Container * cont);
void Collision2Cont_sortByBParam(Collision2Container * cont);

/* don't really need this first one? */
/* bool Collision2_PP(Collision2Container * cont, const Point2 * const A, const Point2 * const B); */
bool Collision2_LP(Collision2Container * cont, const Line2 * const A, const Point2 * const B, bool includeEndPoints);
bool Collision2_AP(Collision2Container * cont, const Arc2 * const A, const Point2 * const B, bool includeEndPoints);
bool Collision2_SP(Collision2Container * cont, const Segment2 * const A, const Point2 * const B, bool includeEndPoints);

bool Collision2_RR(Collision2Container * cont, const Range2 * const A, const Range2 * const B);

bool Collision2_LL(Collision2Container * cont, const Line2 * const A, const Line2 * const B, bool includeEndPoints, bool includeEndToEndPoints);
bool Collision2_LA(Collision2Container * cont, const Line2 * const A, const Arc2 * const B, bool includeEndPoints, bool includeEndToEndPoints);
bool Collision2_AA(Collision2Container * cont, const Arc2 * const A, const Arc2 * const B, bool includeEndPoints, bool includeEndToEndPoints);
bool Collision2_SL(Collision2Container * cont, const Segment2 * const A, const Line2 * const B, bool includeEndPoints, bool includeEndToEndPoints);
bool Collision2_SS(Collision2Container * cont, const Segment2 * const A, const Segment2 * const B, bool includeEndPoints, bool includeEndToEndPoints);
bool Collision2_SLL(Collision2Container * cont, const SegmentList2 * const A, const Line2 * const B, bool includeEndPoints, bool includeEndToEndPoints);
bool Collision2_SLS(Collision2Container * cont, const SegmentList2 * const A, const Segment2 * const B, bool includeEndPoints, bool includeEndToEndPoints);
bool Collision2_SLSL(Collision2Container * cont, const SegmentList2 * const A, const SegmentList2 * const B, bool includeEndPoints, bool includeEndToEndPoints);

#endif



