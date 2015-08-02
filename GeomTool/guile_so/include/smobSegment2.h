#ifndef __GT_SMOB_SEGMENT2_H__
#define __GT_SMOB_SEGMENT2_H__ 1

#include "smobMain.h"
#include <libguile.h>

#include "Segment2.h"
#include "Serializer.h"

extern scm_t_bits tag_Segment2;

void init_Segment2(void);

int print_Segment2(SCM smob, SCM port, scm_print_state *pstate);
SCM mark_Segment2(SCM smob);
size_t free_Segment2(SCM smob);

SCM is_Segment2(SCM smob);
SCM make_Segment2_line(SCM X1, SCM Y1, SCM X2, SCM Y2);
SCM make_Segment2_arc(SCM ROT, SCM X, SCM Y, SCM Radius, SCM startTheta, SCM endTheta);


#endif
