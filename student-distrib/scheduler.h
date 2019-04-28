#ifndef _SCHEDULER
#define _SCHEDULER


#define PIT_REG_MODE 0x43 //0x43         Mode/Command register (write only, a read is ignored)
#define PIT_CHAN0 0x40 //0x40         Channel 0 data port (read/write)
#define PIT_FREQ 100
#define PIT_NUM 1193182
#define PIT_STATUS 0x36
#define PIT_HIGH 0xFF00
#define PIT_LOW 0xFF
#define PIT_SHIFT 8

extern int curr_scheduled;
int total_terminal;
extern int init_flag;
extern int switch_flag;

void pit_handler();
void pit_init();

#endif
