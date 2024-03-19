EI_NIDENT equ 16


; typedef struct {
;         unsigned char   e_ident[EI_NIDENT];
;         Elf64_Half      e_type;
;         Elf64_Half      e_machine;
;         Elf64_Word      e_version;
;         Elf64_Addr      e_entry;
;         Elf64_Off       e_phoff;
;         Elf64_Off       e_shoff;
;         Elf64_Word      e_flags;
;         Elf64_Half      e_ehsize;
;         Elf64_Half      e_phentsize;
;         Elf64_Half      e_phnum;
;         Elf64_Half      e_shentsize;
;         Elf64_Half      e_shnum;
;         Elf64_Half      e_shstrndx;
; } Elf64_Ehdr;

; typedef struct {
; 	Elf64_Word	p_type;
; 	Elf64_Word	p_flags;
; 	Elf64_Off	p_offset;
; 	Elf64_Addr	p_vaddr;
; 	Elf64_Addr	p_paddr;
; 	Elf64_Xword	p_filesz;
; 	Elf64_Xword	p_memsz;
; 	Elf64_Xword	p_align;
; } Elf64_Phdr;

struc Elg64_Ehdr
    .e_ident: resb EI_NIDENT
    .e_type:  resw 1
    .e_machine: resw 1
    .e_version: resd 1
    .e_entry: resq 1
    .e_phoff: resq 1
    .e_shoff: resq 1
    .e_flags: resd 1
    .e_ehsize: resw 1
    .e_phentsize: resw 1
    .e_phnum: resw 1
    .e_shentsize: resw 1
    .e_shnum: resw 1
    .e_shstrndx: resw 1
endstruc

struc Elf64_Phdr
    .p_type resd 1
    .p_flags resd 1
    .p_offset resq 1
    .p_vaddr resq 1
    .p_paddr resq 1
    .p_filesz resq 1
    .p_memsz resq 1
    .p_align resq 1
endstruc

[BITS 32]
[org 0x400200] ; 4MB + 512 bytes

_start:
    cld
    xor eax, eax
    mov edi, 0x1000
    mov ecx, 0x800
    rep stosd

    mov [0x1000], word 0x2003 | 4
    mov [0x2000], word 0x3003 | 4
    mov al, 0x83 | 4	; 2MB PDE (U/S set)
    mov edi, 0x3000

    pde_loop:
    mov [edi+ecx*8], eax
    add eax, 1<<21
    inc ecx
    cmp ecx, 512
    jne pde_loop

    mov eax, cr0
    btr eax, 31
    mov cr0, eax

    mov eax, cr4
    or eax, (1<<5)|(1<<7)
    mov cr4, eax

    mov eax, 0x1000
    mov cr3, eax
    mov ecx, 0xC0000080
    rdmsr
    bts eax, 8
    wrmsr

    mov eax, cr0
    bts eax, 31
    mov cr0, eax

    jmp 0x18:long_mode


start_of_kernel equ 0x402000

[BITS 64]
long_mode:
    call cls
    mov rsi, start_of_kernel
    mov rax, [rsi + Elg64_Ehdr.e_phoff]
    add rax, start_of_kernel
    mov [e_phoff], rax
    xor rax, rax
    mov ax, [rsi + Elg64_Ehdr.e_phentsize]
    mov [e_phentsize], rax
    mov ax, [rsi + Elg64_Ehdr.e_phnum]
    mov [e_phnum], rax

    mov rsi, [e_phoff]
    mov rcx, [e_phnum]
.ph_loop:
    call process_header_entry
    add rsi, [e_phentsize]
    loop .ph_loop
    
    mov rsi, message2
    call write_string
    
    mov rsi, start_of_kernel
    mov rax, [rsi + Elg64_Ehdr.e_entry]
    call rax

.halt:
    hlt
    jmp .halt

process_header_entry: ; rsi - pointer to program header entry
    pushf
    push rax
    push rcx
    push rsi
    push rdi
    mov eax, [rsi + Elf64_Phdr.p_type]
    cmp eax, 1
    jne .skip

    ; retrieve p_paddr and memory size
    cld
    mov rdi, [rsi + Elf64_Phdr.p_paddr]
    mov rcx, [rsi + Elf64_Phdr.p_memsz]
    ; clear rcx bytes at rdi
    xor rax, rax
    rep stosb
    ; retrieve p_offset, p_filesz and p_paddr
    mov rax, [rsi + Elf64_Phdr.p_offset]
    add rax, start_of_kernel
    mov rcx, [rsi + Elf64_Phdr.p_filesz]
    mov rdi, [rsi + Elf64_Phdr.p_paddr]
    mov rsi, rax
    ; copy p_filesz bytes from p_offset + start_of_kernel to p_paddr
    rep movsb
.skip:
    pop rdi
    pop rsi
    pop rcx
    pop rax
    popf
    ret

cls:
    push rdi
    push rax
    push rcx
    mov edi, 0xb8000
    mov eax, 0x0f20
    mov ecx, 80*25
    rep stosd
    pop rcx
    pop rax
    pop rdi
    ret

write_string:
    push rdi
    push rax
    push rbx
    ; string in esi
    mov edi, [video]
    mov ah, 0x0f
    xor ebx, ebx
.loop:
    mov al, [esi+ebx]
    test al, al
    jz .done
    mov [edi+2*ebx], ax
    inc ebx
    jmp .loop

.done:
    add edi, 160 ; next line
    mov [video], edi
    pop rbx
    pop rax
    pop rdi
    ret

video: dq 0xb8000
error_message: db "Error", 0
message2: db "Hello, World!", 0
e_phoff: dq 0
e_phentsize: dq 0
e_phnum: dq 0
