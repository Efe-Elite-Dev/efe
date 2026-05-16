#include "screen.h"
#include "gui.h"
#include <stdint.h>

int active_window = 0; 
uint32_t system_ticks = 0;

int mouse_x = 512; 
int mouse_y = 384; 
uint8_t mouse_cycle = 0;
int8_t mouse_byte[3];

static inline uint8_t inb(uint16_t port) {
    uint8_t data;
    __asm__ __volatile__("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
}

void sky_kernel_delay(int count) {
    for (volatile int i = 0; i < count * 900; i++);
}

void handle_desktop_click(void) {
    if (active_window != 0) {
        if (mouse_x >= 722 && mouse_x <= 754 && mouse_y >= 188 && mouse_y <= 212) {
            active_window = 0; 
            gui_refresh_desktop(mouse_x, mouse_y, system_ticks);
            return;
        }
    }

    if (active_window == 0) {
        if (mouse_x >= 40 && mouse_x <= 76) {
            if (mouse_y >= 40 && mouse_y <= 76) {
                active_window = 1; 
            }
            else if (mouse_y >= 100 && mouse_y <= 136) {
                active_window = 2; 
            }
            else if (mouse_y >= 160 && mouse_y <= 196) {
                active_window = 3; 
            }
            gui_refresh_desktop(mouse_x, mouse_y, system_ticks);
        }
    }
}

void init_mouse(void) {
    outb(0x64, 0xA8);
    while((inb(0x64) & 0x02));
    outb(0x64, 0x20); 
    while(!(inb(0x64) & 0x01));
    uint8_t status = (inb(0x60) | 0x02); 
    while((inb(0x64) & 0x02));
    outb(0x64, 0x60);
    while((inb(0x64) & 0x02));
    outb(0x60, status);
    while((inb(0x64) & 0x02));
    outb(0x64, 0xD4);
    while((inb(0x64) & 0x02));
    outb(0x60, 0xF4); 
    inb(0x60); 
}

void handle_mouse_polling(void) {
    uint8_t status = inb(0x64);
    if ((status & 0x01) && (status & 0x20)) {
        uint8_t data = inb(0x60);
        mouse_byte[mouse_cycle++] = data;
        
        if (mouse_cycle == 3) { 
            mouse_cycle = 0;
            
            if (mouse_byte[0] & 0x01) {
                handle_desktop_click();
                sky_kernel_delay(10);
                return;
            }
            
            int8_t move_x = mouse_byte[1];
            int8_t move_y = mouse_byte[2];
            
            mouse_x += move_x;
            mouse_y -= move_y; 
            
            if (mouse_x < 0) mouse_x = 0;
            if (mouse_x > SCREEN_WIDTH - 5) mouse_x = SCREEN_WIDTH - 5;
            if (mouse_y < 0) mouse_y = 0;
            if (mouse_y > SCREEN_HEIGHT - 5) mouse_y = SCREEN_HEIGHT - 5;
            
            gui_refresh_desktop(mouse_x, mouse_y, system_ticks);
        }
    }
}

void kernel_main(void) {
    gui_refresh_desktop(mouse_x, mouse_y, system_ticks); 
    init_mouse();          

    while (1) {
        handle_mouse_polling();
        
        system_ticks++;
        if (system_ticks % 15 == 0) { 
            gui_refresh_desktop(mouse_x, mouse_y, system_ticks);
        }
        
        sky_kernel_delay(1); 
    }
}
