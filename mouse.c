#include "mouse.h"

// Orijinal değişkenlerin (Hepsini koruyoruz)
int mouse_x = 400;
int mouse_y = 300;
int mouse_left_click = 0;
int mouse_right_click = 0;
int mouse_middle_click = 0;

static uint8_t mouse_cycle = 0;
static unsigned char mouse_packet[3];

// Dışarıdan veya alt sistemlerden gelen harici değişkenler
extern int setup_completed;

// Senin hazırladığın o efsanevi imleç matrisi (Aynen korunuyor)
const uint8_t mouse_pointer_sprite[19][12] = {
    {1,0,0,0,0,0,0,0,0,0,0,0}, {1,1,0,0,0,0,0,0,0,0,0,0}, {1,1,1,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,0,0,0,0,0,0,0,0}, {1,1,1,1,1,0,0,0,0,0,0,0}, {1,1,1,1,1,1,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,0,0,0,0,0}, {1,1,1,1,1,1,1,1,0,0,0,0}, {1,1,1,1,1,1,1,1,1,0,0,0},
    {1,1,1,1,1,1,1,1,1,1,0,0}, {1,1,1,1,1,1,1,1,1,1,1,0}, {1,1,1,1,1,1,1,0,0,0,0,0},
    {1,1,1,0,1,1,1,1,0,0,0,0}, {1,1,0,0,1,1,1,1,0,0,0,0}, {1,0,0,0,0,1,1,1,1,0,0,0},
    {0,0,0,0,0,1,1,1,1,0,0,0}, {0,0,0,0,0,0,1,1,1,0,0,0}, {0,0,0,0,0,0,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0}
};

void init_mouse(void) {
    outb(0x64, 0xA8); while(inb(0x64) & 0x02);
    outb(0x64, 0xD4); while(inb(0x64) & 0x02);
    outb(0x60, 0xF4); while(inb(0x64) & 0x02);
    inb(0x60);
}

/**
 * 🐭 INTERRUPT VE LINKER KÖPRÜSÜ
 * idt.c içindeki assembly kesmesi burayı çağıracak, burası da senin polling mantığını tetikleyecek!
 */
void mouse_handler(void) {
    handle_mouse_polling();
}

void handle_mouse_polling(void) {
    uint8_t status = inb(0x64);
    if ((status & 0x01) && (status & 0x20)) {
        unsigned char current_byte = inb(0x60);
        if (mouse_cycle == 0 && !(current_byte & 0x08)) return; // MOUSE_BIT_SYNC maskesi
        
        mouse_packet[mouse_cycle] = current_byte;
        mouse_cycle++;

        if (mouse_cycle == 3) {
            mouse_cycle = 0;
            if ((mouse_packet[0] & 0x40) || (mouse_packet[0] & 0x80)) return; // Overflow maskeleri

            mouse_left_click   = mouse_packet[0] & 1;
            mouse_right_click  = mouse_packet[0] & 2;
            mouse_middle_click = mouse_packet[0] & 4;

            mouse_x += (char)mouse_packet[1];
            mouse_y -= (char)mouse_packet[2];

            // Sınır kontrolleri (Genişlik: 800, Yükseklik: 600)
            if (mouse_x < 0) mouse_x = 0;
            if (mouse_x > 800 - 12) mouse_x = 800 - 12;
            if (mouse_y < 0) mouse_y = 0;
            if (mouse_y > 600 - 19) mouse_y = 600 - 19;

            if (mouse_left_click) {
                setup_completed = 1; // Kurulumu tamamlama siber tetikleyicisi
            }
        }
    }
}
