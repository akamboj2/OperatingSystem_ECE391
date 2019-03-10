#include "paging.h"

void paging_init() {
    int numPages = VIRTUAL_MEMORY_SPACE / PAGE_MEM_SIZE;    //4 MB/4 kB = 4000 kB/4 kb = 1000 pages (4 MB total space, 4kB per page)
    numPages++;                 //add extra page for the kernel

}