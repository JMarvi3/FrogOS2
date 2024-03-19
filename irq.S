.macro INTERRUPT id
	.globl irq\id
irq\id :
	cli
	push $0
	push $\id
	jmp irq_stub
.endm
.macro INTERRUPT_WITH_ERROR id
	.globl irq\id
irq\id :
	cli
	push $\id
	jmp irq_stub
.endm

INTERRUPT 0
INTERRUPT 1
INTERRUPT 2
INTERRUPT 3
INTERRUPT 4
INTERRUPT 5
INTERRUPT 6
INTERRUPT 7
INTERRUPT_WITH_ERROR 8
INTERRUPT 9
INTERRUPT_WITH_ERROR 10
INTERRUPT_WITH_ERROR 11
INTERRUPT_WITH_ERROR 12
INTERRUPT_WITH_ERROR 13
INTERRUPT_WITH_ERROR 14
INTERRUPT 15
INTERRUPT 16
INTERRUPT 17
INTERRUPT 18
INTERRUPT 19
INTERRUPT 20
INTERRUPT 21
INTERRUPT 22
INTERRUPT 23
INTERRUPT 24
INTERRUPT 25
INTERRUPT 26
INTERRUPT 27
INTERRUPT 28
INTERRUPT 29
INTERRUPT 30
INTERRUPT 31
INTERRUPT 32
INTERRUPT 33
INTERRUPT 34
INTERRUPT 35
INTERRUPT 36
INTERRUPT 37
INTERRUPT 38
INTERRUPT 39
INTERRUPT 40
INTERRUPT 41
INTERRUPT 42
INTERRUPT 43
INTERRUPT 44
INTERRUPT 45
INTERRUPT 46
INTERRUPT 47
INTERRUPT 48

KERNEL_DATA_SEGMENT = 0x10

.global irq_table
irq_table:
    .quad irqs
    
irqs:
    .quad irq0
    .quad irq1
    .quad irq2
    .quad irq3
    .quad irq4
    .quad irq5
    .quad irq6
    .quad irq7
    .quad irq8
    .quad irq9
    .quad irq10
    .quad irq11
    .quad irq12
    .quad irq13
    .quad irq14
    .quad irq15
    .quad irq16
    .quad irq17
    .quad irq18
    .quad irq19
    .quad irq20
    .quad irq21
    .quad irq22
    .quad irq23
    .quad irq24
    .quad irq25
    .quad irq26
    .quad irq27
    .quad irq28
    .quad irq29
    .quad irq30
    .quad irq31
    .quad irq32
    .quad irq33
    .quad irq34
    .quad irq35
    .quad irq36
    .quad irq37
    .quad irq38
    .quad irq39
    .quad irq40
    .quad irq41
    .quad irq42
    .quad irq43
    .quad irq44
    .quad irq45
    .quad irq46
    .quad irq47
    .quad irq48
    
	.extern irq_handler

irq_stub:
    cli
    push %rax
    push %rbx
    push %rcx
    push %rdx
    push %rsi
    push %rdi
    push %rbp
    push %r8
    push %r9
    push %r10
    push %r11
    push %r12
    push %r13
    push %r14
    push %r15

# Save ds, es, fs, gs
    mov %ds, %rax
    push %rax
    mov %es, %rax
    push %rax
    mov %fs, %rax
    push %rax
    mov %gs, %rax
    push %rax

    mov $KERNEL_DATA_SEGMENT, %rax
    mov %rax, %ds
    mov %rax, %es
    mov %rax, %fs
    mov %rax, %gs
    mov %rsp, %rdi
    mov $irq_handler, %rax
    call *%rax
    
# Restore ds, es, fs, gs
    pop %rax
    mov %rax, %gs
    pop %rax
    mov %rax, %fs
    pop %rax
    mov %rax, %es
    pop %rax
    mov %rax, %ds
    pop %r15
    pop %r14
    pop %r13
    pop %r12
    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rbp
    pop %rdi
    pop %rsi
    pop %rdx
    pop %rcx
    pop %rbx
    pop %rax
    add $16, %rsp
    iretq
