#include <exceptions.h>
#include <isr.h>
#include <stdio.h>
#include <stdbool.h>

#define is_tasking() true
#define p_name() "KERNEL"
#define p_pid() 0

void exc_divide_by_zero()
{
	//kerror("Divide by zero at [0x%x:0x%x] EFLAGS: 0x%x\n", cs, eip, eflags);
	printf("Divide by zero!\n");
	if(is_tasking()) {
		printf("Notifying process %s (%d) with SIGILL\n", p_name(), p_pid());
		//send_sig(SIG_ILL);
	}
}

void exc_debug()
{
	printf("Debug!\n");
	if(is_tasking()) {
		printf("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
		//send_sig(SIG_TERM);
	}
}

void exc_nmi()
{
	printf("NMI\n");
	/*if(is_tasking()) {
		printf("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
		//send_sig(SIG_TERM);
	}*/
	return;
}

void exc_brp()
{
	printf("Breakpoint!\n");
	return;
}

void exc_overflow()
{
	printf("Overflow!\n");
	if(is_tasking()) {
		printf("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
		//send_sig(SIG_TERM);
	}
	return;
}

void exc_bound()
{
	printf("Bound range exceeded.\n");
	if(is_tasking()) {
		printf("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
		//send_sig(SIG_TERM);
	}
	return;
}

void exc_invopcode()
{
	printf("Invalid opcode.\n");
	if(is_tasking()) {
		printf("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
		//send_sig(SIG_TERM);
	}
	return;
}

void exc_device_not_avail()
{
	printf("Device not available.\n");
	if(is_tasking()) {
		printf("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
		//send_sig(SIG_TERM);
	}
	return;
}

void exc_double_fault()
{
	printf("Double fault, halting.\n");
	//set_task(0);
	for(;;);
}

void exc_coproc()
{
	printf("Coprocessor fault, halting.\n");
	//set_task(0);
	for(;;);
	return;
}

void exc_invtss()
{
	printf("TSS invalid.\n");
	if(is_tasking()) {
		printf("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
		//send_sig(SIG_TERM);
	}
	return;
}

void exc_segment_not_present()
{
	printf("Segment not present.\n");
	if(is_tasking()) {
		printf("Notifying process %s (%d) with SIGSEGV\n", p_name(), p_pid());
		//send_sig(SIG_SEGV);
	}
	return;
}

void exc_ssf()
{
	printf("Stacksegment faulted.\n");
	if(is_tasking()) {
		printf("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
		//send_sig(SIG_TERM);
	}
	return;
}

void exc_gpf()
{
	printf("General protection fault.\n");
	if(is_tasking()) {
		printf("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
		//send_sig(SIG_TERM);
	}
	return;
}

void exc_pf()
{
	printf("Page fault in %s (%d)\n",p_name(), p_pid());
	if(is_tasking()) {
		printf("Notifying process %s (%d) with SIGTERM\n", p_name(), p_pid());
		//send_sig(SIG_TERM);
	}
	return;
}

void exc_reserved()
{
	printf("This shouldn't happen. Halted.\n");
	//set_task(0);
	for(;;);
	return;
}

void exceptions_init()
{
	print("Installing exceptions handlers\n");
	register_interrupt_handler(0, exc_divide_by_zero);
	register_interrupt_handler(1, exc_debug);
	register_interrupt_handler(2, exc_nmi);
	register_interrupt_handler(3, exc_brp);
	register_interrupt_handler(4, exc_overflow);
	register_interrupt_handler(5, exc_bound);
	register_interrupt_handler(7, exc_device_not_avail);
	register_interrupt_handler(6, exc_invopcode);
	register_interrupt_handler(8, exc_double_fault);
	register_interrupt_handler(9, exc_coproc);
	register_interrupt_handler(10, exc_invtss);
	register_interrupt_handler(11, exc_segment_not_present);
	register_interrupt_handler(12, exc_ssf);
	register_interrupt_handler(13, exc_gpf);
	register_interrupt_handler(14, exc_pf);
	register_interrupt_handler(15, exc_reserved);
	return;
}