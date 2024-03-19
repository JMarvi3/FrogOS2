#include <conio.h>
#include <stdlib.h>
#include <stdio.h>
#include "irq.h"
#include "idt.h"

#include <stddef.h>
#include <syscall.h>
void *malloc(size_t size);
void setup_gdt();
void setup_idt();
void setup_syscall();
void setup_pit();
void setup_process();

// int irq0_handler_func(regs *r)
// {
//     puts("IRQ0\n");
//     return 0;
// }

// int irq42_handler_func(regs *r)
// {
//     puts("IRQ42\n");
//     asm("cli");
//     for(;;) asm("hlt");
//     return 0;
// }

void lidt();
void load_tss();
void setup_syscall();

extern uint64_t pit_counter;

int cmain()
{
    cls();
    asm("cli");
    unsigned char *message = "Long Mode.";
    puts(message);
    uint16_t *sector_number = (uint16_t *) 0x13ffe00;
    printf("Last sector number: %x\n", *sector_number);
    setup_gdt();
    // handler_node *irq0_handler = malloc(sizeof(handler_node));
    // irq0_handler->handler = irq0_handler_func;
    // irq0_handler->next = 0;
    // handler_node *irq42_handler = malloc(sizeof(handler_node));
    // irq42_handler->handler = irq42_handler_func;
    // irq42_handler->next = 0;
    // install_irq_handler(0, irq0_handler);
    // install_irq_handler(42, irq42_handler);
    // load_tss();
    setup_idt();

    // asm("int $40");

    puts("TSS loaded\n");
    setup_syscall();
    setup_pit();
    setup_process();
    for(;;) asm("hlt");
    return 0;
}
