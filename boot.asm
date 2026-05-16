; ====================================================================
; SKY-OS BOOT SCRIPT & INTERRUPT BRIDGE (GRAPHICS ENABLED)
; ====================================================================
MBOOT_PAGE_ALIGN    equ 1 << 0    
MBOOT_MEM_INFO      equ 1 << 1    
MBOOT_VIDEO_MODE    equ 1 << 2    ; Grafik modunu aktif eden sihirli bit!

MBOOT_HEADER_MAGIC  equ 0x1BADB002 
MBOOT_HEADER_FLAGS  equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO | MBOOT_VIDEO_MODE
MBOOT_CHECKSUM      equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

SECTION .multiboot
align 4
    dd MBOOT_HEADER_MAGIC
    dd MBOOT_HEADER_FLAGS
    dd MBOOT_CHECKSUM
    
    ; Multiboot Grafik Alanı (GRUB bu parametrelere göre ekran kartını açar)
    dd 0, 0, 0, 0, 0
    dd 0            ; 0 = Lineer Grafik Modu
    dd 800          ; Genişlik (Width)
    dd 600          ; Yükseklik (Height)
    dd 32           ; Renk Derinliği (BPP)

SECTION .text
global _start
global load_idt
global keyboard_handler_asm
extern kernel_main
extern keyboard_handler_c

_start:
    cli
    mov esp, stack_space
    
    ; GRUB, Multiboot bilgi yapısının adresini EBX register'ına koyar.
    ; Bunu kernel_main'e parametre olarak gönderiyoruz ki gerçek ekran adresini okuyabilelim!
    push ebx        
    call kernel_main
    jmp $

load_idt:
    mov eax, [esp + 4]
    lidt [eax]
    sti 
    ret

keyboard_handler_asm:
    pusha           
    call keyboard_handler_c 
    popa            
    iret            

SECTION .bss
align 16
stack_space:
    resb 8192
