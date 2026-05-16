; =============================================================================
;  Wind OS Saf Çekirdek Önyükleyici Tanımı (VBE Grafik Entegrasyonlu Sürüm)
; =============================================================================

MULTIBOOT_PAGE_ALIGN    equ 1 << 0  ; Sayfa hizalama kurallarını aktif et
MULTIBOOT_MEMORY_INFO   equ 1 << 1  ; GRUB'dan bellek haritası bilgilerini iste
MULTIBOOT_VIDEO_MODE    equ 1 << 2  ; VBE doğrusal grafik modunu aktif et

MULTIBOOT_HEADER_MAGIC  equ 0x1BADB002  ; GRUB'ın tanıyacağı asil sihirli numara
MULTIBOOT_HEADER_FLAGS  equ MULTIBOOT_PAGE_ALIGN | MULTIBOOT_MEMORY_INFO | MULTIBOOT_VIDEO_MODE
MULTIBOOT_CHECKSUM      equ -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)

section .multiboot
align 4
    dd MULTIBOOT_HEADER_MAGIC
    dd MULTIBOOT_HEADER_FLAGS
    dd MULTIBOOT_CHECKSUM
    
    ; Multiboot standardı gereği adres alanları (AOUT kludge) sıfırlanmalıdır
    dd 0
    dd 0
    dd 0
    dd 0
    dd 0
    
    ; Ekran Kartı Çözünürlük ve Donanım Modu Kurulumu (VBE)
    dd 0        ; Mod tipi (0: Doğrusal Grafik / Linear Framebuffer)
    dd 800      ; Ekran Genişliği (Piksel cinsinden)
    dd 600      ; Ekran Yüksekliği (Piksel cinsinden)
    dd 32       ; BPP (Renk Derinliği - Piksel başına 4 byte ARGB)

section .bootstrap_stack, nobits
align 16
stack_bottom:
    resb 16384  ; Çekirdek alt sistemleri için 16 KB'lık saf RAM yığın alanı ayırıyoruz
stack_top:

section .text
global _start
extern kernel_main

_start:
    ; İşlemcinin yığın imlecini (Stack Pointer) ayırdığımız RAM alanının tepesine taşı
    mov esp, stack_top
    
    ; CRITICAL BRIDGE: GRUB'ın EBX kaydına yazdığı multiboot_info adresini yığına it.
    ; Bu sayede kernel_main(struct multiboot_info* mboot) fonksiyonu bu adresi parametre alabilir.
    push ebx    
    
    ; Donanımsal kesmeleri kapat ve Wind OS modern C çekirdeğine zıpla!
    cli         
    call kernel_main

.hang:
    ; Çekirdek bir şekilde geri dönerse işlemciyi sonsuz kilitte tut, çökmesini engelle
    hlt
    jmp .hang
