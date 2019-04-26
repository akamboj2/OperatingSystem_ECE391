#ifndef _SCHEDULER
#define _SCHEDULER


#define PIT_REG_MODE 0x43 //0x43         Mode/Command register (write only, a read is ignored)
#define PIT_CHAN0 0x40 //0x40         Channel 0 data port (read/write)

void pit_handler();
void pit_init();

#endif
