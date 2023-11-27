#ifndef FS_H
#define FS_H

#include "bitmap.h"
#include "block_group.h"
#include "dentry.h"
#include "inode.h"
#include "superblock.h"
#include <fstream>
#include <string>

#define DEBUG 1

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

  inode *current_directory;
  u32 current_directory_i_no;
  u16 current_uid;

private:
  // TODO: rework group_no to u32
  static void debug(std::string message);
  u32 create_inode(u32 group_no, u32 inode_no, inode *inode);
  bitmap *get_block_bitmap(size_t block_group_no);
  bitmap *get_inode_bitmap(size_t group_no);
  inode *get_inode_table(u32 group_no);
  bool set_block_bitmap(size_t block_group_no, bitmap *bm);
  bool set_inode_bitmap(size_t group_no, bitmap *bm);
  bool set_inode_table(u32 group_no, inode *inode_table);
  void read_inode(u32 group_no, u32 inode_no, inode *&inode);
  std::pair<u32, u32> allocate_block();
  void write_block(u32 group_no, inode *i, u32 block_no, char *buffer);
  void make_empty_directory(u32 group_no, u32 inode_no, u32 parent_inode_no,
                            inode *i);
  dentry *make_directory_block();

public:
  FS(std::string filename);
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
