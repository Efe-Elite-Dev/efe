bits 32

; Multiboot Makroları
MODULEALIGN equ 1 << 0
MEMINFO     equ 1 << 1
GRAPHICS    equ 1 << 2  ; GRUB'a VBE Grafik Modunu zorla açtırır
FLAGS       equ MODULEALIGN | MEMINFO | GRAPHICS
MAGIC       equ 0x1BADB002
CHECKSUM    equ -(MAGIC + FLAGS)

section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM
    
    ; Grafik ayarları için GRUB dolgu alanları
    dd 0, 0, 0, 0, 0
    dd 0        ; 0 = Lineer Grafik (LFB) Modu
    dd 800      ; Ekran Genişliği
    dd 600      ; Ekran Yüksekligi
    dd 32       ; Renk Derinliği (BPP)

section .text
global _start
extern kernel_main

_start:
    cli                         ; Kesmeleri kapat
    mov esp, stack_space        ; Güvenli Stack alanını yükle

    ; === KRİTİK KÖPRÜ HİZALAMASI ===
    ; GRUB'ın hazırladığı multiboot_info adresini (ebx) Stack'e atıyoruz.
    ; Böylece kernel.c içindeki ilk parametre (mboot) tam bu adresi yakalayacak.
    push ebx                    
    
    call kernel_main            ; Çekirdeği başlat

.hang:
    hlt
    jmp .hang

section .bss
align 16
stack_bottom:
resb 16384                      ; Çekirdek için 16KB güvenli çalışma alanı
stack_space:
