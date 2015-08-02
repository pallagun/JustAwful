#include "include/smobSegmentList2.h"

scm_t_bits tag_SegmentList2;


void init_SegmentList2(void)
{
  /* printf("\ninit_SegmentList2()\n\n"); */

  tag_SegmentList2 = scm_make_smob_type("gtSegmentList2", sizeof(SegmentList2));
  scm_set_smob_mark (tag_SegmentList2, mark_SegmentList2);
  scm_set_smob_free (tag_SegmentList2, free_SegmentList2);
  scm_set_smob_print(tag_SegmentList2, print_SegmentList2);

  scm_c_define_gsubr("gt-segmentlist2?", 1, 0, 0, is_SegmentList2);
  scm_c_define_gsubr("gt-make-segmentlist2", 0, 0, 0, make_SegmentList2);
  scm_c_define_gsubr("gt-segmentlist2-append", 2, 1, 0, append_SegmentList2);
  scm_c_define_gsubr("gt-segmentlist2-clear", 1, 0, 0, clear_SegmentList2);
  scm_c_define_gsubr("gt-segmentlist2-ptbuff", 1, 0, 0, ptbuff_SegmentList2);
  
  scm_c_define_gsubr("gt-segmentlist2-isclosed", 1, 0, 0, isClosed_SegmentList2);
  scm_c_define_gsubr("gt-segmentlist2-iscontinuous", 1, 0, 0, isContinuous_SegmentList2);
  scm_c_define_gsubr("gt-segmentlist2-iszeroarea", 1, 0, 0, isZeroArea_SegmentList2);
  scm_c_define_gsubr("gt-segmentlist2-getspin", 1, 0, 0, spin_SegmentList2);
  scm_c_define_gsubr("gt-segmentlist2-gettype", 1, 0, 0, type_SegmentList2);
  scm_c_define_gsubr("gt-segmentlist2-updatecache", 1, 0, 0, updateCache_SegmentList2);
  scm_c_define_gsubr("gt-segmentlist2-numsegments", 1, 0, 0, numsegments_SegmentList2);

  scm_c_define_gsubr("gt-segmentlist2-translate", 3, 0, 0, translate_SegmentList2);
  scm_c_define_gsubr("gt-segmentlist2-scaleorigin", 2, 0, 0, scaleOrigin_SegmentList2);
  scm_c_define_gsubr("gt-segmentlist2-blindexpand", 2, 0, 0, blindexpand_SegmentList2);
  scm_c_define_gsubr("gt-segmentlist2-invert", 1, 1, 0, invert_SegmentList2);
  scm_c_define_gsubr("gt-segmentlist2-almostequal", 2, 1, 0, almostEqual_SegmentList2);
  scm_c_define_gsubr("gt-segmentlist2-simplify", 1, 1, 0, simplify_SegmentList2);
  /* scm_c_define_gsubr("gt-segmentlist2-set!", 3, 0, 0, set_SegmentList2); */
  
  /* bool          SegmentList2_pathRange2(SegmentList2 * segs, Range2 * range); */
  /* bool          SegmentList 2_interiorPoint(SegmentList2 * segs, Point2 * point); */
  /* SegmentList2Type SegmentList2_type(SegmentList2 * segs); */
}
int print_SegmentList2(SCM smob, SCM port, scm_print_state *pstate)
{
  char buff[4096];
  SegmentList2 * list;
  
  ASSERT_SEGLIST(smob);
  list = segmentlist2fromSCM(smob);
  
  Serialize_SL(SEXP, list, buff, 4096);

  scm_puts(buff, port);
  return(1);
}
SCM mark_SegmentList2(SCM smob)
{
  DB_PRINTF(("mark_SegmentList2\n"));
  return SCM_BOOL_F;
}
size_t free_SegmentList2(SCM smob)
{
  SegmentList2 * list;

  ASSERT_SEGLIST(smob);
  list = segmentlist2fromSCM(smob);

  DB_PRINTF(("free_SegmentList2\n"));
  
  SegmentList2_destroy(list);
  return (0);
}
SCM is_SegmentList2(SCM smob)
{
  if (SCM_SMOB_PREDICATE(tag_SegmentList2, smob))
    return SCM_BOOL_T;
  
  return SCM_BOOL_F;
}
SCM make_SegmentList2()
{
  SCM smob;
  SegmentList2 * list;
  
  DB_PRINTF(("make_SegmentList2\n"));
  
  list = SegmentList2_create();
  
  SCM_NEWSMOB( smob, tag_SegmentList2, list);
  return smob;
}
SCM append_SegmentList2(SCM smobList, SCM smobSegOrList, SCM updateCache)
{
  SegmentList2 * list;
  bool doCacheUpdate = false;

  ASSERT_SEGLIST(smobList);
  if (updateCache != SCM_UNDEFINED)
    {				/* I have a 3rd option */
      if (scm_is_bool(updateCache))
	{ 			/* I have a VALID 3rd option */
	  if (scm_is_true(updateCache))
	    doCacheUpdate = true;
	}
      /* TODO: else, throw type exception error */
    }
    
  list = segmentlist2fromSCM(smobList);

  if (SCM_SMOB_PREDICATE(tag_Segment2, smobSegOrList))
    SegmentList2_appendCopy(list,(Segment2 *)SCM_SMOB_DATA(smobSegOrList), doCacheUpdate);
  else if (SCM_SMOB_PREDICATE(tag_SegmentList2, smobSegOrList))
    SegmentList2_appendCopyList(list, segmentlist2fromSCM(smobSegOrList), doCacheUpdate);
  else
    SCM_ASSERT(0, smobSegOrList, 2, "append_SegmentList2");

  return SCM_UNSPECIFIED;
}
/* SCM set_SegmentList2(SCM smobList, SCM index, SCM smobSeg) */
/* { */
/*   SegmentList2 * list; */
/*   Segment2 * seg; */
/*   unsigned int i; */
  
