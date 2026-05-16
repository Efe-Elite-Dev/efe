#include <stdint.h>

#define SCREEN_WIDTH     800
#define SCREEN_HEIGHT    600
#define TOTAL_PIXELS     (SCREEN_WIDTH * SCREEN_HEIGHT)

// Windows 11 Fluent Renk Paleti
#define COLOR_BG         0x0A0F24  
#define COLOR_TASKBAR    0x1A2342  
#define COLOR_CURSOR     0x00A2ED  // Parlak Mavi Fare Rengi
#define COLOR_TEXT_WHITE 0xFFFFFF

// Multiboot Protokol Şeması
struct multiboot_info {
    uint32_t flags; uint32_t mem_lower; uint32_t mem_upper; uint32_t boot_device;
    uint32_t cmdline; uint32_t mods_count; uint32_t mods_addr; uint32_t num;
    uint32_t size; uint32_t addr; uint32_t shndx; uint32_t vbe_control_info;
    uint32_t vbe_mode_info; uint16_t vbe_mode; uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off; uint16_t vbe_interface_len;
    uint32_t framebuffer_addr; uint32_t framebuffer_pitch;
    uint32_t framebuffer_width; uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp; uint8_t  framebuffer_type;
} __attribute__((packed));

// Global Sürücü Değişkenleri
uint32_t* vbe_vram = (uint32_t*)0xE0000000; 
uint32_t  vbe_pitch = SCREEN_WIDTH * 4; 
uint32_t  system_ticks = 0;

// Fare Pozisyonu ve Tamponu
int       mouse_x = 400; 
int       mouse_y = 300; 
uint8_t   mouse_cycle = 0;
int8_t    mouse_byte[3];

// 12x19 Boyutunda Klasik İşletim Sistemi Fare Ok İmleci Matrisi (0: Şeffaf, 1: Mavi Cursor)
const uint8_t mouse_pointer_sprite[19][12] = {
    {1,0,0,0,0,0,0,0,0,0,0,0},
    {1,1,0,0,0,0,0,0,0,0,0,0},
    {1,1,1,0,0,0,0,0,0,0,0,0},
    {1,1,1,1,0,0,0,0,0,0,0,0},
    {1,1,1,1,1,0,0,0,0,0,0,0},
    {1,1,1,1,1,1,0,0,0,0,0,0},
    {1,1,1,1,1,1,1,0,0,0,0,0},
    {1,1,1,1,1,1,1,1,0,0,0,0},
    {1,1,1,1,1,1,1,1,1,0,0,0},
    {1,1,1,1,1,1,1,1,1,1,0,0},
    {1,1,1,1,1,1,1,1,1,1,1,0},
    {1,1,1,1,1,1,1,0,0,0,0,0},
    {1,1,1,0,1,1,1,1,0,0,0,0},
    {1,1,0,0,1,1,1,1,0,0,0,0},
    {1,0,0,0,0,1,1,1,1,0,0,0},
    {0,0,0,0,0,1,1,1,1,0,0,0},
    {0,0,0,0,0,0,1,1,1,0,0,0},
    {0,0,0,0,0,0,1,1,0,0,0,0},
    {0,0,0,0,0,0,0,0,0,0,0,0}
};

