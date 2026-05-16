#include <stdint.h>

#define SCREEN_WIDTH     800
#define SCREEN_HEIGHT    600
#define TOTAL_PIXELS     (SCREEN_WIDTH * SCREEN_HEIGHT)

// Windows 11 Fluent Renk Paleti
#define COLOR_BG         0x0A0F24  
#define COLOR_TASKBAR    0x1A2342  
#define COLOR_CURSOR     0x00A2ED  
#define COLOR_TEXT_WHITE 0xFFFFFF

// ====================================================================
// GRUB MULTIBOOT GÜVENLİK YAPILARI
// ====================================================================
// GRUB'ın bize yolladığı Multiboot Info yapısının resmi x86 standart şeması
struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t num;
    uint32_t size;
    uint32_t addr;
    uint32_t shndx;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;      // VBE Mode Info yapısının adresi (Offset 76)
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
    uint32_t framebuffer_addr;   // Doğrudan LFB Adresi (Offset 88)
    uint32_t framebuffer_pitch;  // Satır genişliği byte boyutu (Offset 92)
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
} __attribute__((packed));

// Ekran Kartı Değişkenleri (Guru Meditation Çökme Korumalı Varsayılanlar)
uint32_t* vbe_vram = (uint32_t*)0xE0000000; 
uint32_t  vbe_pitch = SCREEN_WIDTH * 4; 

uint32_t system_ticks = 0;
int mouse_x = 400; 
int mouse_y = 300; 
uint8_t mouse_cycle = 0;
int8_t mouse_byte[3];

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
    // 1. Arka planı temizle
    for (int i = 0; i < TOTAL_PIXELS; i++) {
        back_buffer[i] = COLOR_BG;
    }
    
    // 2. Taskbar alanını boya
    for (int y = SCREEN_HEIGHT - 40; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            back_buffer[y * SCREEN_WIDTH + x] = COLOR_TASKBAR;
        }
    }

    // 3. Yazıları bas
    put_char('S', mx, my, COLOR_CURSOR);
    put_char('K', mx + 8, my, COLOR_CURSOR);
    put_char('Y', mx + 16, my, COLOR_CURSOR);
    
    // 4. ÇÖKMEYİ ENGELLEYEN GÜVENLİ FLUSH ALGORİTMASI
    // Eğer vbe_vram adresi null ise veya yanlışlıkla patladıysa yazma, çökme!
    if (vbe_vram == 0) return;

    uint32_t pixels_per_pitch = vbe_pitch / 4; 
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            // Çizgilerin kaymasını engelleyen matematiksel senkronizasyon
            vbe_vram[y * pixels_per_pitch + x] = back_buffer[y * SCREEN_WIDTH + x];
        }
    }
}

// PS/2 Donanım Giriş/Çıkış Sürücüleri
static inline uint8_t inb(uint16_t port) {
    uint8_t data;
    __asm__ __volatile__("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
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
            if (mouse_x > SCREEN_WIDTH - 24) mouse_x = SCREEN_WIDTH - 24;
            if (mouse_y < 0) mouse_y = 0;
            if (mouse_y > SCREEN_HEIGHT - 40) mouse_y = SCREEN_HEIGHT - 40;
            
            gui_refresh_desktop(mouse_x, mouse_y);
        }
    }
}

extern void init_idt(void);

// MULTIBOOT YAPISINA TAM UYUMLU ANA BAŞLANGIÇ NOKTASI
void kernel_main(struct multiboot_info* mboot) {
    init_idt();            

    // GRUB'ın yolladığı struct yapısını güvenli doğrulamadan geçiriyoruz
    if (mboot != 0) {
        // Bit 11 veya Bit 2 aktifse GRUB bize lineer VBE framebuffer sağlamıştır
        if ((mboot->flags & (1 << 11)) || (mboot->flags & (1 << 2))) {
            
            // Eğer VBE Mode Info alt pointer yapısı doluysa oradan adresi cımbızla çek
            if (mboot->vbe_mode_info != 0) {
                uint32_t* vbe_mode_table = (uint32_t*)mboot->vbe_mode_info;
                uint32_t lfb_check = vbe_mode_table[4]; // Resmi VRAM başlangıç offseti
                uint16_t pitch_check = ((uint16_t*)vbe_mode_table)[8]; // Gerçek Pitch
                
                if (lfb_check != 0) {
                    vbe_vram = (uint32_t*)lfb_check;
                    vbe_pitch = pitch_check;
                }
            }
            // Fallback: Eğer üstteki alt tablo boşsa doğrudan Multiboot ana LFB alanını dene
            else if (mboot->framebuffer_addr != 0) {
                vbe_vram = (uint32_t*)mboot->framebuffer_addr;
                vbe_pitch = mboot->framebuffer_pitch;
            }
        }
    }

    // İlk temiz ekranı bas
    gui_refresh_desktop(mouse_x, mouse_y); 
    init_mouse();          

    while (1) {
        handle_mouse_polling();
        system_ticks++;
        
        if (system_ticks % 300 == 0) { 
            gui_refresh_desktop(mouse_x, mouse_y);
        }
        for (volatile int i = 0; i < 600; i++); // İşlemciyi yormayan ufak bir nefes alma gecikmesi
    }
}

void sky_put_char(char c) {
    static int text_x = 40;
    static int text_y = 500;
    put_char(c, text_x, text_y, COLOR_TEXT_WHITE);
    text_x += 8;
    if(text_x > 740) { text_x = 40; text_y += 16; }
}
