#include "include/Point2.h"

#ifndef NDEBUG
#include <stdio.h>		/* printf() */
void Point2_debug(const Point2 * const pt)
{
  printf("(%.4f,%.4f)",pt->x,pt->y);
}
#endif

void Point2_set3(Point2 * pt, const gtfloat x, const gtfloat y)
{
  assert(pt != NULL);
  pt->x = x;
  pt->y = y;
}
void Point2_add2(Point2 * accumulator, const Point2 * const addition)
{
  assert(accumulator != NULL);
  assert(addition != NULL);

  accumulator->x += addition->x;
  accumulator->y += addition->y;
}
void Point2_sub2(Point2 * accumulator, const Point2 * const subtraction)
{
  assert(accumulator != NULL);
  assert(subtraction != NULL);

  accumulator->x -= subtraction->x;
  accumulator->y -= subtraction->y;
}
void Point2_sub3(Point2 * pt, const Point2 * const A, const Point2 * const B)
{
  assert(pt != NULL);
  assert(A != NULL);
  assert(B != NULL);
  
  pt->x = A->x - B->x;
  pt->y = A->y - B->y;
}
void Point2_add3(Point2 * pt, const Point2 * const A, const Point2 * const B)
{
  assert(pt != NULL);
  assert(A != NULL);
  assert(B != NULL);

  pt->x = A->x + B->x;
  pt->y = A->y + B->y;
}
void Point2_scale2(Point2 * pt, const gtfloat scale)
{
  assert(pt != NULL);

  pt->x *= scale;
  pt->y *= scale;
}
void Point2_scale3(Point2 * pt, const Point2 * const input, const gtfloat scale)
{
  assert(pt != NULL);
  assert(input != NULL);
  
  pt->x = input->x * scale;
  pt->y = input->y * scale;
}
void Point2_invert1(Point2 * pt)
{
  assert(pt != NULL);

  pt->x = -pt->x;
  pt->y = -pt->y;
}
void Point2_invert2(Point2 * output, const Point2 * const input)
{
  assert(output != NULL);
  assert(input != NULL);

  output->x = -input->x;
  output->y = -input->y;
}
bool Point2_eq(const Point2 * const A, const Point2 * const B)
{
  assert(A != NULL);
  assert(B != NULL);
  
  return (A->x == B->x && A->y == B->y);
}
bool Point2_ne(const Point2 * const A, const Point2 * const B)
{
  assert(A != NULL);
  assert(B != NULL);

  return (A->x != B->x || A->y != B->y);
}
gtfloat Point2_theta(const Point2 * const pt)
{
  assert(pt != NULL);

  return atan2(pt->y,pt->x);
}
gtfloat Point2_magSquared(const Point2 * const pt)
{
  assert(pt != NULL);

  return (pt->x*pt->x)+(pt->y*pt->y);
}
gtfloat Point2_mag(const Point2 * const pt)
{
  assert(pt != NULL);

  return sqrt( Point2_magSquared(pt) );
}
gtfloat Point2_distance(const Point2 * const A, const Point2 * const B)
{
  gtfloat delx, dely;
  assert(A != NULL);
  assert(B != NULL);

  delx = A->x - B->x;
  dely = A->y - B->y;
  return sqrt ( (delx*delx) + (dely*dely) );
}
void Point2_normalize(Point2 * pt)
{
  gtfloat mag;
  assert(pt != NULL);
  
  mag = Point2_mag(pt);
  pt->x /= mag;
  pt->y /= mag;
}
void Point2_rotate90(Point2 * pt, const int zSign)
{
  gtfloat temp;
  assert(pt != NULL);
  assert(zSign == -1 || zSign == 1);

  if (zSign > 0)
    {
      temp = pt->y;
      pt->y = pt->x;
      pt->x = -temp;
    }
  else
    {
      /* [ c -s] -90, c = 0, s = -1 so [ 0 1]  // V[0] = V[1]; */
      /* [ s  c]                       [-1 0]  // V[1] = -V[0]; */

      temp = pt->y;
      pt->y = -pt->x;
      pt->x = temp;
    }
}
bool Point2_almostEqual3(const Point2 * const A, const Point2 * const B, const gtfloat maxDelta)
{
  gtfloat temp;
  assert(A != NULL);
  assert(B != NULL);
  assert(maxDelta > 0);

  temp = A->x - B->x;
  if (-maxDelta < temp && temp < maxDelta)
    {
      temp = A->y - B->y;;
      if (-maxDelta < temp && temp < maxDelta)
	return (true);
    }
  return (false);
}
bool Point2_almostEqual2(const Point2 * const A, const Point2 * const B)
{
  assert(A != NULL);
  assert(B != NULL);

  return Point2_almostEqual3(A, B, GT_ALMOST_ZERO);
}
gtfloat Point2_cross(const Point2 * const A, const Point2 * const B)
{
  assert(A != NULL);
  assert(B != NULL);

  return (A->x * B->y  -  A->y * B->x);
}
gtfloat Point2_dot(const Point2 * const A, const Point2 * const B)
{
  assert(A != NULL);
  assert(B != NULL);

  return( A->x * B->x  +  A->y * B->y );
}





