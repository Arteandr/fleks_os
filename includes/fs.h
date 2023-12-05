#ifndef FS_H
#define FS_H

#include "bitmap.h"
#include "block_group.h"
#include "dentry.h"
#include "inode.h"
#include "sha256.h"
#include "shadow.h"
#include "superblock.h"
#include <fstream>
#include <string>

#define DEBUG 0

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
  bool user_exist(const char *login);
  bool user_exist(u32 uid);
  int check_password(const char *login, const char *password);
  u32 get_uid();
  void change_mtime(u32 group_no, u32 inode_no, inode *);
  dentry *make_directory_block();
  info_status directory_info(const char *name, u32 inode_no, u16 type);
  void read_root();

public:
  bitmap *get_block_bitmap(size_t block_group_no);
  bitmap *get_inode_bitmap(size_t group_no);
  FS(std::string filename, bool with_root = false);
  static void format(size_t fs_size, size_t block_size,
                     std::string root_password,
                     std::pair<std::string, std::string> user_data);
  static void log(std::string message, LogLevel log_level = LogLevel::info,
                  bool new_line = true);
  static void log(u32 group_no, std::string message,
                  LogLevel log_level = LogLevel::info);
  static void log(u32 block_no, u32 group_no, std::string message,
                  LogLevel log_level = LogLevel::info);
  void info();
  void list();
  void make_file(const char *filename, u32 size, u16 access = 0);
  void rename(const char *old_filename, const char *new_filename);
  void remove(const char *filename);
  void print_inode_table(u32 group_no);
  void copy(const char *src_filename, const char *dest_filename,
            size_t dest_filename_size);
  size_t read_file(const char *filename, void *&buffer);
  u32 write_file(const char *filename, void *data, u32 size);
  u32 add_user(const char *login, const char *password);
  bool delete_user(u32 uid);
  void add_user_prompt();
  std::string get_current_username();
  std::string get_username_by_uid(u32 uid);
  void chmod(const char *filename, u32 access);
  void users();
  bool login(const char *login, const char *password);
};

#endif
