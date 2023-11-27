#ifndef DENTRY_H
#define DENTRY_H
#include "shared.h"

const u8 FILE_NAME_LENGTH = 128;

const u8 FILE_TYPE_UNKNOWN = 0;
const u8 FILE_TYPE_FILE = 1;
const u8 FILE_TYPE_DIR = 2;

typedef struct dentry {
  u32 inode;
  u32 rec_len;
  u8 name_len;
  u8 file_type;
  char name[FILE_NAME_LENGTH];
} dentry;

#endif // !DENTRY_H
