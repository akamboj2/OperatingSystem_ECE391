#ifndef _RTC
#define _RTC

/*
 * All functions will return:
 *  0   if executed correctly
 * -1   if an error occurred
 */

//only called from idt
void rtc_handler();

//initializating for rtc
int rtc_open(int32_t fd, void* buf, int32_t nbytes);

//close rtc and clears any terminal specific variables
int rtc_close(int32_t fd, void* buf, int32_t nbytes);

//block until next interrupt
//NOT FOR READING RTC FREQUENCY
int rtc_read(int32_t fd, void* buf, int32_t nbytes);

//change frequency of the rtc
int rtc_write(int rate);

//this is solely used for rtc_read
//this is what breaks the loop in rtc_read
//when an interrupt occurs

#endif