#ifndef PAGING_H
#define PAGING_H

#include "constants.h"
#include "types.h"

/*remember this is only one page table in the first page directory. If you want
to have pages in other directories need to create more page tables.
Maybe should have made a pageTable struct of this array so you can define new page
tables easily... or typename/typedef something
*/
uint32_t pageTable[PAGE_TABLE_SIZE] __attribute__((aligned(PAGE_MEM_SIZE)));
uint32_t pgTbl_vidMem[PAGE_TABLE_SIZE] __attribute__((aligned(PAGE_MEM_SIZE)));
uint32_t pageDirectory[PAGE_TABLE_SIZE] __attribute__((aligned(PAGE_MEM_SIZE)));


//struct for the page directory
/*struct pageDirectory {
bit#
31-12    int address[20];    //physical address of 4K aligned page table
11-9    int avail[3];       //free to use
8    int globalPage;     //controls TLB behavior--1 for kernel page
7    int pageSize;       //0 if 4K page directory entry, 1 if 4M page
6    int reserved;       //used by system
5    int accessed;       //1 if access is made to virtual address, not in use here
4    int cache;          //if 0, processor will not cache any memory in PDE's range
3    int writeThrough;   //always 0 because we want writeback
2    int userSupervisor; //if 1, memory mapped by PDE/PTE is user level
1    int readWrite;      //1 for read/write, 0 for read only
0    int present;        //1 for valid PDE
};

//pageTable struct
struct  pageTable{
31-12    int address[20];    //physical base address of 4K aligned pages
11-9    int avail[3];       //free to use
8    int globalPage;     //controls TLB behavior
7    int pageTableAttributeIdx;  //unused, set to 0
6    int dirty;                  //if page mapped by this PTE has been written to, set to 1
5    int accessed;       //1 if access is made to virtual address, not in use here
4    int cache;          //if 0, processor will not cache any memory in PDE's range
3    int writeThrough;   //always 0 because we want writeback
2    int userSupervisor; //if 1, memory mapped by PDE/PTE is user level
1    int readWrite;      //1 for read/write, 0 for read only
0    int present;        //1 for valid PDE
};
*/

extern void paging_init();

#endif /* PAGING_H*/
