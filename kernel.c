#include "sky_subsystem.h"
#include "mouse.h"
#include "gui.h"

// Dışarıdan bağladığımız tüm akıllı alt sistemlerin köprüleri
extern int ai_predict_hardware_load(int mouse_delta_x, int loop_count);
extern void run_exe_subsystem(void);
extern void init_idt(void); // Kesme tablosunu (IDT) işlemciye tanıtır

uint32_t* vbe_vram = 0; 
uint32_t  vbe_pitch = 0;

static inline void io_wait(void) {
    asm volatile ("outb %%al, $0x80" : : "a"(0));
}

/**
 * 🚀 TÜM ALT SİSTEMLERİ BARINDIRAN ULTIMATE X86 ÇEKİRDEĞİ
 */
void kernel_main(struct multiboot_info* mboot) {
    
    // 1. DONANIMSAL GRAFİK ADRESİNİ YAKALA
    if (mboot != 0 && (mboot->framebuffer_addr != 0)) {
        vbe_vram = (uint32_t*)(uintptr_t)mboot->framebuffer_addr;
        vbe_pitch = mboot->framebuffer_pitch;
    } else {
        vbe_vram = (uint32_t*)0xE0000000; // Standart LFB Fallback
        vbe_pitch = 800 * 4;
    }

    // 2. DONANIM SÜRÜCÜLERİNİ VE İŞLEMCİ KESMELERİNİ BAŞLAT
    init_idt();   // Klavye ve donanım kesme köprülerini CPU'ya yükler
    init_mouse(); // Fare alt yapısını hazırlar
    
    // 3. İLK EKRAN TAZELEMESİ (Masaüstü Gece Mavisi Tabanı)
    gui_refresh_desktop();

    uint32_t refresh_counter = 0;
    int current_wait_cycles = 30;

    // === YAPAY ZEKA DESTEKLİ VE KURULUM EKRANLI ANA DÖNGÜ ===
    while (1) {
        // Fare hareket verilerini polled/interrupt yöntemiyle oku
        handle_mouse_polling(); 
        
        refresh_counter++;
        if (refresh_counter >= 15) { // Kurulum ekranının akıcı olması için tazelemeyi hızlandırdık
            gui_refresh_desktop();  // Arka planı boyar
            run_exe_subsystem();    // Üzerine kurulum pencerelerini çakar!
            refresh_counter = 0;
        }
        
        // YAPAY ZEKA DESTEKLİ İŞLEMCİ KORUMASI
        int system_stress = ai_predict_hardware_load(10, refresh_counter);
        
        if (system_stress == 1) {
            current_wait_cycles = 60; // Sistem yüklüyse döngüyü yavaşlat CPU dinlensin
        } else {
            current_wait_cycles = 15; // Sistem rahatsa son sürat ak
        }

        for (volatile int i = 0; i < current_wait_cycles; i++) {
            io_wait();
        }
    }
}
