#ifndef PAGING_H
#define PAGING_H

#include "constants.h"
#include "types.h"

//pageTable struct
struct pageTable {
    int address[20];    //physical address of 4K aligned page table
    int avail[3];       //free to use
    int globalPage;     //controls TLB behavior
    int pageSize;       //0 if 4K page directory entry, 1 if 4M page
    int reserved;       //used by system
    int accessed;       //1 if access is made to virtual address, not in use here
    int cache;          //if 0, processor will not cache any memory in PDE's range
    int writeThrough;   //always 0 because we want writeback
    int userSupervisor; //if 1, memory mapped by PDE/PTE is user level
    int readWrite;      //1 for read/write, 0 for read only
    int present;        //1 for valid PDE
};

//struct for the page directory
struct pageDirectory {
    int address[20];    //physical address of 4K aligned page table
    int avail[3];       //free to use
    int globalPage;     //controls TLB behavior
    int pageTableAttributeIdx;  //unused, set to 0
    int dirty;                  //if page mapped by this PTE has been written to, set to 1
    int accessed;       //1 if access is made to virtual address, not in use here
    int cache;          //if 0, processor will not cache any memory in PDE's range
    int writeThrough;   //always 0 because we want writeback
    int userSupervisor; //if 1, memory mapped by PDE/PTE is user level
    int readWrite;      //1 for read/write, 0 for read only
    int present;        //1 for valid PDE
};

extern void paging_init();

#endif /* PAGING_H*/
