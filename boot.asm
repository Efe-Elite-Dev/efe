bits 32
section .text
        align 4
        dd 0x1BADB002              ; Sihirli sayı (Multiboot)
        dd 0x00000005              ; Flags (VBE Grafik modunu tetikler)
        dd - (0x1BADB002 + 0x00000005)

        ; VBE Grafik Modu Zorunlu İstekleri (800x600x32bpp LFB)
        dd 0, 0, 0, 0, 0
        dd 0                       ; Mode type (0 = LFB)
        dd 800                     ; Width
        dd 600                     ; Height
        dd 32                      ; BPP

global _start
extern kernel_main
extern keyboard_handler_c

_start:
        cli
        mov esp, stack_space
        push ebx                   ; Multiboot info yapısının adresini C'ye gönder
        call kernel_main

global keyboard_handler_asm
keyboard_handler_asm:
        pushad
        call keyboard_handler_c    ; kernel.c içindeki köprüyü tetikle
        popad
        iretd

section .bss
resb 8192
stack_space:

; Modern Linker (.note.GNU-stack) uyarısını kökten çözen kritik satır
section .note.GNU-stack noalloc noexec nowrite progbits
