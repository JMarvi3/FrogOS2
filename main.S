.global install_idt
.global _start
_start:
	call cmain
	jmp .

.global irq0
irq0:
	push $0xdead
	jmp .

.global idt
	.balign 4096
	.comm idt, 4096

.global idt_ptr
idt_ptr:
	.short 4095
	.long idt
	
