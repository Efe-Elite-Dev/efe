; ====================================================================
; SKY-OS BOOT SCRIPT & INTERRUPT BRIDGE
; ====================================================================
MBOOT_PAGE_ALIGN    equ 1 << 0    
MBOOT_MEM_INFO      equ 1 << 1    
MBOOT_HEADER_MAGIC  equ 0x1BADB002 
MBOOT_HEADER_FLAGS  equ MBOOT_PAGE_ALIGN | MBOOT_MEM_INFO
MBOOT_CHECKSUM      equ -(MBOOT_HEADER_MAGIC + MBOOT_HEADER_FLAGS)

SECTION .multiboot
align 4
    dd MBOOT_HEADER_MAGIC
    dd MBOOT_HEADER_FLAGS
    dd MBOOT_CHECKSUM

SECTION .text
; Linker'ın (ld) idt.o içinde arayıp bulamadığı fonksiyonları dışarı açıyoruz
global _start
global load_idt
global keyboard_handler_asm
extern kernel_main
extern keyboard_handler_c ; idt.c içindeki C fonksiyonunu çağıracağız

_start:
    cli
    mov esp, stack_space
    call kernel_main
    jmp $

; IDT Tablosunu işlemciye (IDTR) yükleyen sihirli fonksiyon
load_idt:
    mov eax, [esp + 4]
    lidt [eax]
    sti ; Kesmeleri aktif et!
    ret

; Klavye tuşuna basıldığında donanımın ilk tetiklediği ASM köprüsü
keyboard_handler_asm:
    pusha           ; Tüm genel amaçlı yazmaçları korumaya al
    call keyboard_handler_c ; idt.c içindeki gerçek C koduna zıpla
    popa            ; Yazmaçları geri yükle
    iret            ; Interrupt modundan güvenle çık

SECTION .bss
align 16
stack_space:
    resb 8192 ; 8KB Kernel Stack Alanı
