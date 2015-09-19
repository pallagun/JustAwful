#include "include/Arc2.h"
#include "include/Util.h"

void Internal_Arc2_setAngle(Arc2 * arc, const gtfloat start, const gtfloat end, const short rotation, const bool makeProper)
{
  assert(start != end);
  assert(rotation == 1 || rotation == -1);
  
  arc->angle.start = start;
  arc->angle.end = end;
  arc->angle.rot = rotation;

  if (makeProper)
    {
      if (rotation > 0 && start > end)
	arc->angle.end += 2 * M_PI;
      else if (rotation < 0 && end > start)
	arc->angle.end -= 2 * M_PI;
    }
    
  
  GT_ARC_VALID(arc);
}
void Internal_Arc2_ptAtTheta(const Arc2 * const arc, const gtfloat theta, Point2 * pt)
{
  GT_ARC_VALID(arc);
  assert(pt != NULL);
  
  pt->x = cos(theta) * arc->radius;
  pt->y = sin(theta) * arc->radius;
  Point2_add2(pt, &(arc->center));
}
void Internal_Arc2_dirAtTheta(const gtfloat theta, const short rot, Vec2 * vec)
{
  assert(vec != NULL);
  assert(rot == 1 || rot == -1);
  
  vec->x = sin(theta) * (gtfloat)(-rot);
  vec->y = cos(theta) * (gtfloat)(rot);
}
void Internal_Arc2_delDirAtTheta(const gtfloat theta, const short rot, Vec2 * delDir)
{
  assert(delDir != NULL);
  assert(rot == 1 || rot == -1);
  
  delDir->x = -cos(theta);
  delDir->y = -sin(theta);
}
void Arc2_set4(Arc2 * arc, const Point2 * const center, const gtfloat radius, const Angle * const angle)
{
  GT_ARC_VALID(arc);
  assert(center != NULL);
  
  arc->center = *center;
  arc->radius = radius;
  Internal_Arc2_setAngle(arc, angle->start, angle->end, angle->rot, true);
  Angle_ensureValid( &(arc->angle) );
}
void Arc2_set6(Arc2 * arc, const Point2 * const center, const gtfloat radius, const gtfloat startRadians, const gtfloat endRadians, const short rotation)
{
  assert(center != NULL);
  assert(startRadians != endRadians);
  assert(rotation == 1 || rotation == -1);
  assert(radius > 0);
  arc->center = *center;
  arc->radius = radius;
  Internal_Arc2_setAngle(arc, startRadians, endRadians, rotation, true);
  Angle_ensureValid( &(arc->angle) );
  GT_ARC_VALID(arc);
}
void Arc2_setParametric(Arc2 * arc, const Arc2 * const src, const gtfloat min_t, const gtfloat max_t)
{
  GT_ARC_VALID(arc);
  assert(src != NULL);

  arc->center = src->center;
  arc->radius = src->radius;
  arc->angle.rot = src->angle.rot;
  arc->angle.start    = Angle_getParametricAngle(&(src->angle), min_t);
  arc->angle.end      = Angle_getParametricAngle(&(src->angle), max_t);
  Angle_ensureValid( &(arc->angle) );
}
void Arc2_range2(const Arc2 * const arc, Range2 * range)
{
  Point2 ptTemp;

  GT_ARC_VALID(arc);
  assert(range != NULL);
  
  Range2_initialize(range); 	 /* initialize the range to invalid */
  Arc2_startPoint(arc, &ptTemp); /* grab the start point */
  Range2_encompassPoint(range, &ptTemp); /* have to at least encompass the start point */
  Arc2_endPoint(arc, &ptTemp);
  Range2_encompassPoint(range, &ptTemp); /* you also have to encompas the end point */

  /* X, if PI or 0 radians are in the arc angle, then add them to the range. */
  if (Angle_contains( &(arc->angle), M_PI, true)) /* all the way left is a valid point on this arc? */
    Range1_encompassValue( &(range->x), arc->center.x - arc->radius); /* add in "all the way left" to the X range */
  if (Angle_contains( &(arc->angle), 0, true)) /*  all the way right is a valid point on this arc? */
    Range1_encompassValue( &(range->x), arc->center.x + arc->radius); /* add in "all the way left" to the X range */

  /* Y, if PI/2 or 3/2 PI are in the arc angle, then add them to the range */
  if (Angle_contains( &(arc->angle), -M_PI/2, true)) /* all the way left is a valid point on this arc? */
    Range1_encompassValue( &(range->y), arc->center.y - arc->radius); /* add in "all the way left" to the X range */
  if (Angle_contains( &(arc->angle), M_PI/2, true)) /*  all the way right is a valid point on this arc? */
    Range1_encompassValue( &(range->y), arc->center.y + arc->radius); /* add in "all the way left" to the X range */
}
gtfloat Arc2_length(const Arc2 * const arc)
{				/* TODO: force this to be inlined? */
  GT_ARC_VALID(arc);
  /* C = PI D * arc_width/2PI... right? */
  return arc->radius * Angle_width( &(arc->angle) );
}
void Arc2_pointFarthestAlong(const Arc2 * const arc, const Vec2 * const dir, Point2 * output)
{
  gtfloat tempTheta;
  assert(arc != NULL);
  assert(dir != NULL);
  assert(output != NULL);

  /* need to find the point on the theta on my arc that is cloest to direction.theta */
  tempTheta = Point2_theta(dir);
  /* if the angle contains this theta, then that's your farthest point */
  if (Angle_contains( &(arc->angle), tempTheta, true))
    {
      Internal_Arc2_ptAtTheta(arc, tempTheta, output);
    }
  else
    { 				/* that theta was not in the angle, so it must be either the end point or the start point */
      if ( Angle_distance(tempTheta, arc->angle.start) < Angle_distance(tempTheta, arc->angle.end) )
	{ 			/* the start of the angle is closer to the target point than the end point */
	  Arc2_startPoint(arc, output);	  
	}
      else 			/* the end point is closer to the target direction */
	{
	  Arc2_endPoint(arc, output);
	}
    }
}
unsigned int Arc2_getApproximationSize(const Arc2 * const arc, const gtfloat maxError)
{
  /* determine how many points you need to approximate this arc without having more error than maxError */
  /* formula is BufferSize >= 1 + arcradialspan / (2 * arccos(1 - (maxError/radius))) */
  /* so 1-maxError/radius has to be [-1,1] */
  /* therefore maxError/Radius has to be [0,2] */

  /* TODO: figure this one out again and make this less terrible */
  return 3 + ceil(Angle_width(&(arc->angle)));
  
  /* gtfloat error_over_radius = 100; */
  /* GT_ARC_VALID(arc); */
  /* if (maxError >= 2*arc->radius) */
  /*   { */
  /*     return 3; */
  /*   } */
  /* else */
  /*   { */
  /*     if (maxError <= 0) */
  /* 	error_over_radius = ( (maxError <= 0)?(GT_ALMOST_ZERO):(maxError) )/arc->radius; */

  /*     /\* and here it is in all it's terrible gloryN *\/ */
  /*     return ceil( 1 + (Angle_width(&(arc->angle)) / (2 * acos(1 - (error_over_radius)))));       */
  /*   } */
}
bool Arc2_getApproximation(const Arc2 * const arc, Point2 * pointBuffer, const unsigned int bufferSize)
{
  gtfloat deltheta;
  unsigned int i;    
  GT_ARC_VALID(arc);
  assert(pointBuffer != NULL);
  assert(bufferSize >= 2);

  deltheta = Angle_span(&(arc->angle)); /* get the span of the entire angle. */
  deltheta /= (float) ( bufferSize - 1 ); /* get the delta, buffersize - 1; */
    
  for (i = 0; i < bufferSize; ++i)
    {
      Internal_Arc2_ptAtTheta(arc, arc->angle.start + ((float)i)*deltheta, &(pointBuffer[i]));
    }
  
  return true;
}
void Arc2_startPoint(const Arc2 * const arc, Point2 * start)
{
  GT_ARC_VALID(arc);
  assert(start != NULL);
  Internal_Arc2_ptAtTheta(arc, arc->angle.start, start);
}
void Arc2_endPoint(const Arc2 * const arc, Point2 * end)
{
  GT_ARC_VALID(arc);
  assert(end != NULL);

  Internal_Arc2_ptAtTheta(arc, arc->angle.end, end);
}
gtfloat Arc2_getParametricCoord(const Arc2 * const arc, const Point2 * const pt)
{
  Vec2 displacement;

  GT_ARC_VALID(arc);
  assert(pt != NULL);
  
  Point2_sub3(&displacement, pt, &(arc->center));
  return Angle_getParametricCoord( &(arc->angle), Point2_theta(&displacement));
  
}
void Arc2_parametricPoint(const Arc2 * const arc, const gtfloat coordinate, Point2 * pt)
{
  gtfloat theta;

  GT_ARC_VALID(arc);
  assert(pt != NULL);
  
  theta = Angle_getParametricCoord( &(arc->angle), coordinate);
  Internal_Arc2_ptAtTheta(arc, theta, pt);
}
void Arc2_startDirection(const Arc2 * const arc, Vec2 * dir)
{
  GT_ARC_VALID(arc);
  assert(dir != NULL);

  Internal_Arc2_dirAtTheta(arc->angle.start, arc->angle.rot, dir);
}
void Arc2_endDirection(const Arc2 * const arc, Vec2 * dir)
{
  GT_ARC_VALID(arc);
  assert(dir != NULL);

  Internal_Arc2_dirAtTheta(arc->angle.end, arc->angle.rot, dir);
}
void Arc2_parametricDirection(const Arc2 * const arc, const gtfloat coordinate, Vec2 * dir)
{
  gtfloat theta;
  GT_ARC_VALID(arc);
  assert(dir != NULL);

  theta = Angle_getParametricCoord( &(arc->angle), coordinate);
  Internal_Arc2_dirAtTheta(theta, arc->angle.rot,  dir);
}
void Arc2_startDelDirection(const Arc2 * const arc, Vec2 * dir)
{
  GT_ARC_VALID(arc);
  assert(dir != NULL);

  Internal_Arc2_delDirAtTheta(arc->angle.start, arc->angle.rot, dir);
}
void Arc2_endDelDirection(const Arc2 * const arc, Vec2 * dir)
{
  GT_ARC_VALID(arc);
  assert(dir != NULL);
  
  Internal_Arc2_delDirAtTheta(arc->angle.end, arc->angle.rot, dir);
}
void Arc2_parametricDelDirection(const Arc2 * const arc, const gtfloat coordinate, Vec2 * delDir)
{
  gtfloat theta;
  GT_ARC_VALID(arc);
  assert(delDir != NULL);

  theta = Angle_getParametricCoord( &(arc->angle), coordinate);
  Internal_Arc2_dirAtTheta(theta, arc->angle.rot,  delDir);
}
bool Arc2_almostEqual2(const Arc2 * const A, const Arc2 * const B)
{
  GT_ARC_VALID(A);
  GT_ARC_VALID(B);
  
  return ( Point2_almostEqual2( &(A->center), &(B->center) ) 
	   && GT_ALMOST_EQUAL2(A->radius, B->radius) 
	   && Angle_almostEqual2( &(A->angle), &(B->angle) ));
}
bool Arc2_almostEqualDirectional2(const Arc2 * const A, const Arc2 * const B)
{
  GT_ARC_VALID(A);
  GT_ARC_VALID(B);
  
  return ( Point2_almostEqual2( &(A->center), &(B->center) ) 
	   && GT_ALMOST_EQUAL2(A->radius, B->radius) 
	   && Angle_almostEqualDirectional2( &(A->angle), &(B->angle) ));
}
bool Arc2_almostEqualReverseDirectional2(const Arc2 * const A, const Arc2 * const B)
{
  GT_ARC_VALID(A);
  GT_ARC_VALID(B);

  return ( Point2_almostEqual2( &(A->center), &(B->center) ) 
	   && GT_ALMOST_EQUAL2(A->radius, B->radius) 
	   && Angle_almostEqualReverseDirectional2( &(A->angle), &(B->angle) ));

}
void Arc2_translate(Arc2 * arc, const Vec2 * const delta)
{
  GT_ARC_VALID(arc);
  assert(delta != NULL);
  
  Point2_add2( &(arc->center), delta);
}
void Arc2_scaleOrigin(Arc2 * arc, const gtfloat alpha)
{
  GT_ARC_VALID(arc);
  
  Point2_scale2(&(arc->center), alpha);
  arc->radius *= alpha;  
}
void Arc2_flip1(Arc2 * arc)
{
  GT_ARC_VALID(arc);
  Angle_invert(&(arc->angle));
}
void Arc2_flip2(const Arc2 * const src, Arc2 * dest)
{
  GT_ARC_VALID(src);
  GT_ARC_VALID(dest);
  
  *dest = *src;
  Arc2_flip1(dest);
}
void Arc2_truncateToParametric(Arc2 * arc, const gtfloat min_t, const gtfloat max_t)
{
  gtfloat tempEnd;
  assert(min_t < max_t);
  GT_ARC_VALID(arc);
  
  tempEnd = Angle_getParametricCoord( &(arc->angle), max_t);
  arc->angle.start = Angle_getParametricCoord( &(arc->angle), min_t);
  arc->angle.end = tempEnd;
}
bool Arc2_blindExpand(Arc2 * arc, const gtfloat delta)
{
  /* perform a blind expansion on this line segment and spit out the endDirection as a by product */
  GT_ARC_VALID(arc);
  arc->radius += arc->angle.rot * delta;
  return (arc->radius > 0);
}
bool Arc2_ensureValid(const Arc2 * const arc)
{
  if (arc == NULL)
    return false;

  if (! (GT_ANGLE_VALID_INTERNAL(&(arc->angle))) )
    return false;

  if (Angle_width(&(arc->angle)) <= 0)
    return false;

  if (Angle_width(&(arc->angle)) > M_PI)
    return false;

  return true;
}
