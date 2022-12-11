#include"screen.h"
#include"low_level.h"

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
    
    struct IDT32* idt = (struct IDT32*)0x800;
    idt->selector = 0;
    for(unsigned char i = 0; i < 255; i++){
        *(unsigned short*)(idt + 8*i + 0) = (unsigned short)(&__idt_default_handler & 0x0000ffff);
        *(unsigned short*)(idt + 8*i + 2) = (unsigned short)0x8;
        *(unsigned char*) (idt + 8*i + 4) = 0x00;
        *(unsigned char*) (idt + 8*i + 5) = 0x8e;//0 | IDT_32BIT_INTERRUPT_GATE | IDT_PRESENT;
        *(unsigned short*)(idt + 8*i + 6) = (unsigned short)((&__idt_default_handler & 0xffff0000) >> 16);
    }
    
    _lidt();

}