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

#ifndef M_PI
 #define M_PI (gtfloat)3.141592653589793238462643383279502884197169399375105820974944592
#endif

#define EOM_DEATH { printf("EOM?\n"); exit(1); }

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


