#include "wind_subsystem.h"
#include "gui.h"
#include "exe_subsystem.h"

int setup_stage = 0;
int selected_index = 4; /* Türkiye varsayılan */

void run_exe_subsystem(void) {
    /* OOBE Kurulum Penceresi Çerçevesi */
    gui_draw_rect(150, 100, 500, 400, 0xDCDCDC); 
    gui_draw_rect(150, 100, 500, 30, 0x1E3799); /* Wind OS Şeridi */

    /* 3D Derinlik Gölgeleri */
    gui_draw_rect(150, 495, 500, 5, 0x888888);
    gui_draw_rect(645, 100, 5, 400, 0x888888);

    if (setup_stage == 0) {
        /* BÖLGE SEÇİMİ (Türkiye) */
        gui_draw_rect(180, 150, 440, 200, 0xFFFFFF);
        for(int i = 0; i < 5; i++) {
            uint32_t list_color = (i == selected_index) ? 0x4A69BD : 0xFFFFFF;
            gui_draw_rect(200, 165 + (i * 32), 400, 25, list_color);
        }
        gui_draw_rect(540, 440, 80, 30, 0x1E3799); /* Evet Butonu */
    } 
    else if (setup_stage == 1) {
        /* WI-FI BAĞLANTISI */
        gui_draw_rect(180, 150, 440, 200, 0xFFFFFF);
        gui_draw_rect(200, 220, 400, 30, 0xEEF2F3); 
        gui_draw_rect(520, 440, 100, 30, 0x1E3799); /* İleri Butonu */
    }
    else if (setup_stage == 2) {
        /* KULLANICI ADI OLUŞTURMA */
        gui_draw_rect(180, 150, 440, 200, 0xFFFFFF);
        gui_draw_rect(200, 230, 400, 30, 0xEEF2F3); 
        gui_draw_rect(200, 230, 2, 30, 0x1E3799);  /* Yanıp Sönen İmleç */
        gui_draw_rect(520, 440, 100, 30, 0x218C74); /* Kabul Et Butonu */
    }
}
