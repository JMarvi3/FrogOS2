KERNEL_CODE_SEGMENT = 0x08
KERNEL_DATA_SEGMENT = 0x10
USER_32BIT_CODE_SEGMENT = 0x18
USER_DATA_SEGMENT = 0x20
USER_CODE_SEGMENT = 0x28
KERNEL_TSS_SEGMENT = 0x30


gdtr:
    .word 0
    .quad 0

.global load_gdt
load_gdt:
    mov %rdi, [gdtr+2]
    mov %si, [gdtr]
    lgdt [gdtr]

    sub $16, %rsp
    movq $KERNEL_CODE_SEGMENT, 8(%rsp)
    movabsq $.flush, %rax
    mov %rax, 0(%rsp)
    lretq

.flush:
    mov $KERNEL_DATA_SEGMENT, %rax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs
    mov %ax, %ss
    ret


.global _start
_start:
    ; mov $0x300000, %rsp
    jmp cmain
    cli
.loop:
    hlt
    jmp .loop

.global syscall_handler
syscall_handler:
    push %rdi
    push %rsi
    push %rdx
    push %rcx
    push %r8
    push %r9
    push %r10
    push %r11
    push %rbx
    push %rbp
    push %rax
    mov %rsp, %rdi
    sti
    call ksyscall
    pop %rax
    pop %rbp
    pop %rbx
    pop %r11
    pop %r10
    pop %r9
    pop %r8
    pop %rcx
    pop %rdx
    pop %rsi
    pop %rdi
    sysretq

syscall_handler_ptr:
    .quad syscall_handler

.global setup_syscall
setup_syscall:
    push %rdx
    push %rcx
    push %rax

SYSRET_SELECTOR = (USER_32BIT_CODE_SEGMENT | 3) << 16
SYSCALL_SELECTOR = KERNEL_CODE_SEGMENT
STAR = SYSRET_SELECTOR | SYSCALL_SELECTOR

    mov $STAR, %rdx
    xor %rax, %rax
    mov $0xC0000081, %ecx # STAR MSR
    wrmsr

    mov $0xC0000082, %ecx # LSTAR MSR
    mov [syscall_handler_ptr], %eax
    mov [syscall_handler_ptr+4], %edx
    wrmsr

    mov $0xC0000084, %ecx # FMASK MSR
    mov 0x200, %eax # mask interrupt flag
    xor %edx, %edx
    wrmsr

    mov $0xC0000080, %ecx # EFER MSR
    rdmsr
    or $0x1, %eax
    wrmsr

    pop %rax
    pop %rcx
    pop %rdx
    ret

