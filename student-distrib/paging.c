#include "paging.h"

uint32_t pageTable[PAGE_TABLE_SIZE];
uint32_t pageDirectory[PAGE_TABLE_SIZE] __attribute__((aligned(4096))); //how do we force pageDirectory to be 4 kB aligned (needs to be for CR3)?
// __attribute__((aligned(4096)))

void paging_init() {
    int numPages = VIRTUAL_MEMORY_SPACE / PAGE_MEM_SIZE;    //4 MB/4 kB = 4000 kB/4 kb = 1000 pages (4 MB total space, 4kB per page)
    uint32_t page = 0x016;      //000010110
    uint32_t kernelPage=0x193;  //110010011
    uint32_t videoPage=0x003;   //000000011
    int i;

    for(i = 0; i < PAGE_TABLE_SIZE; i++){
        pageTable[i] = 0;
        pageDirectory[i] = 0;
    }
    pageDirectory[0] = page;
    pageDirectory[1] = kernelPage;
    pageTable[VIDEO/0x1000] = videoPage;
    asm volatile ("PUSHL %edx; MOVL %CR4, %edx; ORL $0x10, %edx; ANDL $0xFFDF, %edx; MOVL %edx, %CR4; POPL %edx");
    asm volatile ("PUSHL %edx; MOVL %CR0, %edx; ORL $0x8000, %edx; MOVL %edx, %CR0; POPL %edx");

    //xFFFFF000 is used to to only set the 20 MSb of pageDirectory address to top 20 bits in CR3
    asm volatile("MOVL %%CR3, %%edx; ANDL 0xFFFFF000, %%edx; ADDL %0,%%edx; MOVL %%edx, %%CR3" //need double %% for registers single % to specify input/output from C
                    : //no outputs
                    : "r"(pageDirectory&&0xFFFFF000) //inputs
                    : %edx); //clobbered registers
    

    //IF IT's Still not working ask about PSE. How does it work if only one page is 4MB but rest are 4KB? 
    //How would processor know whether to read linear address as DIR|OFFSET or DIR|TABLE|OFFSET?

}
