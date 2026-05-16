# === 1. Derleyici ve Bağlayıcı Tanımlamaları ===
CC = gcc
AS = nasm
LD = ld

# === 2. Derleme Bayrakları (Flags) ===
CFLAGS = -m32 -std=gnu99 -ffreestanding -O2 -Wall -Wextra -fno-exceptions -fno-stack-protector -fno-pie
LDFLAGS = -m elf_i386 -T linker.ld

# === 3. Projedeki Nesne Dosyaları ===
OBJS = boot.o kernel.o mouse.o

# === 4. Ana Hedef ===
all: myos.bin

# === 5. Bağlama (Linking) Kuralı ===
myos.bin: $(OBJS)
	$(LD) $(LDFLAGS) -o myos.bin $(OBJS)

# === 6. Assembly Derleme Kuralı ===
boot.o: boot.asm
	$(AS) -f elf32 boot.asm -o boot.o

# === 7. C Kodlarını Derleme Kuralları ===
kernel.o: kernel.c mouse.h
	$(CC) $(CFLAGS) -c kernel.c -o kernel.o

mouse.o: mouse.c mouse.h
	$(CC) $(CFLAGS) -c mouse.c -o mouse.o

# === 8. Temizlik Kuralı ===
clean:
	rm -f *.o myos.bin
