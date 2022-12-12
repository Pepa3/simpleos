#include "screen.h"
#include "low_level.h"

struct IDT32 {
   unsigned short offset_1;        // offset bits 0..15
   unsigned short selector;        // a code segment selector in GDT or LDT
   unsigned char  zero;            // unused, set to 0
   unsigned char  type_attributes; // gate type, dpl, and p fields
   unsigned short offset_2;        // offset bits 16..31
};

void defidt(){};

void main(){
    clear_screen();
    print("Hello,World!\n");
    
    
}