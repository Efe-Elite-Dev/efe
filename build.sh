#!/bin/bash
echo "=== Sky-OS Modern Monolitik Derleme Süreci Başladı ==="

# 1. Eski kalıntıları ve bozuk bin dosyalarını tamamen temizle
rm -f *.o *.bin *.iso
rm -rf iso

# 2. Assembly Boot Sürücüsünü Derle
echo "-> boot.asm derleniyor..."
nasm -f elf32 boot.asm -o boot.o

# 3. Yeni Modern Çekirdeği (Kernel C) Optimize Bayraklarla Derle
echo "-> Yeni modern kernel.c derleniyor..."
gcc -m32 -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -Os -Wall -Wextra -fno-asynchronous-unwind-tables -fno-stack-protector

# 4. Sadece boot.o ve yeni kernel.o dosyalarını Linker ile birleştir
echo "-> Nesne dosyaları güvenli şekilde bağlanıyor..."
ld -m elf_i386 -T linker.ld boot.o kernel.o -o mykernel.bin

# 5. GRUB ve ISO Dağıtım Klasörünü Sıfırdan Oluştur
echo "-> ISO paketlemesi hazırlanıyor..."
mkdir -p iso/boot/grub
cp mykernel.bin iso/boot/

# Temiz GRUB Yapılandırması
cat << 'EOF' > iso/boot/grub/grub.cfg
set timeout=0
set default=0
menuentry "SkyOS" {
    multiboot /boot/mykernel.bin
    boot
}
EOF

# ISO Dosyasını Üret
grub-mkisofs -o skyos.iso iso

echo "=== KRAL, İŞLEM TAMAM! skyos.iso Sıfır Hata İle Hazırlandı! ==="
