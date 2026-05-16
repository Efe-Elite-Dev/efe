#include "wind_subsystem.h"

#define MOUSE_DATA_PORT 0x60
#define MOUSE_STATUS_PORT 0x64

/* exe_subsystem.c içindeki pencerelerin aşama sayacı */
extern int setup_stage; 

/* Sürücü odasının yerel değişkenleri */
static int mouse_cycle = 0;
static unsigned char mouse_packet[3];

/* FARE ODASI YAPAY ZEKASI (Yerel Ajan Değişkenleri) */
static int mouse_click_density = 0;
static int mouse_ai_stress_score = 0;

/* PS/2 Fare hattını ve yardımcı işlemciyi uyandıran ilk donanım kurulumu */
void init_mouse(void) {
    /* 1. Fare hattını (Auxiliary Device) aktif et */
    outb(0x64, 0xA8);
    
    /* 2. Mevcut komut byte'ını oku */
    outb(0x64, 0x20);
    uint8_t status = inb(0x60) | 2; /* Kesme (IRQ12) bitini etkinleştir */
    
    /* 3. Güncellenmiş komut byte'ını klavye/fare işlemcisine geri yaz */
    outb(0x64, 0x60);
    outb(0x60, status);
    
    /* 4. Fareye varsayılan ayarları yükle ve veri akışını aç emri gönder */
    outb(0x64, 0xD4);
    outb(0x60, 0xF4);
    inb(0x60); /* Donanımdan gelen ACK (0xFA) yanıtını tampondan temizlemek için yut */
}

/* FARE YAPAY ZEKASI: Tıklama sıklığından kullanıcının aciliyet/stres haritasını çıkarır */
int ai_mouse_analyze_stress(void) {
    if (mouse_click_density > 12) {
        mouse_ai_stress_score = 3; /* Çok yoğun tıklama var, arayüzü hızlandır */
    } else if (mouse_click_density > 4) {
        mouse_ai_stress_score = 2; /* Rutin fare kullanımı */
    } else {
        mouse_ai_stress_score = 1; /* Fare uykuda veya sakin modda */
    }
    return mouse_ai_stress_score;
}

/* Fareyi port seviyesinde koklayan polled nöbetçi lojiği */
void handle_mouse_polling(void) {
    static int click_lock = 0;

    /* Donanımsal portta veri bekliyor mu? (Bit 0) */
    if (inb(MOUSE_STATUS_PORT) & 1) {
        /* Gelen veri klavyeye mi yoksa fareye mi ait? (Bit 5) */
        if (inb(MOUSE_STATUS_PORT) & 0x20) {
            unsigned char current_byte = inb(MOUSE_DATA_PORT);
            mouse_packet[mouse_cycle] = current_byte;
            mouse_cycle++;

            /* PS/2 fare paketi tamamlandı mı? (3 Byte kuralı) */
            if (mouse_cycle == 3) {
                mouse_cycle = 0; /* Döngüyü sonraki paket için sıfırla */
                
                /* 1. Byte'ın 0. biti Sol Tık (Left Button) bayrağıdır */
                int left_click = mouse_packet[0] & 1;

                if (left_click) {
                    /* Yerel Yapay Zeka sayacını besle */
                    mouse_click_density += 5;
                    
                    /* Tıklama kilidi kontrolü (OOBE pencerelerini tek tek atlatmak için) */
                    if (click_lock == 0) {
                        setup_stage++;
                        if (setup_stage > 2) {
                            setup_stage = 0; /* Döngüsel aşamayı başa sar */
                        }
                        click_lock = 1; /* Elini fareden çekene kadar kilitle */
                    }
                } else {
                    click_lock = 0;
                    /* AI Sönümlendirme filtresi: Zamanla stres puanını düşür */
                    if (mouse_click_density > 0) mouse_click_density--;
                }
            }
        }
    }
}
