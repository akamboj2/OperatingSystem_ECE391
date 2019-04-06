#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "keyboard.h"
#include "rtc.h"
#include "filesys.h"
#include "assembly_linkage.h"
#include "sys_calls.h"

#define PASS 1
#define FAIL 0

#define CONTENT_BUFFER 8000

#define MINSPEED 14
#define MAXSPEED 3
#define RTC_INTERVAL 100

/* format these macros as you see fit */
#define TEST_HEADER 	\
	printf("[TEST %s] Running %s at %s:%d\n", __FUNCTION__, __FUNCTION__, __FILE__, __LINE__)
#define TEST_OUTPUT(name, result)	\
	printf("[TEST %s] Result = %s\n", name, (result) ? "PASS" : "FAIL");

static inline void assertion_failure(){
	/* Use exception #15 for assertions, otherwise
	   reserved by Intel */
	asm volatile("int $15");
}


/* Checkpoint 1 tests */

/* IDT Test - Example
 *
 * Asserts that first 10 IDT entries are not NULL. Also used to test div by zero exception handler
 * Inputs: None
 * Outputs: PASS/FAIL
 * Side Effects: None
 * Coverage: Load IDT, IDT definition
 * Files: x86_desc.h/S
 */
int idt_test(){
	TEST_HEADER;

	int result = PASS;
																								//idt populated test
	/*int i;
	for (i = 0; i < 10; ++i){
	 	if ((idt[i].offset_15_00 == NULL) &&
	 		(idt[i].offset_31_16 == NULL)){
	 		assertion_failure();
	 		result = FAIL;
 		}
	}*/

																								//divide by zero test
	//int a = 0;
	//a = 3/a;

	return result;
}

/*Paging Test
 *
 * Asserts that null ptr results in a page fault
 * Inputs: None
 * Outputs: page fault
 * Side Effects: kernel crashes
 * Coverage: requires idt to be implented in order to result in page fault
 */
int paging_test(){
	/*Values contained in your paging structures
		Dereferencing different address ranges with paging turned on
	*/
	int result = FAIL;
	printf("Testing paging: result at %x\n",&result);
	if (&result&&0x400000){
		//if it's linear addr has bit 22 =1 that means it is in page directory 1
		//which is where kernel code is supposed to be
		result =PASS;
	}

	int* test=NULL;
	printf("%d\n",*test);
	printf("hello\n");
	return result;
}


// add more tests here

/* Checkpoint 2 tests */

/*Terminal Write Test
 *
 * Inputs: None
 * Outputs:
 * Side Effects:
 */
void terminalwrite_test(){
	uint8_t* nullbuf;
	terminal_write(0, nullbuf, 0);
	terminal_write(0, (uint8_t*)"_abcde\n", 0);
	terminal_write(0, (uint8_t*)"_abcde\n", 4);
	terminal_write(0, (uint8_t*)"_abcde\n", 7);
	terminal_write(0, (uint8_t*)"_abcde\n", 8);
}

/*RTC Test
 *
 * Inputs: None
 * Outputs: writes to video memory and clears
 * Side Effects: clears prior state of video memory
 */
void rtc_test(){
	int i, c;
	for(i=MAXSPEED; i<=MINSPEED; i++){	//tests RTC by modifying freq through write and using read to delay
			set_cursors(0,0);
			rtc_write(i);
			for (c = 0; c <= RTC_INTERVAL; c++){
				rtc_read();
				update_cursor(get_screenx(), get_screeny());
			}
			clear();
		}
}

/*directory read test
 *
 * Inputs: None
 * Outputs: reads and lists the directories
 * Side Effects:
 */
void readDir_test(){
	set_cursors(0,0);

	dentry_t testind;
	int* num_entries=(int*)filesys_addr;
	int amt_dentrys=*num_entries;
	int i;
	//printf("File list:\n");
	for(i=0; i<amt_dentrys; i++){
		read_dentry_by_index(i,&testind);

	 	printf("file %d: ",i);
		print_withoutnull(testind.file_name, 32);
		printf(" type: %d, inode: %d \n",testind.file_type,testind.inode_num);
	}
}

