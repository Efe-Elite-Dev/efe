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
    
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0
    
    dd 0        
    dd 800      
    dd 600      
    dd 32       

section .bootstrap_stack, nobits
align 16
stack_bottom:
    resb 16384  ; 16 KB temiz yığın alanı
stack_top:

section .text
global _start
extern kernel_main

_start:
    mov esp, stack_top
    push ebx    
    cli         
    call kernel_main

.hang:
    hlt
    jmp .hang
