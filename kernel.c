#include "wind_subsystem.h"
#include "mouse.h"
#include "gui.h"
#include "keyboard.h"
#include "screen.h"
#include "idt.h"
#include "exe_subsystem.h"
#include "ai_subsystem.h"

uint32_t* vbe_vram = 0; 
uint32_t  vbe_pitch = 0;

static inline void io_wait(void) {
    asm volatile ("outb %%al, $0x80" : : "a"(0));
}

void kernel_main(struct multiboot_info* mboot) {
    /* GURU KORUMASI: Multiboot Grafik Doğrulama Kalkanı */
    if (mboot != 0 && (mboot->flags & (1 << 12)) && (mboot->framebuffer_addr != 0)) {
        vbe_vram = (uint32_t*)(uintptr_t)mboot->framebuffer_addr;
        vbe_pitch = mboot->framebuffer_pitch;
    } else {
        vbe_vram = (uint32_t*)0xFD000000; 
        vbe_pitch = 800 * 4;
    }

    /* Donanım Katmanlarını Sırayla Ayağa Kaldır */
    clear_text_screen();
    init_idt();
    init_keyboard();
    init_mouse();
    
    gui_refresh_desktop();

    uint32_t refresh_counter = 0;
    int current_wait_cycles = 20;

    /* ANA ÇEKİRDEK DÖNGÜSÜ */
    while (1) {
        handle_mouse_polling(); 
        
        refresh_counter++;
        if (refresh_counter >= 15) { 
            gui_refresh_desktop();  
            run_exe_subsystem(); 
            refresh_counter = 0;
        }
        
        int system_stress = ai_predict_hardware_load(7, refresh_counter);
        current_wait_cycles = (system_stress == 1) ? 45 : 10;

        for (volatile int i = 0; i < current_wait_cycles; i++) {
            io_wait();
        }
    }
}
