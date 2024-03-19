#include "irq.h"
#include "idt.h"
#include <port.h>
#include <stdlib.h>
#include <conio.h>
#include <process.h>

unsigned long long pit_counter=0;
void pit_handler(struct regs *r, void *data) {
	++pit_counter;
	disable();
	schedule_process((process_context_t *)r);
	enable();
}

void setup_pit()
{
	irq_handler_t *handler=malloc(sizeof(irq_handler_t));
	if(handler) {
		handler->handler=pit_handler;
		handler->next=0;
		handler->data=0;
		install_irq_handler(0, handler);
		disable();
		outportb(0x42, 0b00110100);
		io_wait();
		outportb(0x40, 0x9c);
		io_wait();
		outportb(0x40, 0x2e);
		io_wait();
		enable();
	}
}