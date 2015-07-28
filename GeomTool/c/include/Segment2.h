#ifndef __GT_SEGMENT2_H__
#define __GT_SEGMENT2_H__

#include "Constants.h"
#include "Line2.h"
#include "Arc2.h"


typedef enum Segment2Type 
  {
    INVALID = 0, LINE, ARC 
  } segment2Type;

typedef struct Segment2
{
  enum Segment2Type type;
  union
  {
    Line2 line;
    Arc2 arc;
  } s;
} Segment2;



void Segment2_setLine(Segment2 * seg, const Line2 * const line);
void Segment2_setArc(Segment2 * seg, const Arc2 * const arc);
void Segment2_translate(Segment2 * seg, const Vec2 * const disp);
void Segment2_scaleOrigin(Segment2 * seg, const gtfloat alpha);
bool Segment2_almostEqual2(const Segment2 * const A, const Segment2 * const B);

/* maybe these should be functions, maybe they should be #defines.... */
#define Seg2_range2(seg,range)				\
  assert((seg).type == LINE || (seg).type == ARC);	\
  if ((seg).type == LINE)				\
    Line2_range2(&((seg).s.line), range);		\
  else							\
    Arc2_range2(&((seg).s.arc), range);

#define Seg2_flip1(seg)					\
  assert((seg).type == LINE || (seg).type == ARC);	\
  if ((seg).type == LINE)				\
    Line2_flip1(&((seg).s.line));			\
  else							\
    Arc2_flip1(&((seg).s.arc));

#define Seg2_flip2(src,dest)				\
  assert((src).type == LINE || (src).type == ARC);	\
  (dest).type = (src).type;				\
  if ((src).type == LINE)				\
    Line2_flip2(&((src).s.line), &((dest).s.line));	\
  else							\
    Arc2_flip2(&((src).s.arc), &((dest).s.arc));

#define Seg2_startPoint(src,ptPoint)			\
  assert((src).type == LINE || (src).type == ARC);	\
  if ((src).type == LINE)				\
    *ptPoint = (src).s.line.start;			\
  else							\
    Arc2_startPoint(&((src).s.arc), ptPoint);

#define Seg2p_startPoint(src,ptPoint)			\
  assert(src->type == LINE || src->type == ARC);	\
  if (src->type == LINE)				\
    *(ptPoint) = src->s.line.start;			\
  else							\
    Arc2_startPoint(&(src->s.arc), ptPoint);

#define Seg2_endPoint(src,ptPoint)			\
  assert(src.type == LINE || src.type == ARC);		\
  if (src.type == LINE)					\
    *ptPoint = src.s.line.end;				\
  else							\
    Arc2_endPoint(&(src.s.arc), ptPoint);

#define Seg2p_endPoint(src,ptPoint)			\
  assert(src->type == LINE || src->type == ARC);	\
  if (src->type == LINE)				\
    *ptPoint = src->s.line.end;				\
  else							\
    Arc2_endPoint(&(src->s.arc), ptPoint);

#define Seg2_startDirection(src, ptVec)			\
  assert(src.type == LINE || src.type == ARC);		\
  if (src.type == LINE)					\
    Line2_direction(&(src.s.line), ptVec);		\
  else							\
    Arc2_startDirection(&(src.s.arc), ptVec);

#define Seg2p_startDirection(src, ptVec)		\
  assert(src->type == LINE || src->type == ARC);	\
  if (src->type == LINE)				\
    Line2_direction(&(src->s.line), ptVec);		\
  else							\
    Arc2_startDirection(&(src->s.arc), ptVec);

#define Seg2_endDirection(src, ptVec)			\
  assert(src.type == LINE || src.type == ARC);		\
  if (src.type == LINE)					\
    Line2_direction(&(src.s.line), ptVec);		\
  else							\
    Arc2_endDirection(&(src.s.arc), ptVec);

#define Seg2p_endDirection(src, ptVec)			\
  assert(src->type == LINE || src->type == ARC);	\
  if (src->type == LINE)				\
    Line2_direction(&(src->s.line), ptVec);		\
  else							\
    Arc2_endDirection(&(src->s.arc), ptVec);

#define Seg2_parametricDirection(src, fParam, ptVec)	\
  assert(src.type == LINE || src.type == ARC);		\
  if (src.type == LINE)					\
    Line2_direction(&(src.s.line), ptVec);		\
  else							\
    Arc2_parametricDirection(&(src.s.arc), (fParam), (ptVec));

#define Seg2p_parametricDirection(src, fParam, ptVec)		\
  assert((src)->type == LINE || (src)->type == ARC);		\
  if ((src)->type == LINE)					\
    Line2_direction(&((src)->s.line), (ptVec));			\
  else								\
    Arc2_parametricDirection(&((src)->s.arc), (fParam), (ptVec));

#define Seg2p_setParametric(target, source, fParamStart, fParamEnd)	\
  assert((source)->type == LINE || (source)->type == ARC);		\
  (target)->type = (source)->type;					\
  if ((source)->type == LINE)						\
    Line2_setParametric(&((target)->s.line), &((source)->s.line), fParamStart, fParamEnd); \
  else									\
    Arc2_setParametric(&((target)->s.arc), &((source)->s.arc), fParamStart, fParamEnd);

#define Seg2p_truncateToParametric(target, fParamStart, fParamEnd)	\
  assert( (target)->type == LINE || (target)->type == ARC);		\
  if ((target)->type == LINE)						\
    Line2_truncateToParametric(&((target)->s.line), fParamStart, fParamEnd); \
  else									\
    Arc2_truncateToParametric(&((target)->s.arc), fParamStart, fParamEnd);


#endif
