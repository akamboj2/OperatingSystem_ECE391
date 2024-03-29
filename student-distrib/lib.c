/* lib.c - Some basic library functions (printf, strlen, etc.)
 * vim:ts=4 noexpandtab */

#include "lib.h"
#include "sys_calls.h"
#include "constants.h"
#include "scheduler.h"

int curr_terminal = 1;


static int screen_x[NUM_T] = {0,0,0};
static int screen_y[NUM_T] = {0,0,0};

char* video_mem = (char *)VIDEO;
char* video_buf1 = (char *)VIDEO1;
char* video_buf2 = (char *)VIDEO2;
char* video_buf3 = (char *)VIDEO3;

//these have been moved to paging.c so they can be mapped as pages and initialized there
// char video_buf1[NUM_ROWS * NUM_COLS] = {' '};
// char video_buf2[NUM_ROWS * NUM_COLS] = {' '};
// char video_buf3[NUM_ROWS * NUM_COLS] = {' '};

/* void clear(void);
 * Inputs: void
 * Return Value: none
 * Function: Clears video memory */
void clear(void) {
    int32_t i;
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        *(uint8_t *)(VIDEO + (i << 1)) = ' ';
        *(uint8_t *)(VIDEO + (i << 1) + 1) = ATTRIB;
    }
}

/* void clear1(void);
 * Inputs: void
 * Return Value: none
 * Function: Clears video memory for terminal 1 */
void clear1(void) {
    int32_t i;
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        *(uint8_t *)(video_buf1 + (i << 1)) = ' ';
        *(uint8_t *)(video_buf1 + (i << 1) + 1) = ATTRIB;
    }
}

/* void clear2(void);
 * Inputs: void
 * Return Value: none
 * Function: Clears video memory for terminal 2 */
void clear2(void) {
    int32_t i;
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        *(uint8_t *)(video_buf2 + (i << 1)) = ' ';
        *(uint8_t *)(video_buf2 + (i << 1) + 1) = ATTRIB;
    }
}

/* void clear3(void);
 * Inputs: void
 * Return Value: none
 * Function: Clears video memory for terminal 3 */
void clear3(void) {
    int32_t i;
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        *(uint8_t *)(video_buf3 + (i << 1)) = ' ';
        *(uint8_t *)(video_buf3 + (i << 1) + 1) = ATTRIB;
    }
}


/* void scroll(int x);
 * Inputs: x - whether to scroll for current terminal or background process
 * Return Value: none
 * Function: Moves video memory up one row and clears bottom row */
void scroll(int x) {
    int32_t i;
    if(x == 0){
        for (i = 0; i < (NUM_ROWS-1) * NUM_COLS; i++) {
          *(uint8_t *)(video_mem + (i << 1)) = *(uint8_t *)(video_mem + ((i+NUM_COLS) << 1));
          *(uint8_t *)(video_mem + (i << 1) + 1) = ATTRIB;
      }
      for (i = (NUM_ROWS-1) * NUM_COLS; i < (NUM_ROWS) * NUM_COLS; i++) {
          *(uint8_t *)(video_mem + (i << 1)) = ' ';
          *(uint8_t *)(video_mem + (i << 1) + 1) = ATTRIB;
      }
    }
    if(x == 1){
        for (i = 0; i < (NUM_ROWS-1) * NUM_COLS; i++) {
          *(uint8_t *)(VIDEO + (i << 1)) = *(uint8_t *)(VIDEO + ((i+NUM_COLS) << 1));
          *(uint8_t *)(VIDEO + (i << 1) + 1) = ATTRIB;
      }
      for (i = (NUM_ROWS-1) * NUM_COLS; i < (NUM_ROWS) * NUM_COLS; i++) {
          *(uint8_t *)(VIDEO + (i << 1)) = ' ';
          *(uint8_t *)(VIDEO + (i << 1) + 1) = ATTRIB;
      }
    }
}

