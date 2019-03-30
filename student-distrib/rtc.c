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

/* rtc_handler() : is called from idt and handles what needs
 *                 to be done whenever an rtc interrupt occurs
 * INPUTS: None
 * OUPUTS: None
 * SIDE EFFECTS: sends eoi signal to PIC and prints for test purposes
 */
void rtc_handler(){
  //printf(" 1");
  cli();

  outb(REG_C, RTC_REG);
  inb(RTC_RW);
  sti();
  nio_flag = 1;
  send_eoi(8);
}

/* rtc_open() : is called in kernel.c, initializes the rtc driver
 * INPUTS: None
 * OUPUTS: 0
 * SIDE EFFECTS: sets nio_flag to 0, communicates with rtc ports
 */
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

/* rtc_close() : does nothing at the moment
 * INPUTS: None
 * OUTPUTS: 0
 * SIDE EFFECTS: None
 */
int rtc_close(){
    return 0;
}

/* rtc_read() : blocks until next interrupt occurs. when this
 *              happens we return 0.
 * INPUTS: None
 * OUPUTS: return 0
 * SIDE EFFECTS: this function does not return until another
 *               interrupt is detected. after this occurs, it
 *               exits the loop and resets the flag.
 */
int rtc_read(){
    //int i;
    while(!nio_flag){   //flag used to hold loop

    }
    nio_flag = 0;
    return 0;
}

/* rtc_write() : changes the RTC frequency
 * INPUTS: rate - frequency to change to
 * OUPUTS: return 0
 * SIDE EFFECTS: chnages the frequency of the RTC
 */
int rtc_write(int rate){
    rate &= 0x0F;
    if (rate >= 15 || rate <= 2)  //ensures the rate is within the bounds
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
