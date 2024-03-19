[bits 16]
[org 0x7c00]

;boot drive is in dl

    cli
    mov esp, 0x9000
    mov [drive_number], dl

    ; enable A20 line
    in al, 0x92
    or al, 2
    out 0x92, al
    
    lgdt [gdtr]

    mov eax, cr0
    mov ebx, eax
    or eax, 1
    mov cr0, eax
    jmp .flush1

.flush1:
    mov eax, 0x20 ; 16-bit data segment (4 GB)
    mov gs, eax
    mov cr0, ebx
    jmp .flush2

.flush2:
    xor eax, eax
    mov gs, eax

    mov esi, unreal_message
    call write_string

    ; (512 * 128 * 64) = 4MB
    ; (512 * 128 * 128) = 8MB
    ; (512 * 128 * 256) = 16MB
SECTORS_TO_READ equ 128 * 256
READ_ITERATIONS equ SECTORS_TO_READ / 128
    ; call read_from_disk READ_ITERATIONS times (reading 128 sectors)
    ; for(ecx = 0; ecx < READ_ITERATIONS; ecx++) {
    ;   read_from_disk(ecx*128)
    ;   memcpy(0x10000, 0x400000 + ecx*0x10000, 0x10000)
    ;}

    xor ecx, ecx
    cld
.loop:
    push ecx
    shl ecx, 7 ; ecx *= 128
    call read_from_disk
    pop ecx
    jc read_error

    ; copy 0x10000 (64KB) bytes from 0x10000 to 0x400000 + ecx*0x10000
    push ecx

    shl ecx, 16 ; ecx *= 0x10000
    add ecx, 0x400000
    mov edi, ecx
    mov ax, 0x1000
    mov fs, ax
    xor esi, esi ; fs:esi = 0x10000
    mov ecx, 0x10000/4
.copy_loop:
    mov eax, fs:[esi]
    mov gs:[edi], eax
    add esi, 4
    add edi, 4
    loop .copy_loop
    
    pop ecx

    inc ecx
    cmp ecx, READ_ITERATIONS
    jl .loop

    mov esi, done_message
    call write_string

    lgdt [gdtr]
    mov eax, cr0
    or eax, 1
    mov cr0, eax
    jmp 0x08:protected_mode


read_error:
    mov esi, error_message
    call write_string
    jmp $


; write message in esi to the screen
write_string:
    mov edi, [video]
    xor ebx, ebx
    mov ah, 0x0f
.write_letter:
    mov al, [esi + ebx] ; get the next character
    cmp al, 0
    je .done_writing
    mov gs:[edi + ebx*2], ax ; write the character to the screen
    inc ebx
    ; add edi, 2
    jmp .write_letter

.done_writing:
    add edi, 160    ; move to the next line
    mov [video], edi
    ret


read_from_disk:
    ; lba in ecx
    ; modifies eax and CF
    push ebx
    push ecx
    push edx
    push esi
    mov [dap+0x8], ecx
    mov dl, [drive_number]
    mov ah, 0x42
    mov bx, 0x55aa
    mov si, dap
    int 0x13
    pop esi
    pop edx
    pop ecx
    pop ebx
    ret

[bits 32]
protected_mode:
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax
    ; write protected_message to the screen
    mov esi, protected_message
    mov edi, [video]
    mov ah, 0x0f
    xor ebx, ebx
    .write_letter:
    mov al, [esi + ebx]
    cmp al, 0
    je .done_writing
    mov [edi + ebx*2], ax
    inc ebx
    jmp .write_letter
    .done_writing:

    jmp 0x08:0x400200

message: db         "Boot sector (asm)----", 0
unreal_message: db  "Unreal mode ---------", 0
protected_message: db  "Protected mode ------", 0
error_message: db   "Error reading disk---", 0
done_message: db    "Done ----------------", 0

video: dq 0xb8000

loop_counter: dq 0
drive_number: db 0

dap:
     db 10, 0 ; size of dap, always 0
     dw 128 ; number of sectors to read
     dw 0, 0x1000 ; offset:segment of buffer
     dq 0 ; LBA of first sector to read

align 8

gdt: dq 0
        ; 0x08 is a 32bit code segment
     dw 0xffff, 0
     db 0, 0x9a, 0xcf, 0
        ; 0x10 is a 32bit data segment
     dw 0xffff, 0
     db 0, 0x92, 0xcf, 0
        ; 0x18 is a 64bit code segment
     dw 0, 0
     db 0, 0x9a, 0x20, 0
        ; 0x20 is a 16bit data segment
     dw 0xffff, 0
     db 0, 0x93, 0x8f, 0

gdtlen equ $-gdt

gdtr:   
  	dw gdtlen-1
	dd gdt

times 510-($-$$) db 0

dw 0xaa55
