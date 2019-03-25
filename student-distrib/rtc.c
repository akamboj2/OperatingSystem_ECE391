#include "x86_desc.h"
#include "idt.h"
#include "rtc.h"
#include "lib.h"
#include "assembly_linkage.h"
#include "i8259.h"

#define REG_A 0x8A
#define REG_C 0x0C
#define RTC_REG 0x70
#define RTC_RW 0x71
#define RATE_RTC 0x0E


volatile int nio_flag = 0;

void rtc_handler(){
  printf(" 1");
  //cli();

  outb(REG_C, RTC_REG);
  inb(RTC_RW);
//  sti();
  nio_flag = 1;
  send_eoi(8);
}

int rtc_open(){
    cli();
    nio_flag = 0;
    char prev = inb(RTC_RW);
    outb(REG_A, RTC_REG);
    prev = inb(RTC_RW);
    outb(REG_A, RTC_REG);
    outb(RATE_RTC | (prev & 0xf0), RTC_RW);
    sti();
    return 0;
}

int rtc_close(){
    return 0;
}

int rtc_read(){
    int i;
    while(!nio_flag){
      for(i = 0; i<1000; i++);
    }
    nio_flag = 0;
    return 0;
}

int rtc_write(int rate){
    rate &= 0x0F;
    if (rate >= 15 || rate <= 2)
        return -1;
    cli();
    char prev;
    outb(REG_A, RTC_REG);
    prev = inb(RTC_RW);
    outb(REG_A, RTC_REG);
    outb((rate | (prev & 0xf0)), RTC_RW);
    sti();
    return 0;
}
