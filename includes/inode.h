#ifndef INODE_H
#define INODE_H

#define S_IRUSR 0x0020 // user read
#define S_IWUSR 0x0010 // user write
#define S_IXUSR 0x0008 // user execute

#define S_IROTH 0x0004 // other read
#define S_IWOTH 0x0002 // other write
#define S_IXOTH 0x0001 // other execute
//
#define S_SYSTEM 0x0040 // system file

#define BLOCKS_COUNT 15 // Количество блоков адресации

#include "shared.h"
typedef struct inode {
  u16 i_mode; // Формат && права файла
  u16 i_uid;
  u32 i_size;  // Размер файла в байтах
  u32 i_ctime; // Количество секунд с 1 января 1970 с момента создания i-нода
  u32 i_mtime; // Количество секунд с 1 января 1970 с момента создания i-нода
  u32 i_blocks; // Количество блоков
  u32 i_block[BLOCKS_COUNT]; // Указатель на блоки данных этого i-нода
  inline bool mode(u16 m) { return (i_mode & m) == m; };
} inode;

#endif // !INODE_H
