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
#define FS_ROOT_INODE_NO 0

enum class LogLevel { info, warning, error };

enum SEARCH_TYPE {
  SAME_ENTRY = 0x001,
  EMPTY_ENTRY = 0x002,
  ALL_ENTRIES = 0x004,

  RETURN_BLOCK = 0x100,
};

typedef struct info_status {
  bool empty;
  bool exist;
  char *block;
  u32 block_no;
  dentry *directory;
  u32 found_count;
} info_status;

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
  u32 create_inode(u32 inode_no, inode *inode);
  u32 create_inode(inode *inode);
  void read_inode(u32 inode_no, inode *&inode);
  char extend_inode(u32 inode_no, inode *i_node);
  void free_inode(u32 inode_no);
  bitmap *get_block_bitmap(size_t block_group_no);
  bitmap *get_inode_bitmap(size_t group_no);
  inode *get_inode_table(u32 group_no);
  bool set_block_bitmap(size_t block_group_no, bitmap *bm);
  bool set_inode_bitmap(size_t group_no, bitmap *bm);
  bool set_inode_table(u32 group_no, inode *inode_table);
  std::pair<u32, u32> allocate_block();
  void free_block(u32 group_no, u32 block_no);
  void write_block(u32 group_no, inode *i, u32 block_no, char *buffer);
  void read_block(u32 group_no, inode *i, u32 block_no, char *&buffer);
  void make_empty_directory(u32 group_no, u32 inode_no, u32 parent_inode_no,
                            inode *i);
  dentry *make_directory_block();
  info_status directory_info(const char *name, u32 inode_no, u16 type);
  void read_root();

public:
  FS(std::string filename, bool with_root = false);
  static void format(size_t fs_size, size_t block_size);
  static void log(std::string message, LogLevel log_level = LogLevel::info,
                  bool new_line = true);
  static void log(u32 group_no, std::string message,
                  LogLevel log_level = LogLevel::info);
  static void log(u32 block_no, u32 group_no, std::string message,
                  LogLevel log_level = LogLevel::info);
  void info();
  void list();
  void make_file(const char *filename);
  void rename(const char *old_filename, const char *new_filename);
};

#endif
