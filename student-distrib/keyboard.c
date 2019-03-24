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

int shift_key = FALSE;
int cap_flag = FALSE;
int ctrl_key = FALSE;
int keyboard_buffer_index = 0;

unsigned char keyboard_buffer[129];

void keyboard_handler(){
  //char print_char;
	char char_list[53] = {'\0','1','2','3','4','5','6','7',
						'8','9','0','-','=','\0','\t','q','w',
						'e','r','t','y','u','i','o','p','[',']',
						'\n','\0','a','s','d','f','g','h','j',
						'k','l',';','\'','`','\0','\\','z',
						'x','c','v','b','n','m',',','.','/'};

  char upper_char_list[53] = {'\0','!','@','#','$','%','^','&',
            '*','(',')','_','+','\0','\t','Q','W',
            'E','R','T','Y','U','I','O','P','{','}',
            '\n','\0','A','S','D','F','G','H','J',
            'K','L',':','\"','~','\0','|','Z',
            'X','C','V','B','N','M','<','>','?'};

	unsigned char c = inb(keyboard_port);

  if(c == 0x2A || c == 0x36){
    shift_key = TRUE;
  }
  else if(c == 0xAA || c == 0xB6){
    shift_key = FALSE;
  }
  else if(c == 0x3A){
    cap_flag = !cap_flag;
  }
  else if(c == 0x1D){
    ctrl_key = TRUE;
  }
  else if(c == 0x9D){
    ctrl_key = FALSE;
  }

  if(ctrl_key && c == 0x26){
    clear();
    set_cursors(0,0);
    update_cursor(0,0);
  }
  else if(c == 0x39){
    if(keyboard_buffer_index < 128){
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
  else if(c == 0x0E && keyboard_buffer_index>=0){
      set_cursors(get_screenx()-1, get_screeny());
      printf("%c", ' ');
      set_cursors(get_screenx()-1, get_screeny());
      update_cursor(get_screenx(), get_screeny());
      keyboard_buffer_index--;
  }
	else if(c<=char_upper && c>=char_lower){
    char print_char;
    if(cap_flag && ((c>=Q && c<=P) || (c>=A && c<=L) || (c>=Z && c<=M)) && !shift_key)
      print_char = upper_char_list[c-1];
    else if(!shift_key || (shift_key && cap_flag))
		   print_char = char_list[c-1];
    else if(shift_key)
  		 print_char = upper_char_list[c-1];
    if(keyboard_buffer_index < 128){
      keyboard_buffer[keyboard_buffer_index] = print_char;
      keyboard_buffer[keyboard_buffer_index + 1] = '\0';
      keyboard_buffer_index++;
      printf("%c", print_char);
    }
	}


	send_eoi(1);
}

int terminal_open(){
  shift_key = FALSE;
  cap_flag = FALSE;
  ctrl_key = FALSE;
  keyboard_buffer_index = 0;
  return 0;
}

int terminal_close(){
  shift_key = FALSE;
  cap_flag = FALSE;
  ctrl_key = FALSE;
  keyboard_buffer_index = 0;
  return 0;
}

int terminal_read(unsigned char * buf){
  int index = 0;
  while (keyboard_buffer[index] != '\0') {
      buf[index] = keyboard_buffer[index];
      index++;
  }
  return index;
}

int terminal_write(unsigned char * buf){
  return puts(buf);
  //RETURN -1?
}
