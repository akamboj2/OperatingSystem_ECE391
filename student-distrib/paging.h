#ifndef PAGING_H
#define PAGING_H

#include "constants.h"
#include "types.h"

struct pageTable {
    int address[20];
    int avail[3];
    int globalPage;
    int pageSize;
    int reserved;
    int accessed;
    int cache;
    int writeThrough;
    int userSupervisor;
    int readWrite;
    int present;
};

struct pageDirectory {
    int address[20];
    int avail[3];
    int globalPage;
    int pageTableAttributeIdx;
    int dirty;
    int accessed;
    int cache;
    int writeThrough;
    int userSupervisor;
    int readWrite;
    int present;
};

extern void paging_init();

#endif /* PAGING_H*/
