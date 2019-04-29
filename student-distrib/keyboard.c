#include "keyboard.h"
#include "x86_desc.h"
#include "idt.h"
#include "lib.h"
#include "i8259.h"
#include "sys_calls.h"
#include "scheduler.h"


#define keyboard_port 0x60
#define char_upper 0x35
#define char_lower 0x01
#define Q 0x10
#define P 0x19
#define A 0x1E
#define L 0x26
#define Z 0x2C
#define M 0x32

#define TRUE 1
#define FALSE 0

#define TAB 6

//Special keypressed
#define KB_BUF_SIZE 128
#define SHIFT_PRESSED1 0x2A
#define SHIFT_PRESSED2 0x36
#define SHIFT_RELEASED1 0xAA
#define SHIFT_RELEASED2 0xB6
#define ALT_PRESSED 0x38
#define ALT_RELEASED 0xB8
#define CAPS_PRESSED 0x3A
#define CTRL_PRESSED 0x1D
#define CTRL_RELEASED 0x9D
#define SPACE 0x39
#define BACKSPACE 0x0E
#define ENTER_PRESS 0x1C
#define _F1 0x3B
#define _F2 0x3C
#define _F3 0x3D

int shift_key = FALSE;
int alt_key = FALSE;
int cap_flag = FALSE;
int ctrl_key = FALSE;
int keyboard_buffer_index = 0;
int enter_flag = 0;

unsigned char keyboard_buffer[KB_BUF_SIZE] = {'\0'};
unsigned char keyboard_buffer1[KB_BUF_SIZE] = {'\0'};
unsigned char keyboard_buffer2[KB_BUF_SIZE] = {'\0'};
unsigned char keyboard_buffer3[KB_BUF_SIZE] = {'\0'};

int kb1_pos = 0;
int kb2_pos = 0;
int kb3_pos = 0;

/*keyboard handler
 *
 * Inputs: None
 * Outputs: Reads keyboard scancodes and outputs to video memory
 * Side Effects: Modifies keyboard_buffer
 *
 */
void keyboard_handler(){
	char char_list[53] = {'\0','1','2','3','4','5','6','7',
						'8','9','0','-','=','\0','\0','q','w',
						'e','r','t','y','u','i','o','p','[',']',
						'\n','\0','a','s','d','f','g','h','j',
						'k','l',';','\'','`','\0','\\','z',
						'x','c','v','b','n','m',',','.','/'};

  char upper_char_list[53] = {'\0','!','@','#','$','%','^','&',
            '*','(',')','_','+','\0','\0','Q','W',
            'E','R','T','Y','U','I','O','P','{','}',
            '\n','\0','A','S','D','F','G','H','J',
            'K','L',':','\"','~','\0','|','Z',
            'X','C','V','B','N','M','<','>','?'};

	unsigned char c = inb(keyboard_port);
	//send_eoi(1);
	send_eoi(1);

  if(c == SHIFT_PRESSED1 || c == SHIFT_PRESSED2){	//shift pressed
    shift_key = TRUE;
  }
  else if(c == SHIFT_RELEASED1 || c == SHIFT_RELEASED2){	//shift key released
    shift_key = FALSE;
  }
	if(c == ALT_PRESSED){	//shift pressed
		alt_key = TRUE;
	}
	else if(c == ALT_RELEASED){	//shift key released
		alt_key = FALSE;
	}
  else if(c == CAPS_PRESSED){ //caps locks pressed - keep track with flag
    cap_flag = !cap_flag;
  }
  else if(c == CTRL_PRESSED){ //control pressed
    ctrl_key = TRUE;
  }
  else if(c == CTRL_RELEASED){ //control released
    ctrl_key = FALSE;
  }

  if(ctrl_key && c == L){	//clears video memory if ctrl-l pressed
    clear();
    set_cursors(0,0);			//reset cursor
    update_cursor(0,0);
  }
	else if(alt_key == TRUE && (c == _F1 || c == _F2 || c == _F3)){
		int j;
		unsigned char * kb_ptr;
		if(curr_terminal == 1){
			kb_ptr = keyboard_buffer1;
		}
		else if(curr_terminal == 2){
			kb_ptr = keyboard_buffer2;
		}
		else if(curr_terminal == 3){
			kb_ptr = keyboard_buffer3;
		}
		if(c == _F1 && curr_terminal != 1){
			 for(j = 0; j < KB_BUF_SIZE; j++){
				 kb_ptr[j] = keyboard_buffer[j];
				 keyboard_buffer[j] = keyboard_buffer1[j];
			 }
			if(curr_terminal == 2)
				kb2_pos = keyboard_buffer_index;
			else if(curr_terminal == 3)
				kb3_pos = keyboard_buffer_index;
			keyboard_buffer_index = kb1_pos;
			switch_flag = 1;
		}
		else if(c == _F2 && curr_terminal != 2){
			for(j = 0; j < KB_BUF_SIZE; j++){
				kb_ptr[j] = keyboard_buffer[j];
				keyboard_buffer[j] = keyboard_buffer2[j];
			}
			if(curr_terminal == 1)
				kb1_pos = keyboard_buffer_index;
			else if(curr_terminal == 3)
				kb3_pos = keyboard_buffer_index;
			keyboard_buffer_index = kb2_pos;
			switch_flag = 2;
		}
		else if(c == _F3 && curr_terminal != 3){
			for(j = 0; j < KB_BUF_SIZE; j++){
				kb_ptr[j] = keyboard_buffer[j];
				keyboard_buffer[j] = keyboard_buffer3[j];
			}
			if(curr_terminal == 1)
				kb1_pos = keyboard_buffer_index;
			else if(curr_terminal == 2)
				kb2_pos = keyboard_buffer_index;
			keyboard_buffer_index = kb3_pos;
			switch_flag = 3;
		}
	}
  else if(c == SPACE){	//outputs space
    if(keyboard_buffer_index < KB_BUF_SIZE){
      char print_char = ' ';
      keyboard_buffer[keyboard_buffer_index] = print_char;
      keyboard_buffer_index++;
      putc2(print_char);
    }
  }
  else if(c == BACKSPACE && keyboard_buffer_index>0){		//adds backspace by overwriting previous char with a space
		if (get_screenx()==0 && get_screeny()==0)
			return; //fixes bug of type,ctrl-l, backspace-->fault
		set_cursors(get_screenx()-1, get_screeny());
    putc2(' ');
    set_cursors(get_screenx()-1, get_screeny());
		update_cursor(get_screenx(), get_screeny());
    keyboard_buffer_index--;

  }
	else if(c == ENTER_PRESS){		//when enter is pressed, call terminal read and write to repeat onto new line
		enter_flag = 1;
	}
	else if(c<=char_upper && c>=char_lower){		//outputs all other types of characters
    char print_char;
    if(cap_flag && ((c>=Q && c<=P) || (c>=A && c<=L) || (c>=Z && c<=M)) && !shift_key)
      print_char = upper_char_list[c-1];
    else if(!shift_key || (shift_key && cap_flag))		//accounts for caps lock and shift keys
		   print_char = char_list[c-1];
    else if(shift_key)
  		 print_char = upper_char_list[c-1];
    if(keyboard_buffer_index < KB_BUF_SIZE && print_char != '\0'){
      keyboard_buffer[keyboard_buffer_index] = print_char;
      keyboard_buffer_index++;
      putc2(print_char);
    }
	}
}

