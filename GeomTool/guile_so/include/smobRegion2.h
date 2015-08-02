#ifndef __GT_SMOB_REGION2_H__
#define __GT_SMOB_REGION2_H__ 1

#include "smobMain.h"
#include <libguile.h>

#include "Segment2.h"
#include "SegmentList2.h"
#include "Region2.h"
#include "Serializer.h"
#include "smobSegment2.h"
#include "smobSegmentList2.h"


extern scm_t_bits tag_Region2;

#define ASSERT_REGION2(REGIONTHING) scm_assert_smob_type(tag_Region2, (REGIONTHING))
#define region2fromSCM(THESMOB) (Region2 *)SCM_SMOB_DATA((THESMOB))

void init_Region2(void);

int print_Region2(SCM smob, SCM port, scm_print_state *pstate);
SCM mark_Region2(SCM smob);
size_t free_Region2(SCM smob);

SCM is_Region2(SCM smob);
SCM make_Region2();
SCM append_Region2(SCM smobReg, SCM smobSegListOrRegion);
SCM clear_Region2(SCM smobReg);
SCM ptbuff_Region2(SCM smobReg);
SCM unique_Region2(SCM smobRegion);
SCM list_Region2(SCM smobRegion, SCM listIdx);
SCM numlists_Region2(SCM smobRegion);
SCM translate_Region2(SCM smobRegion, SCM delta_x, SCM delta_y);
#endif



