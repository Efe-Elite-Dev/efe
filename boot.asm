MBOOT_PAGE_ALIGN    equ 1 << 0
MBOOT_MEM_INFO      equ 1 << 1
MBOOT_VIDEO_MODE    equ 1 << 2
MBOOT_HEADER_MAGIC  equ 0x1BADB002
MBOOT_HEADER_FLAGS  equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO | MBOOT_VIDEO_MODE
MBOOT_CHECKSUM      equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

section .multiboot
align 4
    dd MBOOT_HEADER_MAGIC
    dd MBOOT_HEADER_FLAGS
    dd MBOOT_CHECKSUM
    dd 0, 0, 0, 0, 0
    dd 0
    dd 800         ; Ekran Genişliği
    dd 600         ; Ekran Yüksekliği
    dd 32          ; Renk Derinliği (BPP)

section .text
global _start
global load_idt    ; IDT yükleyicisini dışarıya açıyoruz
extern kernel_main

_start:
    cli
    mov esp, stack_space
    push ebx       ; Multiboot info yapısını kernel_main'e parametre gönder
    call kernel_main
    jmp $

load_idt:
    mov edx, [esp + 4]  ; C kodundan gelen IDT pointer adresini al
    lidt [edx]          ; İşlemciye IDT'yi yükle
    ret                 ; C koduna geri dön

section .bss
align 16
stack_space:
    resb 8192      ; 8KB Kernel Stack Alanı