/* void switch_terminal(int from, int to);
 * Inputs: from - current terminal number, to - terminal to switch to
 * Return Value: none
 * Function: Switches video buffers from current terminal to next terminal */
void switch_terminal(int from, int to) {
  cli();
  int32_t i;
  char * f;
  char * t;
  if(from == T1){
    f = video_buf1;
  }else if(from == T2){
    f = video_buf2;
  }else if(from == T3){
    f = video_buf3;
  }

  if(to == T1){
    t = video_buf1;
  }else if(to == T2){
    t = video_buf2;
  }else if(to == T3){
    t = video_buf3;
  }

  curr_terminal = to;

  update_cursor(screen_x[curr_terminal-1], screen_y[curr_terminal-1]);
  for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
      *(uint8_t *)(f + (i << 1)) = *(uint8_t *)(VIDEO + (i << 1));
      *(uint8_t *)(VIDEO + (i << 1)) = *(uint8_t *)(t + (i << 1));
  }
//  sti();//NO PIT i
}


/* int get_screenx();
 * Inputs: none
 * Return Value: x-position of where to write in video memory
 * Function: Clears video memory */
int get_screenx(){
  return screen_x[curr_terminal-1];
}

/* int get_screeny();
 * Inputs: none
 * Return Value: y-position of where to write in video memory
 * Function: Clears video memory */
int get_screeny(){
  return screen_y[curr_terminal-1];
}

/* void set_cursors(int pos_x, int pos_y)
 * Inputs: x and y postitions to set cursor to
 * Return Value: none
 * Function: changes cursor positon */
void set_cursors(int pos_x, int pos_y){
  //error checking
  if (pos_x<0){
    //without this pos_x would go neagtive with backspace and if you backspaced more than one line
    //and are on the top line then you would crash when updating cursor
    pos_y--;
    pos_x=NUM_COLS-1;
  }
  if(pos_x<0 || pos_x >= NUM_COLS || pos_y<0 ||pos_y>=NUM_ROWS) return;
  
  screen_x[curr_terminal-1] = pos_x;
  screen_y[curr_terminal-1] = pos_y;

}


//credit to https://wiki.osdev.org/Text_Mode_Cursor
/* void update_cursor(int x, int y)
 * Inputs: x and y coordinates to set physical cursor to
 * Return Value: none
 * Function: Modifies position of text to video memory cursor */
void update_cursor(int x, int y){
	uint16_t pos = (y * NUM_COLS) + x;

	outb(0x0F, 0x3D4);
	outb((uint8_t) (pos & 0xFF), 0x3D5);
	outb(0x0E, 0x3D4);
	outb((uint8_t) ((pos >> 8) & 0xFF), 0x3D5);
}

/* int32_t puts(int8_t* s);
 *   Inputs: int_8* s = pointer to a string of characters
 *   Return Value: Number of bytes written
 *    Function: Output a string to the console */
int32_t print_withoutnull(int8_t *buffer, int nbytes){
  int8_t temp_buf[nbytes+1];
  int i;
  for(i = 0; i < nbytes; i++){
    temp_buf[i] = buffer[i];
    //buffer[i] = '\0';
  }
  temp_buf[nbytes] = '\0';
  return putfile(temp_buf, nbytes+1);
}

/* int32_t puts(int8_t* s);
 *   Inputs: int_8* s = pointer to a string of characters
 *   Return Value: Number of bytes written
 *    Function: Output a string to the console */
int32_t putfile(int8_t* s, uint32_t length) {
    register int32_t index = 0;
    while (index <  length) {
        if(s[index] != '\0'){
            if(curr_scheduled == curr_terminal)
              putc2(s[index]);
            else putc(s[index]);
        }
        index++;
    }
    return index;
}


