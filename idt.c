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

// 256 adet kesme giriş vektörü
struct idt_entry idt[256];
struct idt_ptr idtp;

// Dışarıdaki assembly kesme köprüleri
extern void idt_load(uint32_t);
extern void keyboard_handler_asm(void);
extern void mouse_handler_asm(void);

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
 * 🛡️ BOŞ KESME KORUYUCU (DUMMY HANDLER)
 * İşlemci tanımlanmamış bir donanım sinyali alırsa Guru Meditation vermesin, 
 * bu güvenli boş fonksiyona düşüp yoluna devam etsin diye!
 */
void default_exception_handler(void) {
    // PIC'e (Interrupt Controller) sinyali aldığımızı bildirip CPU'yu rahatlatıyoruz
    asm volatile (
        "movb $0x20, %al\n"
        "outb %al, $0x20\n"
        "outb %al, $0xA0\n"
    );
}

/**
 * 🚀 KUSURSUZ INTERRUPT BAŞLATICI
 */
void init_idt(void) {
    idtp.limit = (sizeof(struct idt_entry) * 256) - 1;
    idtp.base = (uint32_t)&idt;

    // KORUMA: İlk önce 256 kesmenin hepsini güvenli boş handler'a bağlıyoruz.
    // Böylece tanımlanmamış en ufak sinyalde bile VirtualBox Triple Fault verip çökemez!
    for (int i = 0; i < 256; i++) {
        idt_set_gate(i, (uint32_t)default_exception_handler, 0x08, 0x8E);
    }

    // Donanımsal PIC IRQ haritalamasını güvenli bölgeye (0x20 ve 0x28) kaydırıyoruz
    asm volatile (
        "movb $0x11, %al\n" "outb %al, $0x20\n" "outb %al, $0xA0\n" // ICW1
        "movb $0x20, %al\n" "outb %al, $0x21\n"                    // ICW2: Master PIC -> 0x20
        "movb $0x28, %al\n" "outb %al, $0xA1\n"                    // ICW2: Slave PIC -> 0x28
        "movb $0x04, %al\n" "outb %al, $0x21\n"                    // ICW3
        "movb $0x02, %al\n" "outb %al, $0xA1\n"
        "movb $0x01, %al\n" "outb %al, $0x21\n" "outb %al, $0xA1\n" // ICW4
        "movb $0x00, %al\n" "outb %al, $0x21\n" "outb %al, $0xA1\n" // Maskeleri sıfırla, tüm kesmeleri aç
    );

    // Ana sürücülerin kesme kapılarını zırhlıyoruz
    idt_set_gate(33, (uint32_t)keyboard_handler_asm, 0x08, 0x8E); // IRQ1: Klavye
    idt_set_gate(44, (uint32_t)mouse_handler_asm, 0x08, 0x8E);    // IRQ12: Fare

    // Hazırladığımız bu zırhlı tabloyu işlemcinin içine yükle
    idt_load((uint32_t)&idtp);
    
    // İşlemci seviyesinde kesme sinyallerini (Interrupts) güvenli modda aktif et
    asm volatile("sti");
}
