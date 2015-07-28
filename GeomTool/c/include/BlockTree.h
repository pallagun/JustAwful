#ifndef __GT_BLOCKTREE_H__
#define __GT_BLOCKTREE_H__

#include <stdbool.h>
#include "BlockVec.h"

/* a not so great tree structure */
typedef struct BlockTree
{
  BlockVec children;
  void * item;
} BlockTree;


#define GT_BLOCKTREE_VALID(BT) assert(0) /* TODO */

BlockTree * BlockTree_create();
bool BlockTree_initialize(BlockTree * tree);
void BlockTree_addChild(BlockTree * parent, const BlockTree * const child);
unsigned int BlockTree_addChildData(BlockTree * parent, void * data);
void BlockTree_removeChild(BlockTree * parent, const unsigned int child_idx);
unsigned int BlockTree_numChildren(const BlockTree * const parent);
BlockTree * BlockTree_child(BlockTree * parent, const unsigned int child_idx);
BlockTree * BlockTree_subsumeChild(BlockTree * parent, const unsigned int child_idx);
void BlockTree_clearChildren(BlockTree * parent, const bool freeItem);
void BlockTree_unitialize(BlockTree * tree, const bool freeItem);
void BlockTree_destroy(BlockTree * tree, const bool freeItem);
  


#endif

