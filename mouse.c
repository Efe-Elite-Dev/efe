#include "wind_subsystem.h"
#include "mouse.h"
#include "gui.h"

#define MOUSE_DATA_PORT 0x60
#define MOUSE_STATUS_PORT 0x64

/* exe_subsystem.c içindeki OOBE aşama kontrol değişkeni */
extern int setup_stage; 

/* Sürücü odasının kendi iç durumu (Paket döngüsü ve tamponu) */
static int mouse_cycle = 0;
static unsigned char mouse_packet[3];

/* Fare donanımını (PS/2 hattını ve yardımcı işlemciyi) aktif eden ilk kurulum */
void init_mouse(void) {
    /* 1. Fare hattını (Auxiliary Device) aktif et */
    outb(0x64, 0xA8);
    
    /* 2. Mevcut komut byte'ını oku */
    outb(0x64, 0x20);
    uint8_t status = inb(0x60) | 2; /* Kesme (IRQ12) bitini etkinleştir */
    
    /* 3. Güncellenmiş komut byte'ını geri yaz */
    outb(0x64, 0x60);
    outb(0x60, status);
    
    /* 4. Fareye varsayılan ayarları yükle ve veri akışını aç emri gönder */
    outb(0x64, 0xD4);
    outb(0x60, 0xF4);
    inb(0x60); /* Donanımdan gelen 'Acknowledge' (0xFA) yanıtını yut ve tamponu temizle */
}

/* Klavyeyi ve fareyi tek tek koklayan polled nöbetçi fonksiyonu */
void handle_mouse_polling(void) {
    /* CRITICAL SCOPE FIX: click_lock artık tüm fonksiyon tarafından görülebilir */
    static int click_lock = 0;

    /* Portta okunacak veri var mı? (Bit 0) */
    if (inb(MOUSE_STATUS_PORT) & 1) {
        /* Gelen veri gerçekten fareye mi ait yoksa klavyeye mi? (Bit 5) */
        if (inb(MOUSE_STATUS_PORT) & 0x20) {
            unsigned char current_byte = inb(MOUSE_DATA_PORT);
            
            mouse_packet[mouse_cycle] = current_byte;
            mouse_cycle++;

            /* PS/2 fare standardında paketler 3 byte'lık seriler halinde gelir */
            if (mouse_cycle == 3) {
                mouse_cycle = 0; /* Paket tamamlandı, bir sonraki döngü için sıfırla */
                
                /* 1. Byte'ın 0. biti Sol Tık (Left Button) durumunu verir */
                int left_click = mouse_packet[0] & 1;

                if (left_click) {
                    /* OOBE pencerelerinde (exe_subsystem) tıklama simülasyonu */
                    if (click_lock == 0) {
                        setup_stage++;
                        if (setup_stage > 2) {
                            setup_stage = 0; /* Başa sar veya masaüstüne geçişi tetikle */
                        }
                        click_lock = 1; /* Tek tıkla çoklu aşama atlamayı engelle */
                    }
                } else {
                    click_lock = 0; /* Elini fareden çekince kilidi aç - ARTIK HATA VERMEZ */
                }
            }
        }
    }
}
