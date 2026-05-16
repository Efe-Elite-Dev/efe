#include "screen.h"
#include <stdint.h>

char cmd_buffer[256];
int cmd_index = 0;

// Kullanıcı Verileri
char user_wifi_pass[64];
int wifi_pass_idx = 0;
char user_time[16] = "12:00"; // Varsayılan Wi-Fi saati
int time_idx = 0;

// ANDROID UI ADIMLARI (State Machine)
// 0: Full Beyaz Hoş Geldiniz & Başlayın Butonu
// 1: Wi-Fi Şifre Girişi veya Atla Ekranı
// 2: Saat Belirleme veya Atla (Wi-Fi'dan Al) Ekranı
// 3: Kurulum Bitti, Başlayabilirsiniz Butonu
// 4: Gerçek Android Esintili Beyaz Sky-Shell masaüstü
int setup_state = 0;

// Port Okuma
static inline uint8_t inb(uint16_t port) {
    uint8_t data;
    __asm__ __volatile__("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

// String Karşılaştırma
int sky_strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

// UI Gecikme Fonksiyonu
void sky_ui_delay(int count) {
    for (volatile int i = 0; i < count * 80000; i++);
}

// TÜM EKRANI BEMBEYAZ BOYAYAN ANDROID UI MOTORU
void paint_android_bg(void) {
    // VGA Metin Modu Belleği: 0xB8000
    // 80 sütun x 25 satır = 2000 hücre
    // Renk Kodu: 0x70 (Arka Plan: Gri/Beyaz, Yazı: Siyah)
    uint16_t *vga_buffer = (uint16_t*)0xB8000;
    for (int i = 0; i < 80 * 25; i++) {
        vga_buffer[i] = (0x70 << 8) | ' '; 
    }
    // İmleci sol üste çekmek için screen.c'nin ekran temizleme fonksiyonunun mantığını taklit ediyoruz
    sky_clear_screen(); 
}

// Renkli ve Arka Planlı Yazı Basma Köprüsü
// NOT: screen.c içindeki standart sky_print'i beyaz arka plana uyarlıyoruz
void ui_print(const char* str) {
    sky_print(str);
}

// Android UI Çizim Merkezi
void draw_android_ui(void) {
    paint_android_bg(); // Ekranı full beyaz yap
    
    // STATE 0: HOŞ GELDİNİZ & BAŞLAYIN
    if (setup_state == 0) {
        ui_print("\n\n\n\n\n");
        ui_print("                         H O S   G E L D I N I Z\n");
        ui_print("                        -------------------------\n\n\n");
        ui_print("                                 Sky-OS\n");
        ui_print("                       Android Tarzi Kurulum Sihirbazi\n\n\n\n\n\n\n");
        ui_print("                      _______________________________ \n");
        ui_print("                     [       ==  BASLAYIN  ==        ]\n");
        ui_print("                      ------------------------------- \n");
        ui_print("\n\n                             ( Gecmek icin ENTER )");
    }
    // STATE 1: WI-FI ŞİFRE / ATLA
    else if (setup_state == 1) {
        ui_print("\n\n\n");
        ui_print("                            W I - F I   A G I\n");
        ui_print("                        -------------------------\n\n");
        ui_print("                         Ag: [ Sky_Net_5G_Fiber ]\n\n\n");
        ui_print("                   Lutfen Sifreyi Girin:\n");
        ui_print("                   Sifre Kutusu: [ ");
        // Kullanıcının yazdığı şifre yıldızları buraya gelecek
    }
    // STATE 2: SAAT BELİRLE / ATLA
    else if (setup_state == 2) {
        ui_print("\n\n\n");
        ui_print("                        S A A T   A Y A R L A R I\n");
        ui_print("                        -------------------------\n\n\n");
        ui_print("             Manuel Saat Girin (Ornek: 14:35)\n");
        ui_print("             Eger Wi-Fi'dan otomatik alinsin istiyorsaniz bos birakin.\n\n\n");
        ui_print("                   Saat Kutusu: [ ");
        // Saat karakterleri buraya gelecek
    }
    // STATE 3: KURULUM BİTTİ / BAŞLAYABİLİRSİNİZ
    else if (setup_state == 3) {
        ui_print("\n\n\n\n\n");
        ui_print("                           K U R U L U M   B I T T I\n");
        ui_print("                        -------------------------\n\n\n");
        ui_print("                        Sistem kullanima tamamen hazir!\n\n\n\n\n\n");
        ui_print("                      _______________________________ \n");
        ui_print("                     [  == BASLAYABILIRSINIZ (ENTER) == ]\n");
        ui_print("                      ------------------------------- \n");
    }
    // STATE 4: GERÇEK BEYAZ ANDROID MASAÜSTÜ KABUĞU
    else if (setup_state == 4) {
        ui_print("========================================================================\n");
        ui_print("  Sky-OS Android-UI Shell v0.8 | Saat: ");
        ui_print(user_time);
        ui_print(" | Net: ");
        if (wifi_pass_idx > 0) ui_print("Bagli (Sky_Net_5G)\n");
        else ui_print("Atlandi (Cevrimdisi)\n");
        ui_print("========================================================================\n");
        ui_print(" Android UI Masaustu Aktif! 'help' veya 'neofetch' yazabilirsiniz.\n\n");
        ui_print("SkyOS> ");
    }
}

// Komut İşleme Merkezi (Sadece Masaüstündeyken çalışır)
void run_command(const char* cmd) {
    ui_print("\n");
    if (sky_strcmp(cmd, "help") == 0) {
        ui_print("--- Kullanilabilir Komutlar ---\n");
        ui_print("help     : Komut listesi\n");
        ui_print("neofetch : Sistem ozellikleri\n");
        ui_print("clear    : Ekran temizleme\n");
    } else if (sky_strcmp(cmd, "neofetch") == 0) {
        ui_print("   Sky-OS Freestanding OS\n");
        ui_print("   UI Tarzi: Android Beyaz Temali OOBE\n");
        ui_print("   Gelistirici: Efe-Elite-Dev\n");
    } else if (sky_strcmp(cmd, "clear") == 0) {
        draw_android_ui();
    } else if (sky_strcmp(cmd, "") == 0) {
        // Boş enter
    } else {
        ui_print("Bilinmeyen komut!\n");
    }
}

// Android Klavye ve Akış Yönetimi
void handle_keyboard_polling(void) {
    if (inb(0x64) & 0x01) {
        uint8_t scancode = inb(0x60);
        if (!(scancode & 0x80)) {
            static const char kbd_us[128] = {
                0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
              '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
                0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0,
             '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0,   '*',   0,
              ' ',   0
            };
            char key = kbd_us[scancode];
            if (key == 0) return;

            // ADIM 0: BAŞLAYIN BUTONU
            if (setup_state == 0) {
                if (key == '\n') {
                    setup_state = 1; // Wi-Fi adımına geç
                    draw_android_ui();
                }
            }
            // ADIM 1: WI-FI ŞİFRESİ VEYA ATLA
            else if (setup_state == 1) {
                if (key == '\n') {
                    user_wifi_pass[wifi_pass_idx] = '\0';
                    // Animasyon efekti: Şifreyi girince veya boş bırakıp (Atla) yapınca geçiş hissi
                    ui_print(" ]\n\n                   Lutfen bekleyin...");
                    sky_ui_delay(3000);
                    
                    setup_state = 2; // Saat adımına geç
                    draw_android_ui();
                } 
                else if (key == '\b') {
                    if (wifi_pass_idx > 0) {
                        wifi_pass_idx--;
                        sky_print("\b \b");
                    }
                } 
                else if (wifi_pass_idx < 63 && key != ' ') {
                    user_wifi_pass[wifi_pass_idx++] = key;
                    sky_put_char('*'); // Şifreyi gizle
                }
            }
            // ADIM 2: SAAT BELİRLE VEYA ATLA (Wi-Fi'dan otomatik al)
            else if (setup_state == 2) {
                if (key == '\n') {
                    if (time_idx > 0) {
                        user_time[time_idx] = '\0'; // Kullanıcı saatini kaydet
                    } else {
                        // Eğer boş bırakıp atladıysa otomatik Wi-Fi saati devreye giriyor!
                        user_time[0] = '1'; user_time[1] = '2'; user_time[2] = ':';
                        user_time[3] = '0'; user_time[4] = '0'; user_time[5] = '\0';
                    }
                    ui_print(" ]\n\n                   Ayarlar kaydediliyor...");
                    sky_ui_delay(3000);
                    
                    setup_state = 3; // Kurulum bitti adımına geç
                    draw_android_ui();
                } 
                else if (key == '\b') {
                    if (time_idx > 0) {
                        time_idx--;
                        sky_print("\b \b");
                    }
                } 
                else if (time_idx < 15) {
                    user_time[time_idx++] = key;
                    sky_put_char(key);
                }
            }
            // ADIM 3: KURULUM BİTTİ BASLİYABİLİRSİNİZ BUTONU
            else if (setup_state == 3) {
                if (key == '\n') {
                    setup_state = 4; // Gerçek Android temalı masaüstü kabuğuna at
                    draw_android_ui();
                }
            }
            // ADIM 4: GERÇEK MASAÜSTÜ VE KOMUT SATIRI
            else if (setup_state == 4) {
                if (key == '\n') {
                    cmd_buffer[cmd_index] = '\0';
                    run_command(cmd_buffer);
                    cmd_index = 0;
                    ui_print("SkyOS> ");
                } 
                else if (key == '\b') {
                    if (cmd_index > 0) {
                        cmd_index--;
                        sky_print("\b \b");
                    }
                } 
                else if (cmd_index < 255) {
                    cmd_buffer[cmd_index++] = key;
                    sky_put_char(key);
                }
            }
        }
    }
}

void kernel_main(void) {
    // Kurulum sihirbazını başlatıyoruz
    setup_state = 0;
    draw_android_ui();

    inb(0x60); // Klavye portunu sıfırla

    while (1) {
        handle_keyboard_polling();
        for (volatile int i = 0; i < 1000; i++);
    }
}
