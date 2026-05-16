#include "sky_subsystem.h"
#include "gui.h"

// kernel.c veya sky_subsystem.c içinden gelen gerçek donanım adres köprüleri
extern uint32_t* vbe_vram;
extern uint32_t  vbe_pitch;

/**
 * 🎨 GENEL DİKDÖRTGEN ÇİZME MOTORU
 * exe_subsystem.c'nin pencereleri çizmek için aradığı ve Linker'ın hata verdiği asıl fonksiyon!
 */
void gui_draw_rect(int x, int y, int width, int height, uint32_t color) {
    // KORUMA KATMANI: Eğer VRAM adresi henüz yüklenmediyse çizim yapıp sistemi çökertme
    if (vbe_vram == 0) {
        return;
    }

    // Sanal makinenin padding (tampon) piksellerini hesaba katan milimetrik genişlik hizalaması
    uint32_t width_pixels = vbe_pitch / 4;

    for (int curr_y = y; curr_y < y + height; curr_y++) {
        // EKRAN SINIR KORUMASI: Sınırların dışına taşarsa es geç (Buffer Overflow Önleyici)
        if (curr_y < 0 || curr_y >= 600) continue;

        for (int curr_x = x; curr_x < x + width; curr_x++) {
            if (curr_x < 0 || curr_x >= 800) continue;

            // Çizgilerin kaymasını ve mor parazitleri engelleyen dinamik indeksleme formülü
            uint32_t pixel_index = (curr_y * width_pixels) + curr_x;
            vbe_vram[pixel_index] = color;
        }
    }
}

/**
 * 🚀 MASAÜSTÜ YENİLEME MOTORU
 * Kernel'ın döngü içinde ekranı tazelemek için çağırdığı ana grafik fonksiyonu.
 */
void gui_refresh_desktop(void) {
    if (vbe_vram == 0) {
        return;
    }

    // 1. Tüm ekranı şık bir Gece Mavisine boyayarak mor/dikey çizgileri yok ediyoruz
    gui_draw_rect(0, 0, 800, 600, 0x1A1A2E);

    // 2. Ekranın tam ortasına ilk işletim sistemi amblemimizi konduruyoruz (Saf Beyaz Kare)
    gui_draw_rect(350, 250, 100, 100, 0xFFFFFF);
}
