target remote | qemu-system-x86_64 -S -gdb stdio -m 128 -fda floppy.img
b *0x7c00
b *0x7d23
b *0x10200
c
