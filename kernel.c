#include "wind_subsystem.h"

/* Global Grafik Ekran Tamponu Göstergeleri */
uint32_t* vbe_vram = 0; 
uint32_t  vbe_pitch = 0;

/* Çekirdek Döngü Sayacı */
int kernel_ai_total_loops = 0;

/* İşlemcinin donanımsal mikro beklemesini yöneten emniyet halkası */
static inline void io_wait(void) {
    asm volatile ("outb %%al, $0x80" : : "a"(0));
}

/* WIND OS YAPAY ZEKA DESTEKLİ ANA GİRİŞ MOTORU */
void kernel_main(struct multiboot_info* mboot) {
    /* MULTIBOOT GRAFİK KALKANI: GRUB'ın ekran kartını ayarlayıp ayarlamadığını doğrula */
    if (mboot != 0 && (mboot->flags & (1 << 12)) && (mboot->framebuffer_addr != 0)) {
        vbe_vram = (uint32_t*)(uintptr_t)mboot->framebuffer_addr;
        vbe_pitch = mboot->framebuffer_pitch;
    } else {
        /* Eğer GRUB grafik moduna geçemezse güvenli mod varsayılan bellek adresini bağla */
        vbe_vram = (uint32_t*)0xFD000000; 
        vbe_pitch = 800 * 4;
    }

    /* Arka plandaki eski VGA metin modu kalıntı ekran belleğini temizle (screen.c) */
    clear_text_screen();
    
    /* Sürücü Odalarını Sırayla Ayağa Kaldır */
    init_idt();
    init_keyboard();
    init_mouse();
    
    /* Grafik Masaüstünün Temel Hatlarını Ekrana Çiz (gui.c) */
    gui_refresh_desktop();

    uint32_t refresh_counter = 0;
    int current_wait_cycles = 20;

    /* YAPAY ZEKA DESTEKLİ SONSUZ ORKESTRA DÖNGÜSÜ */
    while (1) {
        kernel_ai_total_loops++;
        
        /* 1. Donanım Sürücü Odalarını Kokla */
        handle_mouse_polling(); 
        check_keyboard_pure();  
        
        /* 2. Bağımsız Odaların Yapay Zeka Sonuçlarını Al */
        int m_stress = ai_mouse_analyze_stress();
        int k_cadence = ai_keyboard_analyze_cadence();
        
        /* 3. Merkezi Yapay Zekaya Gönder ve Karar Al */
        int central_ai_decision = ai_core_predict_scheduler(m_stress, k_cadence, kernel_ai_total_loops);
        
        /* 4. Ekran Grafik Arayüzünü Güncelle (15 döngüde bir tazele) */
        refresh_counter++;
        if (refresh_counter >= 15) { 
            gui_refresh_desktop();  
            run_exe_subsystem(); /* exe_subsystem OOBE Kurulum pencerelerini ekrana bas */
            refresh_counter = 0;
        }
        
        /* 5. CENTRAL AI TAHMİNLİ DİNAMİK ZAMANLAYICI (Dynamic Scheduler Lojiği) */
        if (central_ai_decision == 2) {
            /* Sistem veya kullanıcı stresi maksimumda: İşlemciyi hiç bekletme, tam güç akıt */
            current_wait_cycles = 4;   
        } else if (central_ai_decision == 1) {
            /* Standart tempo: Dengeli bekleme süresi */
            current_wait_cycles = 18;  
        } else {
            /* Kullanıcı aktif değil: İşlemciyi ve donanımı yormamak için derin güç tasarrufu beklemesi */
            current_wait_cycles = 45;  
        }

        /* Yapay zekanın belirlediği akıllı mikro gecikmeyi işlemci döngüsüne uygula */
        for (volatile int i = 0; i < current_wait_cycles; i++) {
            io_wait();
        }
    }
}
