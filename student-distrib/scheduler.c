#include "scheduler.h"
#include "idt.h"
#include "lib.h"

/*pit handler
 *
 * Inputs: None
 * Outputs: executes at pit interrupt, does scheduling!
 * Side Effects: None
 */
void pit_handler(){
  printf("PIT INTERRUPT!\n");
  send_eoi(0);
}
