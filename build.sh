cat << 'EOF' > build.sh
#!/bin/bash
echo "=== Sky-OS Derleme İşlemi Başladı ==="

# 1. Assembly Bootloader'ı derle
nasm -f elf32 boot.asm -o boot.o

# 2. Tüm C modüllerini tek tek nesne dosyalarına (.o) çevir
gcc -m32 -c kernel.c -o kernel.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra
gcc -m32 -c gui.c -o gui.o -std=gnu99 -ffreestanding -O2 -Wall -Wextra

# 3. Linker ile hepsini tek bir pürüzsüz binary dosyada birleştir (Çıktıyı SkyOS.bin yapıyoruz)
ld -m elf_i386 -T linker.ld boot.o kernel.o gui.o -o SkyOS.bin

# 4. ISO hazırlığı için klasör kontrolü yap ve kopyala
mkdir -p iso/boot/grub
cp SkyOS.bin iso/boot/

# 5. Bootable ISO dosyasını oluştur
grub-mkisofs -o skyos.iso iso

echo "=== skyos.iso Başarıyla Hazırlandı! ==="
EOF
