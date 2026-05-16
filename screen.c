#include "wind_subsystem.h"
#include "screen.h"

void clear_text_screen(void) {
    /* VGA Metin Modu Belleğini Güvenli Temizleme */
    char* video_mem = (char*)0xB8000;
    for (int i = 0; i < 80 * 25 * 2; i += 2) {
        video_mem[i] = ' ';
        video_mem[i+1] = 0x07; /* Siyah Arka Plan, Beyaz Yazı */
    }
}
