#include "sky_subsystem.h"
#include "gui.h"

// kernel.c'den gelen gerçek donanım adres köprüleri
extern uint32_t* vbe_vram;
extern uint32_t  vbe_pitch;

void gui_refresh_desktop(void) {
    if (vbe_vram == 0) {
        return;
    }

    // Sanal makinenin padding (tampon) piksellerini hesaba katan milimetrik genişlik
    uint32_t width_pixels = vbe_pitch / 4; 
    uint32_t screen_width = 800;
    uint32_t screen_height = 600;

    // Tüm ekranı şık bir Gece Mavisine boyuyoruz (Çizgileri ezip geçen formül!)
    for (uint32_t y = 0; y < screen_height; y++) {
        for (uint32_t x = 0; x < screen_width; x++) {
            uint32_t pixel_index = (y * width_pixels) + x;
            vbe_vram[pixel_index] = 0x1A1A2E; 
        }
    }

    // Ekranın tam ortasına siber imzamızı (Saf Beyaz Kare) atıyoruz
    for (uint32_t y = 250; y < 350; y++) {
        for (uint32_t x = 350; x < 450; x++) {
            uint32_t pixel_index = (y * width_pixels) + x;
            vbe_vram[pixel_index] = 0xFFFFFF; 
        }
    }
}
