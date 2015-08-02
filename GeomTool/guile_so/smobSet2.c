#include "include/smobSet2.h"

scm_t_bits tag_Set2;


void init_Set2(void)
{
  tag_Set2 = scm_make_smob_type("gtSet2", sizeof(Set2));
  scm_set_smob_mark (tag_Set2, mark_Set2);
  scm_set_smob_free (tag_Set2, free_Set2);
  scm_set_smob_print(tag_Set2, print_Set2);

  scm_c_define_gsubr("gt-set2?", 1, 0, 0, is_Set2);
  scm_c_define_gsubr("gt-make-set2", 0, 0, 0, make_Set2);
  scm_c_define_gsubr("gt-set2-append", 2, 0, 0, append_Set2);
  scm_c_define_gsubr("gt-set2-clear", 1, 0, 0, clear_Set2);
  scm_c_define_gsubr("gt-set2-ptbuff", 1, 0, 0, ptbuff_Set2);
  
  scm_c_define_gsubr("gt-set2-union", 2, 1, 0, union_Set2);
  scm_c_define_gsubr("gt-set2-intersect", 2, 1, 0, intersect_Set2);
  scm_c_define_gsubr("gt-set2-translate", 3, 0, 0, translate_Set2);
  scm_c_define_gsubr("gt-set2-invert", 1, 1, 0, invert_Set2);
  scm_c_define_gsubr("gt-set2-expand", 2, 1, 0, expand_Set2);

  scm_c_define_gsubr("gt-set2-region", 2, 0, 0, region_Set2);
  scm_c_define_gsubr("gt-set2-numregions", 1, 0, 0, numregions_Set2);
}
int print_Set2(SCM smob, SCM port, scm_print_state *pstate)
{
  char buff[4096];
  ASSERT_SET2(smob);
  
  Serialize_S(SEXP, set2fromSCM(smob), buff, 4096);

  scm_puts(buff, port);
  return(1);
}
SCM mark_Set2(SCM smob)
{
  DB_PRINTF(("mark_Set2\n"));
  return SCM_BOOL_F;
}
size_t free_Set2(SCM smob)
{
  ASSERT_SET2(smob);
  scm_assert_smob_type(tag_Set2, smob);
  
  DB_PRINTF(("free_Set2\n"));
  
  Set2_destroy(set2fromSCM(smob));
  return (0);
}
SCM is_Set2(SCM smob)
{
  if (SCM_SMOB_PREDICATE(tag_Set2, smob))
    return SCM_BOOL_T;
  
  return SCM_BOOL_F;
}
SCM make_Set2()
{
  SCM smob;
  Set2 * set;
  
  DB_PRINTF(("make_Set2\n"));
  
  set = Set2_create();
  
  SCM_NEWSMOB( smob, tag_Set2, set);
  return smob;
}
SCM numregions_Set2(SCM smobSet)
{
  Set2 * set;
  ASSERT_SET2(smobSet);
  set = (Set2 *)SCM_SMOB_DATA(smobSet);
  return scm_from_uint(Set2_numRegions(set));
}
SCM append_Set2(SCM smobSet, SCM smobSegListOrRegionOrSet)
{
  Set2 * set;

  ASSERT_SET2(smobSet);
  set = set2fromSCM(smobSet);

  if (SCM_SMOB_PREDICATE(tag_SegmentList2, smobSegListOrRegionOrSet))
    Set2_appendCopyAsRegion(set, (SegmentList2 *)SCM_SMOB_DATA(smobSegListOrRegionOrSet));
  else if (SCM_SMOB_PREDICATE(tag_Region2, smobSegListOrRegionOrSet))
    Set2_appendCopy(set, (Region2 *)SCM_SMOB_DATA(smobSegListOrRegionOrSet));
  else if (SCM_SMOB_PREDICATE(tag_Set2, smobSegListOrRegionOrSet))
    Set2_appendCopySet(set, (Set2 *) SCM_SMOB_DATA(smobSegListOrRegionOrSet));
  else
    SCM_ASSERT(0, smobSegListOrRegionOrSet, 2, "append_Set2");

  return SCM_UNSPECIFIED;
}
SCM clear_Set2(SCM smobSet)
{
  ASSERT_SET2(smobSet);
  Set2_clear(set2fromSCM(smobSet));
  return SCM_UNSPECIFIED;
}
SCM ptbuff_Set2(SCM smobSet)
{
  /* given a smob (that had better be a set2, stuff its approximation into the list of bytearrays */
  Set2 * set;
  Region2 * reg;
  unsigned int region_i, list_i;
  SCM result = SCM_EOL;

  ASSERT_SET2(smobSet);
  set = set2fromSCM(smobSet);
  for (region_i = 0; region_i < Set2_numRegions(set); ++region_i)
    {
      reg = &Set2_region(set, region_i);
      for (list_i = 0; list_i < Region2_numLists(reg); ++list_i)
	{
	  if (SegmentList2_numSegs(&Region2_list(reg, list_i)) <= 0)
	    continue;
	  
	  result = scm_cons(getByteVecFromSegList2(&Region2_list(reg, list_i)), result);
	}
    }
  
  return result;
}
SCM region_Set2(SCM smobSet, SCM regionIdx)
{
  /* return a copy of the list in region 2 at index */
  Set2 * set;
  Region2 * reg;
  unsigned int idx;
  SCM smob;

  ASSERT_SET2(smobSet);
  smob = SCM_UNSPECIFIED;
  set = set2fromSCM(smobSet);

  SCM_ASSERT_TYPE(scm_is_unsigned_integer(regionIdx, 0, Set2_numRegions(set)-1), regionIdx, 2, "region_Set2", "Valid uint index into smobSet");
  idx = scm_to_uint(regionIdx);

  assert(idx < Set2_numRegions(set));
  
  DB_PRINTF(("region_Set2: make_Region2\n"));
  reg = Region2_createClone(&Set2_region(set, idx));
    
  SCM_NEWSMOB( smob, tag_Region2, reg);

  return smob;
}

