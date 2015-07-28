#include "include/Range1Set.h"

#include <assert.h>
#include <string.h> 		/* for memmove */
#include <stdlib.h> 		/* malloc/free */
#include <stdio.h> 		/* puts */
/* a set of ranges that DONT overlap and are in order */
/* typedef struct Range1Set */
/* { */
/*   unsigned int numRanges; */
/*   Range1 * range; */
/*   unsigned int private_allocRanges; */
/* } Range1Set; */

#ifndef NDEBUG
bool     Range1Set_valid(const Range1Set * const set)
{
  unsigned int i;
  
  GT_RANGE1SET_VALID(set);
  if (set->numRanges <= 1)
    return (true);

  for(i = 0; i < set->numRanges-1; ++i)
    {				/* ensure that this range is below the next range */
      assert(set->range[i].Min < set->range[i].Max);
      assert(set->range[i].Max < set->range[i+1].Min);      
    }
  return(true);
}
#endif


void Internal_resize(Range1Set * set, const unsigned int growth)
{
  void * temp;
  assert(Range1Set_valid(set));
  assert(growth > 0);
  
  temp = realloc(set->range, (set->private_allocRanges + growth) * sizeof(Range1));
  if (temp != NULL)
    {
      set->range = temp;
      set->private_allocRanges += growth;
    }
  else
    {
      puts("ERROR, unable to reallocate memory in SegmentList2::Internal_grow()");
      exit(1);
    }
  assert(Range1Set_valid(set));
}
void Internal_remove3(Range1Set * set, const unsigned int startIdx, const unsigned int endIdx)
{
  /* assert(Range1Set_valid(set));  -- might not actually be valid due to current workings */
  GT_RANGE1SET_VALID(set); 	/* but this should always be true */
  assert(startIdx < set->numRanges && endIdx < set->numRanges);
  /* assert(startIdx != endIdx); 	/\* you should have called Internal_remove2 *\/ */

  if (startIdx < set->numRanges - 1) /* not just whacking the end one off, you have to move stuff down. */
    memmove( &(set->range[startIdx]), &(set->range[endIdx+1]), set->numRanges - endIdx - 1);

  set->numRanges -= endIdx - startIdx + 1;
  assert(Range1Set_valid(set));
}
void Internal_remove2(Range1Set * set, const unsigned int idx)
{
  assert(Range1Set_valid(set));
  assert(idx < set->numRanges);

  if (idx < set->numRanges - 1) /* not just whacking the end one off, you have to move stuff down. */
    memmove( &(set->range[idx]), &(set->range[idx+1]), set->numRanges - idx - 1);

  --set->numRanges;
  assert(Range1Set_valid(set));
}
void Internal_insert(Range1Set * set, const gtfloat Min, const gtfloat Max, const unsigned int idx)
{
  assert(Range1Set_valid(set));
  assert(set != NULL);
  assert(idx <= set->numRanges);
  assert(idx >= 0); 		/* it's a unsigned int... so this is probably pointless */
  
  if (set->numRanges + 1 > set->private_allocRanges)
    Internal_resize(set, set->private_allocRanges); /* double it in size */

  if (idx < set->numRanges)	/* have to move some memory on down the line */
    memmove( &(set->range[idx+1]), &(set->range[idx]), (set->numRanges-idx)*sizeof(Range1));
 
  set->range[idx].Min = Min;
  set->range[idx].Max = Max;
  ++set->numRanges;
  assert(Range1Set_valid(set));
}
Range1Set * Range1Set_create()
{
  Range1Set * temp = malloc(sizeof(Range1Set));

  temp->numRanges = 0;
  temp->private_allocRanges = GT_RANGE1SET_DEFAULT_SIZE;
  temp->range = malloc(sizeof(Range1)*GT_RANGE1SET_DEFAULT_SIZE);

  GT_RANGE1SET_VALID(temp);
  return (temp);  
}
void Range1Set_destroy(Range1Set * set)
{
  assert(Range1Set_valid(set));
  free(set->range);
  free(set);
}
void Range1Set_add(Range1Set * set, const Range1 * const add)
{
  /* check to see if I intersect with anything */
  unsigned int i, j;
  bool merged;

  assert(add != NULL);
  assert(Range1Set_valid(set));
  /* Range1_set3(*addition, add->Min, add->Max); */
  merged = false;

  for (i = 0; i < set->numRanges; ++i)
    {
      if (set->range[i].Max < add->Min)
	continue; 		/* not there yet.  keep on going. */
      else if (set->range[i].Min > add->Max)
	break; 			/* finished passed it.  if you're inserting, insert here.*/
      
      /* if you're here, you need to merge */
      merged = true;
      if (set->range[i].Min > add->Min)
	{			/* merge here. */
	  set->range[i].Min = add->Min;
	}
      if (set->range[i].Max < add->Max)
	{ /* now I need to look ahead to see if I need to gobble up the ranges after me. */
	  for (j = i+1; j < set->numRanges; ++j)
	    {			/* do I need to gobble up this next range? */
	      if (set->range[j].Min > add->Max)
		break; 		/* this range I can't gobble up, stop at that last one. */
	    }
	  --j; /* this is the last range that should be gobbling */
	  if (set->range[j].Max > add->Max)
	    set->range[i].Max = set->range[j].Max; /* steal the end point */
	  else					   /* the addition is bigger */
	    set->range[i].Max = add->Max;
	  /* now remove any ranges between i+1 and j */
	  if (i < j)
	    {
	      Internal_remove3(set, i+1, j);
	    }	    
	}
      break; 			/* get outta here, your work is done. */
    }
  if (merged == false)
    { 				/* unable to merge this range into the other ranges, it's distinct, just insert it */
      /* if (i < set->numRanges) */
      /* 	++i; */

      Internal_insert(set, add->Min, add->Max, i);
    }
  assert(Range1Set_valid(set));
}
void Range1Set_subtract(Range1Set * set, const Range1 * const sub)
{
  /* remove this chunk from the range set. */
  
  unsigned int i,j;
  assert(false); 		/* COMPLETELY UNTESTED - and I'm pretty sure it's wrong :( */
  assert(sub != NULL);
  assert(Range1Set_valid(set));
  
  for (i = 0; i < set->numRanges; ++i)
    {
      if (set->range[i].Min > sub->Max)
	continue; 		/* not there yet.  keep on going. */
      else if (set->range[i].Max < sub->Min)
	break; 			/* finished passed it.*/
      
      /* this segment has to be modified or deleted. */
      for (j = i+1; j < set->numRanges; ++j)
	{
	  if (sub->Max < set->range[i].Min)
	    break;
	}

      --j;
      /* I need to modify all ranges from i to j */
      if (i == j)
	{ /* it's only affecting one single span.  */
	  /* lets see what I have to do to it. */
	  if (set->range[i].Min < sub->Min)
	    { /* keeping the first part apparently. */
	      if (sub->Max < set->range[i].Max)
		{		/* keeping the last part too, bad case - hole punch. */
		  /* I want to end up with two segments, [ range[i].Min -> sub.Min ] and [ sub.Max -> range[i].Max ] */
		  Internal_insert(set, sub->Max, set->range[i].Max, i+1); /* put in the new segment (behind i) */
		  set->range[i].Max = sub->Min; /* and update the i range to end at sub.Min */
		}
	      else 		/* if (set->range[i].Max <= span->Max) */
		{		/* not keeping the last part of this range, only the front part */
		  set->range[i].Max = sub->Min;
		}
	    }
	  else 			/* if (span->Min < set->range[i].Min) */
	    { 			/* you are NOT keeping the first part of this segment */
	      if (sub->Max < set->range[i].Max)
		{		/* keeping the last part, not the front part. */
		  set->range[i].Min = sub->Max;
		}
	      else 		/* if (set->range[i].Max <= span->Max) */
		{		/* not keeping the last part of this range OR the front part of this range... not keeping this range. */
		  Internal_remove2(set, i);
		}
	    }
	}
      else
	{			/* truncate or remove the starting segment */
	  if (set->range[i].Min < sub->Min) 
	    { 			/* you just need to truncate off the back of this range */
	      set->range[i].Max = sub->Min;
	    }
	  else 			/* if (sub->Min <= set->range[i].Min) */
	    {			/* I need to remove this entire range */
	      --i; 		/* queue it up for removal */
	    }
	  /* now truncate or remove the ending segment */
	  if (sub->Max < set->range[j].Max)
	    { 			/* just need to truncate off the first part of this range */
	      set->range[j].Min = sub->Max;
	    }
	  else 			/* if (set->range[j].Max <= sub->Max) */
	    { 
	      ++j;		/* queue this segment up for removal, has to go. */
	    }
	  /* now remove every segment between i and j inclusive */
	  Internal_remove3(set, i, j);
 	}
      break;      
    }
    assert(Range1Set_valid(set));
}
void Range1Set_clear(Range1Set * set)
{
  assert(Range1Set_valid(set)); 
  set->numRanges = 0;
  assert(Range1Set_valid(set));
}



