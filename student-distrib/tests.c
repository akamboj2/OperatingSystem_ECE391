#include "tests.h"
#include "x86_desc.h"
#include "lib.h"

#define PASS 1
#define FAIL 0

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
 * Asserts that first 10 IDT entries are not NULL
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
/* Checkpoint 3 tests */
/* Checkpoint 4 tests */
/* Checkpoint 5 tests */


/* Test suite entry point */
void launch_tests(){
	//TEST_OUTPUT("idt_test", idt_test());
	// launch your tests here
	paging_test();
}
