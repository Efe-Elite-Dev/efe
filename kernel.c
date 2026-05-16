#include <stdint.h>

// Ekran Çözünürlüğü
#define SCREEN_WIDTH     800
#define SCREEN_HEIGHT    600
#define TOTAL_PIXELS     (SCREEN_WIDTH * SCREEN_HEIGHT)

// === Windows 11 / SKY OS Fluent Renk Paleti ===
#define COLOR_BG_TOP       0xE2E7F0  // Açık dumanlı mavi-gri (Üst arka plan)
#define COLOR_BG_BOTTOM    0xCCD4E2  // Yumuşak gri-mavi (Alt arka plan)
#define COLOR_CARD_BG      0xFFFFFF  // Ortadaki dev kurulum kartı (Saf beyaz)
#define COLOR_TEXT_MAIN    0x111625  // Modern koyu lacivert/siyah metin
#define COLOR_TEXT_SUB     0x5C6479  // Açıklama metinleri için soft gri
#define COLOR_ACCENT_BLUE  0x005A9E  // Windows/SKY OS OOBE Gece Mavisi (Buton ve Seçili Alan)
#define COLOR_TEXT_WHITE   0xFFFFFF  // Beyaz harfler

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

// Global Grafik Sürücü Değişkenleri
uint32_t* vbe_vram = (uint32_t*)0xE0000000; 
uint32_t  vbe_pitch = SCREEN_WIDTH * 4; 

// === boot.o Linker Hatasını Çözen Kritik Fonksiyon ===
// boot.asm içindeki 'call keyboard_handler_c' satırı doğrudan burayı çağırır.
void keyboard_handler_c(void) {
    // Şimdilik polling/kesme çakışmasını önlemek için klavye portunu (0x60) boşa çıkarıyoruz
    volatile uint8_t status;
    __asm__ __volatile__("inb $0x64, %0" : "=a"(status));
    if (status & 0x01) {
        volatile uint8_t scancode;
        __asm__ __volatile__("inb $0x60, %0" : "=a"(scancode));
        (void)scancode; // Derleyici uyarısı vermemesi için
    }
}

// === Grafik Çizim Motoru ===

// Soft Arka Plan Gradyanı
void draw_background_gradient(void) {
    uint32_t pixels_per_pitch = vbe_pitch / 4;
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        uint8_t r1 = (COLOR_BG_TOP >> 16) & 0xFF;
        uint8_t g1 = (COLOR_BG_TOP >> 8) & 0xFF;
        uint8_t b1 = COLOR_BG_TOP & 0xFF;
        
        uint8_t r2 = (COLOR_BG_BOTTOM >> 16) & 0xFF;
        uint8_t g2 = (COLOR_BG_BOTTOM >> 8) & 0xFF;
        uint8_t b2 = COLOR_BG_BOTTOM & 0xFF;

        uint8_t r = r1 + ((r2 - r1) * y / SCREEN_HEIGHT);
        uint8_t g = g1 + ((g2 - g1) * y / SCREEN_HEIGHT);
        uint8_t b = b1 + ((b2 - b1) * y / SCREEN_HEIGHT);
        uint32_t color = (r << 16) | (g << 8) | b;

        for (int x = 0; x < SCREEN_WIDTH; x++) {
            vbe_vram[y * pixels_per_pitch + x] = color;
        }
    }
}

// Dikdörtgen Blok Çizici
void draw_rect(int start_x, int start_y, int width, int height, uint32_t color) {
    uint32_t pixels_per_pitch = vbe_pitch / 4;
    for (int y = start_y; y < start_y + height; y++) {
        for (int x = start_x; x < start_x + width; x++) {
            if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
                vbe_vram[y * pixels_per_pitch + x] = color;
            }
        }
    }
}

// Daire Çizici (Sol taraftaki Dünya/İkon alanı simülasyonu)
void draw_circle(int xc, int yc, int r, uint32_t color) {
    uint32_t pixels_per_pitch = vbe_pitch / 4;
    for (int y = yc - r; y <= yc + r; y++) {
        for (int x = xc - r; x <= xc + r; x++) {
            if ((x - xc)*(x - xc) + (y - yc)*(y - yc) <= r*r) {
                if (x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
                    vbe_vram[y * pixels_per_pitch + x] = color;
                }
            }
        }
    }
}

// === Özel OOBE Font Matrisi (Türkçe karakter destek simülasyonlu) ===
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
    ['Q'] = {0x3C, 0x42, 0x42, 0x42, 0x42, 0x4A, 0x3C, 0x0A, 0x00},
    ['F'] = {0x7E, 0x40, 0x40, 0x76, 0x40, 0x40, 0x40, 0x00, 0x00},
    ['L'] = {0x40, 0x40, 0x40, 0x40, 0x40, 0x40, 0x7E, 0x00, 0x00},
    ['?'] = {0x3C, 0x42, 0x02, 0x0C, 0x10, 0x10, 0x00, 0x10, 0x00},
    ['/'] = {0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x00, 0x00},
    ['('] = {0x0C, 0x18, 0x30, 0x30, 0x30, 0x18, 0x0C, 0x00, 0x00},
    [')'] = {0x30, 0x18, 0x0C, 0x0C, 0x0C, 0x18, 0x30, 0x00, 0x00},
    ['-'] = {0x00, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x00, 0x00, 0x00},
    [' '] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}
};

