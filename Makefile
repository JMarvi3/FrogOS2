#CFLAGS=-std=gnu99 -ffreestanding -O2 -fno-pic -fno-inline-small-functions
CFLAGS=-Iinclude -g -std=gnu99 -O2 -fno-pic -fno-inline-small-functions -fno-builtin -nostdinc -mgeneral-regs-only

all: kernel boot stage2

OBJECT_FILES = cmain.o irq.o idt.o mem.o asm.o gdt.o syscall.o pit.o process.o init.o
	
kernel: disk.img $(OBJECT_FILES) lib/libc.a disk.img
	# $(AS) -a=irq.lst -o irq.o irq.S 
	# ld -o kernel.bin -Ttext 0x10200 --oformat binary -Llib cmain.o irq.o idt.o mem.o -l:libc.a
	gcc -g -static -nostdlib -T ld.script -o kernel -Llib $(OBJECT_FILES) -l:libc.a -z noexecstack
	dd bs=512 if=kernel seek=16 conv=notrunc of=disk.img

lib/libc.a: lib/*.c
	$(MAKE) -C lib

boot: boot.asm disk.img
	nasm -l boot.lst boot.asm -o boot
	dd bs=512 if=boot conv=notrunc of=disk.img

stage2: stage2.asm disk.img
	nasm -l stage2.lst stage2.asm -o stage2
	dd bs=512 seek=1 if=stage2 conv=notrunc of=disk.img

disk.img:
	python ./make_disk_img.py

clean:
	rm -rf disk.img
	rm -rf kernel boot stage2 idt.s cmain.s *.o *.lst
	$(MAKE) -C lib clean

QEMU_FLAGS = -m 128 -hda disk.img -monitor vc

debug: kernel boot stage2
	qemu-system-x86_64 $(QEMU_FLAGS) -S -s &
	gdb -x script3.gdb

run: kernel boot stage2
	qemu-system-x86_64 $(QEMU_FLAGS)