/*terminal_open
 *
 * Inputs: None
 * Outputs: return 0
 * Side Effects: None
 */
int32_t terminal_open(const uint8_t* filename){
  return 0;
}

/*terminal_close
 *
 * Inputs: None
 * Outputs: return 0
 * Side Effects: None
 */
int32_t terminal_close(int32_t fd){
  return 0;
}

/*terminal_read
 *
 * Inputs: int struct, user defined buffer, and number of bytes to copy
 * Outputs: return number of bytes copied from keyboard buffer to user
 *					defined buffer
 * Side Effects: none
 */
int32_t terminal_read(int32_t fd, void* buf_t, int32_t nbytes){
	unsigned char* buf = (unsigned char*) buf_t;
	int i;
	int index = 0;
	if(nbytes < 0)
		return -1;
	sti();
	while(!(enter_flag == 1  && curr_terminal == curr_scheduled)){

	}
	enter_flag=0;
	char print_char = '\n';
	putc2(print_char);
  while (index < (nbytes<keyboard_buffer_index ? nbytes:keyboard_buffer_index)) {
      buf[index] = keyboard_buffer[index];
      index++;
  }
	for(i = 0; i < KB_BUF_SIZE; i++){
		keyboard_buffer[i] = '\0';
	}
	keyboard_buffer_index = 0;
  return index;
}

/*terminal_write
 *
 * Inputs: int struct, user defined buffer, and number of bytes to copy
 * Outputs: return number of bytes copied from keyboard buffer to user
 *					defined buffer.
 *					Return -1 if copy unsuccessful.
 * Side Effects: clears prior state of video memory
 */
int32_t terminal_write(int32_t fd, const void* buf_t, int32_t nbytes){
	int i;
	unsigned char* buf = (unsigned char*) buf_t;
	if(buf == NULL || nbytes < 0)
		return -1;
	if(nbytes == 0)
		return 0;
	int x = print_withoutnull((int8_t*)buf, nbytes);
		cli();
		for(i = 0; i < KB_BUF_SIZE; i++){
			//This code was initially to clear garbage typed in while a prog was running. Commented
			//out because interfered by clearing the curr buffer when background process called write
			//if(curr_scheduled == curr_terminal)
				//keyboard_buffer[i] = '\0';
			if(curr_scheduled == 1)
				keyboard_buffer1[i] = '\0';
			else if(curr_scheduled == 2)
				keyboard_buffer2[i] = '\0';
			else if(curr_scheduled == 3)
				keyboard_buffer3[i] = '\0';
		}
		//if(curr_scheduled == curr_terminal)
			//keyboard_buffer_index = 0;
		if(curr_scheduled == 1)
			kb1_pos = 0;
		else if(curr_scheduled == 2)
			kb2_pos = 0;
		else if(curr_scheduled == 3)
			kb3_pos = 0;
	sti();
	return x;
}
