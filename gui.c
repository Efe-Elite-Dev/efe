#include "wind_subsystem.h"
#include "gui.h"

extern uint32_t* vbe_vram;
extern uint32_t  vbe_pitch;

/* Guru Meditation Bellek Taşma Kalkanlı Piksel Çizim Motoru */
void gui_draw_rect(int x, int y, int width, int height, uint32_t color) {
    if (vbe_vram == 0 || vbe_pitch == 0) return;
    
    uint32_t width_pixels = vbe_pitch / 4;

    for (int curr_y = y; curr_y < y + height; curr_y++) {
        if (curr_y < 0 || curr_y >= 600) continue; /* 800x600 Ekran Sınırı */
        
        for (int curr_x = x; curr_x < x + width; curr_x++) {
            if (curr_x < 0 || curr_x >= 800) continue;

            uint32_t pixel_index = (curr_y * width_pixels) + curr_x;
            if (pixel_index < (800 * 600)) {
                vbe_vram[pixel_index] = color;
            }
        }
    }
}

void gui_refresh_desktop(void) {
    if (vbe_vram == 0) return;
    
    /* Gece Mavisi Fütüristik Arka Plan */
    gui_draw_rect(0, 0, 800, 600, 0x0A0B10);
    
    /* Wind OS Tasarım Çizgileri */
    gui_draw_rect(100, 100, 600, 2, 0xFFFFFF);
    gui_draw_rect(700, 100, 2, 400, 0xFFFFFF);
}