SCM intersect_Set2(SCM smobA, SCM smobB, SCM smobSet2Destination)
{
  Set2 * output;
  SCM result = SCM_BOOL_F;
  SCM hasDestination = SCM_BOOL_F;
  bool success;

  if (smobSet2Destination != SCM_UNDEFINED
      && is_Set2(smobSet2Destination))
    {
      hasDestination = SCM_BOOL_T;
      output = set2fromSCM(smobSet2Destination);
    }
  else
    {
      output = Set2_create();
    }

  if (!SCM_SMOB_PREDICATE(tag_Set2, smobA) &&
      !SCM_SMOB_PREDICATE(tag_Region2, smobA) &&
      !SCM_SMOB_PREDICATE(tag_SegmentList2, smobA))
    {
      SCM_ASSERT(0, smobA, 2, "intersect_Set2");
    }

  if (!SCM_SMOB_PREDICATE(tag_Set2, smobB) &&
      !SCM_SMOB_PREDICATE(tag_Region2, smobB) &&
      !SCM_SMOB_PREDICATE(tag_SegmentList2, smobB))
    {
      SCM_ASSERT(0, smobB, 2, "intersect_Set2");
    }

  if (SCM_SMOB_PREDICATE(tag_Set2, smobA))
    {
      if (SCM_SMOB_PREDICATE(tag_Set2, smobB))
	success = Set2_intersectSS(set2fromSCM(smobA), set2fromSCM(smobB), output);
      else if (SCM_SMOB_PREDICATE(tag_Region2, smobB))
	success = Set2_intersectSR(set2fromSCM(smobA), region2fromSCM(smobB), output);	  
      else /* if(SCM_SMOB_PREDICATE(tag_SegmentList22, smob)) */
	success = Set2_intersectSSL(set2fromSCM(smobA), segmentlist2fromSCM(smobB), output);
    }
  else if (SCM_SMOB_PREDICATE(tag_Region2, smobA))
    {
      if (SCM_SMOB_PREDICATE(tag_Set2, smobB))
	success = Set2_intersectSR(set2fromSCM(smobB), region2fromSCM(smobA), output);
      else if (SCM_SMOB_PREDICATE(tag_Region2, smobB))
	success = Set2_intersectRR(region2fromSCM(smobA), region2fromSCM(smobB), output);	  
      else /* if(SCM_SMOB_PREDICATE(tag_SegmentList22, smob)) */
	success = Set2_intersectRSL(region2fromSCM(smobA), segmentlist2fromSCM(smobB), output);
    }
  else /* if(SCM_SMOB_PREDICATE(tag_SegmentList22, smob)) */
    {
      if (SCM_SMOB_PREDICATE(tag_Set2, smobB))
	success = Set2_intersectSSL(set2fromSCM(smobB), segmentlist2fromSCM(smobB), output);
      else if (SCM_SMOB_PREDICATE(tag_Region2, smobB))
	success = Set2_intersectRSL(region2fromSCM(smobB), segmentlist2fromSCM(smobA), output);	  
      else /* if(SCM_SMOB_PREDICATE(tag_SegmentList22, smob)) */
	success = Set2_intersectSLSL(segmentlist2fromSCM(smobA), segmentlist2fromSCM(smobB), output);
    }

  if (success)
    {
      if (hasDestination == SCM_BOOL_F)
	{			/* you didn't pass in the optional 3rd arg, so I'm returning this */
	  SCM_NEWSMOB(result, tag_Set2, output);
	}
      else
	{ /* you did give me the optional 3rd arg, so I'm going to return SCM_BOOL_T here */
	  result = SCM_BOOL_T;
	}
    }
  else
    {
      /* some kind of horrible error */
      Set2_destroy(output);
      result = SCM_BOOL_F;
    }

  return result;  
}

