#include "sky_subsystem.h"

/**
 * 🧠 YAPAY ZEKA TABANLI SİSTEM YÜKÜ TAHMİN MOTORU
 * Fare hareket delta değerine ve döngü hızına bakarak işlemcinin stres seviyesini tahmin eder.
 * Dönüş Değeri: 1 -> Yüksek Stres (Sistem Yoğun), 0 -> Normal Durum (Sistem Rahat)
 */
int ai_predict_hardware_load(int mouse_delta_x, int loop_count) {
    // Yapay zeka ağırlık ve eşik değerleri (Weights and Biases)
    int weight_mouse = 3;
    int weight_loop = 1;
    int ai_threshold = 250;

    // Negatif değerleri siber koruma için pozitife çeviriyoruz (Mutlak Değer)
    if (mouse_delta_x < 0) {
        mouse_delta_x = -mouse_delta_x;
    }

    // Basit ve kararlı bir yapay sinir ağı aktivasyon simülasyonu
    int score = (mouse_delta_x * weight_mouse) + ((loop_count % 100) * weight_loop);

    if (score > ai_threshold) {
        return 1; // Yapay zekaya göre donanım şu an yoğun yük altında!
    }
    
    return 0; // Yapay zekaya göre donanım şu an çok rahat.
}
