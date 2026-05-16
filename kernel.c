#include "sky_subsystem.h"
#include "mouse.h"

// gui.c içindeki masaüstü tazeleme ve başlatma fonksiyonu
extern void gui_refresh_desktop(void);

void kernel_main(struct multiboot_info* mboot) {
    if (mboot != 0 && mboot->framebuffer_addr != 0) {
        vbe_vram = (uint32_t*)(uintptr_t)mboot->framebuffer_addr;
        vbe_pitch = mboot->framebuffer_pitch;
    }

    // Fareyi başlat
    init_mouse();
    
    // İlk masaüstü grafik arayüzünü çiz
    gui_refresh_desktop();

    uint32_t refresh_counter = 0;
    while (1) {
        handle_mouse_polling();
        
        refresh_counter++;
        if (refresh_counter >= 150) {
            gui_refresh_desktop();
            refresh_counter = 0;
        }
        
        // İşlemciyi çok yormamak için minik gecikme (bare-metal delay)
        for (volatile int i = 0; i < 50; i++);
    }
}