/*File Read Test --long text
 *
 * Inputs: None
 * Outputs: reads from a file
 * Side Effects:
 */
void read_text(){
	set_cursors(0,0);

	dentry_t* testd;
	dentry_t test;
	testd=&test;
	read_dentry_by_name((uint8_t*)"",testd);
//	printf("test is %s \n",testd->file_name);
	//printf("testing read_dentry_by_name: %d",testd== )

	uint8_t buf[CONTENT_BUFFER+1];

	file_open((uint8_t*)"verylargetextwithverylongname.tx");
	//file_open("frame0.txt");
	file_read(0,buf,CONTENT_BUFFER);
	file_close(0);
	printf((int8_t*)buf);

}

/*File Read Test
 *
 * Inputs: None
 * Outputs: reads conent from a a file
 * Side Effects:
 */
void read_exec(){
	set_cursors(0,0);

	dentry_t* testd;
	dentry_t test;
	testd=&test;
	read_dentry_by_name((uint8_t*)"",testd);
//	printf("test is %s \n",testd->file_name);
	//printf("testing read_dentry_by_name: %d",testd== )

	uint8_t buf[CONTENT_BUFFER+1];

	file_open((uint8_t*)"fish");
	file_read(0,buf,CONTENT_BUFFER);
	file_close(0);
	putfile((int8_t*)buf);

}


/* Checkpoint 3 tests */

/*sys_call_jmptbl_test
 *	very basic test just to test that system call jump table is working
 * Inputs: None
 * Outputs: none
 * Side Effects: generates "general protection fault" after calling and returning
 								from system call. I think it's bc we're not actually an interrupt
								 so hopefully for actual user lvl system calls
								 fixed by changing to ret instead of iret
 */
void sys_call_jmptbl_test(){
	printf("Calling system_calls_assembly\n");
	asm volatile("MOVL $1,%eax"); //1 calls halt
	asm volatile("int $0x80");
	//system_calls_assembly();
}

/*open_test
 *	calls open in sys_calls.c just to check if it add files to file_array and stuff correctly
 * Inputs: None
 * Outputs: none
 * Side Effects:
 */
void open_test(){
	uint8_t fname[20]="frame0.txt";
	printf("Opening frame0.txt\n");
	open(fname);
	open((uint8_t*) "ls");
	printf("SUCCESS 2==%d\n",open((uint8_t*)"counter"));
	open((uint8_t*)"fish");
	open((uint8_t*)".");
	open((uint8_t*)"cat");
	open((uint8_t*)".");
	open((uint8_t*)"sigtest");
	printf("SHOULD BE failure: %d\n",open((uint8_t*)"testprint"));
	printf("After opening here is what is in file array\n");
	int i =0;
	for(i=0;i<8;i++){//this just prints everything in file array
		printf("at fd:%d flags=%d\n",i,file_array[i].flags);
		if (file_array[i].flags){
			printf(" File type: %d\n",file_array[i].flags & -2);
			file_array[i].fops_table->read(0,NULL,0);//close(i);
		}
	}
	printf("end of loop in open_test\n");
}

/*close_test
 *	tests close system call
 * Inputs: None
 * Outputs: none
 * Side Effects:
 */
 void close_test(){
	 close(3);
	 close(5);
	 int fd1=open((uint8_t*)"counter"), //0
			fd2=open((uint8_t*)"fish"),//1
			fd3=open((uint8_t*)"cat"),//2
			fd4=open((uint8_t*)"pingpong");//3
	 printf("fds %d, %d, %d, %d\n",fd1,fd2,fd3,fd4);

	 close(2);
	 open((uint8_t*)".");//now 2 should have flags:4
	 int i=0;
	 for(i=0;i<8;i++){
		 printf("fd: %d, flags:%d\n",i,file_array[i].flags);
	 }
 }
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	//TEST_OUTPUT("idt_test", idt_test());
	//paging_test();
	//terminalwrite_test();
  //rtc_test();
	//readDir_test();
	//read_text();
	//read_exec();
	//sys_call_jmptbl_test();
	//open_test();
	close_test();
}
