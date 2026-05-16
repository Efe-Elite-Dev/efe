#include <stdint.h>

// Mevcut tüm ekran ayarların ve değişkenlerin (Aynen korunuyor)
#define VBE_START_ADDR   0xE0000000 
#define SCREEN_WIDTH     800
#define SCREEN_HEIGHT    600
#define TOTAL_PIXELS     (SCREEN_WIDTH * SCREEN_HEIGHT)

#define COLOR_BG         0x0A0F24  
#define COLOR_TASKBAR    0x1A2342  
#define COLOR_ACCENT     0x0078D4  
#define COLOR_WIN_BG     0xFFFFFF  
#define COLOR_WIN_TITLE  0x0F172A  
#define COLOR_MODERN_RED 0xE81123  
#define COLOR_CURSOR     0x00A2ED  
#define COLOR_TEXT_WHITE 0xFFFFFF
#define COLOR_TEXT_DARK  0x1E293B

int active_window = 0; 
uint32_t system_ticks = 0;
int mouse_x = 400; 
int mouse_y = 300; 
uint8_t mouse_cycle = 0;
int8_t mouse_byte[3];

typedef struct {
    int x; int y; int width; int height;
    int is_dragging; int drag_offset_x; int drag_offset_y;
} Window;

Window app_window = {175, 120, 450, 320, 0, 0, 0};

// Karakter ve çizim motorların (Aynen kalıyor)
extern unsigned char font_bitmap[128][16]; 
uint32_t back_buffer[TOTAL_PIXELS];

void gui_put_pixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    back_buffer[y * SCREEN_WIDTH + x] = color;
}

// Burası senin ekrana karakter basmak için kullandığın asıl fonksiyon
void put_char(char c, int x, int y, uint32_t color) {
    if (c < 0 || c >= 128) return;
    for (int row = 0; row < 16; row++) {
        unsigned char bits = font_bitmap[(int)c][row];
        for (int col = 0; col < 8; col++) {
            if (bits & (0x80 >> col)) gui_put_pixel(x + col, y + row, color);
        }
    }
}

void gui_refresh_desktop(int mx, int my, uint32_t tick) {
    // Senin mevcut tüm masaüstü, wallpaper, taskbar çizim kodların...
    gui_put_pixel(mx, my, COLOR_CURSOR); // Örnek çizim
    // vga_flush() çağrıların vs...
}

void handle_mouse_polling(void) {
    // Fare okuma algoritmaların...
}

void init_mouse(void) {
    // PS/2 Fare register ayarların...
}

extern void init_idt(void); // idt.c'yi başlatmak için dış çağrı

// ANA ÇEKİRDEK LOOPU
void kernel_main(void) {
    init_idt(); // Kesme tablosunu (IDT) işlemciye bildiriyoruz
    init_mouse(); 
    
    while (1) {
        handle_mouse_polling();
        system_ticks++;
        if (system_ticks % 250 == 0) { 
            gui_refresh_desktop(mouse_x, mouse_y, system_ticks);
        }
    }
}

// ====================================================================
// LINKER HATASINI ÇÖZEN SİHİRLİ KÖPRÜ (HİÇBİR ŞEYİ SİLMEDEN EKLENDİ)
// ====================================================================
// keyboard.c içindeki fonksiyonlar bu ismi arıyordu, onları kendi 
// put_char fonksiyonuna güvenle yönlendiriyoruz.
void sky_put_char(char c) {
    // Statik koordinatlarla ekrana bas veya kendi text/screen sistemine pasla
    static int text_x = 50;
    static int text_y = 400;
    put_char(c, text_x, text_y, COLOR_TEXT_WHITE);
    text_x += 8;
    if(text_x > 700) { text_x = 50; text_y += 16; }
}
