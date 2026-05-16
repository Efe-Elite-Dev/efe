#include <stdint.h>

// ====================================================================
// 1. MODERN SİSTEM TANIMLAMALARI VE VRAM AYARLARI
// ====================================================================
#define VBE_START_ADDR  0xE0000000 
#define SCREEN_WIDTH    1024
#define SCREEN_HEIGHT   768

#define COLOR_TASKBAR          0xCCF2F7FF  
#define COLOR_SEARCH_BAR       0xFFFFFFFF  
#define COLOR_MODERN_CURSOR    0x0067C0    
#define COLOR_WIN_TITLE        0x0F172A    
#define COLOR_MODERN_RED       0xE81123    

int active_window = 0; 
uint32_t system_ticks = 0;

int mouse_x = 512; 
int mouse_y = 384; 
uint8_t mouse_cycle = 0;
int8_t mouse_byte[3];

// ====================================================================
// 2. OPTİMİZE GRAFİK MOTORU (BOYUTU KÜÇÜLTEN YAPI)
// ====================================================================

void gui_put_pixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    uint32_t *vbe_buffer = (uint32_t*)VBE_START_ADDR;
    vbe_buffer[y * SCREEN_WIDTH + x] = color;
}

uint32_t mix_colors(uint32_t bg, uint32_t fg, uint8_t alpha) {
    uint8_t bg_r = (bg >> 16) & 0xFF; uint8_t bg_g = (bg >> 8) & 0xFF; uint8_t bg_b = bg & 0xFF;
    uint8_t fg_r = (fg >> 16) & 0xFF; uint8_t fg_g = (fg >> 8) & 0xFF; uint8_t fg_b = fg & 0xFF;

    uint8_t res_r = ((fg_r * alpha) + (bg_r * (255 - alpha))) / 255;
    uint8_t res_g = ((fg_g * alpha) + (bg_g * (255 - alpha))) / 255;
    uint8_t res_b = ((fg_b * alpha) + (bg_b * (255 - alpha))) / 255;

    return (res_r << 16) | (res_g << 8) | res_b;
}

void gui_draw_rect_alpha(int start_x, int start_y, int width, int height, uint32_t color, uint8_t alpha) {
    uint32_t *vbe_buffer = (uint32_t*)VBE_START_ADDR;
    for (int y = start_y; y < start_y + height; y++) {
        for (int x = start_x; x < start_x + width; x++) {
            if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
                uint32_t bg_color = vbe_buffer[y * SCREEN_WIDTH + x];
                vbe_buffer[y * SCREEN_WIDTH + x] = mix_colors(bg_color, color, alpha);
            }
        }
    }
}

void gui_draw_rect(int start_x, int start_y, int width, int height, uint32_t color) {
    for (int y = start_y; y < start_y + height; y++) {
        for (int x = start_x; x < start_x + width; x++) {
            gui_put_pixel(x, y, color);
        }
    }
}

void gui_draw_rounded_window(int sx, int sy, int w, int h, int r, uint32_t color, uint8_t alpha) {
    uint32_t *vbe_buffer = (uint32_t*)VBE_START_ADDR;
    for (int y = sy; y < sy + h; y++) {
        for (int x = sx; x < sx + w; x++) {
            int dl_x = x - (sx + r); int dr_x = x - (sx + w - r - 1);
            int dt_y = y - (sy + r); int db_y = y - (sy + h - r - 1);

            int is_corner = 0;
            if (x < sx + r && y < sy + r && (dl_x*dl_x + dt_y*dt_y > r*r)) is_corner = 1; 
            if (x > sx + w - r - 1 && y < sy + r && (dr_x*dr_x + dt_y*dt_y > r*r)) is_corner = 1; 
            if (x < sx + r && y > sy + h - r - 1 && (dl_x*dl_x + db_y*db_y > r*r)) is_corner = 1; 
            if (x > sx + w - r - 1 && y > sy + h - r - 1 && (dl_x*dl_x + db_y*db_y > r*r)) is_corner = 1; 

            if (!is_corner && x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
                uint32_t bg_color = vbe_buffer[y * SCREEN_WIDTH + x];
                vbe_buffer[y * SCREEN_WIDTH + x] = mix_colors(bg_color, color, alpha);
            }
        }
    }
}

// ====================================================================
// 3. MATEMATİKSEL DUVAR KAĞIDI VE BİLEŞENLER
// ====================================================================

void gui_render_wallpaper(void) {
    // Döngü içi ağır çarpanları azaltarak binary boyutunu düşürdük
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            uint8_t r = 30 + (x / 25) + (y / 12);  
            uint8_t g = 50 + (x / 50) + (y / 25);  
            uint8_t b = 120 + (x / 10) - (y / 38); 
            gui_put_pixel(x, y, (r << 16) | (g << 8) | b);
        }
    }
}

