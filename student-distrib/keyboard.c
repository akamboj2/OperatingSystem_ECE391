#include "keyboard.h"
#include "x86_desc.h"
#include "idt.h"
#include "lib.h"
#include "i8259.h"


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
#define CAPS_PRESSED 0x3A
#define CTRL_PRESSED 0x1D
#define CTRL_RELEASED 0x9D
#define SPACE 0x39
#define BACKSPACE 0x0E
#define ENTER_PRESS 0x1C

int shift_key = FALSE;
int cap_flag = FALSE;
int ctrl_key = FALSE;
int keyboard_buffer_index = 0;

unsigned char keyboard_buffer[KB_BUF_SIZE];
unsigned char rw_buffer[KB_BUF_SIZE];

/*keyboard handler
 *
 * Inputs: None
 * Outputs: Reads keyboard scancodes and outputs to video memory
 * Side Effects: Modifies keyboard_buffer
 *               Miight clear video memory
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

  if(c == SHIFT_PRESSED1 || c == SHIFT_PRESSED2){	//shift pressed
    shift_key = TRUE;
  }
  else if(c == SHIFT_RELEASED1 || c == SHIFT_RELEASED2){	//shift key released
    shift_key = FALSE;
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

  else if(c == SPACE){	//outputs space
    if(keyboard_buffer_index < KB_BUF_SIZE){
      char print_char = ' ';
      keyboard_buffer[keyboard_buffer_index] = print_char;
      keyboard_buffer[keyboard_buffer_index + 1] = '\0';
      keyboard_buffer_index++;
      printf("%c", print_char);
    }
  }
  /*else if(c == 0x0F){
    prev_tab = TRUE;
    if(keyboard_buffer_index < 122){
      char print_char = ' ';
      keyboard_buffer[keyboard_buffer_index] = print_char;
      keyboard_buffer[keyboard_buffer_index + TAB] = '\0';
      keyboard_buffer_index+=TAB;
      int i;
      for(i = 0; i<TAB; i++)
        printf("%c", print_char);
    }
  }*/
  else if(c == BACKSPACE && keyboard_buffer_index>=0){		//adds backspace by overwriting previous char with a space
      set_cursors(get_screenx()-1, get_screeny());
      printf("%c", ' ');
      set_cursors(get_screenx()-1, get_screeny());
      update_cursor(get_screenx(), get_screeny());
      keyboard_buffer_index--;
  }
	else if(c == ENTER_PRESS){		//when enter is pressed, call terminal read and write to repeat onto new line
		//keyboard_buffer[keyboard_buffer_index] = '\n';
		//keyboard_buffer[keyboard_buffer_index + 1] = '\0';
		int n = terminal_read(0,rw_buffer,KB_BUF_SIZE);
		printf("%c", '\n');
		terminal_write(0,rw_buffer,n);
		int i; 																																						//CLEARS BUFFER FOR TESTING PURPOSES ONLY
		for(i = 0; i < KB_BUF_SIZE; i++){
			keyboard_buffer[i] = '\0';
			rw_buffer[i] = '\0';
		}
		keyboard_buffer_index = 0;
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
      keyboard_buffer[keyboard_buffer_index + 1] = '\0';
      keyboard_buffer_index++;
      printf("%c", print_char);
    }
	}

	send_eoi(1);
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
int32_t terminal_read(int32_t fd, unsigned char* buf, int32_t nbytes){
	int index = 0;
	if(nbytes < 0 || nbytes > KB_BUF_SIZE)
		return -1;
  while (index < nbytes) {
      buf[index] = keyboard_buffer[index];
      index++;
  }
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
int32_t terminal_write(int32_t fd, unsigned char* buf, int32_t nbytes){
	if(buf == NULL || nbytes < 0)
		return -1;
	if(nbytes == 0)
		return 0;
	//if(nbytes < 0 || nbytes >= sizeof(buf)) //less than or equal to account for length the null char adds
		//return -1;
	return print_withoutnull((int8_t*)buf, nbytes);
}