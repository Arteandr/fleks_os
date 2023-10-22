#ifndef FS_H
#define FS_H

#include "bitmap.h"
#include "block_group.h"
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
  static void format(size_t fs_size, size_t block_size);
};

#endif
