#include "wind_subsystem.h"

/* Ekran kartının gerçek bellek göstergeleri */
uint32_t* vbe_vram = 0; 
uint32_t  vbe_pitch = 0;

/* BÜYÜK HAFIZA DÜZELTMESİ: 
   Diziyi fonksiyonların DIŞINDA, global ve statik olarak tanımlıyoruz.
   Böylece yığın (stack) alanında yer kaplamaz, işlemciyi çökertmez (Triple fault engellendi). */
static uint32_t back_buffer[800 * 600]; 

int kernel_ai_total_loops = 0;

static inline void io_wait(void) {
    asm volatile ("outb %%al, $0x80" : : "a"(0));
}

/* Gizli arabellekteki resmi ana ekrana basan motor */
void swap_buffers(void) {
    uint32_t pixels_per_line = vbe_pitch / 4;
    for (int y = 0; y < 600; y++) {
        for (int x = 0; x < 800; x++) {
            vbe_vram[y * pixels_per_line + x] = back_buffer[y * 800 + x];
        }
    }
}

/* WIND OS ANA GİRİŞ MERKEZİ */
void kernel_main(struct multiboot_info* mboot) {
    /* GRUB grafik verilerini doğrula */
    if (mboot != 0 && (mboot->flags & (1 << 12)) && (mboot->framebuffer_addr != 0)) {
        vbe_vram = (uint32_t*)(uintptr_t)mboot->framebuffer_addr;
        vbe_pitch = mboot->framebuffer_pitch;
    } else {
        vbe_vram = (uint32_t*)0xFD000000; 
        vbe_pitch = 800 * 4;
    }

    /* Eski VGA metin kalıntılarını temizle */
    clear_text_screen();
    
    /* Donanım odalarını ayağa kaldır */
    init_idt();
    init_keyboard();
    init_mouse();
    
    uint32_t refresh_counter = 0;
    int current_wait_cycles = 20;

    while (1) {
        kernel_ai_total_loops++;
        
        /* Donanım portlarını dinle */
        handle_mouse_polling(); 
        check_keyboard_pure();  
        
        /* AI ajan verilerini topla */
        int m_stress = ai_mouse_analyze_stress();
        int k_cadence = ai_keyboard_analyze_cadence();
        int central_ai_decision = ai_core_predict_scheduler(m_stress, k_cadence, kernel_ai_total_loops);
        
        refresh_counter++;
        
        /* KRİTİK MANTIK DÜZELTMESİ: 
           Ekrana basma işlemini (swap_buffers) sadece ve sadece çizim bittiği an, 
           yani if şartının tam İÇİNDE tetikliyoruz! */
        if (refresh_counter >= 8) { 
            /* 1. Arka planı gizli tamponda boya */
            gui_refresh_desktop();  
            
            /* 2. Pencereleri gizli tamponun üzerine bindir */
            run_exe_subsystem(); 
            
            /* 3. Çizim tamamen bitti! Şimdi gizli tamponu şak diye ekrana fırlat */
            swap_buffers();
            
            refresh_counter = 0;
        }
        
        /* AI Kararına göre işlemci dinlendirme döngüsü */
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
