#include "sky_subsystem.h"
#include "mouse.h"

// Dışarıdaki alt sistem fonksiyonları
extern void gui_refresh_desktop(void);
extern int ai_predict_hardware_load(int mouse_delta_x, int loop_count);

// === DONANIM ADRES KÖPRÜLERİ ===
// sky_subsystem.c içindeki asıl değişkenlere extern köprüsü kuruyoruz
extern uint32_t* vbe_vram; 
extern uint32_t  vbe_pitch;

// İşlemciyi uyutan donanımsal gecikme fonksiyonu
static inline void io_wait(void) {
    asm volatile ("outb %%al, $0x80" : : "a"(0));
}

void kernel_main(struct multiboot_info* mboot) {
    // KORUMA KATMANI: Multiboot yapısı var mı ve GRUB grafik modu adresini pasladı mı?
    if (mboot != 0) {
        if (mboot->framebuffer_addr != 0) {
            vbe_vram = (uint32_t*)(uintptr_t)mboot->framebuffer_addr;
            vbe_pitch = mboot->framebuffer_pitch;
        }
    }

    // Donanım Sürücüleri Başlatılıyor
    init_mouse();
    gui_refresh_desktop();

    uint32_t refresh_counter = 0;
    int current_wait_cycles = 30; // Başlangıç döngü değeri

    // === YAPAY ZEKA DESTEKLİ SİBER DÖNGÜ ===
    while (1) {
        handle_mouse_polling();
        
        refresh_counter++;
        if (refresh_counter >= 150) {
            gui_refresh_desktop();
            refresh_counter = 0;
        }
        
        // Yapay zeka farenin hareket hızı ve döngü sayısına bakıp yükü tahmin ediyor
        int system_stress = ai_predict_hardware_load(10, refresh_counter);
        
        if (system_stress == 1) {
            current_wait_cycles = 60; // Yapay Zeka Kararı: Donanımı koru, es ver
        } else {
            current_wait_cycles = 15; // Yapay Zeka Kararı: Donanım rahat, performansı kökle
        }

        // Yapay zekanın dinamik olarak belirlediği kararlılık gecikmesi
        for (volatile int i = 0; i < current_wait_cycles; i++) {
            io_wait();
        }
    }
}
