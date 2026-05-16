#include <stdint.h>
#include "mouse.h" 

// Ekran Çözünürlüğü ve Grafik Makroları
#define SCREEN_WIDTH     800
#define SCREEN_HEIGHT    600
#define TOTAL_PIXELS     (SCREEN_WIDTH * SCREEN_HEIGHT)

// Donanım Port Tanımlamaları
#define KEYBOARD_DATA_PORT   0x60
#define KEYBOARD_STATUS_PORT 0x64

// === Windows 11 / SKY OS Fluent Renk Paleti ===
#define COLOR_BG_TOP       0xE2E7F0  
#define COLOR_BG_BOTTOM    0xCCD4E2  
#define COLOR_CARD_BG      0xFFFFFF  
#define COLOR_TEXT_MAIN    0x111625  
#define COLOR_TEXT_SUB     0x5C6479  
#define COLOR_ACCENT_BLUE  0x005A9E  
#define COLOR_TEXT_WHITE   0xFFFFFF  
#define COLOR_CURSOR       0x00A2ED  
#define COLOR_GREEN_SUCCESS 0x107C41 

int setup_completed = 0;

// Multiboot Yapısı
struct multiboot_info {
    uint32_t flags; uint32_t mem_lower; uint32_t mem_upper; uint32_t boot_device;
    uint32_t cmdline; uint32_t mods_count; uint32_t mods_addr; uint32_t num;
    uint32_t size; uint32_t addr; uint32_t shndx; uint32_t vbe_control_info;
    uint32_t vbe_mode_info; uint16_t vbe_mode; uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off; uint16_t vbe_interface_len;
    uint32_t framebuffer_addr; uint32_t framebuffer_pitch;
    uint32_t framebuffer_width; uint32_t framebuffer_height;
    uint8_t  framebuffer_type;
} __attribute__((packed));

uint32_t* vbe_vram = (uint32_t*)0xE0000000; 
uint32_t  vbe_pitch = SCREEN_WIDTH * 4; // Varsayılan pitch değeri
uint32_t  back_buffer[TOTAL_PIXELS]; 

/* Donanımdan veri okuyan Assembly köprüsü */
static inline uint8_t inb(uint16_t port) {
    uint8_t data;
    __asm__ __volatile__("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

/* Linker hatasını önleyen eski ASM köprü fonksiyonu */
void keyboard_handler_c(void) {
    volatile uint8_t status = inb(KEYBOARD_STATUS_PORT);
    if (status & 0x01) {
        volatile uint8_t scancode = inb(KEYBOARD_DATA_PORT);
        (void)scancode;
    }
}

/* Tarama Kodunu ASCII karakterine çeviren harita */
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
        case 0x39: return ' ';  
        case 0x1C: return '\n'; 
        default: return 0;      
    }
}

// Grafik Çizim Araçları (Back Buffer Tabanlı)
void draw_background_gradient(void) {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        uint8_t r1 = (COLOR_BG_TOP >> 16) & 0xFF; uint8_t g1 = (COLOR_BG_TOP >> 8) & 0xFF; uint8_t b1 = COLOR_BG_TOP & 0xFF;
        uint8_t r2 = (COLOR_BG_BOTTOM >> 16) & 0xFF; uint8_t g2 = (COLOR_BG_BOTTOM >> 8) & 0xFF; uint8_t b2 = COLOR_BG_BOTTOM & 0xFF;
        uint8_t r = r1 + ((r2 - r1) * y / SCREEN_HEIGHT);
        uint8_t g = g1 + ((g2 - g1) * y / SCREEN_HEIGHT);
        uint8_t b = b1 + ((b2 - b1) * y / SCREEN_HEIGHT);
        uint32_t color = (r << 16) | (g << 8) | b;
        for (int x = 0; x < SCREEN_WIDTH; x++) back_buffer[y * SCREEN_WIDTH + x] = color;
    }
}

void draw_rect(int start_x, int start_y, int width, int height, uint32_t color) {
    for (int y = start_y; y < start_y + height; y++) {
        for (int x = start_x; x < start_x + width; x++) {
            if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) back_buffer[y * SCREEN_WIDTH + x] = color;
        }
    }
}

void draw_circle(int xc, int yc, int r, uint32_t color) {
    for (int y = yc - r; y <= yc + r; y++) {
        for (int x = xc - r; x <= xc + r; x++) {
            if ((x - xc)*(x - xc) + (y - yc)*(y - yc) <= r*r) {
                if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) back_buffer[y * SCREEN_WIDTH + x] = color;
            }
        }
    }
}

