#include <kernel.h>
#include <isr.h>
#include <screen.h>
#include <stdio.h>
#include <string.h>
#include <disk.h>
#include <memory.h>
#include <exceptions.h>

#define kernel_end VIDEO_ADDRESS //probably the best address for now
device_t* main_disk;
int n = 0;

void kernel_main() {
  clear_screen();
  set_cursor_offset(0);
  isr_install();
  irq_install();
  exceptions_init();
  mm_init(kernel_end);
  ata_init();
  main_disk = get_disk(1);

  print("Kernel initialized\n> ");
}

void user_input(char *input) {
  if(strcmp(input, "END") == 0) {
    print("Stopping the CPU. Bye!\n");
    __asm__ __volatile__("hlt");

  }else if(strcmp(input, "READ") == 0){
    uint8_t buf[512];
    main_disk->read(buf,n++,1,main_disk);
    for(int i=0;i<512;i++){
      printf("%x",*(buf+i));
    }

  }else if(strcmp(input, "TEST") == 0){
    printf("Test #1: free, malloc\n");
    for (size_t i = 0; i < 100; i++){
      uint8_t * buf = malloc(200*i);
      for (size_t j = 0; j < 200*i; j++)
      {
        buf[j] = 0xff;
      }
      
      free(buf);
    }
    free(malloc(123));

  }else{
    printf("Command %s not found\n",input);
  }
  printf("\n> ");

}