/*   ASSERT_SEGLIST(smobList); */
/*   scm_assert_smob_type(tag_Segment2, smobSeg); */
  
/*   list = segmentlist2fromSCM(smobList); */
/*   i = scm_to_unsigned_integer(index, 0, SegmentList2_numSegs(list)); */
/*   seg = (Segment2 *)SCM_SMOB_DATA(smobSeg); */
  
/* } */
SCM numsegments_SegmentList2(SCM smobList)
{
  SegmentList2 * list;
  ASSERT_SEGLIST(smobList);
  list = segmentlist2fromSCM(smobList);

  return scm_from_uint(SegmentList2_numSegs(list));
}
SCM isClosed_SegmentList2(SCM smobList)
{
  SegmentList2 * list;
  ASSERT_SEGLIST(smobList);
  list = segmentlist2fromSCM(smobList);

  if (SegmentList2_isClosed(list) == true)
    return SCM_BOOL_T;
  else
    return SCM_BOOL_F;
}
SCM isContinuous_SegmentList2(SCM smobList)
{
  SegmentList2 * list;
  ASSERT_SEGLIST(smobList);
  list = segmentlist2fromSCM(smobList);

  if (SegmentList2_isContinuous(list) == true)
    return SCM_BOOL_T;
  else
    return SCM_BOOL_F;
}
SCM isZeroArea_SegmentList2(SCM smobList)
{
  SegmentList2 * list;
  ASSERT_SEGLIST(smobList);
  list = segmentlist2fromSCM(smobList);

  if (SegmentList2_isZeroArea(list) == true)
    return SCM_BOOL_T;
  else
    return SCM_BOOL_F;  
}
SCM spin_SegmentList2(SCM smobList)
{
  SegmentList2 * list;
  ASSERT_SEGLIST(smobList);
  list = segmentlist2fromSCM(smobList);

  return scm_from_int(SegmentList2_spin(list));
}
SCM type_SegmentList2(SCM smobList)
{
  SegmentList2 * list;
  SegmentList2Type type;

  ASSERT_SEGLIST(smobList);
  list = segmentlist2fromSCM(smobList);

  type = SegmentList2_type(list);
  if (0 <= type && type <= 4)
    return scm_from_locale_string(SegmentList2TypeText[type]);
  else
    return scm_from_locale_string("INVALID");
}
SCM clear_SegmentList2(SCM smobList)
{
  SegmentList2 * list;
  ASSERT_SEGLIST(smobList);
  list = segmentlist2fromSCM(smobList);
      
  SegmentList2_clear(list);

  return SCM_UNSPECIFIED;
}
SCM translate_SegmentList2(SCM smobList, SCM delta_x, SCM delta_y)
{
  Vec2 delta;
  SegmentList2 * list;
  
  ASSERT_SEGLIST(smobList);
  list = segmentlist2fromSCM(smobList);

  delta.x = scm_to_double(delta_x);
  delta.y = scm_to_double(delta_y);

  SegmentList2_translate(list, delta);

  return SCM_UNSPECIFIED;  
}
SCM scaleOrigin_SegmentList2(SCM smobList, SCM alpha)
{
  double scale;
  SegmentList2 * list;
  
  ASSERT_SEGLIST(smobList);
  list = segmentlist2fromSCM(smobList);

  scale = scm_to_double(alpha);

  SegmentList2_scaleOrigin(list, scale);

  return SCM_UNSPECIFIED;  
}
SCM blindexpand_SegmentList2(SCM smobList, SCM delta)
{
  /* bool        SegmentList2_blindExpansion(const SegmentList2 * const input, const gtfloat growth, SegmentList2 * output); */
  SegmentList2 * list, * output;
  double d;
  SCM result;
  ASSERT_SEGLIST(smobList);
  list = segmentlist2fromSCM(smobList);
  d = scm_to_double(delta);
  output = SegmentList2_create();
  
  if (SegmentList2_blindExpansion(list, d, output))
    {				/* wohoo- it worked */
      SCM_NEWSMOB( result, tag_SegmentList2, output);
    }
  else
    {
      result = SCM_UNSPECIFIED;
    }
  
  return result;
}
SCM almostEqual_SegmentList2(SCM smobListA, SCM smobListB, SCM boolStrict)
{
  SegmentList2 * A, *B;
  SCM result;
  
  ASSERT_SEGLIST(smobListA);
  ASSERT_SEGLIST(smobListB);
  

  A = segmentlist2fromSCM(smobListA);
  B = segmentlist2fromSCM(smobListB);

  if (boolStrict != SCM_UNDEFINED
      && scm_is_bool(boolStrict)
      && scm_is_true(boolStrict))
    {
      result = ( (SegmentList2_almostEqual2(A,B,false) == true) ? SCM_BOOL_T : SCM_BOOL_F );
    }
  else
    {
      result = ( (SegmentList2_almostEqual2(A,B,true) == true) ? SCM_BOOL_T : SCM_BOOL_F);
    }

  return result;
}
SCM invert_SegmentList2(SCM smobList, SCM smobListDest)
{
  SegmentList2 * list, * output;
  SCM result;
  
  ASSERT_SEGLIST(smobList);
  list = segmentlist2fromSCM(smobList);

  if (smobListDest != SCM_UNDEFINED)
    {				/* I have a destination! no need to modify smobList */
      if (is_SegmentList2(smobListDest))
	{				/* I have a *valid* destination */
	  output = (SegmentList2 *)(SCM_SMOB_DATA(smobListDest));
	  SegmentList2_flip2(list, output);
	  result = SCM_UNDEFINED;
	}
      else
	{
	  /* TODO: throw some kind of error here */
	  result = SCM_BOOL_F;
	}
    }
  else
    {
      /* no output specified, just flip the input */
      SegmentList2_flip1(list);
      result = SCM_UNDEFINED;
    }
  
  return result;
}
SCM getByteVecFromSegList2(const SegmentList2 * const list)
{
  /* function spits out a vector (in 3 space!!! flattens the seglist to z=0 to be in 3 space) */
  /* TODO: something tells me I can do this without the malloc, I can probably do all the work
     inside the scm_c_make_bytevector that I made */
  
  unsigned int ptBuffSize, i, vert_i;
  Point2 * ptBuff;
  float * bytevectorf;
  SCM result;
  result = SCM_BOOL_F;
  
  ptBuffSize = SegmentList2_getApproximationSize(list, GT_APPROX_MAX_ERROR);
  if ( (ptBuff = malloc(ptBuffSize * sizeof(Point2))) == NULL)
    {
      printf("EOM\n");
      exit(1);
    }
  
  ptBuffSize = SegmentList2_getApproximation(list, GT_APPROX_MAX_ERROR, ptBuff, ptBuffSize);
  if (ptBuffSize > 0)
    {
      result = scm_c_make_bytevector(ptBuffSize * sizeof(float) * 3);
      bytevectorf = (float *)SCM_BYTEVECTOR_CONTENTS(result);
      for (i = 0; i < ptBuffSize; ++i)
	{
	  vert_i = i * 3;
	  bytevectorf[vert_i  ] = ptBuff[i].x;
	  bytevectorf[vert_i+1] = ptBuff[i].y;
	  bytevectorf[vert_i+2] = 0;
	}
    }
  return result;
}
SCM ptbuff_SegmentList2(SCM smobList)
{
  /* given a smob (that had better be a segmentlist, stuff its approximation into the bytearray */
  SegmentList2 * list;
  ASSERT_SEGLIST(smobList);
  list = segmentlist2fromSCM(smobList);

  return scm_list_1(getByteVecFromSegList2(list));
}

