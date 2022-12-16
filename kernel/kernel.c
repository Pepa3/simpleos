#include <kernel.h>
#include <isr.h>
#include <screen.h>
#include <stdio.h>
#include <string.h>
#include <disk.h>
#include <memory.h>
#include <exceptions.h>

#define kernel_end 55*0x200 //number of sectors we read
device_t* primary_disk;
int n = 0;

void kernel_main() {
  clear_screen();
  isr_install();
  irq_install();
  set_cursor_offset(0);
  print("Type something, it will go through the kernel\n");
  print("Type END to halt the CPU\n> ");
  //printf("A string: %s\nA decimal: %d\nA hex-value: %x\nA character: %c\n","World",123, 0xabc,'X');
  
  exceptions_init();
  mm_init(kernel_end);
  primary_disk = ata_init();
  
}

void user_input(char *input) {
  if(strcmp(input, "END") == 0) {
    print("Stopping the CPU. Bye!\n");
    __asm__ __volatile__("hlt");

  }else if(strcmp(input, "READ") == 0){
    uint8_t buf[512];
    primary_disk->read(buf,n++,1,primary_disk);
    for(int i=0;i<512;i++){
      printf("%x",*(buf+i));
    }

  }else{
    printf("Command %s not found\n",input);
  }

}
