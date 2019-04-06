#include "sys_calls.h"
#include "types.h"
#include "lib.h"
#include "filesys.h"
#include "assembly_linkage.h"

/*halt
 * Description: None
 * Inputs: None
 * Outputs/Return Values: Returns 0 on succes, -1 on failure
 * Side Effects: none
 */
int32_t halt (uint8_t status){
  printf("IN HALT!\n");
  return 0;
}

/*execute
 * Description: None
 * Inputs: None
 * Outputs/Return Values: Returns 0 on succes, -1 on failure
 * Side Effects: none
 */
int32_t execute (const uint8_t* command){
//  printf("In execute!\n");
  if(command == NULL){
    return -1;
  }
  uint8_t filename[32];
  uint8_t data[32];

  dentry_t *test;
  //read_dentry_by_name((uint8_t*)"",test);

  //memcpy(filename, command, 32);
  file_open((uint8_t*)"ls");
  if(file_read(0,data,32) == -1){
      file_close(0);
      return -1;
  }
  file_close(0);
  if(data[1] != 'E' || data[2] != 'L' || data[3] != 'F')
    return -1;
  //print_withoutnull((int8_t*)data, 4);
  //printf("%s", filename);*/
  printf("\nIn execute\n");
  //asm volatile("iret");
  //context_switch();
  return 0;
}

/*read
 * Description: None
 * Inputs: None
 * Outputs/Return Values: Returns 0 on succes, -1 on failure
 * Side Effects: none
 */
int32_t read (int32_t fd, void* buf, int32_t nbytes){
  return 0;
}

/*write
 * Description: None
 * Inputs: None
 * Outputs/Return Values: Returns 0 on succes, -1 on failure
 * Side Effects: none
 */
int32_t write (int32_t fd, const void* buf, int32_t nbytes){
  return 0;
}

/*open
 * Description: None
 * Inputs: None
 * Outputs/Return Values: Returns 0 on succes, -1 on failure
 * Side Effects: none
 */
int32_t open (const uint8_t* filename){
  return 0;
}

/*close
 * Description: None
 * Inputs: None
 * Outputs/Return Values: Returns 0 on succes, -1 on failure
 * Side Effects: none
 */
int32_t close (int32_t fd){
  return 0;
}

/*getargs
 * Description: None
 * Inputs: None
 * Outputs/Return Values: Returns 0 on succes, -1 on failure
 * Side Effects: none
 */
int32_t getargs (uint8_t* buf, int32_t nbytes){
  return 0;
}

/*vidmap
 * Description: None
 * Inputs: None
 * Outputs/Return Values: Returns 0 on succes, -1 on failure
 * Side Effects: none
 */
int32_t vidmap (uint8_t** screen_start){
  return 0;
}

/*set_handler
 * Description: None
 * Inputs: None
 * Outputs/Return Values: Returns 0 on succes, -1 on failure
 * Side Effects: none
 */
int32_t set_handler (int32_t signum, void* handler_address){
  return 0;
}

/*sigreturn
 * Description: None
 * Inputs: None
 * Outputs/Return Values: Returns 0 on succes, -1 on failure
 * Side Effects: none
 */
int32_t sigreturn (void){
  return 0;
}
