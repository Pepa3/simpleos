#include <kernel.h>
#include <isr.h>
#include <screen.h>
#include <stdio.h>
#include <string.h>
#include <disk.h>
#include <memory.h>
#include <exceptions.h>
#include <argparser.h>

#define kernel_end VIDEO_ADDRESS //probably the best address for now
int a,b,c,d,e,f = 0;

void kernel_main() {
  clear_screen();
  set_cursor_offset(0);
  isr_install();
  irq_install();
  exceptions_init();
  mm_init(kernel_end);
  ata_init();

  print("Kernel initialized\n> ");
  //clear_keyboard_buffer();
}

void user_input(char *input) {
  args_t* args = (args_t*)malloc(sizeof(args_t));
  parse_args(args,input);

  if(strcmp(args->str, "END") == 0) {
    print("Stopping the CPU. Bye!\n");
    __asm__ __volatile__("hlt");

  }else if(strcmp(args->str, "READ") == 0){
    uint16_t buf[256];
    ata_read(buf,0,1,get_disk(1));
    for(int i=0;i<256;i++){
      printf("%x",*(buf+i));
    }

  }else if(strcmp(args->str, "WRITE") == 0){
    uint16_t buf[256];
    ata_read(buf,0,1,get_disk(0));
    ata_write_one(buf,0,get_disk(1));

  }else if(strcmp(args->str, "TEST") == 0){
   /* int i = atoi(args->next->str, 10);
    printf("%d",i);*/
  }else{
    printf("Command %s not found\n",args->str);
  }
  printf("\n> ");
  free_args(args);
}
