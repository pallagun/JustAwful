#ifndef __GT_SMOB_SET2_H__
#define __GT_SMOB_SET2_H__ 1

#include "smobMain.h"
#include <libguile.h>

#include "Segment2.h"
#include "SegmentList2.h"
#include "Region2.h"
#include "Set2.h"
#include "Serializer.h"
#include "smobSegment2.h"
#include "smobSegmentList2.h"
#include "smobRegion2.h"


extern scm_t_bits tag_Set2;

#define ASSERT_SET2(THESMOB) scm_assert_smob_type(tag_Set2, (THESMOB))
#define set2fromSCM(THESMOB) (Set2 *)SCM_SMOB_DATA((THESMOB))

void init_Set2(void);

int print_Set2(SCM smob, SCM port, scm_print_state *pstate);
SCM mark_Set2(SCM smob);
size_t free_Set2(SCM smob);

SCM is_Set2(SCM smob);
SCM make_Set2();
SCM append_Set2(SCM smobSet, SCM smobSegListOrRegionOrSet);
SCM clear_Set2(SCM smobSet);
SCM ptbuff_Set2(SCM smobSet);
SCM region_Set2(SCM smobSet, SCM regionIdx);
SCM numregions_Set2(SCM smobSet);

SCM translate_Set2(SCM smobSet, SCM delta_x, SCM delta_y);

SCM union_Set2(SCM smobSegListA, SCM smobSegListB, SCM smobSet2Destination);
SCM intersect_Set2(SCM smobSegListA, SCM smobSegListB, SCM smobSet2Destination);
SCM invert_Set2(SCM smobThing, SCM smobDestination);
SCM expand_Set2(SCM smobGt, SCM expansionFloat, SCM smobSet2Destination);
/* SCM unionRSL_Set2(SCM smobRegionA, SCM smobSegListB, SCM smobSet2Destination); */
/* SCM intersectRSL_Set2(SCM smobRegionA, SCM smobSegListB, SCM smobSet2Destination); */
/* SCM unionRR_Set2(SCM smobRegionA, SCM smobRegionB, SCM smobSet2Destination); */
/* SCM intersectRR_Set2(SCM smobRegionA, SCM smobRegionB, SCM smobSet2Destination); */

/* SCM unionSSL_Set2(SCM smobSetA, SCM smobSegListB, SCM smobSet2Destination); */
/* SCM intersectSSL_Set2(SCM smobSetA, SCM smobSegListB, SCM smobSet2Destination); */
/* SCM unionSR_Set2(SCM smobSetA, SCM smobRegionB, SCM smobSet2Destination); */
/* SCM intersectSR_Set2(SCM smobSetA, SCM smobRegionB, SCM smobSet2Destination); */
/* SCM unionSS_Set2(SCM smobSetA, SCM smobSetB, SCM smobSet2Destination); */
/* SCM intersectSS_Set2(SCM smobSetA, SCM smobSetB, SCM smobSet2Destination); */


#endif



