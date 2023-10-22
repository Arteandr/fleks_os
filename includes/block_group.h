#ifndef BLOCK_GROUP_H
#define BLOCK_GROUP_H

#define BLOCKS_PER_GROUP 8192

#include "bitmap.h"
#include "inode.h"
#include "shared.h"
typedef struct group_desc {
  u32 bg_block_bitmap;
  u32 bg_inode_bitmap;
  u32 bg_inode_table;
  u16 bg_free_blocks_count;
  u16 bg_free_inodes_count;
} group_desc;

#endif // !BLOCK_GROUP_H
