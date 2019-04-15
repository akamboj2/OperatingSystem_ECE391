#ifndef SYS_CALLS_H
#define SYS_CALLS_H

#include "types.h"
#include "filesys.h"
#include "rtc.h"
#include "keyboard.h"

#define FD_FLAG_PRESENT 1
#define FD_FLAG_FILE 2
#define FD_FLAG_DIRECTORY 4
#define FD_FLAG_RTC 8
#define MAX_OPEN_FILES 8
#define _ONE_STACK_ENTRY 0x04
#define MAX_ARGS 127

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

//this stuff moved to sys_calls.c
// extern ftable file_table;
// extern ftable dir_table;
// extern ftable rtc_table;
//next two functions are dummies for the stdin and out table
// static int32_t stdin_write(int32_t a, const void* b, int32_t c){return 0;};
// static int32_t stdout_read(int32_t a,void* b,int32_t c){return 0;};
// static ftable stdin_table = {&terminal_read,&stdin_write,&terminal_open,&terminal_close};
// static ftable stdout_table = {&stdout_read,&terminal_write,&terminal_open,&terminal_close};

typedef struct fd_struct{
  ftable* fops_table; //file operations table (see below jump tables)
  int32_t inode;  //inode number
  int32_t f_pos; //file position
  int32_t flags;  //flags indicating present (bit 1) and device type (see defines above)
} fd_struct;


typedef struct pcb_t pcb_t;
extern int32_t curr_process;

struct pcb_t{
  fd_struct file_array[MAX_OPEN_FILES];
  int32_t file_arr_size;
  int32_t process_num;
  int32_t k_stack;  //kernel stack number
  pcb_t * parent_task;
  pcb_t * child_task;
  uint32_t eip;
  uint32_t esp;
  uint32_t ebp;
  uint32_t esp0;
  uint8_t args[MAX_ARGS];
};

pcb_t * getPCB(int32_t curr);

#endif
