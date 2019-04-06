#ifndef SYS_CALLS_H
#define SYS_CALLS_H

#include "types.h"
#include "filesys.h"
#include "rtc.h"

#define FD_FLAG_PRESENT 1
#define FD_FLAG_FILE 2
#define FD_FLAG_DIRECTORY 4
#define FD_FLAG_RTC 8
#define MAX_OPEN_FILES 8

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


typedef struct ftable{
    int32_t (*read)(int32_t,void*,int32_t);
    int32_t (*write) (int32_t, const void*, int32_t);
    int32_t (*open) (const uint8_t*);
    int32_t (*close) (int32_t);
} ftable;

static ftable file_table = {&file_read, &file_write, &file_open, &file_close};
static ftable dir_table = {&dir_read, &dir_write, &dir_open, &dir_close};
static ftable rtc_table = {&rtc_read, &rtc_write, &rtc_open, &rtc_close};

typedef struct fd_struct{
  ftable* fops_table; //file operations table (see below jump tables)
  int32_t inode;  //inode number
  int32_t f_pos; //file position
  int32_t flags;  //flags indicating present (bit 1) and device type (see defines above)
} fd_struct;

fd_struct file_array[MAX_OPEN_FILES];
static int32_t file_arr_size = 0;




#endif
