#include <stdint.h>

// ====================================================================
// 1. IDT YAPILARI VE TANIMLAMALARI
// ====================================================================
#define IDT_MAX_DESCRIPTORS 256

// Her bir IDT girdisinin (Gate Descriptor) mimari yapısı
struct idt_entry {
    uint16_t isr_low;   // ISR (Interrupt Service Routine) adresinin alt 16 biti
    uint16_t kernel_cs; // Kernel Code Segment seçici (GDT'den gelir, genelde 0x08)
    uint8_t  reserved;  // Boş bırakılan alan (Her zaman 0 olmak zorunda)
    uint8_t  attributes;// Giriş tipi ve izinler (Örn: 0x8E - Interrupt Gate, Ring 0)
    uint16_t isr_high;  // ISR adresinin üst 16 biti
} __attribute__((packed));

// İşlemcinin IDTR kaydına (Register) yükleyeceği işaretçi yapısı
struct idt_ptr {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed));

// 256 adet kesme için IDT tablosu dizisi
__attribute__((aligned(0x10))) 
static struct idt_entry idt[IDT_MAX_DESCRIPTORS];
static struct idt_ptr   idtr;

// ====================================================================
// 2. ASSEMBLY KÖPRÜLERİ (EXTERN REFERANSLAR)
// ====================================================================
// Linker'ın (ld) bulamadığı o meşhur iki fonksiyonu extern ile buraya bağlıyoruz.
// Bu fonksiyonlar boot.asm içinde tanımlı ve işlemci seviyesinde çalışıyorlar.
extern void keyboard_handler_asm(void); // Assembly'deki klavye interrupt tetikleyicisi
extern void load_idt(uint32_t idt_ptr_addr); // LIDT komutunu çalıştıran ASM fonksiyonu

// ====================================================================
// 3. IDT KAPISI AYARLAMA FONKSİYONU
// ====================================================================
void idt_set_descriptor(uint8_t vector, void (*isr)(void), uint8_t flags) {
    struct idt_entry *descriptor = &idt[vector];

    descriptor->isr_low   = (uint32_t)isr & 0xFFFF;
    descriptor->kernel_cs = 0x08; // Çekirdek kod segmenti (Standart x86 Kernel)
    descriptor->reserved  = 0;
    descriptor->attributes= flags;
    descriptor->isr_high  = ((uint32_t)isr >> 16) & 0xFFFF;
}

// ====================================================================
// 4. KERNEL KLAVYE KESME İŞLEYİCİSİ (C KATMANI)
// ====================================================================
// Assembly'deki 'keyboard_handler_asm' kesmeyi yakalayıp işi bitirince 
// doğrudan bu C fonksiyonunu çağırır.
void keyboard_handler_c(void) {
    // PIC (Programmable Interrupt Controller) çipine kesmeyi aldığımızı bildiriyoruz (EOI)
    // Bunu yapmazsak klavye sadece 1 kere basar, bir daha asla çalışmaz!
    uint8_t status = 0;
    
    // Klavyeden gelen scancode verisini oku (Port 0x60)
    __asm__ __volatile__("inb $0x60, %0" : "=a"(status));
    
    // PIC master çipine End of Interrupt (EOI) sinyali gönder (Port 0x20)
    __asm__ __volatile__("outb %%al, $0x20" : : "a"(0x20));
}

// ====================================================================
// 5. IDT İNİTİALİZASYON ANA FONKSİYONU
// ====================================================================
void init_idt(void) {
    // 1. Tüm IDT tablosunu sıfırlayarak temizle
    for (int i = 0; i < IDT_MAX_DESCRIPTORS; i++) {
        idt_set_descriptor(i, 0, 0);
    }

    // 2. Klavye Kesmesini Kaydet (IRQ 1 -> x86 Mimaride Kesme 33'e (0x21) denk gelir)
    // Flag olarak 0x8E veriyoruz: Present=1, DPL=00 (Ring 0), Type=1110 (32-bit Interrupt Gate)
    idt_set_descriptor(33, keyboard_handler_asm, 0x8E);

    // 3. IDT İşaretçisini (IDTR yapısını) doldur
    idtr.limit = (uint16_t)(sizeof(struct idt_entry) * IDT_MAX_DESCRIPTORS) - 1;
    idtr.base  = (uint32_t)&idt;

    // 4. Hazırladığımız IDT tablosunu Assembly fonksiyonu üzerinden işlemciye yükle!
    load_idt((uint32_t)&idtr);
}
