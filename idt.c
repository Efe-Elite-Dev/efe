#include "sky_subsystem.h"

// x86 IDT Giriş Yapısı
struct idt_entry {
    uint16_t base_low;
    uint16_t sel;
    uint8_t  always0;
    uint8_t  flags;
    uint16_t base_high;
} __attribute__((packed));

// x86 IDT Pointer Yapısı
struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

struct idt_entry idt[256];
struct idt_ptr idtp;

// Harici klavye assembly köprüsü (Eğer bu da hata verirse bunu da içeri alabiliriz)
extern void keyboard_handler_asm(void);

/**
 * 🔒 IDT GİRİŞ KAPISI AYARLAYICI
 */
void idt_set_gate(uint8_t num, uint32_t base, uint16_t sel, uint8_t flags) {
    idt[num].base_low = (base & 0xFFFF);
    idt[num].base_high = ((base >> 16) & 0xFFFF);
    idt[num].sel = sel;
    idt[num].always0 = 0;
    idt[num].flags = flags;
}

/**
 * 🚀 IDT_LOAD FONKSİYONUNU C İÇİNE GÖMÜYORUZ (Linker Hatası Çözüldü!)
 */
void idt_load(uint32_t ptr_address) {
    asm volatile("lidt (%0)" : : "r"(ptr_address));
}

/**
 * 🛡️ BOŞ KESME KORUYUCU (DUMMY HANDLER)
 */
void default_exception_handler(void) {
    asm volatile (
        "movb $0x20, %al\n"
        "outb %al, $0x20\n"
        "outb %al, $0xA0\n"
    );
}

/**
 * 🐭 MOUSE_HANDLER_ASM FONKSİYONUNU İÇERİ ALIYORUZ (Linker Hatası Çözüldü!)
 * Fare kesmesi geldiğinde doğrudan C içindeki fare işleme fonksiyonunu tetikler.
 */
extern void mouse_handler(void); // mouse.c içindeki orijinal fonksiyon
__attribute__((naked)) void mouse_handler_asm(void) {
    asm volatile(
        "pushal\n"              // Tüm yazmaçları (registers) korumaya al
        "cld\n"                 // String yön flag'ini temizle
        "call mouse_handler\n"  // Asıl fare sürücüsünü çağır
        "popal\n"               // Yazmaçları geri yükle
        "iretl\n"               // Kesmeden güvenli bir şekilde dön
    );
}

/**
 * 🚀 KUSURSUZ INTERRUPT BAŞLATICI
 */
void init_idt(void) {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (uint32_t)&idt;

    // 256 kesmenin hepsini güvenli boş handler'a bağlıyoruz.
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, (uint32_t)default_exception_handler, 0x08, 0x8E);
    }

    // PIC IRQ haritalamasını güvenli bölgeye (0x20 ve 0x28) kaydırıyoruz
    asm volatile (
        "movb $0x11, %al\n" "outb %al, $0x20\n" "outb %al, $0xA0\n" 
        "movb $0x20, %al\n" "outb %al, $0x21\n"                    
        "movb $0x28, %al\n" "outb %al, $0xA1\n"                    
        "movb $0x04, %al\n" "outb %al, $0x21\n"                    
        "movb $0x02, %al\n" "outb %al, $0xA1\n"
        "movb $0x01, %al\n" "outb %al, $0x21\n" "outb %al, $0xA1\n" 
        "movb $0x00, %al\n" "outb %al, $0x21\n" "outb %al, $0xA1\n" 
    );

    // Sürücülerin kesme kapılarını bağlıyoruz
    idt_set_gate(33, (uint32_t)keyboard_handler_asm, 0x08, 0x8E); // IRQ1: Klavye
    idt_set_gate(44, (uint32_t)mouse_handler_asm, 0x08, 0x8E);    // IRQ12: Fare

    // Hazırladığımız zırhlı tabloyu işlemciye yüklüyoruz
    idt_load((uint32_t)&idtp);
    
    // Kesmeleri aktif et
    asm volatile("sti");
}
