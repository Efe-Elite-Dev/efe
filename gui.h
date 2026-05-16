#include "sky_subsystem.h"

// kernel.c içinden gelen gerçek donanım adres köprüleri
extern uint32_t* vbe_vram;
extern uint32_t  vbe_pitch;

void gui_refresh_desktop(void) {
    // KORUMA: Eğer kernel henüz VRAM adresini yakalayamadıysa çizim yapıp sistemi kilitleme
    if (vbe_vram == 0) {
        return;
    }

    // VirtualBox için milimetrik satır genişliği hesabı (uint32_t cinsinden pitch)
    // Donanımsal pitch byte cinsindendir, 4 byte'a (32-bit) bölerek piksel adedini buluyoruz
    uint32_t width_pixels = vbe_pitch / 4; 

    // Çözünürlüğü garantiye alalım (800x600 standart multiboot modu)
    uint32_t screen_width = 800;
    uint32_t screen_height = 600;

    // TÜM EKRANI TEMİZ BİR SKI BLUE / MOR RENGE BOYAMA DÖNGÜSÜ
    for (uint32_t y = 0; y < screen_height; y++) {
        for (uint32_t x = 0; x < screen_width; x++) {
            
            // === KRİTİK HİZALAMA FORMÜLÜ ===
            // Çizgilerin üst üste binmesini engelleyen asıl siber formül budur!
            uint32_t pixel_index = (y * width_pixels) + x;
            
            // Şık bir işletim sistemi arka plan rengi (Örn: Derin Gece Mavisi/Mor -> 0x1A1A2E)
            vbe_vram[pixel_index] = 0x1A1A2E; 
        }
    }

    // === ÖRNEK MASAÜSTÜ LOGOSU VEYA PENCERESİ ===
    // Ekranın ortasına minik, temiz beyaz bir siber kare çizelim ki çalıştığını anlayalım
    for (uint32_t y = 250; y < 350; y++) {
        for (uint32_t x = 350; x < 450; x++) {
            uint32_t pixel_index = (y * width_pixels) + x;
            vbe_vram[pixel_index] = 0xFFFFFF; // Saf Beyaz
        }
    }
}
