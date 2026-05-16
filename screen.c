#include "wind_subsystem.h"
#include "mouse.h"

void init_mouse(void) {
    /* Fare Kontrolcüsünü Uyandırma Komutları */
    outb(0x64, 0xA8);
    outb(0x64, 0x20);
    uint8_t status = inb(0x60) | 2;
    outb(0x64, 0x60);
    outb(0x60, status);
}

void handle_mouse_polling(void) {
    /* Fare Tampon Temizliği (Polled Okuma) */
    if ((inb(0x64) & 1) == 1) {
        inb(0x60); 
    }
}
