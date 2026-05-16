#include <stdint.h>

// ====================================================================
// 1. %100 UYUMLU GÜVENLİ GRAFİK MODU AYARLARI
// ====================================================================
#define VBE_START_ADDR   0xE0000000 
#define SCREEN_WIDTH     800
#define SCREEN_HEIGHT    600
#define TOTAL_PIXELS     (SCREEN_WIDTH * SCREEN_HEIGHT)

// Windows 11 / Akıcı Arayüz Renk Paleti (Hex Format)
#define COLOR_BG         0x0A0F24  
#define COLOR_TASKBAR    0x1A2342  
#define COLOR_ACCENT     0x0078D4  
#define COLOR_WIN_BG     0xFFFFFF  
#define COLOR_WIN_TITLE  0x0F172A  
#define COLOR_MODERN_RED 0xE81123  
#define COLOR_CURSOR     0x00A2ED  
#define COLOR_TEXT_WHITE 0xFFFFFF
#define COLOR_TEXT_DARK  0x1E293B

// Sistem Durum Değişkenleri
int active_window = 0; 
uint32_t system_ticks = 0;

int mouse_x = 400; // Ekran ortasında başla
int mouse_y = 300; 
uint8_t mouse_cycle = 0;
int8_t mouse_byte[3];

// Sürükleyebileceğimiz Örnek Uygulama Penceresi Yapısı
typedef struct {
    int x; int y; int width; int height;
    int is_dragging; int drag_offset_x; int drag_offset_y;
} Window;

Window app_window = {175, 120, 450, 320, 0, 0, 0};

// ====================================================================
// 2. 8x16 BITMAP FONT TANIMLAMASI (HATAYI ÇÖZEN GERÇEK MATRİS)
// ====================================================================
// Linker'ın bulamadığı font_bitmap dizisini buraya doğrudan tanımlıyoruz.
unsigned char font_bitmap[128][16] = {
    ['A'] = {0x18, 0x24, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['B'] = {0x7C, 0x42, 0x42, 0x42, 0x7C, 0x42, 0x42, 0x42, 0x7C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['C'] = {0x3C, 0x42, 0x42, 0x40, 0x40, 0x40, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['D'] = {0x78, 0x44, 0x42, 0x42, 0x42, 0x42, 0x42, 0x44, 0x78, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['E'] = {0x7E, 0x40, 0x40, 0x40, 0x76, 0x40, 0x40, 0x40, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['I'] = {0x3C, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['K'] = {0x42, 0x44, 0x48, 0x70, 0x50, 0x48, 0x44, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['L'] = {0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['M'] = {0x42, 0x66, 0x5A, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['N'] = {0x42, 0x62, 0x52, 0x4A, 0x46, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['O'] = {0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['P'] = {0x7C, 0x42, 0x42, 0x42, 0x7C, 0x40, 0x40, 0x40, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['R'] = {0x7C, 0x42, 0x42, 0x42, 0x7C, 0x48, 0x44, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['S'] = {0x3C, 0x42, 0x40, 0x3C, 0x02, 0x02, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['T'] = {0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['U'] = {0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['W'] = {0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x5A, 0x66, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['X'] = {0x42, 0x42, 0x24, 0x18, 0x18, 0x24, 0x42, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['Y'] = {0x42, 0x42, 0x42, 0x24, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['-'] = {0x00, 0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['>'] = {0x40, 0x20, 0x10, 0x08, 0x04, 0x08, 0x10, 0x20, 0x40, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['_'] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7E, 0x00},
    [' '] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

// ====================================================================
// 3. GRAFİK VE EKRAN RENDER MOTORU
// ====================================================================
uint32_t back_buffer[TOTAL_PIXELS];

void gui_put_pixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    back_buffer[y * SCREEN_WIDTH + x] = color;
}

// Karakter basma motoru artık buradaki font_bitmap dizisini doğrudan okuyacak
void put_char(char c, int x, int y, uint32_t color) {
    if (c < 0 || c >= 128) return;
    for (int row = 0; row < 16; row++) {
        unsigned char bits = font_bitmap[(int)c][row];
        for (int col = 0; col < 8; col++) {
            if (bits & (0x80 >> col)) {
                gui_put_pixel(x + col, y + row, color);
            }
        }
    }
}

void gui_refresh_desktop(int mx, int my, uint32_t tick) {
    // Masaüstü arka plan çizimi (Dikey çizgi bug'ını önleyen tasarım)
    for (int i = 0; i < TOTAL_PIXELS; i++) {
        back_buffer[i] = COLOR_BG;
    }
    
    // İmleci ve sistem tik sayacını simüle et
    put_char('S', mx, my, COLOR_CURSOR);
    put_char('K', mx + 8, my, COLOR_CURSOR);
    put_char('Y', mx + 16, my, COLOR_CURSOR);
    
    // Arka belleği gerçek ekran kartı VRAM'ine boşalt (Flush)
    uint32_t *vram = (uint32_t*)VBE_START_ADDR;
    for (int i = 0; i < TOTAL_PIXELS; i++) {
        vram[i] = back_buffer[i];
    }
}

// ====================================================================
// 4. DONANIM SÜRÜCÜLERİ VE INTERRUPT BAĞLANTILARI
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
    for (volatile int i = 0; i < count * 800; i++);
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
            int8_t move_x = mouse_byte[1];
            int8_t move_y = mouse_byte[2];
            
            mouse_x += move_x;
            mouse_y -= move_y; 
            
            if (mouse_x < 0) mouse_x = 0;
            if (mouse_x > SCREEN_WIDTH - 6) mouse_x = SCREEN_WIDTH - 6;
            if (mouse_y < 0) mouse_y = 0;
            if (mouse_y > SCREEN_HEIGHT - 6) mouse_y = SCREEN_HEIGHT - 6;
            
            gui_refresh_desktop(mouse_x, mouse_y, system_ticks);
        }
    }
}

extern void init_idt(void); // idt.c'deki fonksiyonu çağırabilmek için dış referans

// ====================================================================
// 5. ANA KERNEL BAŞLANGIÇ NOKTASI (MAIN LOOP)
// ====================================================================
void kernel_main(void) {
    init_idt();            // IDT Tablosunu yükle
    gui_refresh_desktop(mouse_x, mouse_y, system_ticks); 
    init_mouse();          // Fareyi hazırla

    while (1) {
        handle_mouse_polling();
        system_ticks++;
        
        if (system_ticks % 250 == 0) { 
            gui_refresh_desktop(mouse_x, mouse_y, system_ticks);
        }
        sky_kernel_delay(1); 
    }
}

// ====================================================================
// 6. KEYBOARD.C İÇİN KLAVYE HARF BASMA KÖPRÜSÜ
// ====================================================================
void sky_put_char(char c) {
    static int text_x = 40;
    static int text_y = 500;
    put_char(c, text_x, text_y, COLOR_TEXT_WHITE);
    text_x += 8;
    if(text_x > 740) { text_x = 40; text_y += 16; }
}
