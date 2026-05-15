MBALIGN  equ  1 << 0
MEMINFO  equ  1 << 1
FLAGS    equ  MBALIGN | MEMINFO
MAGIC    equ  0x1BADB002
CHECKSUM equ -(MAGIC + FLAGS)

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

section .bss
align 16
stack_bottom:
    resb 16384
stack_top:

section .text
global _start
extern kernel_main
global load_idt               ; idt.c buna bu isimle erişecek
global keyboard_handler_asm   ; idt.c buna bu isimle erişecek
extern keyboard_handler

_start:
    mov esp, stack_top
    push eax
    push ebx

    cli
    call kernel_main

.hang:
    hlt
    jmp .hang

; IDT tablosunu işlemciye bildiren fonksiyon
load_idt:
    mov edx, [esp + 4]
    lidt [edx]
    ret

; Klavye kesme köprüsü
keyboard_handler_asm:
    pushad
    cld
    call keyboard_handler
    popad
    iretd

; GNU-stack uyarısını susturmak için eklenen güvenli alt bilgi sektörü
section .note.GNU-stack noalloc noexec nowrite progbits
