#include "paging.h"

//NEED 4KB alignment for PT and PD arrays
//Needed for: CR3 Register, and table entries (PTE AND PDE) assume alignment that's why they only use
//top 20 bits of Page_table base address (bits 31-12)

//this is already 4kB aligned bc it has 1024 (1KB) entries of 4 bytes each entry (int32=4B)
//^^jk that's not true bc it could have been statically allocated at addreess not divisble by 4kB even if its size is 4KB

/*paging_init
* initializes the paging_initno args
* modifies the pagetable and page directory
*/
void paging_init() {
//    int numPages = VIRTUAL_MEMORY_SPACE / PAGE_MEM_SIZE;    //4 MB/4 kB = 4000 kB/4 kb = 1000 pages (4 MB total space, 4kB per page)
    //Specifications of bits 8-0 of pde and pte of a normal Page, the  Kernel Page and the video Page
    uint32_t page = 0x017;      //000010111
    uint32_t kernelPage=0x193;  //110010011
    uint32_t videoPage=0x003;   //000000011
    int i;

    for(i = 0; i < PAGE_TABLE_SIZE; i++){
        pageTable[i] = 0;
        pageDirectory[i] = 0;
    }

    //don't need to mask and add bc the bottom bits should be zero if aligned correctly (and it must be aligned correctly to work)
    pageDirectory[0] =  (unsigned)pageTable + page;
    pageDirectory[1] = KERNEL_PHYS_ADDR + kernelPage; //how does processor know this block only has 1 table?--PSE question
    pageTable[VIDEO/PAGE_MEM_SIZE] = VIDEO+videoPage; //addes page-table base address (bits 31-12) to videoPage (bits 8-0), (we ignore 11-9)

    //xFFFFF000 is used to to only set the 20 MSb of pageDirectory address to top 20 bits in CR3
    asm volatile("MOVL %%CR3, %%edx; ANDL $0x00000FFF, %%edx; ADDL %0,%%edx; MOVL %%edx, %%CR3;" //need double %% for registers single % to specify input/output from C
                    : //no outputs
                    : "r"(pageDirectory) //inputs (need to mask in this case to preserve rest of CR3)
                    : "%edx"); //clobbered registers

    asm volatile ("MOVL %CR4, %edx; ORL $0x10, %edx; MOVL %edx, %CR4; ");// ANDL $0xFFDF, %edx;
    asm volatile ("MOVL %CR0, %edx; ORL $0x80000000, %edx; MOVL %edx, %CR0; leave; ret;");


    //IF IT's Still not working ask about PSE. How does it work if only one page is 4MB but rest are 4KB?
    //How would processor know whether to read linear address as DIR|OFFSET or DIR|TABLE|OFFSET?

    /*Question: How does page dir[1] map to kernel page if offset is
    0x00CF9A00  0000FFFF in gdt x86_desc.c?
    x00 00 00000 "Base" in segment descriptor format or linear address
    ^that is directory 0, table 0 offset 0
    with KERNEL_CS as x10 (16) logical address jumps to # Set up an entry for kernel CS
     and then linear address it has is 0x0. so that's first entry of page table?
     but i thought first entry of page table is 0-4MB?
     shouldn't "base" be 4 KB bc that's alignment of

    */
}
