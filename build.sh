cat << 'EOF' > build.sh
#!/bin/bash
echo "=== Sky-OS Derleme İşlemi Başladı ==="

# Derleme adımları
nasm -f elf32 boot.asm -o boot.o
gcc -m32 -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
ld -m elf_i386 -T linker.ld boot.o kernel.o -o mykernel.bin

# ISO hazırlığı
cp mykernel.bin iso/boot/
grub-mkisofs -o skyos.iso iso

echo "=== skyos.iso Hazır! ==="
EOF
