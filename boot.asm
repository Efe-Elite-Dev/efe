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
global load_idt
global keyboard_handler_asm

extern kernel_main
extern keyboard_handler  ; keyboard.c veya idt.c içindeki C fonksiyonun

_start:
    cli                         ; Kesmeleri kapat
    mov esp, stack_space        ; Güvenli Stack alanını yükle

    ; === MULTIBOOT KÖPRÜSÜ ===
    push ebx                    ; GRUB multiboot_info adresini C çekirdeğine pasla
    
    call kernel_main            ; Çekirdeği başlat

.hang:
    hlt
    jmp .hang

; === IDT TABLOSUNU İŞLEMCİYE YÜKLEYEN ASKER ===
load_idt:
    mov edx, [esp + 4]          ; C'den gelen IDT pointer adresini al
    lidt [edx]                  ; IDT'yi CPU'ya yükle
    sti                         ; Kesmeleri tekrar aktif et
    ret

; === KLAVYE INTERRUPT KÖPRÜSÜ (IRQ 1) ===
keyboard_handler_asm:
    pusha                       ; Tüm genel amaçlı yazmaçları korumaya al
    call keyboard_handler       ; C dilindeki asıl klavye işleyicisini çağır
    popa                        ; Yazmaçları geri yükle
    iretd                       ; Kesme dönüşü yap (Interrupt Return)

section .bss
align 16
stack_bottom:
resb 16384                      ; Çekirdek için 16KB güvenli çalışma alanı
stack_space:
