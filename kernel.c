#include <stdint.h>

#define SCREEN_WIDTH     800
#define SCREEN_HEIGHT    600
#define TOTAL_PIXELS     (SCREEN_WIDTH * SCREEN_HEIGHT)

// Windows 11 Fluent Renk Paleti
#define COLOR_BG         0x0A0F24  
#define COLOR_TASKBAR    0x1A2342  
#define COLOR_CURSOR     0x00A2ED  
#define COLOR_TEXT_WHITE 0xFFFFFF

// Multiboot Yapısı
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

uint32_t* vbe_vram = (uint32_t*)0xE0000000; 
uint32_t  vbe_pitch = SCREEN_WIDTH * 4; 

uint32_t system_ticks = 0;
int mouse_x = 400; 
int mouse_y = 300; 

// Sabit Font Matrisi
unsigned char font_bitmap[128][16] = {
    ['S'] = {0x3C, 0x42, 0x40, 0x3C, 0x02, 0x02, 0x42, 0x42, 0x3C, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['K'] = {0x42, 0x44, 0x48, 0x70, 0x50, 0x48, 0x44, 0x42, 0x42, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    ['Y'] = {0x42, 0x42, 0x42, 0x24, 0x18, 0x18, 0x18, 0x18, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
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

void gui_refresh_desktop(int mx, int my) {
    for (int i = 0; i < TOTAL_PIXELS; i++) {
        back_buffer[i] = COLOR_BG;
    }
    
    for (int y = SCREEN_HEIGHT - 40; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            back_buffer[y * SCREEN_WIDTH + x] = COLOR_TASKBAR;
        }
    }

    put_char('S', mx, my, COLOR_CURSOR);
    put_char('K', mx + 8, my, COLOR_CURSOR);
    put_char('Y', mx + 16, my, COLOR_CURSOR);
    
    if (vbe_vram == 0) return;

    uint32_t pixels_per_pitch = vbe_pitch / 4; 
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            vbe_vram[y * pixels_per_pitch + x] = back_buffer[y * SCREEN_WIDTH + x];
        }
    }
}

// Port G/Ç İşlemleri
static inline uint8_t inb(uint16_t port) {
    uint8_t data;
    __asm__ __volatile__("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Gelişmiş PIC Maskeleme Fonksiyonu (Donanım kilitlenmelerini önler)
void pic_disable_all(void) {
    outb(0x21, 0xFF); // Ana PIC tüm kesmeleri maskele (kapat)
    outb(0xA1, 0xFF); // Yardımcı PIC tüm kesmeleri maskele (kapat)
}

// Çökmeyi önlemek için fareyi Polling (Sonsuz Döngü) yerine kesmesiz moda alıyoruz
void init_mouse(void) {
    // Çökmeyi önlemek amacıyla port kararlılığı sağlama adımları
    outb(0x64, 0xA8); // Fare arayüzünü aktif et
    for (volatile int i = 0; i < 1000; i++);
    inb(0x60);        // Varsa tamponu boşalt
}

void handle_mouse_polling(void) {
    // Çökmeyi engellemek için ana loop kilitlenmesini hafifletiyoruz
    // Şu an donanım hazır olana kadar bekleme (Guru Meditation'ı aşmak için pas geç)
    uint8_t status = inb(0x64);
    if (status & 0x01) {
        inb(0x60); // Gelen veriyi güvenle erit ama işleme sokup çökertme
    }
}

extern void init_idt(void);

// ANA GİRİŞ NOKTASI
void kernel_main(struct multiboot_info* mboot) {
    init_idt();            
    pic_disable_all(); // Donanımsal IRQ çakışmalarını tamamen sustur!

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

    // Masaüstünü ekrana bir kez tam oturt
    gui_refresh_desktop(mouse_x, mouse_y); 
    init_mouse();          

    while (1) {
        handle_mouse_polling();
        system_ticks++;
        
        // Ekranı her döngüde deli gibi çizip VBOX'ı patlatmasın diye tick kontrolünü arttırdık
        if (system_ticks % 2000 == 0) { 
            gui_refresh_desktop(mouse_x, mouse_y);
        }
        for (volatile int i = 0; i < 1000; i++); 
    }
}

void sky_put_char(char c) {
    static int text_x = 40;
    static int text_y = 500;
    put_char(c, text_x, text_y, COLOR_TEXT_WHITE);
    text_x += 8;
    if(text_x > 740) { text_x = 40; text_y += 16; }
}