/* Standard printf().
 * Only supports the following format strings:
 * %%  - print a literal '%' character
 * %x  - print a number in hexadecimal
 * %u  - print a number as an unsigned integer
 * %d  - print a number as a signed integer
 * %c  - print a character
 * %s  - print a string
 * %#x - print a number in 32-bit aligned hexadecimal, i.e.
 *       print 8 hexadecimal digits, zero-padded on the left.
 *       For example, the hex number "E" would be printed as
 *       "0000000E".
 *       Note: This is slightly different than the libc specification
 *       for the "#" modifier (this implementation doesn't add a "0x" at
 *       the beginning), but I think it's more flexible this way.
 *       Also note: %x is the only conversion specifier that can use
 *       the "#" modifier to alter output. */
int32_t printf(int8_t *format, ...) {

    /* Pointer to the format string */
    int8_t* buf = format;

    /* Stack pointer for the other parameters */
    int32_t* esp = (void *)&format;
    esp++;

    while (*buf != '\0') {
        switch (*buf) {
            case '%':
                {
                    int32_t alternate = 0;
                    buf++;

format_char_switch:
                    /* Conversion specifiers */
                    switch (*buf) {
                        /* Print a literal '%' character */
                        case '%':
                            putc('%');
                            break;

                        /* Use alternate formatting */
                        case '#':
                            alternate = 1;
                            buf++;
                            /* Yes, I know gotos are bad.  This is the
                             * most elegant and general way to do this,
                             * IMHO. */
                            goto format_char_switch;

                        /* Print a number in hexadecimal form */
                        case 'x':
                            {
                                int8_t conv_buf[64];
                                if (alternate == 0) {
                                    itoa(*((uint32_t *)esp), conv_buf, 16);
                                    puts(conv_buf);
                                } else {
                                    int32_t starting_index;
                                    int32_t i;
                                    itoa(*((uint32_t *)esp), &conv_buf[8], 16);
                                    i = starting_index = strlen(&conv_buf[8]);
                                    while(i < 8) {
                                        conv_buf[i] = '0';
                                        i++;
                                    }
                                    puts(&conv_buf[starting_index]);
                                }
                                esp++;
                            }
                            break;

                        /* Print a number in unsigned int form */
                        case 'u':
                            {
                                int8_t conv_buf[36];
                                itoa(*((uint32_t *)esp), conv_buf, 10);
                                puts(conv_buf);
                                esp++;
                            }
                            break;

                        /* Print a number in signed int form */
                        case 'd':
                            {
                                int8_t conv_buf[36];
                                int32_t value = *((int32_t *)esp);
                                if(value < 0) {
                                    conv_buf[0] = '-';
                                    itoa(-value, &conv_buf[1], 10);
                                } else {
                                    itoa(value, conv_buf, 10);
                                }
                                puts(conv_buf);
                                esp++;
                            }
                            break;

                        /* Print a single character */
                        case 'c':
                            putc((uint8_t) *((int32_t *)esp));
                            esp++;
                            break;

                        /* Print a NULL-terminated string */
                        case 's':
                            puts(*((int8_t **)esp));
                            esp++;
                            break;

                        default:
                            break;
                    }

                }
                break;

            default:
                putc(*buf);
                break;
        }
        buf++;
    }
    return (buf - format);
}

/* Special printf() that prints to screen no matter what process running.
 * Only supports the following format strings:
 * %%  - print a literal '%' character
 * %x  - print a number in hexadecimal
 * %u  - print a number as an unsigned integer
 * %d  - print a number as a signed integer
 * %c  - print a character
 * %s  - print a string
 * %#x - print a number in 32-bit aligned hexadecimal, i.e.
 *       print 8 hexadecimal digits, zero-padded on the left.
 *       For example, the hex number "E" would be printed as
 *       "0000000E".
 *       Note: This is slightly different than the libc specification
 *       for the "#" modifier (this implementation doesn't add a "0x" at
 *       the beginning), but I think it's more flexible this way.
 *       Also note: %x is the only conversion specifier that can use
 *       the "#" modifier to alter output. */
