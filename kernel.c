#include "wind_subsystem.h"
#include "mouse.h"
#include "gui.h"
#include "keyboard.h"
#include "screen.h"
#include "idt.h"
#include "exe_subsystem.h"
#include "ai_subsystem.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

/* DIŞ MODÜL KÖPRÜLERİ: Diğer odalardan (mouse.c vb.) gelen donanım lojikleri */
extern void handle_mouse_polling(void); 

/* Global Ekran Tamponu Tanımlamaları */
uint32_t* vbe_vram = 0; 
uint32_t  vbe_pitch = 0;

/* İşlemcinin mikro gecikmelerini yöneten emniyet halkası */
static inline void io_wait(void) {
    asm volatile ("outb %%al, $0x80" : : "a"(0));
}

/* Klavye Tarama Kodu (Scan Code) -> Grafik Arayüz ASCII Haritası */
char scancode_to_ascii(unsigned char scancode) {
    switch(scancode) {
        case 0x1E: return 'A'; case 0x30: return 'B'; case 0x2E: return 'C';
        case 0x20: return 'D'; case 0x12: return 'E'; case 0x21: return 'F';
        case 0x22: return 'G'; case 0x23: return 'H'; case 0x17: return 'I';
        case 0x24: return 'J'; case 0x25: return 'K'; case 0x26: return 'L';
        case 0x32: return 'M'; case 0x31: return 'N'; case 0x18: return 'O';
        case 0x19: return 'P'; case 0x10: return 'Q'; case 0x13: return 'R';
        case 0x1F: return 'S'; case 0x14: return 'T'; case 0x16: return 'U';
        case 0x2F: return 'V'; case 0x11: return 'W'; case 0x2D: return 'X';
        case 0x15: return 'Y'; case 0x2C: return 'Z';
        case 0x39: return ' ';  /* Boşluk (Space) */
        case 0x1C: return '\n'; /* Enter */
        default: return 0;
    }
}

/* Klavyeyi donanımsal porttan polled (nöbetçi) yöntemiyle dinleyen fonksiyon */
void check_keyboard_pure(void) {
    static unsigned char last_scancode = 0;
    
    /* inb fonksiyonu wind_subsystem.h içinden çakışmasız olarak çekiliyor */
    if (inb(KEYBOARD_STATUS_PORT) & 1) {
        unsigned char scancode = inb(KEYBOARD_DATA_PORT);
        
        /* Sadece tuşa basıldığında tetiklen (Elini çekince pas geç) */
        if (scancode != last_scancode && scancode < 0x80) {
            char ascii = scancode_to_ascii(scancode);
            if (ascii != 0) {
                /* Grafik OOBE modüllerine font/yazı beslemesi buraya bağlanacak */
            }
        }
        last_scancode = scancode;
    }
}

/* WIND OS SAF KERNEL ANA GİRİŞ KAPISI */
void kernel_main(struct multiboot_info* mboot) {
    /* GURU KORUMASI: Multiboot Grafik Doğrulama Kalkanı */
    if (mboot != 0 && (mboot->flags & (1 << 12)) && (mboot->framebuffer_addr != 0)) {
        vbe_vram = (uint32_t*)(uintptr_t)mboot->framebuffer_addr;
        vbe_pitch = mboot->framebuffer_pitch;
    } else {
        /* Güvenli mod varsayılan donanım adresi */
        vbe_vram = (uint32_t*)0xFD000000; 
        vbe_pitch = 800 * 4;
    }

    /* Arka plandaki eski VGA metin bellek kalıntılarını temizle (screen.c) */
    clear_text_screen();
    
    /* Sürücü Odalarını ve Donanım Katmanlarını Sırayla Ayağa Kaldır */
    init_idt();
    init_keyboard();
    init_mouse();
    
    /* Grafik Masaüstü Temelini İlk Kez Çiz (gui.c) */
    gui_refresh_desktop();

    uint32_t refresh_counter = 0;
    int current_wait_cycles = 20;

    /* ANA ÇEKİRDEK ORKESTRA DÖNGÜSÜ: İşlemci durmaksızın sürücüleri tarar */
    while (1) {
        handle_mouse_polling(); /* mouse.c dosyasındaki fare odasını dürt */
        check_keyboard_pure();  /* Klavyeden gelen ham verileri dinle */
        
        refresh_counter++;
        if (refresh_counter >= 15) { 
            gui_refresh_desktop();  
            run_exe_subsystem(); /* exe_subsystem.c içindeki pencereleri ekrana bas */
            refresh_counter = 0;
        }
        
        /* Yapay zeka zamanlayıcısı ile donanım yükünü tahmin et (ai_subsystem.c) */
        int system_stress = ai_predict_hardware_load(7, refresh_counter);
        current_wait_cycles = (system_stress == 1) ? 45 : 10;

        for (volatile int i = 0; i < current_wait_cycles; i++) {
            io_wait();
        }
    }
}