SCM updateCache_SegmentList2(SCM smobList)
{
  SegmentList2 * list;
  ASSERT_SEGLIST(smobList);
  list = segmentlist2fromSCM(smobList);

  if (SegmentList2_cacheValid(list) == false)
    SegmentList2_refreshCache(list);

  return SCM_UNSPECIFIED;  
}
SCM simplify_SegmentList2(SCM smobList, SCM optionalOutput)
{
  SegmentList2 * list, * output;
  SCM result;
  /* bool SegmentList2_simplify(const SegmentList2 * const input, SegmentList2 * output, unsigned int tryLevel, const bool isContinuous, const bool isClosed) */
  ASSERT_SEGLIST(smobList);
  list = segmentlist2fromSCM(smobList);
  
  if (optionalOutput != SCM_UNDEFINED
    && is_SegmentList2(optionalOutput))
    {				/* simplify the list and write it to optionalOutput */
      ASSERT_SEGLIST(optionalOutput);
      output = segmentlist2fromSCM(optionalOutput);
      SegmentList2_clear(output);

      if (SegmentList2_simplify(list, output, 0, SegmentList2_isContinuous(list), SegmentList2_isClosed(list)) != true)
	SCM_ASSERT(0, smobList, 1, "simplify_SegmentList2");
      
      result = SCM_UNSPECIFIED;	/* my work here is done, bail out */
    }
  else
    {				/* simplyf the smobList (need a temp worker to pull that off */
      output = SegmentList2_create1(SegmentList2_numSegs(list));
      if (output == NULL)
	{
	  printf("EOM\n");
	  exit(1);
	}

      /* if (SegmentList2_simplify(list, output, 0, SegmentList2_isContinuous(list), SegmentList2_isClosed(list)) != true~) */
      /* 	{ */
      /* 	  /\* not true, simplify wasn't able to simplify, but it's still valid output *\/ */
      /* 	  SCM_ASSERT(0, smobList, 1, "simplify_SegmentList2"); */
      /* 	} */
      SegmentList2_simplify(list, output, 0, SegmentList2_isContinuous(list), SegmentList2_isClosed(list));
      SCM_NEWSMOB(result, tag_SegmentList2, output);
    }
  
  return result;
}

