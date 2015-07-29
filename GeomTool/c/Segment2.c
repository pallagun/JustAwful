#include "include/Segment2.h"



void Segment2_setLine(Segment2 * seg, const Line2 * const line)
{
  assert(seg != NULL);
  assert(line != NULL);
  seg->type = LINE;
  seg->s.line = *line;
}
void Segment2_setArc(Segment2 * seg, const Arc2 * const arc)
{
  assert(seg != NULL);
  assert(arc != NULL);
  seg->type = ARC;
  seg->s.arc = *arc;
}
void Segment2_translate(Segment2 * seg, const Vec2 * const delta)
{
  assert(seg != NULL);
  assert(delta != NULL);
  assert(seg->type == LINE || seg->type == ARC);
  if (seg->type == LINE)
    Line2_translate(&(seg->s.line), delta);
  else
    Arc2_translate(&(seg->s.arc), delta);
}
void Segment2_scaleOrigin(Segment2 * seg, const gtfloat alpha)
{
  assert(seg != NULL);
  assert(alpha != 0);
  assert(seg->type == LINE || seg->type == ARC);
  if (seg->type == LINE)
    Line2_scaleOrigin(&(seg->s.line), alpha);
  else
    Arc2_scaleOrigin(&(seg->s.arc), alpha);  
}
bool Segment2_almostEqual2(const Segment2 * const A, const Segment2 * const B)
{
  assert(A != NULL);
  assert(B != NULL);

  if (A->type == ARC && B->type == ARC)
    {
      return Arc2_almostEqual2(&(A->s.arc), &(B->s.arc));
    }
  else if (A->type == LINE && B->type == LINE)
    {
      return Line2_almostEqual2(&(A->s.line), &(B->s.line));
    }
  return false;
}