SCM union_Set2(SCM smobA, SCM smobB, SCM smobSet2Destination)
{
  Set2 * output;
  SCM result = SCM_BOOL_F;
  SCM hasDestination = SCM_BOOL_F;
  bool success;

  if (smobSet2Destination != SCM_UNDEFINED
      && is_Set2(smobSet2Destination))
    {
      hasDestination = SCM_BOOL_T;
      output = set2fromSCM(smobSet2Destination);
    }
  else
    {
      output = Set2_create();
    }

  if (!SCM_SMOB_PREDICATE(tag_Set2, smobA) &&
      !SCM_SMOB_PREDICATE(tag_Region2, smobA) &&
      !SCM_SMOB_PREDICATE(tag_SegmentList2, smobA))
    {
      SCM_ASSERT(0, smobA, 2, "union_Set2");
    }

  if (!SCM_SMOB_PREDICATE(tag_Set2, smobB) &&
      !SCM_SMOB_PREDICATE(tag_Region2, smobB) &&
      !SCM_SMOB_PREDICATE(tag_SegmentList2, smobB))
    {
      SCM_ASSERT(0, smobB, 2, "union_Set2");
    }

  if (SCM_SMOB_PREDICATE(tag_Set2, smobA))
    {
      if (SCM_SMOB_PREDICATE(tag_Set2, smobB))
	success = Set2_unionSS(set2fromSCM(smobA), set2fromSCM(smobB), output);
      else if (SCM_SMOB_PREDICATE(tag_Region2, smobB))
	success = Set2_unionSR(set2fromSCM(smobA), region2fromSCM(smobB), output);	  
      else /* if(SCM_SMOB_PREDICATE(tag_SegmentList22, smob)) */
	success = Set2_unionSSL(set2fromSCM(smobA), segmentlist2fromSCM(smobB), output);
    }
  else if (SCM_SMOB_PREDICATE(tag_Region2, smobA))
    {
      if (SCM_SMOB_PREDICATE(tag_Set2, smobB))
	success = Set2_unionSR(set2fromSCM(smobB), region2fromSCM(smobA), output);
      else if (SCM_SMOB_PREDICATE(tag_Region2, smobB))
	success = Set2_unionRR(region2fromSCM(smobA), region2fromSCM(smobB), output);	  
      else /* if(SCM_SMOB_PREDICATE(tag_SegmentList22, smob)) */
	success = Set2_unionRSL(region2fromSCM(smobA), segmentlist2fromSCM(smobB), output);
    }
  else /* if(SCM_SMOB_PREDICATE(tag_SegmentList22, smob)) */
    {
      if (SCM_SMOB_PREDICATE(tag_Set2, smobB))
	success = Set2_unionSSL(set2fromSCM(smobB), segmentlist2fromSCM(smobB), output);
      else if (SCM_SMOB_PREDICATE(tag_Region2, smobB))
	success = Set2_unionRSL(region2fromSCM(smobB), segmentlist2fromSCM(smobA), output);	  
      else /* if(SCM_SMOB_PREDICATE(tag_SegmentList22, smob)) */
	success = Set2_unionSLSL(segmentlist2fromSCM(smobA), segmentlist2fromSCM(smobB), output);
    }

  if (success)
    {
      if (hasDestination == SCM_BOOL_F)
	{			/* you didn't pass in the optional 3rd arg, so I'm returning this */
	  SCM_NEWSMOB(result, tag_Set2, output);
	}
      else
	{ /* you did give me the optional 3rd arg, so I'm going to return SCM_BOOL_T here */
	  result = SCM_BOOL_T;
	}
    }
  else
    {
      /* some kind of horrible error */
      Set2_destroy(output);
      result = SCM_BOOL_F;
    }

  return result;  
}
SCM translate_Set2(SCM smobRegion, SCM delta_x, SCM delta_y)
{
  Vec2 delta;
    
  ASSERT_SET2(smobRegion);

  delta.x = scm_to_double(delta_x);
  delta.y = scm_to_double(delta_y);

  Set2_translate(set2fromSCM(smobRegion), delta);

  return SCM_UNSPECIFIED;  
}
SCM invert_Set2(SCM smobGt, SCM smobSet2Destination)
{
  SCM hasDestination;
  SCM result;
  bool success; 
  Set2 * output;

  /* type check on smobGt */
  if (!SCM_SMOB_PREDICATE(tag_Set2, smobGt) &&
      !SCM_SMOB_PREDICATE(tag_Region2, smobGt))
    {
      SCM_ASSERT(0, smobGt, 1, "invert_Set2");
    }
  
  if (smobSet2Destination != SCM_UNDEFINED
      && is_Set2(smobSet2Destination))
    {
      hasDestination = SCM_BOOL_T;
      output = set2fromSCM(smobSet2Destination);
    }
  else
    {
      output = Set2_create();
    }

  success = false;
  if (SCM_SMOB_PREDICATE(tag_Set2, smobGt))
    success = Set2_invert(set2fromSCM(smobGt), output);
  else if (SCM_SMOB_PREDICATE(tag_Region2, smobGt))
    success = Set2_invertR(region2fromSCM(smobGt), output);


  if (success)
    {
      if (hasDestination == SCM_BOOL_F)
	{			/* you didn't pass in the optional 3rd arg, so I'm returning this */
	  SCM_NEWSMOB(result, tag_Set2, output);
	}
      else
	{ /* you did give me the optional 3rd arg, so I'm going to return SCM_BOOL_T here */
	  result = SCM_BOOL_T;
	}
    }
  else
    {
      /* some kind of horrible error */
      Set2_destroy(output);
      result = SCM_BOOL_F;
    }

  return result;
}
SCM expand_Set2(SCM smobGt, SCM expansionFloat, SCM smobSet2Destination)
{
  /* bool Set2_expandSL(const SegmentList2 * const segs, const gtfloat expansion, Set2 * output) */
  
  SCM hasDestination;
  SCM result;
  bool success; 
  Set2 * output;
  double expansion;
  
  /* type check on smobGt */
  if (!SCM_SMOB_PREDICATE(tag_Set2, smobGt) &&
      !SCM_SMOB_PREDICATE(tag_Region2, smobGt) &&
      !SCM_SMOB_PREDICATE(tag_SegmentList2, smobGt))
    {
      SCM_ASSERT(0, smobGt, 1, "expand_Set2");
    }

  expansion = scm_to_double(expansionFloat);
  
  if (smobSet2Destination != SCM_UNDEFINED
      && is_Set2(smobSet2Destination))
    {
      hasDestination = SCM_BOOL_T;
      output = set2fromSCM(smobSet2Destination);
    }
  else
    {
      output = Set2_create();
    }

  success = false;
  if (SCM_SMOB_PREDICATE(tag_SegmentList2, smobGt))
    success = Set2_expandSL(segmentlist2fromSCM(smobGt), expansion, output);
  else
    SCM_ASSERT(0, smobGt, 1, "fix this!");

  if (success)
    {
      if (hasDestination == SCM_BOOL_F)
	{			/* you didn't pass in the optional 3rd arg, so I'm returning this */
	  SCM_NEWSMOB(result, tag_Set2, output);
	}
      else
	{ /* you did give me the optional 3rd arg, so I'm going to return SCM_BOOL_T here */
	  result = SCM_BOOL_T;
	}
    }
  else
    {
      /* some kind of horrible error */
      Set2_destroy(output);
      result = SCM_BOOL_F;
    }

  return result;
}
