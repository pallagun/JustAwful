#include "include/Line2.h"
#include <math.h> 		/* for abs() */

#ifndef NDEBUG
void Line2_debug(const Line2 * const line)
{
  assert(line != NULL);
  printf("(line ");
  Point2_debug(&(line->start));
  printf(" ");
  Point2_debug(&(line->end));
  printf(")");
}
#endif

void Line2_set3(Line2 * line, const Point2 * const start, const Point2 * const end)
{
  assert(line != NULL);
  assert(start != NULL);
  assert(end != NULL);
  
  line->start = *start;
  line->end   = *end;
}
void Line2_set5(Line2 * line, const gtfloat startX, const gtfloat startY, const gtfloat endX, const gtfloat endY)
{
  assert(line != NULL);
  
  line->start.x = startX;
  line->start.y = startY;
  line->end.x = endX;
  line->end.y = endY;
}
void Line2_setParametric(Line2 * line, const Line2 * const source, const gtfloat min_t, const gtfloat max_t)
{
  assert(line != NULL);
  assert(source != NULL);
  
  Line2_parametricPoint(source, min_t, &(line->start));
  Line2_parametricPoint(source, max_t, &(line->end));
}
void Line2_range2(const Line2 * const line, Range2 * range)
{
  assert(line != NULL);
  assert(range != NULL);
  
  if (line->start.x < line->end.x)
    {
      range->x.Min = line->start.x;
      range->x.Max = line->end.x;
    }
  else
    {
      range->x.Min = line->end.x;
      range->x.Max = line->start.x;
    }

  if (line->start.y < line->end.y)
    {
      range->y.Min = line->start.y;
      range->y.Max = line->end.y;
    }
  else
    {
      range->y.Min = line->end.y;
      range->y.Max = line->start.y;
    }  
}
void Line2_direction(const Line2 * const line, Vec2 * dir)
{
  assert(line != NULL);
  assert(dir != NULL);

  Line2_vector(line, dir);
  Point2_normalize(dir);
}
void Line2_vector(const Line2 * const line, Vec2 * characteristicVector)
{
  assert(line != NULL);
  assert(characteristicVector != NULL);
  
  characteristicVector->x = line->end.x - line->start.x;
  characteristicVector->y = line->end.y - line->start.y;
}
gtfloat Line2_length(const Line2 * const line)
{
  assert(line != NULL);

  return sqrt(Line2_sqLength(line));
}
gtfloat Line2_sqLength(const Line2 * const line)
{
  gtfloat delx, dely;
  assert(line != NULL);
  
  delx = line->start.x - line->end.x;
  dely = line->start.y - line->end.y;
  return ( delx*delx + dely*dely );
}
gtfloat Line2_boxLength(const Line2 * const line)
{
  gtfloat delx, dely;
  assert(line != NULL);

  delx = fabs(line->start.x - line->end.x);
  dely = fabs(line->start.y - line->end.y);
  return ( (delx > dely) ? (delx) : (dely) );
}
void Line2_pointFarthestAlong(const Line2 * const line, const Vec2 * const dir, Point2 * output)
{
  Vec2 charVec;
  assert(line != NULL);
  assert(dir != NULL);
  assert(output != NULL);

  Line2_vector(line, &charVec);
  if (Point2_dot(&charVec, dir) > 0)
    { 				/* the direction vector is going in the same direction as the line */
      *output = line->end;
    }
  else
    {
      *output = line->start;
    }
}
gtfloat Line2_getParametricCoord(const Line2 * const line, const Point2 * const pt)
{
  Vec2 vec, start2pt;

  assert(line != NULL);
  assert(pt != NULL);
  
  Point2_sub3(&start2pt, pt, &(line->start));
  Line2_vector(line, &vec);
  
  return Point2_dot(&vec, &start2pt) / Line2_sqLength(line);
}
Span1 Line2_getParametricSpan(const Line2 * const line, const Point2 * const startPt, const Point2 * const endPt)
{
  Span1 output;
  Vec2 vec, displacement;
  gtfloat sqLen;

  assert(line != NULL);
  assert(startPt != NULL);
  assert(endPt != NULL);
  
  Line2_vector(line, &vec);
  sqLen = Line2_sqLength(line);

  Point2_sub3(&displacement, startPt, &(line->start));
  output.start = Point2_dot(&vec, &displacement) / sqLen;
  
  Point2_sub3(&displacement, endPt, &(line->start));
  output.end = Point2_dot(&vec, &displacement) / sqLen;

  return output;
}
void Line2_parametricPoint(const Line2 * const line, const gtfloat coordinate, Point2 * pt)
{
  Vec2 charVec;

  assert(line != NULL);
  assert(pt != NULL);
  
  Line2_vector(line, &charVec);
  
  Point2_scale2(&charVec, coordinate);
  Point2_add3(pt, &(line->start), &charVec);
}
bool Line2_almostEqual2(const Line2 * const A, const Line2 * const B)
{
  assert(A != NULL);
  assert(B != NULL);

  return Line2_almostEqualDirectional2(A,B) || Line2_almostEqualReverseDirectional2(A,B);
}
bool Line2_almostEqualDirectional2(const Line2 * const A, const Line2 * const B)
{
  assert(A != NULL);
  assert(B != NULL);
  
  return Point2_almostEqual2( &(A->start), &(B->start) ) && Point2_almostEqual2( &(A->end), &(B->end) );
}
bool Line2_almostEqualReverseDirectional2(const Line2 * const A, const Line2 * const B)
{
  assert(A != NULL);
  assert(B != NULL);

  return Point2_almostEqual2( &(A->end), &(B->start) ) && Point2_almostEqual2( &(A->start), &(B->end) );
}
void Line2_translate(Line2 * line, const Vec2 * const delta)
{
  assert(line != NULL);
  assert(delta != NULL);

  Point2_add2( &(line->start), delta);
  Point2_add2( &(line->end)  , delta);
}
void Line2_scaleOrigin(Line2 * line, const gtfloat alpha)
{
  assert(line != NULL);

  Point2_scale2( &(line->start), alpha);
  Point2_scale2( &(line->end)  , alpha);
}
void Line2_flip1(Line2 * line)
{
  Point2 temp;

  assert(line != NULL);
  
  temp = line->end;
  line->end = line->start;
  line->start = temp;
}
void Line2_flip2(const Line2 * const src, Line2 * dest)
{
  assert(src != NULL);
  assert(dest != NULL);
  
  dest->end   = src->start;
  dest->start = src->end;
}
void Line2_truncateToParametric(Line2 * line, const gtfloat min_t, const gtfloat max_t)
{
  Point2 start, vec;

  assert(line != NULL);
  assert(min_t < max_t);
  
  Line2_vector(line, &vec);
  start = line->start;
  /* irritating variable switching :( */
  line->start = vec;  		/* pretend that the start point is the vector */
  Point2_scale2( &(line->start), min_t ); /* scale the "vector" by the desired min, giving you offset */
  Point2_add2( &(line->start), &start );  /* add in the start point to the offset to get the final start poin */
  
  line->end = vec;
  Point2_scale2( &(line->end), max_t );
  Point2_add2( &(line->end), &start );  
}
void Line2_blindExpand(Line2 * line, const gtfloat delta, Vec2 * expandDir)
{
  /* perform a blind expansion on this line segment and spit out the vector which describes the translation direction*/
  Vec2 trans;

  assert(line != NULL);
  assert(expandDir != NULL);
  assert(delta != 0);

  Line2_direction(line, expandDir);
  Point2_rotate90(expandDir, -1);
  trans = *expandDir;
  Point2_scale2(&trans, delta);
  Line2_translate(line, &trans);
}

