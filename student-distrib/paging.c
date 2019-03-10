#include "paging.h"
#include "types.h"

uint32_t pageTable[PAGE_TABLE_SIZE];
uint32_t pageDirectory[PAGE_TABLE_SIZE];
// __attribute__((aligned(4096)))

void paging_init() {
    int numPages = VIRTUAL_MEMORY_SPACE / PAGE_MEM_SIZE;    //4 MB/4 kB = 4000 kB/4 kb = 1000 pages (4 MB total space, 4kB per page)
    uint32_t page = 0x016;//10010110;
    uint32_t kernelPage=0x193;
    uint32_t videoPage=0x003;
    int i;

    for(i = 0; i < PAGE_TABLE_SIZE; i++){
        pageTable[i] = 0;
        pageDirectory[i] = 0;
    }
    pageDirectory[0] = page;
    pageDirectory[1] = kernelPage;
    pageTable[VIDEO/0x1000] = videoPage;
    asm volatile ("MOVL %CR0, %edx; ORL $0x8000, %edx; MOVL %edx, %CR0");
    asm volatile ("MOVL %CR4, %edx; ORL $0x10, %edx; ANDL $0xFFDF, %edx; MOVL %edx, %CR4");
}
