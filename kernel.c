#ifndef GUI_H
#define GUI_H

#include <stdint.h>
#include "exe_subsystem.h"
#include "deb_subsystem.h"
#include "sky_subsystem.h"

#define VBE_START_ADDR  0xE0000000 
#define SCREEN_WIDTH    1024
#define SCREEN_HEIGHT   768

#define COLOR_TASKBAR          0xEEF2F7  
#define COLOR_SEARCH_BAR       0xFFFFFF  
#define COLOR_MODERN_CURSOR    0x0067C0  
#define COLOR_WIN_TITLE        0x0067C0  
#define COLOR_MODERN_RED       0xE81123  

extern int active_window;

void gui_put_pixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    uint32_t *vbe_buffer = (uint32_t*)VBE_START_ADDR;
    vbe_buffer[y * SCREEN_WIDTH + x] = color;
}

void gui_draw_rect(int start_x, int start_y, int width, int height, uint32_t color) {
    for (int y = start_y; y < start_y + height; y++) {
        for (int x = start_x; x < start_x + width; x++) {
            gui_put_pixel(x, y, color);
        }
    }
}

void gui_render_wallpaper(void) {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            uint8_t r = 30 + ((x * 40) / SCREEN_WIDTH) + ((y * 60) / SCREEN_HEIGHT);  
            uint8_t g = 50 + ((x * 20) / SCREEN_WIDTH) + ((y * 30) / SCREEN_HEIGHT);  
            uint8_t b = 120 + ((x * 100) / SCREEN_WIDTH) - ((y * 20) / SCREEN_HEIGHT); 
            if (x > 300 && x < 700 && y > 200 && y < 550) {
                r += 40; g += 25; b += 30;
            }
            gui_put_pixel(x, y, (r << 16) | (g << 8) | b);
        }
    }
}

void gui_draw_icons(void) {
    gui_draw_rect(40, 40, 32, 32, 0x0078D4);  // exe
    gui_draw_rect(46, 52, 20, 8, 0xFFFFFF); 
    
    gui_draw_rect(40, 100, 32, 32, 0xA8193D); // deb
    gui_draw_rect(50, 106, 12, 20, 0xE0E0E0); 
    
    gui_draw_rect(40, 160, 32, 32, 0xFFB900); // sky
    gui_draw_rect(48, 168, 16, 16, 0x111111); 
}

// Ana Pencere Kapsayıcı Tasarımı
void gui_draw_window_frame(int win_x, int win_y, int win_w, int win_h) {
    gui_draw_rect(win_x, win_y, win_w, win_h, 0xFFFFFF); // Beyaz Çerçeve
    gui_draw_rect(win_x, win_y, win_w, 30, COLOR_WIN_TITLE); // Başlık Barı
    gui_draw_rect(win_x + win_w - 35, win_y, 35, 30, COLOR_MODERN_RED); // Kapatma Butonu
}

void gui_render_taskbar(uint32_t tick_count) {
    gui_draw_rect(0, 718, SCREEN_WIDTH, 50, COLOR_TASKBAR);
    int search_bar_x = (SCREEN_WIDTH / 2) - 150; 
    gui_draw_rect(search_bar_x, 724, 300, 36, COLOR_SEARCH_BAR);
    
    int start_icon_x = search_bar_x - 100;
    gui_draw_rect(start_icon_x, 726, 32, 32, 0x0078D4);      
    gui_draw_rect(start_icon_x + 40, 726, 32, 32, 0xFFB900); 
    gui_draw_rect(start_icon_x + 80, 726, 32, 32, 0x107C41); 

    if ((tick_count / 20) % 2 == 0) gui_draw_rect(950, 734, 16, 16, 0x0067C0); 
    else gui_draw_rect(950, 734, 16, 16, 0x7F8C8D); 
}

void gui_draw_mouse(int mx, int my) {
    uint32_t c = COLOR_MODERN_CURSOR; uint32_t w = 0xFFFFFF; 
    gui_put_pixel(mx, my, c);
    gui_put_pixel(mx, my + 1, c);   gui_put_pixel(mx + 1, my + 1, c);
    gui_put_pixel(mx, my + 2, c);   gui_put_pixel(mx + 1, my + 2, w); gui_put_pixel(mx + 2, my + 2, c);
    gui_put_pixel(mx, my + 3, c);   gui_put_pixel(mx + 1, my + 3, w); gui_put_pixel(mx + 2, my + 3, w); gui_put_pixel(mx + 3, my + 3, c);
    gui_put_pixel(mx, my + 4, c);   gui_put_pixel(mx + 1, my + 4, w); gui_put_pixel(mx + 2, my + 4, c);
    gui_put_pixel(mx, my + 5, c);   gui_put_pixel(mx + 1, my + 5, c);
}

// Alt Sistemleri Akıllıca Çağıran Refresh Mekanizması
static inline void gui_refresh_desktop(int mx, int my, uint32_t tick) {
    gui_render_wallpaper();
    gui_draw_icons();
    
    // Pencere açıksa önce dış iskeleti, sonra ait olduğu alt sistemi çiz
    if (active_window != 0) {
        int wx = 262, wy = 184;
        gui_draw_window_frame(wx, wy, 500, 350);
        
        if (active_window == 1)      run_exe_subsystem(wx, wy);
        else if (active_window == 2) run_deb_subsystem(wx, wy);
        else if (active_window == 3) run_sky_subsystem(wx, wy, tick);
    }
    
    gui_render_taskbar(tick);
    gui_draw_mouse(mx, my);
}

#endif
