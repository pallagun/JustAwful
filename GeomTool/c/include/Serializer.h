#ifndef __GT_SERIALIZER_H__
#define __GT_SERIALIZER_H__ 1

#include "Constants.h"
#include "Point2.h"
#include "Line2.h"
#include "Arc2.h"
#include "Segment2.h"
#include "SegmentList2.h"
#include "Region2.h"
#include "Set2.h"

typedef enum SerializeMode
{
  NOT_VALID = 0,
  HUMAN,
  JSON,
  BINARY,
  SEXP
} SerializeMode;

#define GT_SERMODE_VALID(mode) assert(mode == JSON || mode == BINARY || mode == SEXP)

/* these functions should all return the number of bytes(chars) they have consumed to write/read  */
/* I'm pretty sure that this is the worst possible way to do any of these things, probably come
   back and fix this at some point TODO: that */
  
int    Serialize_P(const SerializeMode mode, const Point2 * const input, char * buff, const unsigned int buffSize);
int    Deserialize_P(const SerializeMode mode, char * input, Point2 * output);

int    Serialize_L(const SerializeMode mode, const Line2 * const input, char * buff, const unsigned int buffSize);
int    Deserialize_L(const SerializeMode mode, char * input, Line2 * output);

int    Serialize_A(const SerializeMode mode, const Arc2 * const input, char * buff, const unsigned int buffSize);
int    Deserialize_A(const SerializeMode mode, char * input, Arc2 * output);

int    Serialize_Seg(const SerializeMode mode, const Segment2 * const input, char * buff, const unsigned int buffSize);
int    Deserialize_Seg(const SerializeMode mode, char * input, Segment2 * output);

/* char * Serialize_SL(const SerializeMode mode, const SegmentList2 * input, unsigned int * outputSize); */
int    Serialize_SL_buffMax(const SerializeMode mode, const SegmentList2 * const input);
int    Serialize_SL(const SerializeMode mode, const SegmentList2 * const input, char * buff, const unsigned int buffSize);
int    Deserialize_SL(const SerializeMode mode, char * input, SegmentList2 * output);

/* char * Serialize_R(const SerializeMode mode, const Region2 * input, unsigned int * outputSize); */
int    Serialize_R_buffMax(const SerializeMode mode, const Region2 * const input);
int    Serialize_R(const SerializeMode mode, const Region2 * const input, char * buff, const unsigned int buffSize);
int    Deserialize_R(const SerializeMode mode, char * input, Region2 * output);

/* char * Serialize_S3(const SerializeMode mode, const Set2 * input, unsigned int * outputSize); */
int    Serialize_S_buffMax(const SerializeMode mode, const Set2 * const input);
int    Serialize_S(const SerializeMode mode, const Set2 * const input, char * buff, const unsigned int buffSize);
int    Deserialize_S(const SerializeMode mode, char * input, Set2 * output);





#endif


