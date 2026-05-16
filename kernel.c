#include "screen.h"
#include <stdint.h>

// Klavyeden gelen harfleri biriktirmek için komut hafızası (Buffer)
char cmd_buffer[256];
int cmd_index = 0;

// Port okuma fonksiyonu (Klavye çipine doğrudan erişim için)
static inline uint8_t inb(uint16_t port) {
    uint8_t data;
    __asm__ __volatile__("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

// Port yazma fonksiyonu
static inline void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
}

// String karşılaştırma fonksiyonumuz
int sky_strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

// US Klavye Harf Haritası
static const char kbd_us[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
  '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0,
 '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0,   '*',   0,
  ' ',   0
};

// Komutları işleyen ana merkez
void run_command(const char* cmd) {
    sky_print("\n");
    
    if (sky_strcmp(cmd, "help") == 0) {
        sky_print("--- Sky-OS Kullanilabilir Komutlar ---\n");
        sky_print("help     : Komut listesini gosterir.\n");
        sky_print("neofetch : Sistem ozelliklerini ekrana basar.\n");
        sky_print("clear    : Ekrani temizler.\n");
        sky_print("--------------------------------------\n");
    } else if (sky_strcmp(cmd, "neofetch") == 0) {
        sky_print("      .---.        Sky-OS [Safe AI Kernel]\n");
        sky_print("     /     \\       Mimari: x86 (32-bit) freestanding\n");
        sky_print("     \\     /       Surum: v0.4-UltraPolling\n");
        sky_print("      `---'        Dil: %100 Saf C & Assembly\n");
        sky_print("                   Gelistirici: Efe-Elite-Dev\n");
    } else if (sky_strcmp(cmd, "clear") == 0) {
        sky_clear_screen();
    } else if (sky_strcmp(cmd, "") == 0) {
        // Boş enter basılırsa hiçbir şey yapma
    } else {
        sky_print("Hata: Bilinmeyen komut! 'help' yaziniz.\n");
    }
}

// Kesmesiz Saf Polling Klavye Sürücüsü
void handle_keyboard_polling(void) {
    // 0x64 portunun ilk biti (0x01) 1 ise: "Klavye çipinde okunmayı bekleyen harf var" demektir!
    if (inb(0x64) & 0x01) {
        uint8_t scancode = inb(0x60); // Donanımdan ham tarama kodunu çek
        
        // Sadece tuşa basılma anını yakala (Tuştan elini çekme kodlarını filtrele)
        if (!(scancode & 0x80)) {
            char key = kbd_us[scancode];
            
            if (key == '\n') { // ENTER TUŞUNA BASILDIYSA
                cmd_buffer[cmd_index] = '\0'; // Komut stringini bitir
                run_command(cmd_buffer);      // Komutu çalıştır
                cmd_index = 0;                // Bufferı sıfırla
                sky_print("SkyOS> ");
            } 
            else if (key == '\b') { // BACKSPACE TUŞUNA BASILDIYSA
                if (cmd_index > 0) {
                    cmd_index--;
                    // Ekrandan son harfi silme simülasyonu yapıyoruz (Geri gel, boşluk bas, geri gel)
                    sky_print("\b \b"); 
                }
            } 
            else if (key != 0 && cmd_index < 255) { // NORMAL HARF BASILDIYSA
                cmd_buffer[cmd_index++] = key; // Hafızaya kaydet
                sky_put_char(key);             // Ekrana yazdır
            }
        }
    }
}

void kernel_main(void) {
    sky_clear_screen();
    
    sky_print("========================================================================\n");
    sky_print("          Sky-OS Safe AI Kernel - Ultra Polling Klavye Aktif!          \n");
    sky_print("========================================================================\n\n");
    
    sky_print("[+] VGA Metin modu ekran surucusu: STABIL\n");
    sky_print("[+] Donanimsal Kesme Korumasi (Anti-Guru Mode): AKTIF\n");
    sky_print("[+] Doğrudan Port Taramali Klavye Motoru Devrede.\n\n");
    sky_print("SkyOS> ");

    // Klavye çipini temizle
    inb(0x60);

    // ANA MOTOR DÖNGÜSÜ: İşlemciyi çökertmeden saniyede binlerce kez klavye portunu yoklar
    while (1) {
        handle_keyboard_polling();
        
        // Küçük bir zaman boşluğu bırakarak sanal makine işlemcisinin şişmesini engelliyoruz
        for (volatile int i = 0; i < 1000; i++);
    }
}
