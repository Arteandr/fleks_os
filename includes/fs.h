#ifndef FS_H
#define FS_H

#include "bitmap.h"
#include "block_group.h"
#include "inode.h"
#include "superblock.h"
#include <fstream>
#include <string>

#define FS_FILENAME "filesystem"

enum class LogLevel { info, warning, error };

class FS {
  std::fstream fd;

  super_block *superblock;
  bitmap *block_bitmap;
  bitmap *inode_bitmap;
  inode *inode_table;

private:
  static void log(std::string message, LogLevel log_level = LogLevel::info);
  static void log(u32 group_no, std::string message,
                  LogLevel log_level = LogLevel::info);
  static void log(u32 block_no, u32 group_no, std::string message,
                  LogLevel log_level = LogLevel::info);
  static void debug(std::string message);

public:
  FS(std::string filename);
  static void format(size_t fs_size, size_t block_size);
};

#endif
