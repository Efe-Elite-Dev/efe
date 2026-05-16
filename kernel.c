#include "wind_subsystem.h"

uint32_t* vbe_vram = 0; 
uint32_t  vbe_pitch = 0;

/* Ana Motor Durum Sayaçları */
int kernel_ai_total_loops = 0;

static inline void io_wait(void) {
    asm volatile ("outb %%al, $0x80" : : "a"(0));
}

/* WIND OS YAPAY ZEKA DESTEKLİ ANA MOTOR */
void kernel_main(struct multiboot_info* mboot) {
    /* VBE Grafik Doğrulama Katmanı */
    if (mboot != 0 && (mboot->flags & (1 << 12)) && (mboot->framebuffer_addr != 0)) {
        vbe_vram = (uint32_t*)(uintptr_t)mboot->framebuffer_addr;
        vbe_pitch = mboot->framebuffer_pitch;
    } else {
        vbe_vram = (uint32_t*)0xFD000000; 
        vbe_pitch = 800 * 4;
    }

    /* Arka Plan Temizliği ve Sürücü Odalarının Başlatılması */
    clear_text_screen();
    init_idt();
    init_keyboard();
    init_mouse();
    
    gui_refresh_desktop();

    uint32_t refresh_counter = 0;
    int current_wait_cycles = 20;

    while (1) {
        kernel_ai_total_loops++;
        
        /* 1. Sürücü Odalarını Dürt */
        handle_mouse_polling(); 
        check_keyboard_pure();  
        
        /* 2. Her Odanın İçindeki AI Tahminlerini Topla ve Merkezi AI Kararını Ver */
        int m_stress = ai_mouse_analyze_stress();
        int k_cadence = ai_keyboard_analyze_cadence();
        
        int central_ai_decision = ai_core_predict_scheduler(m_stress, k_cadence, kernel_ai_total_loops);
        
        /* 3. Arayüz Tazeleme Kontrolü */
        refresh_counter++;
        if (refresh_counter >= 15) { 
            gui_refresh_desktop();  
            run_exe_subsystem(); /* Grafik OOBE Ekranlarını Yönet */
            refresh_counter = 0;
        }
        
        /* 4. CENTRAL AI TAHMİNLİ DİNAMİK ZAMANLAYICI (Dynamic Scheduler Gecikmesi) */
        if (central_ai_decision == 2) {
            current_wait_cycles = 4;   /* Yoğun donanım etkileşimi: İşlemci tam güçte akar */
        } else if (central_ai_decision == 1) {
            current_wait_cycles = 18;  /* Standart çalışma temposu */
        } else {
            current_wait_cycles = 45;  /* Kullanıcı aktif değil: İşlemciyi korumak için derin uyku */
        }

        /* Akıllı Gecikmeyi Uygula */
        for (volatile int i = 0; i < current_wait_cycles; i++) {
            io_wait();
        }
    }
}
