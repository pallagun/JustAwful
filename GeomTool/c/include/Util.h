#ifndef __GT_UTIL_H__
#define __GT_UTIL_H__ 1

#include <stdbool.h>
#include "Constants.h"

#define GT_ALMOST_EQUAL3(A,B,DELTA) ( -DELTA < A-B && A-B < DELTA )
#define GT_ALMOST_EQUAL2(A,B) GT_ALMOST_EQUAL3((A),(B),GT_ALMOST_ZERO)

/* TODO: are while loops in #define statments bad? I think they might be. */
#define normalizeZeroTo2Pi(val) { while((val)>2*M_PI){(val)-=2*M_PI;}; while((val) < 0){(val)+=2*M_PI;}; }

 /* TODO: isn't this in a library someplace */
#define deg2rad(val) (val)*M_PI/180.0
#define rad2deg(val) (val)*180.0/M_PI
#define sign(val) (((val) >= 0) ? (1) : (-1))


bool gt_almost_equalrad2(const gtfloat A, const gtfloat B);






#endif
