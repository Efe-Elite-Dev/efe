#!/bin/bash
echo "==> Wind OS (Full AI-Core Edition) Temizlik ve Derleme Başlatıldı..."

# 1. Eski önbellek ve ISO klasör yapılarını kökten süpür
rm -rf isodir
rm -f *.o kernel.bin windos.iso

# Temiz dizin ağacını baştan oluştur
mkdir -p isodir/boot/grub

# 2. Önyükleyiciyi (boot.asm) Derle
nasm -f elf32 boot.asm -o boot.o

# 3. Bağımsız Yapay Zekalı Çekirdek Odalarını Derle
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

# 4. MUTLAK ZIRHLI LİNKLEME (linker.ld kuralı aktif ve boot.o her şeyin başında olmak zorunda)
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

# 5. İkili Dosyaları ISO İskeletine Güvenli Şekilde Dağıt
cp kernel.bin isodir/boot/kernel.bin
cp grub.cfg isodir/boot/grub/grub.cfg

# 6. GRUB Kurtarma Aracıyla Önyüklenebilir Canavar ISO'yu Çıkart
grub-mkrescue -o windos.iso isodir

echo "==> İşlem başarıyla tamamlandı aga! windos.iso VirtualBox için hazır."
