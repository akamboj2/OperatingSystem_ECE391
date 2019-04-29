#ifndef CONSTANTS_H
#define CONSTANTS_H

#define VIRTUAL_MEMORY_SPACE    0xFFFFFFFF  //4294967296 -1=2^32 bytes; 4GB
#define PAGE_MEM_SIZE           0x1000      //4096 = 2^12 bytes; 4 KB
#define PAGE_TABLE_SIZE         0x10        //1024 = 2^5 bytes; 1 KB
#define _128MB                  0x8000000   //128MB
#define _8MB                    0x800000
#define _4MB                    0x400000
#define _8KB                    0x2000
#define _4B                     0x20
#define PROG_LOAD_ADDR          0x08048000
#define MAP_MASK                0x008F    //present (0), rw (1), user/super (2), cache (4), 4MB pg (7)//shouldn't bit 4 be 1 for kernel and program and 0 for video
#define DEL                     0x7F
#define USER_VID_ADDR           0x08400000 //this is just one directory entry after prog load addr

#define VIDEO                   0xB8000     //note: this may already be defined in lib.c
#define VIDEO1                  0xB9000 //so each vid buf is just the next page (next 4kb after video mem)
#define VIDEO2                  0xBA000
#define VIDEO3                  0xBB000
#define KERNEL_PHYS_ADDR        0x400000    //1048576 = 2^20: 4 MB

#define NUM_T                   3
#define T1                      1
#define T2                      2
#define T3                      3

#define MAX_PROCESS_COUNT       6

#endif /* CONSTANTS_H */
