#ifndef CONSTANTS_H
#define CONSTANTS_H

#define VIRTUAL_MEMORY_SPACE    0xFFFFFFFF  //4294967296 -1=2^32 bytes; 4GB
#define PAGE_MEM_SIZE           0x1000      //4096 = 2^12 bytes; 4 KB
#define PAGE_TABLE_SIZE         0x10        //1024 = 2^5 bytes; 1 KB
#define _128MB                  0x8000000   //128MB
#define _8MB                    0x800000
#define _4MB                    0x400000
#define _8KB                    0x2000
#define PROG_LOAD_ADDR          0x08048000
#define MAP_MASK                0x0087
                                            //^note this is also pg directory size
#define VIDEO                   0xB8000     //note: this may already be defined in lib.c
#define KERNEL_PHYS_ADDR        0x400000    //1048576 = 2^20: 4 MB
#endif /* CONSTANTS_H */
