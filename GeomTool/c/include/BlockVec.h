#ifndef __GT_BLOCKVEC_H__
#define __GT_BLOCKVEC_H__

/* the worst possible <std::vector> ever seen */
typedef struct BlockVec
{
  void * item;
  unsigned int numItems;
  unsigned int private_itemSize;
  unsigned int private_bufferSize; /* this is the number of items, not bytes - rename for clarity at some point */
} BlockVec;

typedef enum 
  {
    BlockVec_error = 1,
    BlockVec_ok = 2
  } BlockVec_result;


#define GT_BLOCKVEC_VALID(BV) assert((BV) != NULL			\
				      && (BV)->private_itemSize >= 4	\
				      && (BV)->numItems <= (BV)->private_bufferSize \
				      && !((BV)->item == NULL && (BV)->private_bufferSize > 0))

void BlockVec_initialize(BlockVec * vec, const unsigned int itemSize);
void BlockVec_uninitialize(BlockVec * vec);
void BlockVec_clear(BlockVec * vec);
void BlockVec_resize(BlockVec * vec, const unsigned int minSize, const unsigned int defaultOversize);
void * BlockVec_itemRef(BlockVec * vec, const unsigned int idx);
void BlockVec_copy(BlockVec * dest, const BlockVec * const source);
void BlockVec_pop(BlockVec * vec);
void BlockVec_remove(BlockVec * vec, const unsigned int idx);
void BlockVec_removeRange(BlockVec * vec, const unsigned int startIdx, const unsigned int endIdx);
void BlockVec_insertCopy(BlockVec * vec, const void * const item, const unsigned int idx);
void BlockVec_appendCopy(BlockVec * vec, const void * const item);
void BlockVec_appendNull(BlockVec * vec);
BlockVec_result BlockVec_appendCopyList(BlockVec * vec, const BlockVec * const toAppend);


#endif

