#include <kernel.h>
#include <isr.h>
#include <screen.h>
#include <stdio.h>
#include <string.h>
#include <disk.h>
#include <memory.h>
#include <exceptions.h>

extern uint32_t kernel_end;

void kernel_main() {
  clear_screen();
  isr_install();
  irq_install();
  set_cursor_offset(0);
  print("Type something, it will go through the kernel\n");
  print("Type END to halt the CPU\n> ");
  printf("A string: %s\nA decimal: %d\nA hex-value: %x\nA character: %c","World",123, 0xabc,'X');
  
  exceptions_init();
  mm_init(kernel_end);
  ata_init();
}

void user_input(char *input) {
  if (strcmp(input, "END") == 0) {
    print("Stopping the CPU. Bye!\n");
    __asm__ __volatile__("hlt");
  }

  print("You said: ");
  print(input);
  print("\n> ");
}
