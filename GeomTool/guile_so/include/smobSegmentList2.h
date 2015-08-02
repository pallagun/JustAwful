#ifndef __GT_SMOB_SEGMENTLIST2_H__
#define __GT_SMOB_SEGMENTLIST2_H__ 1

#include "smobMain.h"
#include <libguile.h>

#include "Segment2.h"
#include "SegmentList2.h"
#include "Serializer.h"
#include "smobSegment2.h"

#define GT_APPROX_MAX_ERROR 0.1

extern scm_t_bits tag_SegmentList2;

#define ASSERT_SEGLIST(THESMOB) scm_assert_smob_type(tag_SegmentList2, (THESMOB))
#define segmentlist2fromSCM(THESMOB) (SegmentList2 *)SCM_SMOB_DATA((THESMOB))

void init_SegmentList2(void);

int print_SegmentList2(SCM smob, SCM port, scm_print_state *pstate);
SCM mark_SegmentList2(SCM smob);
size_t free_SegmentList2(SCM smob);

SCM is_SegmentList2(SCM smob);
SCM make_SegmentList2();
SCM append_SegmentList2(SCM smobList, SCM smobSegOrList, SCM updateCache);
SCM clear_SegmentList2(SCM smobList);
SCM almostEqual_SegmentList2(SCM smobListA, SCM smobListB, SCM boolStrict);

SCM getByteVecFromSegList2(const SegmentList2 * const list);
SCM ptbuff_SegmentList2(SCM smobList);

SCM updateCache_SegmentList2(SCM smobList);
SCM isClosed_SegmentList2(SCM smobList);
SCM isContinuous_SegmentList2(SCM smobList);
SCM isZeroArea_SegmentList2(SCM smobList);
SCM spin_SegmentList2(SCM smobList);
SCM type_SegmentList2(SCM smobList);
SCM numsegments_SegmentList2(SCM smobList);

SCM translate_SegmentList2(SCM smobList, SCM delta_x, SCM delta_y);
SCM scaleOrigin_SegmentList2(SCM smobList, SCM alpha);
SCM blindexpand_SegmentList2(SCM smobList, SCM delta);
SCM invert_SegmentList2(SCM smobList, SCM smobListDest);
/* helper function here */
SCM getByteVecFromSegList2(const SegmentList2 * const list);
SCM simplify_SegmentList2(SCM smobList, SCM optionalOutput);

#endif

