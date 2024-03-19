#include <stdint.h>

struct regs
{
    uint64_t ds, es, fs, gs; // Segment registers
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rbp, rdi, rsi, rdx, rcx, rbx, rax;
    uint64_t int_no, err_code;
    uint64_t rip, cs, rflags, rsp, ss;
};

typedef struct irq_handler_struct
{
    void (*handler)(struct regs *, void *);
    struct irq_handler_struct *next;
    void *data;
} irq_handler_t;

void install_irq_handler(int num, irq_handler_t *node);