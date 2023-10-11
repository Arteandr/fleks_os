#ifndef FS_H
#define FS_H

#include "bitmap.h"
#include "inode.h"
#include "superblock.h"
#include <fstream>
#include <string>
#define FS_FILENAME "filesystem"

class FS {
  std::fstream fd;

  super_block *superblock;
  bitmap *block_bitmap;
  bitmap *inode_bitmap;
  inode *inode_table;

public:
  FS(std::string filename);
};

#endif
