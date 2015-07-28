#include "include/BlockVec.h"

#include <assert.h>
#include <stdlib.h> 		/* malloc */
#include <string.h> 		/* memcpy, memmove */
/* BlockVec
  unsigned int numObjs;
  void * obj;
  unsigned int private_objSize;
  unsigned int private_bufferObjs; */

#define EOMDIE exit(5)
#define GT_BLOCKVEC_OVERSIZE 2
#define GT_BLOCKVEC_ERROR 0

/* GT_BLOCKVEC_VALID */
void BlockVec_initialize(BlockVec * vec, const unsigned int itemSize)
{
  assert(vec != NULL);
  assert(itemSize >= 4);
  vec->private_itemSize = itemSize;
  vec->private_bufferSize = 0;
  vec->numItems = 0;
  vec->item = NULL;
}
void BlockVec_uninitialize(BlockVec * vec)
{
  if (vec->item)
    free(vec->item);
  
  vec->private_itemSize = 0;
  vec->private_bufferSize = 0;
  vec->numItems = 0;
  vec->item = NULL;
}
void BlockVec_clear(BlockVec * vec)
{
  GT_BLOCKVEC_VALID(vec);
  vec->numItems = 0;
}
void BlockVec_resize(BlockVec * vec, const unsigned int minSize, const unsigned int defaultOversize)
{
  void * temp;
  GT_BLOCKVEC_VALID(vec);
  if ((minSize >= vec->numItems && minSize < vec->private_bufferSize / 2) || /* you want this shrunk and I'll let you */
      (minSize > vec->private_bufferSize)) /* needs to get bigger */
    {				
      temp = realloc(vec->item, (minSize + defaultOversize) * vec->private_itemSize);
      if (temp == NULL)
	EOMDIE;
      
      vec->item = temp;
      vec->private_bufferSize = (minSize + defaultOversize);
    }

  GT_BLOCKVEC_VALID(vec);
}
void * BlockVec_itemRef(BlockVec * vec, const unsigned int idx)
{
  return (vec->item + idx * vec->private_itemSize);
}
void BlockVec_copy(BlockVec * dest, const BlockVec * const source)
{
  GT_BLOCKVEC_VALID(source);
  GT_BLOCKVEC_VALID(dest);
  
  if ( source->private_itemSize != dest->private_itemSize
       || source->numItems > dest->private_bufferSize )
    {
      BlockVec_uninitialize(dest);
      BlockVec_initialize(dest, source->private_itemSize);
      BlockVec_resize(dest, source->numItems, 0);
    }
  
  memcpy(dest->item, source->item, source->numItems*source->private_itemSize);
  dest->numItems = source->numItems;
}
void BlockVec_pop(BlockVec * vec)
{
  GT_BLOCKVEC_VALID(vec);
  --vec->numItems;
}
void BlockVec_remove(BlockVec * vec, const unsigned int idx)
{
  GT_BLOCKVEC_VALID(vec);
  assert (idx < vec->numItems);

  if( idx < vec->numItems - 1)
    {
      memmove(vec->item + (idx * vec->private_itemSize),
	      vec->item + ((idx + 1) * vec->private_itemSize),
	      vec->private_itemSize * (vec->numItems - 1 - idx));
    }
  --vec->numItems;
}
void BlockVec_removeRange(BlockVec * vec, const unsigned int startIdx, const unsigned int endIdx)
{
  GT_BLOCKVEC_VALID(vec);
  assert (startIdx < vec->numItems);
  assert (endIdx < vec->numItems);
  assert (startIdx <= endIdx);

  if( endIdx < vec->numItems - 1)
    {
      memmove(vec->item + (startIdx * vec->private_itemSize),
	      vec->item + ((endIdx + 1) * vec->private_itemSize),
	      vec->private_itemSize * (vec->numItems - 1 - endIdx));
    }
  vec->numItems -= (endIdx - startIdx) + 1;
}
void BlockVec_insertCopy(BlockVec * vec, const void * const item, const unsigned int idx)
{
  assert(0);
}
void BlockVec_appendCopy(BlockVec * vec, const void * const item)
{
  GT_BLOCKVEC_VALID(vec);
  assert(item != NULL);

  if (vec->numItems >= vec->private_bufferSize)
    BlockVec_resize(vec, vec->numItems + 1, GT_BLOCKVEC_OVERSIZE);

  memcpy(vec->item + (vec->numItems*vec->private_itemSize), item, vec->private_itemSize);
  ++vec->numItems;
}
BlockVec_result BlockVec_appendCopyList(BlockVec * vec, const BlockVec * const toAppend)
{
  GT_BLOCKVEC_VALID(vec);
  GT_BLOCKVEC_VALID(toAppend);
  
  if (vec->private_itemSize == toAppend->private_itemSize)
    {
      if (vec->numItems + toAppend->numItems >= vec->private_bufferSize)
	BlockVec_resize(vec, vec->numItems + toAppend->numItems, GT_BLOCKVEC_OVERSIZE);

      memcpy(vec->item + (vec->numItems*vec->private_itemSize), toAppend->item, vec->private_itemSize*toAppend->numItems);
      vec->numItems += toAppend->numItems;
      return BlockVec_ok;
    }
  else
    return BlockVec_error;
}

void BlockVec_appendNull(BlockVec * vec)
{
  GT_BLOCKVEC_VALID(vec);

  if (vec->numItems + 1 > vec->private_bufferSize)
    BlockVec_resize(vec, vec->private_bufferSize+1, 0);
  
  ++vec->numItems;
}
