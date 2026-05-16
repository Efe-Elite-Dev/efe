#!/bin/bash
echo "==> Wind OS (AI-Full Modular) Derleme İşlemi Başladı..."

# Kalıntıları süpür
rm -f *.o kernel.bin windos.iso
rm -rf isodir

# 1. Önyükleyici
nasm -f elf32 boot.asm -o boot.o

# 2. AI Tabanlı Odalar ve Çekirdek
gcc -m32 -ffreestanding -O2 -Wall -Wextra -fno-pie -fno-stack-protector -c kernel.c -o kernel.o
gcc -m32 -ffreestanding -O2 -Wall -Wextra -fno-pie -fno-stack-protector -c gui.c -o gui.o
gcc -m32 -ffreestanding -O2 -Wall -Wextra -fno-pie -fno-stack-protector -c exe_subsystem.c -o exe_subsystem.o
gcc -m32 -ffreestanding -O2 -Wall -Wextra -fno-pie -fno-stack-protector -c ai_subsystem.c -o ai_subsystem.o
gcc -m32 -ffreestanding -O2 -Wall -Wextra -fno-pie -fno-stack-protector -c mouse.c -o mouse.o
gcc -m32 -ffreestanding -O2 -Wall -Wextra -fno-pie -fno-stack-protector -c keyboard.c -o keyboard.o
gcc -m32 -ffreestanding -O2 -Wall -Wextra -fno-pie -fno-stack-protector -c wind_subsystem.c -o wind_subsystem.o
gcc -m32 -ffreestanding -O2 -Wall -Wextra -fno-pie -fno-stack-protector -c screen.c -o screen.o
gcc -m32 -ffreestanding -O2 -Wall -Wextra -fno-pie -fno-stack-protector -c idt.c -o idt.o
gcc -m32 -ffreestanding -O2 -Wall -Wextra -fno-pie -fno-stack-protector -c deb_subsystem.c -o deb_subsystem.o

# 3. ZIRHLI LİNKLEME (boot.o başta + linker.ld koruması)
gcc -m32 -T linker.ld -nostdlib -no-pie -o kernel.bin \
    boot.o \
    kernel.o \
    gui.o \
    exe_subsystem.o \
    ai_subsystem.o \
    mouse.o \
    keyboard.o \
    wind_subsystem.o \
    screen.o \
    idt.o \
    deb_subsystem.o

# 4. ISO Yapılandırma
mkdir -p isodir/boot/grub
cp kernel.bin isodir/boot/kernel.bin
cp grub.cfg isodir/boot/grub/grub.cfg
grub-mkrescue -o windos.iso isodir

echo "==> Tamamen AI Modüllerle Donatılmış windos.iso Hazır!"
