#ifndef _RTC
#define _RTC
#define F_1024 1024
#define F_512 512
#define F_256 256
#define F_128 128
#define F_64 64
#define F_32 32
#define F_16 16
#define F_8 8
#define F_4 4
#define F_2 2

#define R_1 0x06
#define R_2 0x07
#define R_3 0x08
#define R_4 0x09
#define R_5 0x0A
#define R_6 0x0B
#define R_7 0x0C
#define R_8 0x0D
#define R_9 0x0E
#define R_10 0x0F
/*
 * All functions will return:
 *  0   if executed correctly
 * -1   if an error occurred
 */

//only called from idt
void rtc_handler();

//initializating for rtc
int rtc_open(const uint8_t* fd);

//close rtc and clears any terminal specific variables
int rtc_close(int32_t fd);

//block until next interrupt
//NOT FOR READING RTC FREQUENCY
int rtc_read(int32_t fd, void* buf, int32_t nbytes);

//change frequency of the rtc
int rtc_write(int32_t rate, const void* buf, int32_t nbytes);

//this is solely used for rtc_read
//this is what breaks the loop in rtc_read
//when an interrupt occurs

#endif