// Optimize Font Matrisi
unsigned char font_bitmap[128][16] = {
    ['S'] = {0x3C, 0x42, 0x40, 0x3C, 0x02, 0x02, 0x42, 0x42, 0x3C},
    ['K'] = {0x42, 0x44, 0x48, 0x70, 0x50, 0x48, 0x44, 0x42, 0x42},
    ['Y'] = {0x42, 0x42, 0x42, 0x24, 0x18, 0x18, 0x18, 0x18, 0x18},
    ['A'] = {0x18, 0x24, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x42, 0x42},
    ['B'] = {0x7C, 0x42, 0x42, 0x42, 0x7C, 0x42, 0x42, 0x42, 0x7C},
    ['C'] = {0x3C, 0x42, 0x40, 0x40, 0x40, 0x40, 0x40, 0x42, 0x3C},
    ['D'] = {0x78, 0x44, 0x42, 0x42, 0x42, 0x42, 0x42, 0x44, 0x78},
    ['E'] = {0x7E, 0x40, 0x40, 0x40, 0x76, 0x40, 0x40, 0x40, 0x7E},
    ['F'] = {0x7E, 0x40, 0x40, 0x40, 0x76, 0x40, 0x40, 0x40, 0x40},
    ['G'] = {0x3C, 0x42, 0x40, 0x40, 0x4E, 0x42, 0x42, 0x42, 0x3C},
    ['H'] = {0x42, 0x42, 0x42, 0x42, 0x7E, 0x42, 0x42, 0x42, 0x42},
    ['I'] = {0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18},
    ['J'] = {0x06, 0x06, 0x06, 0x06, 0x06, 0x06, 0x46, 0x42, 0x3C},
    ['L'] = {0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x7E},
    ['M'] = {0x42, 0x66, 0x5A, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42},
    ['N'] = {0x42, 0x62, 0x52, 0x4A, 0x46, 0x42, 0x42, 0x42, 0x42},
    ['O'] = {0x3C, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C},
    ['P'] = {0x7C, 0x42, 0x42, 0x42, 0x7C, 0x40, 0x40, 0x40, 0x40},
    ['R'] = {0x7C, 0x42, 0x42, 0x42, 0x7C, 0x48, 0x44, 0x42, 0x42},
    ['T'] = {0x7E, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18, 0x18},
    ['U'] = {0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x3C},
    ['V'] = {0x42, 0x42, 0x42, 0x42, 0x42, 0x42, 0x24, 0x24, 0x18},
    ['Z'] = {0x7E, 0x02, 0x04, 0x08, 0x16, 0x20, 0x40, 0x40, 0x7E},
    [' '] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

const char scancode_to_ascii[] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '[', ']', '\n', 0,
    'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ';', '\'', '`', 0, '\\',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', ',', '.', '/', 0, '*', 0, ' '
};

uint32_t back_buffer[TOTAL_PIXELS];

void gui_put_pixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    back_buffer[y * SCREEN_WIDTH + x] = color;
}

void put_char(char c, int x, int y, uint32_t color) {
    if (c < 0 || c >= 128) return;
    for (int row = 0; row < 16; row++) {
        unsigned char bits = font_bitmap[(int)c][row];
        for (int col = 0; col < 8; col++) {
            if (bits & (0x80 >> col)) gui_put_pixel(x + col, y + row, color);
        }
    }
}

void sky_put_char(char c) {
    static int text_x = 40;
    static int text_y = 400; // Yazı alanını yukarı çektik ki rahat görünsün
    put_char(c, text_x, text_y, COLOR_TEXT_WHITE);
    text_x += 8;
    if (text_x > 740) { text_x = 40; text_y += 16; }
}

// Masaüstü Motoru - GERÇEK FARE İMLECİ ÇİZİMLİ SÜRÜM
void gui_refresh_desktop(int mx, int my) {
    // 1. Masaüstü Arka Planı
    for (int i = 0; i < TOTAL_PIXELS; i++) {
        back_buffer[i] = COLOR_BG;
    }
    
    // 2. Alt Görev Çubuğu
    for (int y = SCREEN_HEIGHT - 40; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            back_buffer[y * SCREEN_WIDTH + x] = COLOR_TASKBAR;
        }
    }

    // 3. SKY Logosunu Sol Üste Sabitliyoruz (İmleçten bağımsız dursun)
    put_char('S', 30, 30, COLOR_TEXT_WHITE);
    put_char('K', 38, 30, COLOR_TEXT_WHITE);
    put_char('Y', 46, 30, COLOR_TEXT_WHITE);
    
    // 4. GERÇEK MAVİ OK FARE İMLECİNİ ÇİZME ALGORİTMASI
    for (int row = 0; row < 19; row++) {
        for (int col = 0; col < 12; col++) {
            if (mouse_pointer_sprite[row][col] == 1) {
                gui_put_pixel(mx + col, my + row, COLOR_CURSOR);
            }
        }
    }

    if (vbe_vram == 0) return;

    // Ekrana kopyalama köprüsü
    uint32_t pixels_per_pitch = vbe_pitch / 4; 
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            vbe_vram[y * pixels_per_pitch + x] = back_buffer[y * SCREEN_WIDTH + x];
        }
    }
}

