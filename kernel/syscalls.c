#include <syscalls.h>
#include <stdint.h>
#include <stddef.h>
#include <stdio.h>
#include <memory.h>
#include <keyboard.h>
#include <isr.h>
#include <idt.h>

#define p_pid() 2

void syscall(registers_t r){
	/*
	* eax=0
	*/
	switch(r.ebx)
	{
		case 0: // restart
			break;
		case 1: // exit
			goto end;
			break;
		case 2: // read stdin ecx = 
			break;
		case 3:	// write stdout ecx = str*
			printf("%s",r.ecx);
			break;
		case 4:	// write stderr ecx = str*
			printf("ERR: %s",r.ecx);
			break;
		default:
			break;
	}
	end:;
}

void syscall_init()
{
	printf("Registering syscall interface\n");
	register_interrupt_handler(45, syscall);
    //set_idt_gate(0x80,(uint32_t) syscall);
}