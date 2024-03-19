#include "idt.h"
#include "irq.h"
#include <conio.h>
#include <string.h>
#include <stdio.h>
#include <port.h>

#define KERNEL_CODE_SEGMENT 0x08
#define KERNEL_DATA_SEGMENT 0x10
#define USER_CODE_SEGMENT 0x18
#define USER_DATA_SEGMENT 0x20
#define KERNEL_TSS_SEGMENT 0x28

typedef struct __attribute__((packed)){
   uint16_t offset_1;        // offset bits 0..15
   uint16_t selector;        // a code segment selector in GDT or LDT
   uint8_t  ist;             // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
   uint8_t  type_attributes; // gate type, dpl, and p fields
   uint16_t offset_2;        // offset bits 16..31
   uint32_t offset_3;        // offset bits 32..63
   uint32_t zero;            // reserved
} InterruptDescriptor64;

extern void **irq_table;
InterruptDescriptor64 idt[256] __attribute__((aligned(4096)));
irq_handler_t *irq_handlers[49] = {0};

void install_idt_handler(int num, void *func)
{
	InterruptDescriptor64 *entry = &idt[num];
	entry->selector = KERNEL_CODE_SEGMENT;
	entry->ist = 0;
	entry->type_attributes = 0xEF;//0x8E;
	entry->zero = 0;
	entry->offset_1 = (uint64_t)func & 0xffff;
	entry->offset_2 = ((uint64_t)func >> 16) & 0xffff;
	entry->offset_3 = ((uint64_t)func >> 32);
}

char *exception_labels[] = {
	"Divide by zero",
	"Debug",
	"Non-maskable interrupt",
	"Breakpoint",
	"Overflow",
	"Bound range exceeded",
	"Invalid opcode",
	"Device not available",
	"Double fault",
	"Coprocessor segment overrun",
	"Invalid TSS",
	"Segment not present",
	"Stack-segment fault",
	"General protection fault",
	"Page fault",
	"Reserved",
	"x87 FPU floating-point error",
	"Alignment check",
	"Machine check",
	"SIMD floating-point exception",
	"Virtualization exception",
	"Control protection exception",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Reserved",
	"Security exception",
	"Reserved",
	"Reserved",
	"Reserved"
};

void irq_handler(struct regs *r)
{
	uint64_t int_no = r->int_no;


	if (int_no < 32) {
		printf("Exception %s at %x:%x (%x)", exception_labels[int_no], r->cs, r->rip, r->err_code);
		if (int_no == 0xe) {
			uint64_t cr2;
			asm("mov %%cr2, %0" : "=r"(cr2));
			printf(" cr2: %x\n", cr2);
		} else {
			puts("\n");
		}
		for(;;) asm("hlt");
	} else {
		int irq_no = int_no - 32;
		if (irq_handlers[irq_no] != 0) {
			irq_handler_t *head = irq_handlers[irq_no];
			while (head != 0) {
				head->handler(r, head->data);
				head = head->next;
			}
		}
		if (int_no <= 48) { /// Hardware interrupt
			if (int_no >= 40) outportb(0xA0, 0x20);
			outportb(0x20, 0x20);
		}
	}
}

void install_irq_handler(int num, irq_handler_t *node)
{
	disable();
	if (irq_handlers[num] == 0) irq_clear_mask(num);
	node->next = irq_handlers[num];
	irq_handlers[num] = node;
	enable();
}

struct __attribute__((packed))
{
	uint16_t size;
	uint64_t offset;
} idtr;

void disable()
{
	asm("cli");
}

void enable()
{
	asm("sti");
}

void setup_idt()
{ 	
	memset(idt, 0, sizeof(idt));
	for(int i=0; i<49; ++i) {
		if (i<16)
			printf("%d: %p\n", i, irq_table[i]);
		install_idt_handler(i, irq_table[i]);
	}
	idtr.size = 256 * 16 - 1;
	idtr.offset = (uint64_t)idt;
	asm("lidt %0\n" ::"m"(idtr));
	// Set up PIC
	outportb(0x20,0x11);
	io_wait();
	outportb(0xA0,0x11);
	io_wait();
	outportb(0x21,0x20);
	io_wait();
	outportb(0xA1,0x28);
	io_wait();
	outportb(0x21,0x04);
	io_wait();
	outportb(0xA1,0x02);
	io_wait();
	outportb(0x21,0x01);
	io_wait();
	outportb(0xA1,0x01);
	io_wait();
	// mask out all interrupts
	outportb(0x21,0xfb);
	io_wait();
	outportb(0xA1,0xff);
	io_wait();
	enable();
}

void irq_set_mask(uint8_t irq)
{
	uint16_t port = 0x21;
	uint8_t value;
	if(irq>=8) {
		port=0xA1; irq-=8;
	}
	disable();
	value=inportb(port)|(1<<irq);
	outportb(port,value);
	enable();
}

void irq_clear_mask(uint8_t irq)
{
	if(irq>=16) return;
	uint16_t port = 0x21;
	uint8_t value;
	if(irq>=8) {
		port=0xA1; irq-=8;
	}
	disable();
	value=inportb(port)&~(1<<irq);
	outportb(port,value);
	enable();
}
