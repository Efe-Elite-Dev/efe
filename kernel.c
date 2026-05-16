#include "screen.h"
#include <stdint.h>

char cmd_buffer[256];
int cmd_index = 0;

// Kullanıcı Hafızası
char user_wifi_pass[64];
int wifi_pass_idx = 0;
char user_time[16];
int time_idx = 0;

// YENİ KURULUM ADIMLARI
// 0: Beyaz Arka Planlı Hoş Geldiniz
// 1: Wi-Fi Arama Animasyonu
// 2: Wi-Fi Bağlanma Ekranı (Şifre Girişi)
// 3: Wi-Fi Bağlanıyor Animasyonu -> Otomatik Geçiş
// 4: Saat Ayarlama Ekranı
// 5: Hazırsınız Ekranı
// 6: Gerçek Sky-Shell Masaüstü Ortamı
int setup_state = 0;

// Port işlemleri
static inline uint8_t inb(uint16_t port) {
    uint8_t data;
    __asm__ __volatile__("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

// String karşılaştırma
int sky_strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

// Zaman oyalama fonksiyonu (Animasyonlar için)
void sky_delay(int count) {
    for (volatile int i = 0; i < count * 50000; i++);
}

// Gelişmiş Ekran Çizici
void draw_setup_screen(void) {
    sky_clear_screen();
    
    // STATE 0: HOŞ GELDİNİZ (Arka Planı Beyaz Yapma Simülasyonlu)
    if (setup_state == 0) {
        sky_print("========================================================================\n");
        sky_print("                      SKY-OS KURULUMUNA HOS GELDINIZ!                  \n");
        sky_print("========================================================================\n\n");
        sky_print("          [ Sky-OS Safe AI Kernel - Yeni Nesil Baslangic Sihirbazi ]     \n\n");
        sky_print("   * Donanim katmanlari basariyla haritalandi.\n");
        sky_print("   * VGA Video text modu optimize edildi.\n");
        sky_print("   * Cekirdek (Kernel) modulleri guvenli modda yuklendi.\n\n");
        sky_print("------------------------------------------------------------------------\n");
        sky_print("       Sistemi kurmaya baslamak icin [ENTER] tusuna basiniz...          \n");
        sky_print("------------------------------------------------------------------------\n");
    }
    // STATE 1: WI-FI ARANIYOR ANİMASYONU
    else if (setup_state == 1) {
        sky_print("========================================================================\n");
        sky_print("                      ADIM 1: AGA BAGLANIN                             \n");
        sky_print("========================================================================\n\n");
        sky_print("   Cevredeki Wi-Fi aglari taraniyor, lutfen bekleyiniz...\n\n");
        
        sky_delay(2000);
        sky_print("   [ ] Taraniyor.   (20%)\n");
        sky_delay(2000);
        sky_print("   [ ] Taraniyor..  (50%)\n");
        sky_delay(2000);
        sky_print("   [ ] Taraniyor... (80%)\n");
        sky_delay(2000);
        
        // Otomatik olarak şifre ekranına fırlatıyoruz (Animasyonlu geçiş)
        setup_state = 2;
        draw_setup_screen();
    }
    // STATE 2: WI-FI BAĞLANMA EKRANI
    else if (setup_state == 2) {
        sky_print("========================================================================\n");
        sky_print("                      ADIM 1: AGA BAGLANIN                             \n");
        sky_print("========================================================================\n\n");
        sky_print("   Bulunan Ag: [ Sky_Net_5G_Fiber ]\n\n");
        sky_print("   Lutfen ag sifresini giriniz (Gecmek icin direkt ENTER basabilirsin):\n");
        sky_print("   Sifre: ");
    }
    // STATE 3: WI-FI BAĞLANIYOR ANİMASYONU VE OTOMATİK GEÇİŞ
    else if (setup_state == 3) {
        sky_print("========================================================================\n");
        sky_print("                      ADIM 1: AGA BAGLANIN                             \n");
        sky_print("========================================================================\n\n");
        sky_print("   Ag baglantisi kuruluyor...\n\n");
        
        sky_delay(3000);
        sky_print("   [+] IP Adresi Aliniyor: 192.168.1.144\n");
        sky_delay(2000);
        sky_print("   [+] Baglanti Basarili! Internet Aktif.\n\n");
        sky_delay(3000);
        
        // Otomatik olarak saat ekranına geçiş
        setup_state = 4;
        draw_setup_screen();
    }
    // STATE 4: SAAT AYARLAMA EKRANI
    else if (setup_state == 4) {
        sky_print("========================================================================\n");
        sky_print("                      ADIM 2: SAAT VE TARIH AYARI                       \n");
        sky_print("========================================================================\n\n");
        sky_print("   Sistem saatini manuel olarak senkronize edin.\n\n");
        sky_print("   Lutfen saati giriniz (Ornek: 14:30):\n");
        sky_print("   Saat: ");
    }
    // STATE 5: HAZIRSIANIZ EKRANI
    else if (setup_state == 5) {
        sky_print("========================================================================\n");
        sky_print("                      KURULUM TAMAMLANDI!                              \n");
        sky_print("========================================================================\n\n");
        sky_print("   Her sey hazir! Sky-OS kullanima hazir duruma getirildi.\n\n");
        sky_print("   [+] Internet Durumu: Bagli\n");
        sky_print("   [+] Sistem Saati   : ");
        sky_print(user_time);
        sky_print("\n   [+] Kullanici Tipi  : Elite Developer\n\n");
        sky_print("------------------------------------------------------------------------\n");
        sky_print("        Sisteme giris yapmak icin [ENTER] tusuna basiniz...             \n");
        sky_print("------------------------------------------------------------------------\n");
    }
    // STATE 6: GERÇEK MASAÜSTÜ KABUĞU
    else if (setup_state == 6) {
        sky_print("========================================================================\n");
        sky_print("          Sky-OS v0.7 - Interactive Custom OOBE Shell Loaded           \n");
        sky_print("========================================================================\n");
        sky_print(" Sistem Saati: ");
        sky_print(user_time);
        sky_print(" | 'help' veya 'neofetch' yazarak test edebilirsin.\n\n");
        sky_print("SkyOS> ");
    }
}

// Komut Satırı Motoru
void run_command(const char* cmd) {
    sky_print("\n");
    
    if (sky_strcmp(cmd, "help") == 0) {
        sky_print("--- Sky-OS Kullanilabilir Komutlar ---\n");
        sky_print("help     : Komut listesini gosterir.\n");
        sky_print("neofetch : Sistem ozelliklerini ve logoyu basar.\n");
        sky_print("clear    : Ekrani temizler.\n");
        sky_print("--------------------------------------\n");
    } 
    else if (sky_strcmp(cmd, "neofetch") == 0) {
        sky_print("      .---.        Sky-OS [Safe AI Kernel]\n");
        sky_print("     /     \\       Mimari: x86 (32-bit) freestanding\n");
        sky_print("     \\     /       Surum: v0.7-OOBE-Wizard\n");
        sky_print("      `---'        Gelistirici: Efe-Elite-Dev\n");
        sky_print("                   Net Stat: Connected (Sky_Net_5G)\n");
    } 
    else if (sky_strcmp(cmd, "clear") == 0) {
        sky_clear_screen();
    }
    else if (sky_strcmp(cmd, "") == 0) {
        // Boş enter
    } 
    else {
        sky_print("Hata: Bilinmeyen komut! 'help' yaziniz.\n");
    }
}

// Klavye Yonetimi
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

            // STATE 0: Hoş Geldiniz ekranında Enter'a basıp Wi-Fi aramaya geçme
            if (setup_state == 0) {
                if (key == '\n') {
                    setup_state = 1; 
                    draw_setup_screen(); // Wi-Fi tarama animasyonunu başlatır
                }
            }
            // STATE 2: Wi-Fi Şifre Girişi
            else if (setup_state == 2) {
                if (key == '\n') {
                    user_wifi_pass[wifi_pass_idx] = '\0';
                    setup_state = 3; // Bağlanıyor animasyonu ve otomatik geçişe atla
                    draw_setup_screen();
                } 
                else if (key == '\b') {
                    if (wifi_pass_idx > 0) {
                        wifi_pass_idx--;
                        sky_print("\b \b");
                    }
                } 
                else if (wifi_pass_idx < 63) {
                    user_wifi_pass[wifi_pass_idx++] = key;
                    sky_put_char('*'); // Şifre güvenliği için ekrana yıldız basıyoruz!
                }
            }
            // STATE 4: Saat Girişi
            else if (setup_state == 4) {
                if (key == '\n') {
                    if (time_idx > 0) {
                        user_time[time_idx] = '\0';
                        setup_state = 5; // Hazırsınız ekranına geç
                        draw_setup_screen();
                    }
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
            // STATE 5: Hazırsınız Ekranında Enter basıp sisteme girme
            else if (setup_state == 5) {
                if (key == '\n') {
                    setup_state = 6; // Gerçek Masaüstüne at
                    draw_setup_screen();
                }
            }
            // STATE 6: Gerçek Komut Satırı
            else if (setup_state == 6) {
                if (key == '\n') {
                    cmd_buffer[cmd_index] = '\0';
                    run_command(cmd_buffer);
                    cmd_index = 0;
                    sky_print("SkyOS> ");
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
    setup_state = 0;
    draw_setup_screen();

    inb(0x60); // Klavye port temizliği

    while (1) {
        handle_keyboard_polling();
        for (volatile int i = 0; i < 1000; i++);
    }
}