void put_char(char c, int x, int y, uint32_t color) {
    if (vbe_vram == 0 || c < 0 || c >= 128) return;
    uint32_t pixels_per_pitch = vbe_pitch / 4;
    for (int row = 0; row < 16; row++) {
        unsigned char bits = font_bitmap[(int)c][row];
        for (int col = 0; col < 8; col++) {
            if (bits & (0x80 >> col)) {
                if (x + col >= 0 && x + col < SCREEN_WIDTH && y + row >= 0 && y + row < SCREEN_HEIGHT) {
                    vbe_vram[(y + row) * pixels_per_pitch + (x + col)] = color;
                }
            }
        }
    }
}

void put_string(const char* s, int x, int y, uint32_t color) {
    while (*s) {
        put_char(*s, x, y, color);
        x += 8;
        s++;
    }
}

// === SKY OS OOBE GÖRSEL YERLEŞİM MOTORU ===
void draw_sky_os_oobe_ui(void) {
    // 1. Arka Planı Gradyanla Doldur
    draw_background_gradient();

    // 2. Ortadaki Büyük Beyaz Kart (Kurulum Alanı)
    int card_w = 700;
    int card_h = 480;
    int card_x = (SCREEN_WIDTH - card_w) / 2;
    int card_y = (SCREEN_HEIGHT - card_h) / 2;
    draw_rect(card_x, card_y, card_w, card_h, COLOR_CARD_BG);

    // ====================================================================
    // 🌍 SOL ALAN: KÜRESEL AKRİLİK GRAFİK (Yüklediğin Görsellerdeki Gibi)
    // ====================================================================
    int left_center_x = card_x + 160;
    int left_center_y = card_y + 240;
    
    // Üç katmanlı yuvarlak modern grafik simülasyonu
    draw_circle(left_center_x, left_center_y, 75, 0x3B82F6);           // Dış Mavi Küre
    draw_circle(left_center_x - 12, left_center_y + 12, 55, 0x1D4ED8); // İç Koyu Bölge
    draw_circle(left_center_x + 20, left_center_y - 20, 35, 0x6366F1); // İndigo Parlama

    // Sol Üst Köşe Bilgi Metni
    put_string("SKY OS OOBE Setup", card_x + 30, card_y + 30, COLOR_TEXT_SUB);

    // ====================================================================
    // 📋 SAĞ ALAN: BAŞLIKLAR VE DİKEY LİSTE SEÇENEKLERİ
    // ====================================================================
    int right_content_x = card_x + 340;
    int right_content_y = card_y + 60;

    // Ana Soru Başlığı (Görsel 1)
    put_string("Bu dogru ulke/bolge mi?", right_content_x, right_content_y, COLOR_TEXT_MAIN);
    
    // Alt Bilgi Satırları
    put_string("SKY OS dil ve bolge ayarlari otomatik", right_content_x, right_content_y + 22, COLOR_TEXT_SUB);
    put_string("olarak sisteme entegre edilecektir.", right_content_x, right_content_y + 38, COLOR_TEXT_SUB);

    // Dikey Menü Listesi Düzeni
    int item_x = right_content_x;
    int item_y = right_content_y + 85;
    int item_w = 320;
    int item_h = 36;

    // Eleman 1
    put_string("Almanya", item_x + 12, item_y + 10, COLOR_TEXT_SUB);
    
    // Eleman 2
    item_y += item_h + 6;
    put_string("Amerika Birlesik Devletleri", item_x + 12, item_y + 10, COLOR_TEXT_SUB);

    // Eleman 3: AKTİF SEÇİLİ BÖLGE (Görseldeki parlayan mavi şerit!)
    item_y += item_h + 6;
    draw_rect(item_x, item_y, item_w, item_h, COLOR_ACCENT_BLUE);
    put_string("Turkiye (SKY OS Core)", item_x + 12, item_y + 12, COLOR_TEXT_WHITE);

    // Eleman 4
    item_y += item_h + 6;
    put_string("Turkmenistan", item_x + 12, item_y + 10, COLOR_TEXT_SUB);

    // Eleman 5
    item_y += item_h + 6;
    put_string("Turkce Q / F Klavye Modu", item_x + 12, item_y + 10, COLOR_TEXT_SUB);

    // ====================================================================
    // 🔘 SAĞ ALT ALAN: MODERN BEYAZ/MAVİ "EVET" BUTONU
    // ====================================================================
    int btn_w = 110;
    int btn_h = 32;
    int btn_x = card_x + card_w - btn_w - 30;
    int btn_y = card_y + card_h - btn_h - 30;

    // Buton Gövdesi
    draw_rect(btn_x, btn_y, btn_w, btn_h, COLOR_ACCENT_BLUE);
    
    // Buton Metni ("Evet")
    put_string("Evet", btn_x + 38, btn_y + 8, COLOR_TEXT_WHITE);
}

// === ANA ÇEKİRDEK GİRİŞİ ===
void kernel_main(struct multiboot_info* mboot) {
    // VBE Linear Framebuffer Adres Bağlantısı
    if (mboot != 0) {
        if ((mboot->flags & (1 << 11)) || (mboot->flags & (1 << 2))) {
            if (mboot->framebuffer_addr != 0) {
                vbe_vram = (uint32_t*)mboot->framebuffer_addr;
                vbe_pitch = mboot->framebuffer_pitch;
            }
        }
    }

    // Arayüzü Ekrana Çiz
    draw_sky_os_oobe_ui();

    // İşlemciyi HLT durumunda kilitle (Sonsuz Döngü)
    while (1) {
        __asm__ __volatile__("hlt");
    }
}
