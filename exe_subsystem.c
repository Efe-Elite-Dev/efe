#include "wind_subsystem.h"

/* kernel.c içindeki küresel ekran göstergeleri */
extern uint32_t* vbe_vram;
extern uint32_t  vbe_pitch;

/* OOBE (İlk Kurulum) Aşama Sayacı */
int setup_stage = 0;

/* Yapay zeka modüllerinin durumuna göre ekrana basılacak AI metin durumları */
extern int central_ai_prediction_level;

/* VBE Grafik Modunda Belirtilen Koordinata Saf Renk Basan Piksel Motoru */
void draw_pixel_pure(int x, int y, uint32_t color) {
    if (x >= 0 && x < 800 && y >= 0 && y < 600) {
        /* Genişlik 800 piksel olduğu için her satır (pitch) baz alınarak piksel adresi bulunur */
        vbe_vram[y * (vbe_pitch / 4) + x] = color;
    }
}

/* Belirtilen Alanı Saf Renkle Dolduran Dikdörtgen Motoru (Dolgu) */
void draw_filled_rect(int x, int y, int width, int height, uint32_t color) {
    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            draw_pixel_pure(x + j, y + i, color);
        }
    }
}

/* Pencere Kenarlıkları İçin İçi Boş Çizgi Çerçeve Motoru */
void draw_rect_outline(int x, int y, int width, int height, uint32_t color) {
    /* Üst ve Alt Çizgiler */
    for (int i = 0; i < width; i++) {
        draw_pixel_pure(x + i, y, color);
        draw_pixel_pure(x + i, y + height - 1, color);
    }
    /* Sol ve Sağ Çizgiler */
    for (int i = 0; i < height; i++) {
        draw_pixel_pure(x, y + i, color);
        draw_pixel_pure(x + width - 1, y + i, color);
    }
}

/* WIND OS MODERN OOBE PENCERE TASARIMCISI */
void draw_custom_window(int x, int y, int width, int height, const char* title, uint32_t body_color) {
    /* 1. Hayalet izleri önlemek için pencerenin oturacağı tüm arka planı temizle */
    draw_filled_rect(x, y, width, height, body_color);

    /* 2. Pencere Üst Barı (Mavi Başlık Çubuğu - Yüksekliği 24 piksel olarak sabitlendi) */
    draw_filled_rect(x, y, width, 24, 0x001A4473); // Derin Wind OS Mavisi

    /* 3. Pencere Dış İnce Kontur Çizgisi (Görsel kaymaları sıfırlayan milimetrik kenarlık) */
    draw_rect_outline(x, y, width, height, 0x00FFFFFF); // Beyaz Çerçeve Zırhı

    /* 4. Sağ Alt Köşedeki "İleri/Tamam" Buton Alanı (Tıklama Odası) */
    int btn_w = 80;
    int btn_h = 25;
    int btn_x = x + width - btn_w - 15;
    int btn_y = y + height - btn_h - 15;
    draw_filled_rect(btn_x, btn_y, btn_w, btn_h, 0x000A3A6B); // Buton İç Rengi
    draw_rect_outline(btn_x, btn_y, btn_w, btn_h, 0x00FFFFFF); // Buton Çerçevesi
}

/* YAPAY ZEKA DESTEKLİ ÇALIŞAN ALT SUBSYSTEM */
void run_exe_subsystem(void) {
    /* Pencere Başlangıç Koordinatları: 800x600 ekranda tam ortalama */
    int win_x = 150;
    int win_y = 100;
    int win_w = 500;
    int win_h = 380;

    /* Fare tıklandıkça tetiklenen döngüsel OOBE pencereleri */
    if (setup_stage == 0) {
        /* AŞAMA 1: Yapay Zekalı Kurulum Karşılaması */
        draw_custom_window(win_x, win_y, win_w, win_h, "Wind OS - AI Setup Step 1", 0x00CCCCCC);
        
        /* Videodaki o tuhaf kalın orta çubuğu kaldırdık. 
           Bunun yerine sadece şık bir AI Durum indikatör çizgisi çekiyoruz */
        uint32_t ai_status_color = (central_ai_prediction_level == 2) ? 0x00FF0000 : 0x0000FF00;
        draw_filled_rect(win_x + 40, win_y + win_h - 60, win_w - 80, 4, ai_status_color);
        
    } else if (setup_stage == 1) {
        /* AŞAMA 2: Donanım Odaları Kalibrasyon Ekranı */
        draw_custom_window(win_x, win_y, win_w, win_h, "Hardware Calibration Step 2", 0x00BBBBBB);
        
        /* Kalibrasyon durum şeridi */
        draw_filled_rect(win_x + 50, win_y + 150, 400, 8, 0x0000FF00);
        
    } else if (setup_stage == 2) {
        /* AŞAMA 3: Masaüstüne Geçiş Hazırlığı */
        draw_custom_window(win_x, win_y, win_w, win_h, "Finalizing OS Setup Step 3", 0x00AAAAAA);
        
        /* Bitirme şeridi */
        draw_filled_rect(win_x + 50, win_y + 200, 400, 12, 0x000A3A6B);
    }
}
