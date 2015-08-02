#include "include/smobRegion2.h"

scm_t_bits tag_Region2;

void init_Region2(void)
{
  /* printf("\ninit_Region2()\n\n"); */

  tag_Region2 = scm_make_smob_type("gtRegion2", sizeof(Region2));
  scm_set_smob_mark (tag_Region2, mark_Region2);
  scm_set_smob_free (tag_Region2, free_Region2);
  scm_set_smob_print(tag_Region2, print_Region2);

  scm_c_define_gsubr("gt-region2?", 1, 0, 0, is_Region2);
  scm_c_define_gsubr("gt-make-region2", 0, 0, 0, make_Region2);
  scm_c_define_gsubr("gt-region2-append", 2, 0, 0, append_Region2);
  scm_c_define_gsubr("gt-region2-clear", 1, 0, 0, clear_Region2);
  scm_c_define_gsubr("gt-region2-ptbuff", 1, 0, 0, ptbuff_Region2);
  scm_c_define_gsubr("gt-region2-unique", 1, 0, 0, unique_Region2);
  scm_c_define_gsubr("gt-region2-list", 2, 0, 0, list_Region2);
  scm_c_define_gsubr("gt-region2-numlists", 1, 0, 0, numlists_Region2);
  scm_c_define_gsubr("gt-region2-translate", 2, 0, 0, translate_Region2);
}
int print_Region2(SCM smob, SCM port, scm_print_state *pstate)
{
  char buff[4096];
  Region2 * reg;

  ASSERT_REGION2(smob);
  reg = region2fromSCM(smob);
  
  Serialize_R(SEXP, reg, buff, 4096);

  scm_puts(buff, port);
  return(1);
}
SCM mark_Region2(SCM smob)
{
  DB_PRINTF(("mark_Region2\n"));
  return SCM_BOOL_F;
}
size_t free_Region2(SCM smob)
{
  Region2 * reg;

  ASSERT_REGION2(smob);
  reg = region2fromSCM(smob);
  
  DB_PRINTF(("free_Region2\n"));
  
  Region2_destroy(reg);
  return (0);
}
SCM is_Region2(SCM smob)
{
  if (SCM_SMOB_PREDICATE(tag_Region2, smob))
    return SCM_BOOL_T;
  
  return SCM_BOOL_F;
}
SCM make_Region2()
{
  SCM smob;
  Region2 * reg;
  
  DB_PRINTF(("make_Region2\n"));
  
  reg = Region2_create();
  
  SCM_NEWSMOB( smob, tag_Region2, reg);
  return smob;
}
SCM numlists_Region2(SCM smobReg)
{
  Region2 * reg;
  ASSERT_REGION2(smobReg);
  reg = region2fromSCM(smobReg);
  return scm_from_uint(Region2_numLists(reg));
}
SCM append_Region2(SCM smobReg, SCM smobSegListOrRegion)
{
  Region2 * reg;
  ASSERT_REGION2(smobReg);
  reg = region2fromSCM(smobReg);

  if (SCM_SMOB_PREDICATE(tag_SegmentList2, smobSegListOrRegion))
    Region2_appendCopy(reg, (SegmentList2 *)SCM_SMOB_DATA(smobSegListOrRegion));
  else if (SCM_SMOB_PREDICATE(tag_Region2, smobSegListOrRegion))
    Region2_appendCopySet(reg, (Region2 *) SCM_SMOB_DATA(smobSegListOrRegion));
  else
    SCM_ASSERT(0, smobSegListOrRegion, 2, "append_Region2");

  return SCM_UNSPECIFIED;
}
SCM clear_Region2(SCM smobReg)
{
  Region2 * reg;
  ASSERT_REGION2(smobReg);
  reg = region2fromSCM(smobReg);

  Region2_clear(reg);
  return SCM_UNSPECIFIED;
}
SCM ptbuff_Region2(SCM smobReg)
{
  /* given a smob (that had better be a region2, stuff its approximation into the list of bytearrays */
  Region2 * reg;
  unsigned int numLists, i;
  SCM result = SCM_EOL;
  
  ASSERT_REGION2(smobReg);
  reg = region2fromSCM(smobReg);
  numLists = Region2_numLists(reg);
  
/*   /\* scm_cons(x,y) - scheme cons! *\/ */

  for (i = 0; i < numLists; ++i)
    result = scm_cons(getByteVecFromSegList2(&Region2_list(reg, i)), result);
  
  return result;
}
SCM unique_Region2(SCM smobRegion)
{
  /* I think I should be hiding this from scheme? */
  SCM scmOut;
  Region2 * input, * output;

  ASSERT_REGION2(smobRegion);
  input = region2fromSCM(smobRegion);
  output = Region2_create();
  if (SegmentList2Set_makeUnique(input, output))
    {
      SCM_NEWSMOB( scmOut, tag_Region2, output);
    }
  else
    {
      Region2_destroy(output);
      scmOut = SCM_BOOL_F;
    }
  return scmOut;
}
SCM list_Region2(SCM smobRegion, SCM listIdx)
{
  /* return a copy of the list in region 2 at index */
  Region2 * reg;
  SegmentList2 * list;
  unsigned int idx;
  SCM smob;

  ASSERT_REGION2(smobRegion);
  smob = SCM_UNSPECIFIED;
  reg = region2fromSCM(smobRegion);

  SCM_ASSERT_TYPE(scm_is_unsigned_integer(listIdx, 0, Region2_numLists(reg)-1), listIdx, 2, "list_Region2", "Valid uint index into smobRegion");
  idx = scm_to_uint(listIdx);

  assert(idx < Region2_numLists(reg));

  DB_PRINTF(("list_Region2: make_SegmentList2\n"));
  list = SegmentList2_createClone(&Region2_list(reg, idx));
    
  SCM_NEWSMOB( smob, tag_SegmentList2, list);

  return smob;  
}
SCM translate_Region2(SCM smobRegion, SCM delta_x, SCM delta_y)
{
  Vec2 delta;
    
  ASSERT_REGION2(smobRegion);

  delta.x = scm_to_double(delta_x);
  delta.y = scm_to_double(delta_y);

  Region2_translate(region2fromSCM(smobRegion), delta);

  return SCM_UNSPECIFIED;  
}

