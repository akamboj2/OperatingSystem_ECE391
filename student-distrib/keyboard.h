#ifndef _KEYBOARD
#define _KEYBOARD

#include "types.h"

void keyboard_handler();

int32_t terminal_open(const uint8_t* filename);

int32_t terminal_close(int32_t fd);

int32_t terminal_read(int32_t fd, unsigned char* buf, int32_t nbytes);

int32_t terminal_write(int32_t fd, unsigned char* buf, int32_t nbytes);

#endif
