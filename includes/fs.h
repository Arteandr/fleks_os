#include "bitmap.h"
#include "inode.h"
#include "superblock.h"

#ifndef FS_H
#define FS_H

class FS {
  super_block *superblock;
  bitmap *block_bitmap;
  bitmap *inode_bitmap;
  inode *inode_table;
};

#endif
