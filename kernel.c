#include "wind_subsystem.h"

/* Ekran kartının gerçek bellek göstergeleri */
uint32_t* vbe_vram = 0; 
uint32_t  vbe_pitch = 0;

/* DOUBLE BUFFERING ZIRHI: Ekrandaki git-gelleri önleyen gizli RAM ekranı */
uint32_t back_buffer[800 * 600]; 

int kernel_ai_total_loops = 0;

static inline void io_wait(void) {
    asm volatile ("outb %%al, $0x80" : : "a"(0));
}

/* Gizli ekrandaki tüm pikselleri tek bir hamlede gerçek ekran kartına kopyalayan motor */
void swap_buffers(void) {
    uint32_t pixels_per_line = vbe_pitch / 4;
    for (int y = 0; y < 600; y++) {
        for (int x = 0; x < 800; x++) {
            vbe_vram[y * pixels_per_line + x] = back_buffer[y * 800 + x];
        }
    }
}

/* WIND OS YAPAY ZEKA DESTEKLİ ANA MOTOR */
void kernel_main(struct multiboot_info* mboot) {
    if (mboot != 0 && (mboot->flags & (1 << 12)) && (mboot->framebuffer_addr != 0)) {
        vbe_vram = (uint32_t*)(uintptr_t)mboot->framebuffer_addr;
        vbe_pitch = mboot->framebuffer_pitch;
    } else {
        vbe_vram = (uint32_t*)0xFD000000; 
        vbe_pitch = 800 * 4;
    }

    clear_text_screen();
    init_idt();
    init_keyboard();
    init_mouse();
    
    uint32_t refresh_counter = 0;
    int current_wait_cycles = 20;

    while (1) {
        kernel_ai_total_loops++;
        
        /* 1. Sürücü odalarını dinle */
        handle_mouse_polling(); 
        check_keyboard_pure();  
        
        /* 2. AI Tahminlerini topla */
        int m_stress = ai_mouse_analyze_stress();
        int k_cadence = ai_keyboard_analyze_cadence();
        int central_ai_decision = ai_core_predict_scheduler(m_stress, k_cadence, kernel_ai_total_loops);
        
        /* 3. ARKA TAMPONDA ÇİZİM (Her şey gizli ekranda hazırlanıyor) */
        refresh_counter++;
        if (refresh_counter >= 8) { 
            /* Önce arka planı gizli ekrande temizle, göz kırpma yapamaz! */
            gui_refresh_desktop();  
            /* Sonra pencereleri gizli ekrana çiz */
            run_exe_subsystem(); 
            
            /* 4. ZIRHLI GEÇİŞ: Hazır olan gizli ekranı anında gerçek ekrana fırlat */
            swap_buffers();
            
            refresh_counter = 0;
        }
        
        /* 5. AI Tahminli Dinamik Zamanlayıcı */
        if (central_ai_decision == 2) {
            current_wait_cycles = 4;   
        } else if (central_ai_decision == 1) {
            current_wait_cycles = 15;  
        } else {
            current_wait_cycles = 35;  
        }

        for (volatile int i = 0; i < current_wait_cycles; i++) {
            io_wait();
        }
    }
}