void gui_draw_icons(void) {
    gui_draw_rounded_window(40, 40, 36, 36, 6, 0x0078D4, 255);
    gui_draw_rect(48, 50, 20, 10, 0xFFFFFF); 
    
    gui_draw_rounded_window(40, 100, 36, 36, 6, 0xA8193D, 255); 
    gui_draw_rect(52, 108, 12, 20, 0xE0E0E0); 
    
    gui_draw_rounded_window(40, 160, 36, 36, 6, 0xFFB900, 255); 
    gui_draw_rect(50, 168, 16, 16, 0x111111); 
}

void gui_draw_window_frame(int win_x, int win_y, int win_w, int win_h) {
    gui_draw_rect_alpha(win_x + 8, win_y + 8, win_w, win_h, 0x000000, 80);
    gui_draw_rounded_window(win_x, win_y, win_w, win_h, 12, 0xFFFFFF, 220); 
    gui_draw_rounded_window(win_x, win_y, win_w, 32, 12, COLOR_WIN_TITLE, 240);
    gui_draw_rect(win_x, win_y + 20, win_w, 12, COLOR_WIN_TITLE); 
    gui_draw_rounded_window(win_x + win_w - 40, win_y + 4, 32, 24, 6, COLOR_MODERN_RED, 255);
}

void gui_render_taskbar(uint32_t tick_count) {
    gui_draw_rect_alpha(0, 718, SCREEN_WIDTH, 50, 0xEEEEEE, 180);
    
    int search_bar_x = (SCREEN_WIDTH / 2) - 150; 
    gui_draw_rounded_window(search_bar_x, 724, 300, 36, 8, 0xFFFFFF, 255);
    
    int start_icon_x = search_bar_x - 100;
    gui_draw_rounded_window(start_icon_x, 726, 32, 32, 6, 0x0078D4, 255);      
    gui_draw_rounded_window(start_icon_x + 40, 726, 32, 32, 6, 0xFFB900, 255); 
    gui_draw_rounded_window(start_icon_x + 80, 726, 32, 32, 6, 0x107C41, 255); 

    if ((tick_count / 20) % 2 == 0) {
        gui_draw_rounded_window(950, 734, 18, 18, 4, 0x0067C0, 255); 
    } else {
        gui_draw_rounded_window(950, 734, 18, 18, 4, 0x7F8C8D, 255); 
    }
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

// ====================================================================
// 4. ALTSİSTEMLER VE POLLED SEÇENEKLER
// ====================================================================

void run_exe_subsystem(int win_x, int win_y) {
    gui_draw_rect(win_x + 15, win_y + 45, 470, 290, 0xF3F3F3);
    gui_draw_rect(win_x + 50, win_y + 80, 22, 22, 0x0078D4); 
    gui_draw_rect(win_x + 75, win_y + 80, 22, 22, 0x0078D4); 
    gui_draw_rect(win_x + 50, win_y + 105, 22, 22, 0x0078D4); 
    gui_draw_rect(win_x + 75, win_y + 105, 22, 22, 0x0078D4); 
}

void run_deb_subsystem(int win_x, int win_y) {
    gui_draw_rect(win_x + 15, win_y + 45, 470, 290, 0x212121);
    gui_draw_rect(win_x + 40, win_y + 70, 40, 40, 0xD70A53);
}

void run_sky_subsystem(int win_x, int win_y, uint32_t current_tick) {
    gui_draw_rect(win_x + 15, win_y + 45, 470, 290, 0x0F172A);
    gui_draw_rect(win_x + 35, win_y + 65, 30, 30, 0xFFB900);
    
    // AI Render görsel döngüsünü optimize ettik (Şişmeyi önlemek için tek döngü)
    int pulse = (current_tick / 5) % 8;
    gui_draw_rect(win_x + 20, win_y + 110, 460, 220, 0x1E1B4B); 
    for (int i = 0; i < 8; i++) {
        int h = 20 + (i * pulse) % 40;
        gui_draw_rect(win_x + 50 + (i * 45), win_y + 240 - h, 15, h, 0x8B5CF6);
    }
}

void gui_refresh_desktop(int mx, int my, uint32_t tick) {
    gui_render_wallpaper();
    gui_draw_icons();
    
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

// ====================================================================
// 5. HARDWARE DRIVERS & LOOP
// ====================================================================

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
            if (mouse_y >= 40 && mouse_y <= 76) active_window = 1; 
            else if (mouse_y >= 100 && mouse_y <= 136) active_window = 2; 
            else if (mouse_y >= 160 && mouse_y <= 196) active_window = 3; 
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
