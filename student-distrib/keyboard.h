#ifndef _KEYBOARD
#define _KEYBOARD

#include "types.h"

//prints out direct user keyboard inputs
void keyboard_handler();

//unused
int32_t terminal_open(const uint8_t* filename);

 //unused
int32_t terminal_close(int32_t fd);

//reads keyboard buffer into another buffer
int32_t terminal_read(int32_t fd, void* buf_t, int32_t nbytes);

//writes data from buffer to video memory
int32_t terminal_write(int32_t fd, const void* buf_t, int32_t nbytes);

#endif
