#include "include/smobSegment2.h"
#include "include/smobSegmentList2.h"
#include "include/smobRegion2.h"
#include "include/smobSet2.h"

extern scm_t_bits tag_Segment2;
extern scm_t_bits tag_SegmentList2;
extern scm_t_bits tag_Region2;

void init_geomTool(void)
{
  init_Segment2();
  init_SegmentList2();
  init_Region2();
  init_Set2();
}