static inline uint8_t inb(uint16_t port) {
    uint8_t data;
    __asm__ __volatile__("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
}

void pic_disable_all(void) {
    outb(0x21, 0xFF); 
    outb(0xA1, 0xFF); 
}

// PS/2 Fareyi güvenle dinlemek için veri akışını açan fonksiyon
void init_mouse(void) {
    outb(0x64, 0xA8); // Fare arayüzünü aç
    while(inb(0x64) & 0x02);
    outb(0x64, 0xD4); // Fareye komut yollayacağımızı bildir
    while(inb(0x64) & 0x02);
    outb(0x60, 0xF4); // Veri akışını başlat (Enable Packet Streaming)
    while(inb(0x64) & 0x02);
    inb(0x60);        // Varsa eski çöp tamponu temizle
}

// GELİŞMİŞ SENKRONİZE FARE SÜRÜCÜSÜ (ÇÖKMEYİ VE KİLİTLENMEYİ ENGELLER)
void handle_mouse_polling(void) {
    uint8_t status = inb(0x64);
    // Bit 0 aktif (veri var) VE Bit 5 aktifse (gelen veri fareye aitse) oku!
    if ((status & 0x01) && (status & 0x20)) {
        uint8_t data = inb(0x60);
        mouse_byte[mouse_cycle++] = data;
        
        if (mouse_cycle == 3) { 
            mouse_cycle = 0;
            
            // PS/2 Fare koordinat dönüştürme matematiği
            int8_t move_x = mouse_byte[1];
            int8_t move_y = mouse_byte[2];
            
            mouse_x += move_x / 2; // Taşmayı önlemek için hassasiyeti yumuşattık
            mouse_y -= move_y / 2; 
            
            // Ekran sınırlarında imleci hapset (Dışarı kaçıp çökertmesin)
            if (mouse_x < 0) mouse_x = 0;
            if (mouse_x > SCREEN_WIDTH - 12) mouse_x = SCREEN_WIDTH - 12;
            if (mouse_y < 0) mouse_y = 0;
            if (mouse_y > SCREEN_HEIGHT - 40) mouse_y = SCREEN_HEIGHT - 40;
            
            gui_refresh_desktop(mouse_x, mouse_y);
        }
    }
}

// SENKRONİZE KLAVYE POLLING SÜRÜCÜSÜ
void handle_keyboard_polling(void) {
    uint8_t status = inb(0x64);
    // Bit 0 aktif (veri var) VE Bit 5 pasifse (gelen veri fareye değil, klavyeye aitse) oku!
    if ((status & 0x01) && !(status & 0x20)) {
        uint8_t scancode = inb(0x60);
        if (scancode < 0x80) {
            char ascii_char = scancode_to_ascii[scancode];
            if (ascii_char != 0) {
                sky_put_char(ascii_char);
                gui_refresh_desktop(mouse_x, mouse_y);
            }
        }
    }
}

extern void init_idt(void);

// ANA ÇEKİRDEK GİRİŞİ
void kernel_main(struct multiboot_info* mboot) {
    init_idt();            
    pic_disable_all(); 

    if (mboot != 0) {
        if ((mboot->flags & (1 << 11)) || (mboot->flags & (1 << 2))) {
            if (mboot->vbe_mode_info != 0) {
                uint32_t* vbe_mode_table = (uint32_t*)mboot->vbe_mode_info;
                uint32_t lfb_check = vbe_mode_table[4];
                uint16_t pitch_check = ((uint16_t*)vbe_mode_table)[8];
                if (lfb_check != 0) {
                    vbe_vram = (uint32_t*)lfb_check;
                    vbe_pitch = pitch_check;
                }
            } else if (mboot->framebuffer_addr != 0) {
                vbe_vram = (uint32_t*)mboot->framebuffer_addr;
                vbe_pitch = mboot->framebuffer_pitch;
            }
        }
    }

    gui_refresh_desktop(mouse_x, mouse_y); 
    init_mouse();          

    while (1) {
        handle_mouse_polling();
        handle_keyboard_polling();
        
        system_ticks++;
        if (system_ticks % 2500 == 0) { 
            gui_refresh_desktop(mouse_x, mouse_y);
        }
        for (volatile int i = 0; i < 1200; i++); 
    }
}
