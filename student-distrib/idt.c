/*Interrupt desciptor table
  Useful references: ia-32 intel manual page 145, 156
*/

void idt_init(){
  int i;
  for (i=0;i<10;i++){
    idt[i].offset_15_00=NULL;
    idt[i].offset_31_16=NULL;
  }
}
