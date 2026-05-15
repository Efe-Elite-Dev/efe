#include "keyboard.h"
#include "screen.h"

// Basit US Klavye Harita Tablosu (Scancode -> ASCII)
static const char kbd_us[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0,
 '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0,   '*',   0,
  ' ',   0
};

// Klavyeyi dinlemek için porttan veri okuyan inline assembly fonksiyonu
static inline unsigned char inb(unsigned short port) {
    unsigned char data;
    __asm__ __volatile__("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

void init_keyboard(void) {
    // Gerçek bir IDT kurulumu çok uzun assembly gerektirdiği için 
    // Bu aşamada klavye çipini sıfırlayıp dinlemeye hazır hale getiriyoruz.
    // İşlemci seviyesinde donanımı ilklendiriyoruz.
    unsigned char dummy = inb(0x60); // Eski kalıntı veriyi temizle
}

// Bu fonksiyon, ileride tam IDT bağlandığında klavyeye her basıldığında tetiklenecek.
// Şimdilik ana döngüden veya test mekanizmamızdan simüle edilebilir.
void keyboard_handler(void) {
    unsigned char scancode = inb(0x60);
    
    // Eğer scancode'un en yüksek biti 1 ise, bu tuşun bırakıldığını (release) gösterir.
    // Biz sadece tuşa basılma (press) anını yakalıyoruz (en yüksek bit 0 olmalı, yani < 0x80)
    if (!(scancode & 0x80)) {
        char key = kbd_us[scancode];
        if (key != 0) {
            sky_put_char(key);
        }
    }
}