int32_t printf2(int8_t *format, ...) {

    /* Pointer to the format string */
    int8_t* buf = format;

    /* Stack pointer for the other parameters */
    int32_t* esp = (void *)&format;
    esp++;

    while (*buf != '\0') {
        switch (*buf) {
            case '%':
                {
                    int32_t alternate = 0;
                    buf++;

format_char_switch:
                    /* Conversion specifiers */
                    switch (*buf) {
                        /* Print a literal '%' character */
                        case '%':
                            putc2('%');
                            break;

                        /* Use alternate formatting */
                        case '#':
                            alternate = 1;
                            buf++;
                            /* Yes, I know gotos are bad.  This is the
                             * most elegant and general way to do this,
                             * IMHO. */
                            goto format_char_switch;

                        /* Print a number in hexadecimal form */
                        case 'x':
                            {
                                int8_t conv_buf[64];
                                if (alternate == 0) {
                                    itoa(*((uint32_t *)esp), conv_buf, 16);
                                    puts2(conv_buf);
                                } else {
                                    int32_t starting_index;
                                    int32_t i;
                                    itoa(*((uint32_t *)esp), &conv_buf[8], 16);
                                    i = starting_index = strlen(&conv_buf[8]);
                                    while(i < 8) {
                                        conv_buf[i] = '0';
                                        i++;
                                    }
                                    puts2(&conv_buf[starting_index]);
                                }
                                esp++;
                            }
                            break;

                        /* Print a number in unsigned int form */
                        case 'u':
                            {
                                int8_t conv_buf[36];
                                itoa(*((uint32_t *)esp), conv_buf, 10);
                                puts2(conv_buf);
                                esp++;
                            }
                            break;

                        /* Print a number in signed int form */
                        case 'd':
                            {
                                int8_t conv_buf[36];
                                int32_t value = *((int32_t *)esp);
                                if(value < 0) {
                                    conv_buf[0] = '-';
                                    itoa(-value, &conv_buf[1], 10);
                                } else {
                                    itoa(value, conv_buf, 10);
                                }
                                puts2(conv_buf);
                                esp++;
                            }
                            break;

                        /* Print a single character */
                        case 'c':
                            putc2((uint8_t) *((int32_t *)esp));
                            esp++;
                            break;

                        /* Print a NULL-terminated string */
                        case 's':
                            puts2(*((int8_t **)esp));
                            esp++;
                            break;

                        default:
                            break;
                    }

                }
                break;

            default:
                putc2(*buf);
                break;
        }
        buf++;
    }
    return (buf - format);
}

/* int32_t puts(int8_t* s);
 *   Inputs: int_8* s = pointer to a string of characters
 *   Return Value: Number of bytes written
 *    Function: Output a string to the video buffer of process running */
int32_t puts(int8_t* s) {
    register int32_t index = 0;
    while (s[index] != '\0') {
        putc(s[index]);
        index++;
    }
    return index;
}

/* int32_t puts2(int8_t* s);
 *   Inputs: int_8* s = pointer to a string of characters
 *   Return Value: Number of bytes written
 *    Function: Output a string to the console no matter what process running*/
int32_t puts2(int8_t* s) {
    register int32_t index = 0;
    while (s[index] != '\0') {
        putc2(s[index]);
        index++;
    }
    return index;
}

/* void putc(uint8_t c);
 * Inputs: uint_8* c = character to print
 * Return Value: void
 *  Function: Output a character to the running process */
void putc(uint8_t c) {
    if(c == '\n' || c == '\r') {
        screen_y[curr_scheduled-1]++;
        screen_x[curr_scheduled-1] = 0;
    } else {
        *(uint8_t *)(video_mem + ((NUM_COLS * screen_y[curr_scheduled-1] + screen_x[curr_scheduled-1]) << 1)) = c;
        *(uint8_t *)(video_mem + ((NUM_COLS * screen_y[curr_scheduled-1] + screen_x[curr_scheduled-1]) << 1) + 1) = ATTRIB;
        screen_x[curr_scheduled-1]++;
        screen_y[curr_scheduled-1] = (screen_y[curr_scheduled-1] + (screen_x[curr_scheduled-1] / NUM_COLS));
        screen_x[curr_scheduled-1] %= NUM_COLS;
    }
    if(screen_y[curr_scheduled-1] >= NUM_ROWS){
      scroll(0);
      screen_y[curr_scheduled-1]--;
      screen_x[curr_scheduled-1] = 0;
    }
    if(curr_scheduled == curr_terminal)
      update_cursor(screen_x[curr_scheduled-1], screen_y[curr_scheduled-1]);
}

