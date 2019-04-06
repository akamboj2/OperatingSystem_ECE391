#ifndef _FILESYS_H
#define _FILESYS_H

#include "types.h"

#define MEM_SIZE_4kB          0x1000      //4096 = 2^12 bytes; 4 KB
#define NUM_INODES            64          // 63 inodes + bootblock

typedef struct dentry_t{
  int8_t file_name[32]; //this struct just needs to take up 64 bytes;
  int file_type;
  int inode_num;
  char reserved[24];
} dentry_t;

uint8_t FILE_NAME[33];    //file scope variable indicating name of file to open
dentry_t* dir_entry;
extern int32_t filesys_addr;

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

int32_t file_read (int32_t fd, void* buf, int32_t nbytes);
int32_t file_write (int32_t fd, const void* buf, int32_t nbytes);
int32_t file_open (const uint8_t* filename);
int32_t file_close (int32_t fd);

int32_t dir_read (int32_t fd, void* buf, int32_t nbytes);
int32_t dir_write (int32_t fd, const void* buf, int32_t nbytes);
int32_t dir_open (const uint8_t* filename);
int32_t dir_close (int32_t fd);

#endif /* _FILESYS_H */
