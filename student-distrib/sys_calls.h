#ifndef SYS_CALLS_H
#define SYS_CALLS_H

#include "types.h"
#include "filesys.h"
#include "rtc.h"

#define FD_FLAG_PRESENT 1
#define FD_FLAG_FILE 2
#define FD_FLAG_DIRECTORY 4
#define FD_FLAG_RTC 8

int32_t halt (uint8_t status);
int32_t execute (const uint8_t* command);
int32_t read (int32_t fd, void* buf, int32_t nbytes);
int32_t write (int32_t fd, const void* buf, int32_t nbytes);
int32_t open (const uint8_t* filename);
int32_t close (int32_t fd);
int32_t getargs (uint8_t* buf, int32_t nbytes);
int32_t vidmap (uint8_t** screen_start);
int32_t set_handler (int32_t signum, void* handler_address);
int32_t sigreturn (void);

// union tables{
//   struct file_table{
//     int32_t (*file_read)(int32_t,void*,int32_t);
//     int32_t (*file_write) (int32_t, const void*, int32_t);
//     int32_t (*file_open) (const uint8_t*);
//     int32_t (*file_close) (int32_t);
//   } file_table ={&file_read, &file_write, &file_open, &file_close};
//
//   struct dir_table{
//     int32_t (*dir_read) (int32_t, void*, int32_t);
//     int32_t (*dir_write) (int32_t, const void*, int32_t);
//     int32_t (*dir_open) (const uint8_t*);
//     int32_t (*dir_close) (int32_t);
//   } dir_table;
//
//   struct rtc_table{
//     int32_t (*rtc_open);
//     int32_t (*rtc_close);
//     int32_t (*rtc_read);
//     int32_t (*rtc_write);
//   } rtc_table;
// } tables;

void (*file_table[4])={file_read, file_write, file_open, file_close};
typedef struct fd_struct{
  void* (*fops_table[4]); //file operations table (see below jump tables)
  int32_t inode;  //inode number
  int32_t f_pos; //file position
  int32_t flags;  //flags indicating present (bit 1) and device type (see defines above)
} fd_struct;

fd_struct file_array[8];
static int32_t file_arr_size = 0;




#endif
