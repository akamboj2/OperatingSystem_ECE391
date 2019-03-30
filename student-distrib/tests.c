#include "tests.h"
#include "x86_desc.h"
#include "lib.h"
#include "rtc.h"
#include "filesys.h"


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
	dentry_t* testd;
	dentry_t test;
	testd=&test;
	read_dentry_by_name((uint8_t*)"sigtest",testd);
//	printf("test is %s \n",testd->file_name);
	//printf("testing read_dentry_by_name: %d",testd== )

	dentry_t* testind=&test;
	int* num_entries=(int*)filesys_addr;
	int amt_dentrys=*num_entries;
	int i;
	//printf("File list:\n");
	for(i=0; i<amt_dentrys; i++){
		read_dentry_by_index(i,testind);

	 	printf("file %d: ",i);
		print_withoutnull(testind->file_name, 32);
		printf(" type: %d, inode: %d \n",testind->file_type,testind->inode_num);
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
 */
void sys_call_jmptbl_test(){
	printf("Calling system_calls_assembly\n");
	asm volatile("MOVL $1,%eax"); //1 calls halt
	system_calls_assembly();
}
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	//TEST_OUTPUT("idt_test", idt_test());
	//paging_test();
//	rtc_test();
	//readDir_test();
	//read_text();
	//read_exec();
		sys_call_jmptbl_test();
}
