#ifndef __GT_CONTAINMENT_H__
#define __GT_CONTAINMENT_H__ 1

#include "Constants.h"
#include "Range1.h"
#include "Range2.h"
#include "SegmentList2.h"
#include "Collision1.h"
#include "Collision2.h"
#include "Region2.h"
#include "Set2.h"
/* bunch of functions to test for containment */
/* basically, does A contain B */

bool Containment_R1F(const Range1 * const A, const gtfloat B, const bool allowContact);
bool Distinct_R1F(const Range1 * const A, const gtfloat B, const bool allowContact);

bool Containment_R1R1(const Range1 * const A, const Range1 * const B, const bool allowContact);
bool Distinct_R1R1(const Range1 * const A, const Range1 * const B, const bool allowContact);

bool Containment_R2P(const Range2 * const A, const Point2 * const B, const bool allowContact);
bool Distinct_R2P(const Range2 * const A, const Point2 * const B, const bool allowContact);
bool Containment_R2R2(const Range2 * const A, const Range2 * const B, const bool allowContact);
bool Distinct_R2R2(const Range2 * const A, const Range2 * const B, const bool allowContact);

bool Containment_SLP(const SegmentList2 * const A, const Point2 * const B, const bool allowContact);
bool Distinct_SLP(const SegmentList2 * const A, const Point2 * const B, const bool allowContact);
bool Containment_SLSL(const SegmentList2 * const A, const SegmentList2 * const B, const bool allowContact);
bool Distinct_SLSL(const SegmentList2 * const A, const SegmentList2 * const B, const bool allowContact);

bool Containment_RSL(const Region2 * const A, const SegmentList2 * const B, const bool allowContact);
bool Distinct_RSL(const Region2 * const A, const SegmentList2 * const B, const bool allowContact);
bool Containment_RR(const Region2 * const A, const Region2 * const B, const bool allowContact);
bool Distinct_RR(const Region2 * const A, const Region2 * const B, const bool allowContact);

bool Containment_SSL(const Set2 * const A, const SegmentList2 * const B, const bool allowContact);
bool Distinct_SSL(const Set2 * const A, const SegmentList2 * const B, const bool allowContact);
bool Containment_SR(const Set2 * const A, const Region2 * const B, const bool allowContact);
bool Distinct_SR(const Set2 * const A, const Region2 * const B, const bool allowContact);
bool Containment_SS(const Set2 * const A, const Set2 * const B, const bool allowContact);
bool Distinct_SS(const Set2 * const A, const Set2 * const B, const bool allowContact);



#endif

