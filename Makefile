CC = gcc
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra -fno-pie -fno-stack-protector
LDFLAGS = -m32 -T linker.ld -nostdlib -no-pie

OBJS = boot.o kernel.o gui.o exe_subsystem.o ai_subsystem.o mouse.o wind_subsystem.o keyboard.o screen.o idt.o deb_subsystem.o

all: windos.iso

windos.iso: kernel.bin grub.cfg
	mkdir -p isodir/boot/grub
	cp kernel.bin isodir/boot/kernel.bin
	cp grub.cfg isodir/boot/grub/grub.cfg
	grub-mkrescue -o windos.iso isodir

kernel.bin: $(OBJS) linker.ld
	$(CC) $(LDFLAGS) -o kernel.bin $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

%.o: %.asm
	nasm -f elf32 $< -o $@

clean:
	rm -f *.o kernel.bin windos.iso
	rm -rf isodir