unsigned char font_bitmap[128][16] = {
    ['S'] = {0x3C, 0x42, 0x40, 0x3C, 0x02, 0x02, 0x42, 0x42, 0x3C},
    ['K'] = {0x42, 0x44, 0x48, 0x70, 0x50, 0x48, 0x44, 0x42, 0x42},
    ['Y'] = {0x42, 0x42, 0x42, 0x24, 0x18, 0x18, 0x18, 0x18, 0x18},
    ['O'] = {0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C},
    ['B'] = {0x7C, 0x42, 0x42, 0x42, 0x7C, 0x42, 0x42, 0x42, 0x7C},
    ['u'] = {0x00, 0x00, 0x42, 0x42, 0x42, 0x46, 0x3A, 0x00, 0x00},
    ['d'] = {0x02, 0x02, 0x3E, 0x42, 0x42, 0x42, 0x3E, 0x00, 0x00},
    ['o'] = {0x00, 0x00, 0x3C, 0x42, 0x42, 0x42, 0x3C, 0x00, 0x00},
    ['g'] = {0x00, 0x00, 0x3E, 0x42, 0x42, 0x3E, 0x02, 0x3C, 0x00},
    ['r'] = {0x00, 0x00, 0x2E, 0x30, 0x20, 0x20, 0x20, 0x00, 0x00},
    ['i'] = {0x08, 0x00, 0x08, 0x08, 0x08, 0x08, 0x0C, 0x00, 0x00},
    ['l'] = {0x0C, 0x08, 0x08, 0x08, 0x08, 0x08, 0x0E, 0x00, 0x00},
    ['e'] = {0x00, 0x00, 0x3C, 0x42, 0x7E, 0x40, 0x3C, 0x00, 0x00},
    ['m'] = {0x00, 0x00, 0x5C, 0x62, 0x42, 0x42, 0x42, 0x00, 0x00},
    ['s'] = {0x00, 0x00, 0x3C, 0x40, 0x3C, 0x02, 0x3C, 0x00, 0x00},
    ['t'] = {0x08, 0x08, 0x3E, 0x08, 0x08, 0x08, 0x06, 0x00, 0x00},
    ['a'] = {0x00, 0x00, 0x3E, 0x02, 0x3E, 0x42, 0x3E, 0x00, 0x00},
    ['k'] = {0x20, 0x20, 0x24, 0x28, 0x30, 0x28, 0x24, 0x00, 0x00},
    ['v'] = {0x00, 0x00, 0x42, 0x42, 0x42, 0x24, 0x18, 0x00, 0x00},
    ['E'] = {0x7E, 0x40, 0x40, 0x76, 0x40, 0x40, 0x7E, 0x00, 0x00},
    ['T'] = {0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00},
    ['C'] = {0x3C, 0x42, 0x40, 0x40, 0x40, 0x40, 0x3C, 0x00, 0x00},
    ['M'] = {0x42, 0x66, 0x5A, 0x42, 0x42, 0x42, 0x42, 0x00, 0x00},
    ['n'] = {0x00, 0x00, 0x5C, 0x62, 0x42, 0x42, 0x42, 0x00, 0x00},
    ['z'] = {0x00, 0x00, 0x7E, 0x04, 0x08, 0x10, 0x7E, 0x00, 0x00},
    ['-'] = {0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00},
    [' '] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

void put_char(char c, int x, int y, uint32_t color) {
    if ((unsigned char)c >= 128) return;
    for (int row = 0; row < 16; row++) {
        unsigned char bits = font_bitmap[(int)c][row];
        for (int col = 0; col < 8; col++) {
            if (bits & (0x80 >> col)) {
                if (x + col >= 0 && x + col < SCREEN_WIDTH && y + row >= 0 && y + row < SCREEN_HEIGHT) {
                    back_buffer[(y + row) * SCREEN_WIDTH + (x + col)] = color;
                }
            }
        }
    }
}

void put_string(const char* s, int x, int y, uint32_t color) {
    while (*s) { put_char(*s, x, y, color); x += 8; s++; }
}

void check_keyboard(void) {
    static unsigned char last_scancode = 0;
    if (inb(KEYBOARD_STATUS_PORT) & 1) {
        unsigned char scancode = inb(KEYBOARD_DATA_PORT);
        if (scancode != last_scancode && scancode < 0x80) {
            char ascii = scancode_to_ascii(scancode);
            if (ascii == '\n') {
                setup_completed = 1;
            }
        }
        last_scancode = scancode;
    }
}

void render_interface(void) {
    draw_background_gradient();

    int card_w = 700; int card_h = 480;
    int card_x = (SCREEN_WIDTH - card_w) / 2; int card_y = (SCREEN_HEIGHT - card_h) / 2;
    draw_rect(card_x, card_y, card_w, card_h, COLOR_CARD_BG);

    int left_center_x = card_x + 160; int left_center_y = card_y + 240;
    
    if (setup_completed) {
        draw_circle(left_center_x, left_center_y, 75, COLOR_GREEN_SUCCESS);           
        draw_circle(left_center_x - 12, left_center_y + 12, 55, 0x0E6233); 
        draw_circle(left_center_x + 20, left_center_y - 20, 35, 0x1ED760); 
    } else {
        draw_circle(left_center_x, left_center_y, 75, 0x3B82F6);           
        draw_circle(left_center_x - 12, left_center_y + 12, 55, 0x1D4ED8); 
        draw_circle(left_center_x + 20, left_center_y - 20, 35, 0x6366F1); 
    }

    put_string("SKY OS OOBE Setup", card_x + 30, card_y + 30, COLOR_TEXT_SUB);

    int right_content_x = card_x + 340; int right_content_y = card_y + 60;
    
    if (setup_completed) {
        put_string("Kurulum Basariyla Tamamlandi!", right_content_x, right_content_y, COLOR_GREEN_SUCCESS);
        put_string("SKY OS Core artik kullanima hazir.", right_content_x, right_content_y + 22, COLOR_TEXT_MAIN);
        put_string("Sistem arka plan cekirdegi baslatiliyor...", right_content_x, right_content_y + 38, COLOR_TEXT_SUB);
    } else {
        put_string("Bu dogru ulke/bolge mi?", right_content_x, right_content_y, COLOR_TEXT_MAIN);
        put_string("SKY OS dil ve bolge ayarlari otomatik", right_content_x, right_content_y + 22, COLOR_TEXT_SUB);
        put_string("olarak sisteme entegre edilecektir.", right_content_x, right_content_y + 38, COLOR_TEXT_SUB);

        int item_x = right_content_x; int item_y = right_content_y + 85;
        int item_w = 320; int item_h = 36;

        put_string("Almanya", item_x + 12, item_y + 10, COLOR_TEXT_SUB);
        item_y += item_h + 6;
        put_string("Amerika Birlesik Devletleri", item_x + 12, item_y + 10, COLOR_TEXT_SUB);
        
        draw_rect(item_x, item_y, item_w, item_h, COLOR_ACCENT_BLUE);
        put_string("Turkiye (SKY OS Core)", item_x + 12, item_y + 12, COLOR_TEXT_WHITE);

        item_y += item_h + 6;
        put_string("Turkmenistan", item_x + 12, item_y + 10, COLOR_TEXT_SUB);
        item_y += item_h + 6;
        put_string("Turkce Q / F Klavye Modu", item_x + 12, item_y + 10, COLOR_TEXT_SUB);

        int btn_w = 110; int btn_h = 32;
        int btn_x = card_x + card_w - btn_w - 30; int btn_y = card_y + card_h - btn_h - 30;
        draw_rect(btn_x, btn_y, btn_w, btn_h, COLOR_ACCENT_BLUE);
        put_string("Evet", btn_x + 38, btn_y + 8, COLOR_TEXT_WHITE);
    }

    // 🖱️ FARE OKUNU EN ÜSTE ÇİZ
    for (int row = 0; row < 19; row++) {
        for (int col = 0; col < 12; col++) {
            if (mouse_pointer_sprite[row][col] == 1) {
                int px = mouse_x + col;
                int py = mouse_y + row;
                if (px >= 0 && px < SCREEN_WIDTH && py >= 0 && py < SCREEN_HEIGHT) {
                    back_buffer[py * SCREEN_WIDTH + px] = COLOR_CURSOR;
                }
            }
        }
    }

    // 🛠️ KRİTİK KOORDİNAT DÜZELTMESİ:
    // Back buffer'daki pikselleri gerçek VRAM'e aktarırken donanımın 'vbe_pitch' hizalamasını kullanıyoruz!
    uint32_t pixels_per_pitch = vbe_pitch / 4; 
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            vbe_vram[y * pixels_per_pitch + x] = back_buffer[y * SCREEN_WIDTH + x];
        }
    }
}

// Ana Çekirdek Girişi
void kernel_main(struct multiboot_info* mboot) {
    // Multiboot yapısından donanımsal LFB (Linear Framebuffer) bilgilerini çek
    if (mboot != 0) {
        if (mboot->framebuffer_addr != 0) {
            vbe_vram = (uint32_t*)(uintptr_t)mboot->framebuffer_addr;
            vbe_pitch = mboot->framebuffer_pitch; // Ekran kartının gerçek satır genişliği!
        }
    }

    init_mouse();          
    render_interface();

    uint32_t refresh_counter = 0;
    while (1) {
        handle_mouse_polling(); 
        check_keyboard();       
        
        refresh_counter++;
        if (refresh_counter >= 150) { 
            render_interface(); 
            refresh_counter = 0;
        }
        
        for (volatile int i = 0; i < 50; i++); 
    }
}
