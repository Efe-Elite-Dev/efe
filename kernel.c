#include "sky_subsystem.h"
#include "mouse.h"

// gui.c veya screen.c içindeki arayüzü tetiklemek için extern fonksiyon
extern void render_interface(void);

void keyboard_handler_c(void) {
    if (inb(0x64) & 0x01) {
        volatile uint8_t scancode = inb(0x60);
        if (scancode == 0x1C) { // Enter tuşu
            setup_completed = 1;
        }
    }
}

void kernel_main(struct multiboot_info* mboot) {
    if (mboot != 0 && mboot->framebuffer_addr != 0) {
        vbe_vram = (uint32_t*)(uintptr_t)mboot->framebuffer_addr;
        vbe_pitch = mboot->framebuffer_pitch;
    }

    init_mouse();
    
    // İlk arayüz çizimi (gui.c içindeki render fonksiyonunu çağırır)
    render_interface();

    uint32_t refresh_counter = 0;
    while (1) {
        handle_mouse_polling();
        keyboard_handler_c();
        
        refresh_counter++;
        if (refresh_counter >= 150) {
            render_interface();
            refresh_counter = 0;
        }
        
        for (volatile int i = 0; i < 50; i++);
    }
}
