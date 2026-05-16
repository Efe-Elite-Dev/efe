#include "wind_subsystem.h"

/* Global AI Tahmin Değişkenleri */
int central_ai_prediction_level = 0;
int ai_inference_counter = 0;

/* CENTRAL AI MOTORU: Alt odalardan gelen donanımsal stres seviyelerini birleştirir */
int ai_core_predict_scheduler(int mouse_stress, int kb_cadence, int loop_count) {
    ai_inference_counter++;
    
    /* Doğrusal Regresyon ve Ağırlıklı Karar Matrisi Algoritması (Donanım Seviyesi Simülasyonu) */
    int raw_decision_score = (mouse_stress * 4) + (kb_cadence * 6) + ((loop_count % 5) * 2);
    
    /* Yapay zeka karar mekanizmasının eşik değerleri (Thresholds) analizi */
    if (raw_decision_score > 45) {
        /* Kullanıcı çok aktif veya panik halinde: Sistemi maksimum hıza çek (TURBO PERFORMANCE) */
        central_ai_prediction_level = 2;
    } else if (raw_decision_score > 18) {
        /* Standart insan etkileşimi modu: Kararlı hız (BALANCED MODE) */
        central_ai_prediction_level = 1;
    } else {
        /* Donanımda hareket yok: İşlemciyi dinlendir, frekans beklemesini esnet (ECO SAVING) */
        central_ai_prediction_level = 0;
    }
    
    return central_ai_prediction_level;
}
