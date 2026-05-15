#include "screen.h"
#include "keyboard.h"

void kernel_main(void) {
    // Önce ekranı temizle ve başlat
    sky_clear_screen();
    
    // Üstte şık bir başlık barı yapalım
    sky_print("========================================================================\n");
    sky_print("             Sky-OS Safe AI Kernel - Surucu Katmani Yuklendi!          \n");
    sky_print("========================================================================\n\n");
    
    sky_print("[+] VGA Metin modu surucusu aktif edildi.\n");
    sky_print("[+] Donanim kesme tablosu (IDT) hazirlaniyor...\n");
    
    // Klavyeyi ve donanım portlarını ilklendiriyoruz
    init_keyboard();
    
    sky_print("[+] Klavye surucusu guvenli modda baslatildi!\n");
    sky_print("[+] Sky-OS canli. IDT tablosu baglanana kadar sistem kilitlendi.\n\n");
    sky_print("Sistem Hazir> ");

    // GURU MEDITATION ENGELLEME: 
    // İşlemciyi kesmelere açmadan (sti demeden) saf kilit döngüsüne alıyoruz.
    // Böylece harici donanım sinyalleri makineyi çökertemez.
    while (1) {
        __asm__ __volatile__("cli\n\thlt");
    }
}
