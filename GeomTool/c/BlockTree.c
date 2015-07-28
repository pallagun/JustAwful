#include "include/BlockTree.h"

#include <assert.h>
#include <stdlib.h> 		/* malloc */

#define MALLOC_OR_DIE(PTR, SIZE) if ( ( (PTR) = malloc(SIZE) ) == NULL ) exit(5)

BlockTree * BlockTree_create()
{
  BlockTree * output;
  MALLOC_OR_DIE(output, sizeof(BlockTree));
  if (!BlockTree_initialize(output))
    {
      free(output);
      output = NULL;
    }
  output->item = NULL;
  return output;
}
bool BlockTree_initialize(BlockTree * tree)
{
  BlockVec_initialize((BlockVec*)tree, sizeof(BlockTree *));
  return true;
}
void BlockTree_addChild(BlockTree * parent, const BlockTree * const child)
{
  assert(parent != NULL);
  assert(child != NULL);
    
  BlockVec_appendCopy((BlockVec*)parent, &child);
}
unsigned int BlockTree_addChildData(BlockTree * parent, void * data)
{
  BlockTree * newNode;
  assert(parent != NULL);

  newNode = BlockTree_create();
  assert(newNode != NULL);

  newNode->item = data;

  BlockTree_addChild(parent, newNode);
  assert(BlockTree_numChildren(parent) > 0);
  return BlockTree_numChildren(parent)-1;
  
}
unsigned int BlockTree_numChildren(const BlockTree * const node)
{
  return ((BlockVec*)node)->numItems;
}
BlockTree * BlockTree_child(BlockTree * parent, const unsigned int child_idx)
{
  return *(BlockTree **)(BlockVec_itemRef((BlockVec*)parent, child_idx));
}
BlockTree * BlockTree_subsumeChild(BlockTree * parent, const unsigned int child_idx)
{
  BlockTree * newChild;
  BlockTree ** toWhack;
  newChild = BlockTree_create();
  BlockTree_addChild(newChild, BlockTree_child(parent, child_idx));

  toWhack = (BlockTree **)(BlockVec_itemRef((BlockVec*)parent, child_idx));
  *toWhack = newChild;
  return newChild;  
}
void BlockTree_removeChild(BlockTree * parent, const unsigned int child_idx)
{
  BlockVec_remove((BlockVec*)parent, child_idx);
}
void BlockTree_clearChildren(BlockTree * parent, const bool freeItem)
{
  unsigned int i;
  
  for (i = 0; i < BlockTree_numChildren(parent); ++i)
    BlockTree_destroy(BlockTree_child(parent, i), freeItem);

  BlockVec_clear((BlockVec*)parent);
}
void BlockTree_unitialize(BlockTree * tree, const bool freeItem)
{
  BlockTree_clearChildren(tree, freeItem);
  BlockVec_uninitialize((BlockVec*)tree);
  
  if (freeItem && tree->item != NULL)
    free(tree->item);
}
void BlockTree_destroy(BlockTree * tree, const bool freeItem)
{
  BlockTree_unitialize(tree, freeItem);
  free(tree);
}
