#include "x86_desc.h"
#include "idt.h"
#include "rtc.h"
#include "lib.h"
#include "assembly_linkage.h"
#include "i8259.h"
#include "scheduler.h"

#define REG_A 0x8A
#define REG_C 0x0C
#define RTC_REG 0x70
#define RTC_RW 0x71
#define RATE_RTC 0x0E


volatile int rtc_active = 0;
volatile int rtc_rates[3]={1,1,1}; //min to 1
volatile int rtc_counter[3]={1,1,1};
/* rtc_handler() : is called from idt and handles what needs
 *                 to be done whenever an rtc interrupt occurs
 * INPUTS: None
 * OUPUTS: None
 * SIDE EFFECTS: sends eoi signal to PIC and prints for test purposes
 */
void rtc_handler(){
  //printf(" 1");
  //cli(); //no need for this, it is an interrupt gate !
  //rtc_counter-=1;
  rtc_counter[0]--;
  if (rtc_counter[0]<0) rtc_counter[0]=0;
  rtc_counter[1]--;
  if (rtc_counter[1]<0) rtc_counter[1]=0;
  rtc_counter[2]--;
  if (rtc_counter[2]<0) rtc_counter[2]=0;
  outb(REG_C, RTC_REG);
  inb(RTC_RW);
//  sti();
  //nio_flag = 1;
  send_eoi(8);
}

/* rtc_open() : is called in kernel.c, initializes the rtc driver
 * INPUTS: fd - unused
 * OUPUTS: 0
 * SIDE EFFECTS: sets nio_flag to 0, communicates with rtc ports
 */
int rtc_open(const uint8_t* fd){
    cli();
    if(rtc_active) return; //this makes the rtc only called once
    rtc_active = 1;
    char prev = inb(RTC_RW);
    outb(REG_A, RTC_REG);
    prev = inb(RTC_RW);
    outb(REG_A, RTC_REG);
    outb(RATE_RTC | (prev & 0xf0), RTC_RW);

    //this was taken from the previous rtc_write function
    uint32_t rate=R_1;
    outb(REG_A, RTC_REG);
    prev = inb(RTC_RW);
    outb(REG_A, RTC_REG);
    outb((rate | (prev & 0xf0)), RTC_RW); //writes to the rtc?
    sti();
    return 0;
}

/* rtc_close() : does nothing at the moment
 * INPUTS: fd - unused
 * OUTPUTS: 0
 * SIDE EFFECTS: None
 */
int rtc_close(int32_t fd){
    return 0;
}

/* rtc_read() : blocks until next interrupt occurs. when this
 *              happens we return 0.
 * INPUTS: fd, buf, nbytes - all are unused
 * OUPUTS: return 0
 * SIDE EFFECTS: this function does not return until another
 *               interrupt is detected. after this occurs, it
 *               exits the loop and resets the flag.
 */
int rtc_read(int32_t fd, void* buf, int32_t nbytes){
    //int i;
    rtc_counter[curr_scheduled-1]=F_1024/rtc_rates[curr_scheduled-1];
    while(rtc_counter[curr_scheduled-1]>0){   //flag used to hold loop
      //wait for rtc_counter (in rtc_handler) to decrement to zero
    }
    //nio_flag = 0;
    return 0;
}

/* rtc_write() : changes the RTC frequency
 * INPUTS: rate - frequency to change to
 * OUPUTS: return 0
 * SIDE EFFECTS: chnages the frequency of the RTC
 */
int rtc_write(int32_t fd, const void* buf, int32_t nbytes){
    if(nbytes != 4 || buf == NULL){
      return -1;
    }
    int32_t freq =*(int32_t*)(buf);
    if (freq >F_1024 || freq<F_2) return -1;

    rtc_rates[curr_scheduled-1]=freq;

    // uint32_t rate;
    // //ensures that the RTC frequency can only be switched to a power of two
    // if(freq == F_1024)
    //   rate = R_1;
    // else if(freq == F_512)
    //   rate = R_2;
    // else if(freq == F_256)
    //   rate = R_3;
    // else if(freq == F_128)
    //   rate = R_4;
    // else if(freq == F_64)
    //   rate = R_5;
    // else if(freq == F_32)
    //   rate = R_6;
    // else if(freq == F_16)
    //   rate = R_7;
    // else if(freq == F_8)
    //   rate = R_8;
    // else if(freq == F_4)
    //   rate = R_9;
    // else if(freq == F_2)
    //   rate = R_10;
    // else return -1;
    //
    // if (rate >= 15 || rate <= 2)  //ensures the rate is within the bounds
    //     return -1;

    //rtc_rates[curr_scheduled]=freq;
    // cli();
    // char prev;
    // outb(REG_A, RTC_REG);
    // prev = inb(RTC_RW);
    // outb(REG_A, RTC_REG);
    // outb((rate | (prev & 0xf0)), RTC_RW); //writes to the rtc?
    // sti();
    return 0;
}