/* void putc2(uint8_t c);
 * Inputs: uint_8* c = character to print
 * Return Value: void
 *  Function: Output a character to the console */
void putc2(uint8_t c) {
    if(c == '\n' || c == '\r') {
        screen_y[curr_terminal-1]++;
        screen_x[curr_terminal-1] = 0;
    } else {
        *(uint8_t *)(VIDEO + ((NUM_COLS * screen_y[curr_terminal-1] + screen_x[curr_terminal-1]) << 1)) = c;
        *(uint8_t *)(VIDEO + ((NUM_COLS * screen_y[curr_terminal-1] + screen_x[curr_terminal-1]) << 1) + 1) = ATTRIB;
        screen_x[curr_terminal-1]++;
        screen_y[curr_terminal-1] = (screen_y[curr_terminal-1] + (screen_x[curr_terminal-1] / NUM_COLS));
        screen_x[curr_terminal-1] %= NUM_COLS;
    }
    if(screen_y[curr_terminal-1] >= NUM_ROWS){
      scroll(1);
      screen_y[curr_terminal-1]--;
      screen_x[curr_terminal-1] = 0;
    }
    update_cursor(screen_x[curr_terminal-1], screen_y[curr_terminal-1]);
}

/* int8_t* itoa(uint32_t value, int8_t* buf, int32_t radix);
 * Inputs: uint32_t value = number to convert
 *            int8_t* buf = allocated buffer to place string in
 *          int32_t radix = base system. hex, oct, dec, etc.
 * Return Value: number of bytes written
 * Function: Convert a number to its ASCII representation, with base "radix" */
int8_t* itoa(uint32_t value, int8_t* buf, int32_t radix) {
    static int8_t lookup[] = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    int8_t *newbuf = buf;
    int32_t i;
    uint32_t newval = value;

    /* Special case for zero */
    if (value == 0) {
        buf[0] = '0';
        buf[1] = '\0';
        return buf;
    }

    /* Go through the number one place value at a time, and add the
     * correct digit to "newbuf".  We actually add characters to the
     * ASCII string from lowest place value to highest, which is the
     * opposite of how the number should be printed.  We'll reverse the
     * characters later. */
    while (newval > 0) {
        i = newval % radix;
        *newbuf = lookup[i];
        newbuf++;
        newval /= radix;
    }

    /* Add a terminating NULL */
    *newbuf = '\0';

    /* Reverse the string and return */
    return strrev(buf);
}

/* int8_t* strrev(int8_t* s);
 * Inputs: int8_t* s = string to reverse
 * Return Value: reversed string
 * Function: reverses a string s */
int8_t* strrev(int8_t* s) {
    register int8_t tmp;
    register int32_t beg = 0;
    register int32_t end = strlen(s) - 1;

    while (beg < end) {
        tmp = s[end];
        s[end] = s[beg];
        s[beg] = tmp;
        beg++;
        end--;
    }
    return s;
}

/* uint32_t strlen(const int8_t* s);
 * Inputs: const int8_t* s = string to take length of
 * Return Value: length of string s
 * Function: return length of string s */
uint32_t strlen(const int8_t* s) {
    register uint32_t len = 0;
    while (s[len] != '\0')
        len++;
    return len;
}

/* void* memset(void* s, int32_t c, uint32_t n);
 * Inputs:    void* s = pointer to memory
 *          int32_t c = value to set memory to
 *         uint32_t n = number of bytes to set
 * Return Value: new string
 * Function: set n consecutive bytes of pointer s to value c */
