#include "wind_subsystem.h"

#define MOUSE_DATA_PORT 0x60
#define MOUSE_STATUS_PORT 0x64

extern int setup_stage; 

static int mouse_cycle = 0;
static unsigned char mouse_packet[3];

/* FARE AI İÇ DEĞİŞKENLERİ */
static int mouse_click_density = 0;
static int mouse_ai_stress_score = 0;

void init_mouse(void) {
    outb(0x64, 0xA8);
    outb(0x64, 0x20);
    uint8_t status = inb(0x60) | 2; 
    outb(0x64, 0x60);
    outb(0x60, status);
    outb(0x64, 0xD4);
    outb(0x60, 0xF4);
    inb(0x60); 
}

/* FARE ODASI YAPAY ZEKASI: Tıklama hızını tarayarak donanımsal stres seviyesi tahmin eder */
int ai_mouse_analyze_stress(void) {
    if (mouse_click_density > 10) {
        mouse_ai_stress_score = 3; /* Kullanıcı hızlı hızlı tıklıyor, sistemi hızlandır */
    } else if (mouse_click_density > 3) {
        mouse_ai_stress_score = 2; /* Normal etkileşim */
    } else {
        mouse_ai_stress_score = 1; /* Boşta veya kararsız */
    }
    return mouse_ai_stress_score;
}

void handle_mouse_polling(void) {
    static int click_lock = 0;

    if (inb(MOUSE_STATUS_PORT) & 1) {
        if (inb(MOUSE_STATUS_PORT) & 0x20) {
            unsigned char current_byte = inb(MOUSE_DATA_PORT);
            mouse_packet[mouse_cycle] = current_byte;
            mouse_cycle++;

            if (mouse_cycle == 3) {
                mouse_cycle = 0; 
                int left_click = mouse_packet[0] & 1;

                if (left_click) {
                    /* AI Sayaç beslemesi */
                    mouse_click_density += 4;
                    
                    if (click_lock == 0) {
                        setup_stage++;
                        if (setup_stage > 2) setup_stage = 0;
                        click_lock = 1;
                    }
                } else {
                    click_lock = 0;
                    if (mouse_click_density > 0) mouse_click_density--; /* Sönümlendirme */
                }
            }
        }
    }
}
