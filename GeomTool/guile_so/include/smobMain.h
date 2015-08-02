#ifndef __GT_SMOB_MAIN_H__
#define __GT_SMOB_MAIN_H__ 1


/* #ifndef NDEBUG */
/*  #define SMOBDEBUG 1 */
/* #endif */

#ifdef DB_PRINTF
 #undef DB_PRINTF
#endif

#ifdef DB_RUN
 #undef DB_RUN
#endif

#ifdef SMOBDEBUG
#include <stdio.h>
  #define DB_PRINTF(STUFF) printf STUFF
#else
 #define DB_PRINTF(STUFF) ((void)(0))
#endif

#ifdef SMOBDEBUG
 #define DB_RUN(STUFF) STUFF
#else
 #define DB_RUN(STUFF) ((void)(0))
#endif




#endif