void* memset(void* s, int32_t c, uint32_t n) {
    c &= 0xFF;
    asm volatile ("                 \n\
            .memset_top:            \n\
            testl   %%ecx, %%ecx    \n\
            jz      .memset_done    \n\
            testl   $0x3, %%edi     \n\
            jz      .memset_aligned \n\
            movb    %%al, (%%edi)   \n\
            addl    $1, %%edi       \n\
            subl    $1, %%ecx       \n\
            jmp     .memset_top     \n\
            .memset_aligned:        \n\
            movw    %%ds, %%dx      \n\
            movw    %%dx, %%es      \n\
            movl    %%ecx, %%edx    \n\
            shrl    $2, %%ecx       \n\
            andl    $0x3, %%edx     \n\
            cld                     \n\
            rep     stosl           \n\
            .memset_bottom:         \n\
            testl   %%edx, %%edx    \n\
            jz      .memset_done    \n\
            movb    %%al, (%%edi)   \n\
            addl    $1, %%edi       \n\
            subl    $1, %%edx       \n\
            jmp     .memset_bottom  \n\
            .memset_done:           \n\
            "
            :
            : "a"(c << 24 | c << 16 | c << 8 | c), "D"(s), "c"(n)
            : "edx", "memory", "cc"
    );
    return s;
}

/* void* memset_word(void* s, int32_t c, uint32_t n);
 * Description: Optimized memset_word
 * Inputs:    void* s = pointer to memory
 *          int32_t c = value to set memory to
 *         uint32_t n = number of bytes to set
 * Return Value: new string
 * Function: set lower 16 bits of n consecutive memory locations of pointer s to value c */
void* memset_word(void* s, int32_t c, uint32_t n) {
    asm volatile ("                 \n\
            movw    %%ds, %%dx      \n\
            movw    %%dx, %%es      \n\
            cld                     \n\
            rep     stosw           \n\
            "
            :
            : "a"(c), "D"(s), "c"(n)
            : "edx", "memory", "cc"
    );
    return s;
}

/* void* memset_dword(void* s, int32_t c, uint32_t n);
 * Inputs:    void* s = pointer to memory
 *          int32_t c = value to set memory to
 *         uint32_t n = number of bytes to set
 * Return Value: new string
 * Function: set n consecutive memory locations of pointer s to value c */
void* memset_dword(void* s, int32_t c, uint32_t n) {
    asm volatile ("                 \n\
            movw    %%ds, %%dx      \n\
            movw    %%dx, %%es      \n\
            cld                     \n\
            rep     stosl           \n\
            "
            :
            : "a"(c), "D"(s), "c"(n)
            : "edx", "memory", "cc"
    );
    return s;
}

/* void* memcpy(void* dest, const void* src, uint32_t n);
 * Inputs:      void* dest = destination of copy
 *         const void* src = source of copy
 *              uint32_t n = number of byets to copy
 * Return Value: pointer to dest
 * Function: copy n bytes of src to dest */
void* memcpy(void* dest, const void* src, uint32_t n) {
    asm volatile ("                 \n\
            .memcpy_top:            \n\
            testl   %%ecx, %%ecx    \n\
            jz      .memcpy_done    \n\
            testl   $0x3, %%edi     \n\
            jz      .memcpy_aligned \n\
            movb    (%%esi), %%al   \n\
            movb    %%al, (%%edi)   \n\
            addl    $1, %%edi       \n\
            addl    $1, %%esi       \n\
            subl    $1, %%ecx       \n\
            jmp     .memcpy_top     \n\
            .memcpy_aligned:        \n\
            movw    %%ds, %%dx      \n\
            movw    %%dx, %%es      \n\
            movl    %%ecx, %%edx    \n\
            shrl    $2, %%ecx       \n\
            andl    $0x3, %%edx     \n\
            cld                     \n\
            rep     movsl           \n\
            .memcpy_bottom:         \n\
            testl   %%edx, %%edx    \n\
            jz      .memcpy_done    \n\
            movb    (%%esi), %%al   \n\
            movb    %%al, (%%edi)   \n\
            addl    $1, %%edi       \n\
            addl    $1, %%esi       \n\
            subl    $1, %%edx       \n\
            jmp     .memcpy_bottom  \n\
            .memcpy_done:           \n\
            "
            :
            : "S"(src), "D"(dest), "c"(n)
            : "eax", "edx", "memory", "cc"
    );
    return dest;
}

