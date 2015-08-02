#ifndef __GT_GEOMCONSTANTS_H__
#define __GT_GEOMCONSTANTS_H__ 1

typedef double gtfloat;

 /* points this far apart are considered "the same" */
#define GT_ALMOST_ZERO (gtfloat)0.0000005

/* This is a really big number, but still something logical */
/* If you are bigger than this value I'll think something is wrong */
#define GT_LARGE_VALUE (gtfloat)999999999999.9
#define GT_UNREASONABLY_LARGE_VALUE (gtfloat) 99999999999999999999999999.99
 /* This is a completely reasonable value! */
#define GT_REASONABLE_VALUE (gtfloat)1.0

 /* blah blah whatever. */
#define GT_EXACTLY_ONE (gtfloat)1.0
#define GT_EXACTLY_ZERO (gtfloat)0.0

#define GT_ALMOST_EQUAL3(A,B,DELTA) ( -DELTA < A-B && A-B < DELTA )
#define GT_ALMOST_EQUAL2(A,B) ( -GT_ALMOST_ZERO < A-B && A-B < GT_ALMOST_ZERO )

#ifndef M_PI
 #define M_PI (gtfloat)3.141592653589793238462643383279502884197169399375105820974944592
#endif

/* TODO: are while loops in #define statments bad? I think they might be. */
#define normalizeZeroTo2Pi(val) { while((val)>2*M_PI){(val)-=2*M_PI;}; while((val) < 0){(val)+=2*M_PI;}; }

 /* TODO: isn't this in a library someplace */
#define deg2rad(val) (val)*M_PI/180.0
#define rad2deg(val) (val)*180.0/M_PI
#define sign(val) (((val) >= 0) ? (1) : (-1))


#ifndef DB_PRINTF
 #ifdef NDEBUG
  #define DB_PRINTF (void(0))
 #else
  #include <stdio.h>
  #define DB_PRINTF(STUFF) printf STUFF
 #endif
#endif

#ifndef DB_RUN
 #ifdef NDEBUG
  #define DB_RUN (void(0))
 #else
  #define DB_RUN(STUFF) STUFF
 #endif
#endif

#endif


