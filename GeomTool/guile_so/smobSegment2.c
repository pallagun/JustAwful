#include "include/smobSegment2.h"


scm_t_bits tag_Segment2;

void init_Segment2(void)
{
  DB_PRINTF(("\ninit_Segment2()\n\n"));

  tag_Segment2 = scm_make_smob_type("gtSegment2", sizeof(Segment2));
  scm_set_smob_mark (tag_Segment2, mark_Segment2);
  scm_set_smob_free (tag_Segment2, free_Segment2);
  scm_set_smob_print(tag_Segment2, print_Segment2);

  scm_c_define_gsubr("gt-segment2?", 1, 0, 0, is_Segment2);
  scm_c_define_gsubr("gt-make-line2", 4, 0, 0, make_Segment2_line);
  scm_c_define_gsubr("gt-make-arc2", 6, 0, 0, make_Segment2_arc);
}

int print_Segment2(SCM smob, SCM port, scm_print_state * pstate)
{
  char buff[128];
  Segment2 * seg;

  scm_assert_smob_type(tag_Segment2, smob);
  seg = (Segment2 *)SCM_SMOB_DATA(smob);

  Serialize_Seg(SEXP, seg, buff, 128);
  scm_puts(buff, port);
  return (1);  
}
SCM mark_Segment2(SCM smob)
{
  DB_PRINTF(("mark_Segment2\n"));
  return SCM_BOOL_F;
}
size_t free_Segment2(SCM smob)
{
  Segment2 * seg;

  scm_assert_smob_type(tag_Segment2, smob);
  seg = (Segment2 *)SCM_SMOB_DATA(smob);
  
  DB_PRINTF(("free_Segment2\n"));
  free(seg);
  return (0);
}
SCM is_Segment2(SCM smob)
{
  if (SCM_SMOB_PREDICATE(tag_Segment2, smob))
    return SCM_BOOL_T;
  
  return SCM_BOOL_F;
}
SCM make_Segment2_line(SCM X1, SCM Y1, SCM X2, SCM Y2)
{
  SCM smob;
  Segment2 * seg;

  DB_PRINTF(("make_Segment2_line\n"));
  
  seg = malloc(sizeof(Segment2));

  seg->type = LINE;
  seg->s.line.start.x = scm_to_double(X1);
  seg->s.line.start.y = scm_to_double(Y1);
  seg->s.line.end.x   = scm_to_double(X2);
  seg->s.line.end.y   = scm_to_double(Y2);

  SCM_NEWSMOB( smob, tag_Segment2, seg);
  return smob;
}
SCM make_Segment2_arc(SCM ROT, SCM X, SCM Y, SCM Radius, SCM startTheta, SCM endTheta)
{
  SCM smob;
  Segment2 * seg;
  
  DB_PRINTF(("make_Segment2_arc\n"));

  seg = malloc(sizeof(Segment2));
  seg->type = ARC;
  seg->s.arc.angle.rot   = scm_to_int16 (ROT       );
  seg->s.arc.angle.start = scm_to_double(startTheta);
  seg->s.arc.angle.end   = scm_to_double(endTheta  );
  seg->s.arc.radius      = scm_to_double(Radius    );
  seg->s.arc.center.x    = scm_to_double(X         );
  seg->s.arc.center.y    = scm_to_double(Y         );

  SCM_NEWSMOB( smob, tag_Segment2, seg);
  return smob;
}