/* void* memmove(void* dest, const void* src, uint32_t n);
 * Description: Optimized memmove (used for overlapping memory areas)
 * Inputs:      void* dest = destination of move
 *         const void* src = source of move
 *              uint32_t n = number of byets to move
 * Return Value: pointer to dest
 * Function: move n bytes of src to dest */
void* memmove(void* dest, const void* src, uint32_t n) {
    asm volatile ("                             \n\
            movw    %%ds, %%dx                  \n\
            movw    %%dx, %%es                  \n\
            cld                                 \n\
            cmp     %%edi, %%esi                \n\
            jae     .memmove_go                 \n\
            leal    -1(%%esi, %%ecx), %%esi     \n\
            leal    -1(%%edi, %%ecx), %%edi     \n\
            std                                 \n\
            .memmove_go:                        \n\
            rep     movsb                       \n\
            "
            :
            : "D"(dest), "S"(src), "c"(n)
            : "edx", "memory", "cc"
    );
    return dest;
}

/* int32_t strncmp(const int8_t* s1, const int8_t* s2, uint32_t n)
 * Inputs: const int8_t* s1 = first string to compare
 *         const int8_t* s2 = second string to compare
 *               uint32_t n = number of bytes to compare
 * Return Value: A zero value indicates that the characters compared
 *               in both strings form the same string.
 *               A value greater than zero indicates that the first
 *               character that does not match has a greater value
 *               in str1 than in str2; And a value less than zero
 *               indicates the opposite.
 * Function: compares string 1 and string 2 for equality */
int32_t strncmp(const int8_t* s1, const int8_t* s2, uint32_t n) {
    int32_t i;
    for (i = 0; i < n; i++) {
        if ((s1[i] != s2[i]) || (s1[i] == '\0') /* || s2[i] == '\0' */) {

            /* The s2[i] == '\0' is unnecessary because of the short-circuit
             * semantics of 'if' expressions in C.  If the first expression
             * (s1[i] != s2[i]) evaluates to false, that is, if s1[i] ==
             * s2[i], then we only need to test either s1[i] or s2[i] for
             * '\0', since we know they are equal. */
            return s1[i] - s2[i];
        }
    }
    return 0;
}

/* int8_t* strcpy(int8_t* dest, const int8_t* src)
 * Inputs:      int8_t* dest = destination string of copy
 *         const int8_t* src = source string of copy
 * Return Value: pointer to dest
 * Function: copy the source string into the destination string */
int8_t* strcpy(int8_t* dest, const int8_t* src) {
    int32_t i = 0;
    while (src[i] != '\0') {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
    return dest;
}

/* int8_t* strcpy(int8_t* dest, const int8_t* src, uint32_t n)
 * Inputs:      int8_t* dest = destination string of copy
 *         const int8_t* src = source string of copy
 *                uint32_t n = number of bytes to copy
 * Return Value: pointer to dest
 * Function: copy n bytes of the source string into the destination string */
int8_t* strncpy(int8_t* dest, const int8_t* src, uint32_t n) {
    int32_t i = 0;
    while (src[i] != '\0' && i < n) {
        dest[i] = src[i];
        i++;
    }
    while (i < n) {
        dest[i] = '\0';
        i++;
    }
    return dest;
}

/* void test_interrupts(void)
 * Inputs: void
 * Return Value: void
 * Function: increments video memory. To be used to test rtc */
void test_interrupts(void) {
    int32_t i;
    for (i = 0; i < NUM_ROWS * NUM_COLS; i++) {
        video_mem[i << 1]++;
    }
}
