MULTIBOOT_PAGE_ALIGN    equ 1 << 0
MULTIBOOT_MEMORY_INFO   equ 1 << 1
MULTIBOOT_VIDEO_MODE    equ 1 << 2
MULTIBOOT_HEADER_MAGIC  equ 0x1BADB002
MULTIBOOT_HEADER_FLAGS  equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_VIDEO_MODE
MULTIBOOT_CHECKSUM      equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

section .multiboot
align 4
    dd MULTIBOOT_HEADER_MAGIC
    dd MULTIBOOT_HEADER_FLAGS
    dd MULTIBOOT_CHECKSUM
    
    ; Grafik alanları (Metin dışı modlarda sıfırlanmalıdır)
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0
    
    ; VBE Ekran Kartı Mod Tanımlamaları
    dd 0        ; Çözünürlük modu (Doğrusal Framebuffer)
    dd 800      ; Genişlik
    dd 600      ; Yükseklik
    dd 32       ; Renk Derinliği (BPP)

section .bootstrap_stack, nobits
align 16
stack_bottom:
    resb 16384  ; Çekirdek alt sistemleri için 16 KB RAM alanı
stack_top:

section .text
global _start
extern kernel_main

_start:
    ; Yığın imlecini ayarla
    mov esp, stack_top
    
    ; GRUB multiboot pointer adresini C'ye gönder
    push ebx    
    
    ; Kesmeleri kapat ve ana motoru ateşle
    cli         
    call kernel_main

.hang:
    hlt
    jmp .hang
