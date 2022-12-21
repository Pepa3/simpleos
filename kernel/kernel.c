#include <kernel.h>
#include <isr.h>
#include <screen.h>
#include <stdio.h>
#include <string.h>
#include <disk.h>
#include <memory.h>
#include <exceptions.h>
#include <argparser.h>
#include <syscalls.h>
#include <paging.h>
#include <timer.h>

#define kernel_end VIDEO_ADDRESS //probably the best address for now
#define END asm volatile("hlt");
int a,b,c,d,e,f = 0;

void kernel_main() {
  clear_screen();
  set_cursor_offset(0);
  mm_init(kernel_end);
  isr_install();
  irq_install();
  exceptions_init();
  ata_init();
  paging_init();
  syscall_init();

  print("Kernel Initialized!");
  wait(1);// 1 sec
  clear_screen();

  printf("> ");
}

void user_input(char *input) {
  args_t* args = (args_t*)malloc(sizeof(args_t));
  parse_args(args,input);

  if(strcmp(args->str, "END") == 0) {
    print("Stopping the CPU. Bye!\n");
    asm volatile("hlt");

  }else if(strcmp(args->str, "READ") == 0){
    if(args->next==0){printf("Please provide disk number"); goto end;}

    int diskn = 1;
    if(args->next->str){
      if(strcmp(args->next->str,"0")){ // disk 0
        diskn = 0;
      }else if(strcmp(args->next->str,"1")){ // disk 1
        diskn = 1;
      }
    }
    
    uint16_t buf[256];
    ata_read(buf,0,1,get_disk(diskn));
    for(int i=0;i<256;i++){
      printf("%x",*(buf+i));
    }

  }else if(strcmp(args->str, "WRITE") == 0){
    uint16_t buf[256];
    ata_read(buf,0,1,get_disk(0));
    ata_write(buf,0,1,get_disk(1));

  }else if(strcmp(args->str, "TEST") == 0){
    printf("Test");
  }else{
    printf("Command %s not found\n",args->str);
  }
  end:
  printf("\n> ");
  free_args(args);
}
