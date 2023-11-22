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

struct fs_obs {
  bool empty;
  bool exist;
  u8 *block;
  u16 block_no;
};

class FS {
  std::fstream fd;

  super_block *superblock;
  group_desc *gdt;

private:
  static void debug(std::string message);

public:
  FS(std::string filename);
  bool set_block_bitmap(size_t block_group_no, bitmap *bm);
  bitmap *get_block_bitmap(size_t block_group_no);
  u32 allocate_block();
  static void format(size_t fs_size, size_t block_size);
  static void log(std::string message, LogLevel log_level = LogLevel::info,
                  bool new_line = true);
  static void log(u32 group_no, std::string message,
                  LogLevel log_level = LogLevel::info);
  static void log(u32 block_no, u32 group_no, std::string message,
                  LogLevel log_level = LogLevel::info);

  void info();
};

#endif
