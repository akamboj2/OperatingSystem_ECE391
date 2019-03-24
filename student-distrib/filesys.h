#ifndef _FILESYS_H
#define _FILESYS_H

#define MEM_SIZE_4kB          0x1000      //4096 = 2^12 bytes; 4 KB

int32_t read_dentry_by_name (const uint8_t* fname, dentry_t* dentry);
int32_t read_dentry_by_index (uint32_t index, dentry_t* dentry);
int32_t read_data (uint32_t inode, uint32_t offset, uint8_t* buf, uint32_t length);

int file_open();
int file_close();
int file_read(int count, char* buf);
int file_write();

#endif /* _FILESYS_H */