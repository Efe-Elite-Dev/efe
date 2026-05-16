#ifndef SKY_SUBSYSTEM_H
#define SKY_SUBSYSTEM_H

#include <stdint.h>
#include "ai_subsystem.h" // Yapay zeka çekirdeğini buraya entegre ediyoruz

void gui_draw_rect(int start_x, int start_y, int width, int height, uint32_t color);

static inline void run_sky_subsystem(int win_x, int win_y, uint32_t current_tick) {
    // Derin Siber Mavi Arka Plan
    gui_draw_rect(win_x + 15, win_y + 45, 470, 290, 0x0F172A);
    
    // Sky-OS Yerel Altın Logosu ve Yapay Zeka Modu Başlığı Alanı
    gui_draw_rect(win_x + 35, win_y + 65, 30, 30, 0xFFB900);
    gui_draw_rect(win_x + 80, win_y + 75, 120, 12, 0x38BDF8); // AI Aktif Şeridi (Açık Mavi)

    // Yapay Zekaya Sahte Bir Girdi Hash'i gönderiyoruz (Örn: Zaman ilerledikçe niyeti analiz etsin)
    // Gerçekte burası klavyeden girilen stringin karması (hash) olacak aga
    uint32_t fake_input_hash = (current_tick / 100); 
    int current_intent = sky_ai_analyze_intent(fake_input_hash);

    // AI Grafik Motorunu Çalıştır!
    run_ai_core_render(win_x, win_y, current_tick, current_intent);
}

#endif
