#include <stdint.h>

// ====================================================================
// 1. MODERN SİSTEM TANIMLAMALARI VE VRAM AYARLARI
// ====================================================================
#define VBE_START_ADDR  0xE0000000 
#define SCREEN_WIDTH    1024
#define SCREEN_HEIGHT   768

// Modern Windows Renk Paleti (Alpha Destekli)
#define COLOR_TASKBAR          0xCCF2F7FF  // Yarı şeffaf açık gri/mavi
#define COLOR_SEARCH_BAR       0xFFFFFFFF  // Saf beyaz
#define COLOR_MODERN_CURSOR    0x0067C0    // Windows Mavi
#define COLOR_WIN_TITLE        0x0F172A    // Koyu lacivert (başlık)
#define COLOR_MODERN_RED       0xE81123    // Windows Kapatma Kırmızısı

// Sistem Durum Değişkenleri
int active_window = 0; // 0: Masaüstü, 1: EXE, 2: DEB, 3: Sky-AI
uint32_t system_ticks = 0;

// Fare Durum Değişkenleri
int mouse_x = 512; 
int mouse_y = 384; 
uint8_t mouse_cycle = 0;
int8_t mouse_byte[3];

// ====================================================================
// 2. MODERN GRAFİK MOTORU (ALPHA BLENDING & GEOMETRİ)
// ====================================================================

// Ekrana ham piksel basma (Temel Fonksiyon)
void gui_put_pixel(int x, int y, uint32_t color) {
    if (x < 0 || x >= SCREEN_WIDTH || y < 0 || y >= SCREEN_HEIGHT) return;
    uint32_t *vbe_buffer = (uint32_t*)VBE_START_ADDR;
    vbe_buffer[y * SCREEN_WIDTH + x] = color;
}

// İki rengi Alpha (Şeffaflık) değerine göre karıştırma (Modern Görünümün Sırrı)
uint32_t mix_colors(uint32_t bg, uint32_t fg, uint8_t alpha) {
    uint8_t bg_r = (bg >> 16) & 0xFF; uint8_t bg_g = (bg >> 8) & 0xFF; uint8_t bg_b = bg & 0xFF;
    uint8_t fg_r = (fg >> 16) & 0xFF; uint8_t fg_g = (fg >> 8) & 0xFF; uint8_t fg_b = fg & 0xFF;

    // Lineer interpolasyon: res = (fg * a + bg * (255 - a)) / 255
    uint8_t res_r = ((fg_r * alpha) + (bg_r * (255 - alpha))) / 255;
    uint8_t res_g = ((fg_g * alpha) + (bg_g * (255 - alpha))) / 255;
    uint8_t res_b = ((fg_b * alpha) + (bg_b * (255 - alpha))) / 255;

    return (res_r << 16) | (res_g << 8) | res_b;
}

// Yarı şeffaf dikdörtgen çizme
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

// Düz renkli dikdörtgen çizme
void gui_draw_rect(int start_x, int start_y, int width, int height, uint32_t color) {
    for (int y = start_y; y < start_y + height; y++) {
        for (int x = start_x; x < start_x + width; x++) {
            gui_put_pixel(x, y, color);
        }
    }
}

// Windows 11 Tarzı Yuvarlak Köşeli Yarı Şeffaf Pencere Çizme
void gui_draw_rounded_window(int sx, int sy, int w, int h, int r, uint32_t color, uint8_t alpha) {
    uint32_t *vbe_buffer = (uint32_t*)VBE_START_ADDR;
    for (int y = sy; y < sy + h; y++) {
        for (int x = sx; x < sx + w; x++) {
            // Köşe merkezlerine olan mesafeleri hesapla
            int dl_x = x - (sx + r); int dr_x = x - (sx + w - r - 1);
            int dt_y = y - (sy + r); int db_y = y - (sy + h - r - 1);

            int is_corner = 0;
            // Sol Üst, Sağ Üst, Sol Alt, Sağ Alt köşeleri kontrol et
            if (x < sx + r && y < sy + r && (dl_x*dl_x + dt_y*dt_y > r*r)) is_corner = 1; 
            if (x > sx + w - r - 1 && y < sy + r && (dr_x*dr_x + dt_y*dt_y > r*r)) is_corner = 1; 
            if (x < sx + r && y > sy + h - r - 1 && (dl_x*dl_x + db_y*db_y > r*r)) is_corner = 1; 
            if (x > sx + w - r - 1 && y > sy + h - r - 1 && (dr_x*dr_x + db_y*db_y > r*r)) is_corner = 1; 

            if (!is_corner && x >= 0 && x < SCREEN_WIDTH && y >= 0 && y < SCREEN_HEIGHT) {
                uint32_t bg_color = vbe_buffer[y * SCREEN_WIDTH + x];
                vbe_buffer[y * SCREEN_WIDTH + x] = mix_colors(bg_color, color, alpha);
            }
        }
    }
}

// ====================================================================
// 3. MASAÜSTÜ RENDER VE RESİMLİ ARKA PLAN (WALLPAPER)
// ====================================================================

// Matematiksel Modern Degrade Duvar Kağıdı (Resim Gibi Görünür)
void gui_render_wallpaper(void) {
    for (int y = 0; y < SCREEN_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            // Karmaşık renk geçişleri (Mavi/Mor tonları)
            uint8_t r = 30 + ((x * 40) / SCREEN_WIDTH) + ((y * 60) / SCREEN_HEIGHT);  
            uint8_t g = 50 + ((x * 20) / SCREEN_WIDTH) + ((y * 30) / SCREEN_HEIGHT);  
            uint8_t b = 120 + ((x * 100) / SCREEN_WIDTH) - ((y * 20) / SCREEN_HEIGHT); 

            // Ortada hafif bir aydınlanma efekti (Windows 11 Bloom gibi)
            if (x > 300 && x < 700 && y > 200 && y < 550) {
                r += 45; g += 30; b += 40; 
            }
            gui_put_pixel(x, y, (r << 16) | (g << 8) | b);
        }
    }
}

// Masaüstü İkonlarını Çizme (Windows Tarzı, Renkli ve Yuvarlak)
void gui_draw_icons(void) {
    // 1. EXE Subsystem İkonu (Mavi, Modern Yuvarlak)
    gui_draw_rounded_window(40, 40, 36, 36, 6, 0x0078D4, 255);
    gui_draw_rect(48, 50, 20, 10, 0xFFFFFF); // İç beyaz pencere

    // 2. DEB Subsystem İkonu (Bordo/Kırmızı)
    gui_draw_rounded_window(40, 100, 36, 36, 6, 0xA8193D, 255); 
    gui_draw_rect(52, 108, 12, 20, 0xE0E0E0); // İç simge

    // 3. Sky-AI Subsystem İkonu (Altın/Turuncu)
    gui_draw_rounded_window(40, 160, 36, 36, 6, 0xFFB900, 255); 
    gui_draw_rect(50, 168, 16, 16, 0x111111); // İç AI çekirdeği
}

// Windows Tarzı Pencere Çerçevesi (Yuvarlak, Gölceli ve Yarı Şeffaf Başlık)
void gui_draw_window_frame(int win_x, int win_y, int win_w, int win_h) {
    // Pencere Gölgesi (Derinlik Efekti)
    gui_draw_rect_alpha(win_x + 8, win_y + 8, win_w, win_h, 0x000000, 80);

    // Ana Pencere Gövdesi (Buzlu Cam - Mica Efekti)
    gui_draw_rounded_window(win_x, win_y, win_w, win_h, 12, 0xFFFFFF, 220); 

    // Modern Koyu Başlık Barı (Üst Kısım)
    gui_draw_rounded_window(win_x, win_y, win_w, 32, 12, COLOR_WIN_TITLE, 240);
    gui_draw_rect(win_x, win_y + 20, win_w, 12, COLOR_WIN_TITLE); // Köşeleri birleştirme

    // Kapatma Butonu (Modern Kırmızı, Yuvarlak)
    gui_draw_rounded_window(win_x + win_w - 40, win_y + 4, 32, 24, 6, COLOR_MODERN_RED, 255);
}

// Windows 11 Tarzı Ortalanmış Taskbar (Görev Çubuğu)
void gui_render_taskbar(uint32_t tick_count) {
    // Taskbar Arka Planı (Yarı Şeffaf)
    gui_draw_rect_alpha(0, 718, SCREEN_WIDTH, 50, 0xEEEEEE, 180);
    
    // Ortadaki Arama Barı (Yuvarlak)
    int search_bar_x = (SCREEN_WIDTH / 2) - 150; 
    gui_draw_rounded_window(search_bar_x, 724, 300, 36, 8, 0xFFFFFF, 255);
    
    // Ortadaki Sistem İkonları
    int start_icon_x = search_bar_x - 100;
    gui_draw_rounded_window(start_icon_x, 726, 32, 32, 6, 0x0078D4, 255);      // Başlat
    gui_draw_rounded_window(start_icon_x + 40, 726, 32, 32, 6, 0xFFB900, 255); // Dosyalar
    gui_draw_rounded_window(start_icon_x + 80, 726, 32, 32, 6, 0x107C41, 255); // Tarayıcı

    // Sağ alttaki sistem tıkı / Saat (Yanıp sönen mavi nokta)
    if ((tick_count / 20) % 2 == 0) {
        gui_draw_rounded_window(950, 734, 18, 18, 4, 0x0067C0, 255); 
    } else {
        gui_draw_rounded_window(950, 734, 18, 18, 4, 0x7F8C8D, 255); 
    }
}

// Modern Windows Fare İmleci
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
// 4. SUBSYSTEM APPLAR (RENKLİ VE RESİMLİ İÇERİK)
// ====================================================================

// EXE Uygulama Penceresi (Windows 11 Ayarlar Paneli gibi)
void run_exe_subsystem(int win_x, int win_y) {
    // İç panel (Açık Gri)
    gui_draw_rect(win_x + 15, win_y + 45, 470, 290, 0xF3F3F3);

    // Windows Logosunu temsil eden mavi kareler
    gui_draw_rect(win_x + 50, win_y + 80, 22, 22, 0x0078D4); 
    gui_draw_rect(win_x + 75, win_y + 80, 22, 22, 0x0078D4); 
    gui_draw_rect(win_x + 50, win_y + 105, 22, 22, 0x0078D4); 
    gui_draw_rect(win_x + 75, win_y + 105, 22, 22, 0x0078D4); 
}

// DEB Uygulama Penceresi (Linux Terminal)
void run_deb_subsystem(int win_x, int win_y) {
    // Koyu terminal arka planı
    gui_draw_rect(win_x + 15, win_y + 45, 470, 290, 0x212121);
    
    // Kırmızı Debian logosu (Blok)
    gui_draw_rect(win_x + 40, win_y + 70, 40, 40, 0xD70A53);
}

// Sky-AI Niyet Analizi (Basit Simülasyon)
int sky_ai_analyze_intent(uint32_t input_hash) {
    if (input_hash % 3 == 0) return 1; // Üretim
    if (input_hash % 3 == 1) return 2; // Analiz
    return 0; // Bekleme
}

// Sky-AI Render Motoru (Canlı, Dinamik Görselleştirme)
void run_ai_core_render(int win_x, int win_y, uint32_t current_tick, int intent) {
    // AI İç Paneli (Koyu Mor)
    gui_draw_rect(win_x + 20, win_y + 110, 460, 220, 0x1E1B4B); 
    
    int pulse = (current_tick / 5) % 8;
    
    if (intent == 1) {
        // Üretim Modu: Yeşil dalgalar
        for (int i = 0; i < 4; i++) {
            gui_draw_rect(win_x + 40 + (i * 100), win_y + 150 + (pulse * 2), 80, 20, 0x10B981);
        }
    } else if (intent == 2) {
        // Analiz Modu: Mavi hareketli kareler
        for (int i = 0; i < 6; i++) {
            gui_draw_rect(win_x + 60 + (i * 60) + pulse, win_y + 180 - (i * 5), 15, 15, 0x06B6D4); 
        }
    } else {
        // Bekleme Modu: Mor sinüs dalgası simülasyonu
        for (int i = 0; i < 10; i++) {
            int wave_height = 20 + (i * pulse) % 50;
            if (wave_height > 80) wave_height = 80;
            gui_draw_rect(win_x + 50 + (i * 40), win_y + 240 - wave_height, 15, wave_height, 0x8B5CF6); 
        }
    }
}

// Sky-AI Uygulama Penceresi (Dinamik Resim)
void run_sky_subsystem(int win_x, int win_y, uint32_t current_tick) {
    // AI Arka Plan (Koyu Lacivert)
    gui_draw_rect(win_x + 15, win_y + 45, 470, 290, 0x0F172A);

    // AI Kimlik Kartı
    gui_draw_rect(win_x + 35, win_y + 65, 30, 30, 0xFFB900); // Sarı kare
    gui_draw_rect(win_x + 80, win_y + 75, 120, 12, 0x38BDF8); // İsim çizgisi

    // Canlı AI Görselini Renderla
    uint32_t fake_input_hash = (current_tick / 100); 
    int current_intent = sky_ai_analyze_intent(fake_input_hash);
    run_ai_core_render(win_x, win_y, current_tick, current_intent);
}

// ====================================================================
// 5. MODERN DESKTOP REFRESH MOTORU (ANA GÖVDE)
// ====================================================================

// Tüm masaüstünü sıfırdan ve katman katman çizen fonksiyon
void gui_refresh_desktop(int mx, int my, uint32_t tick) {
    // Katman 1: Duvar Kağıdı (En altta)
    gui_render_wallpaper();

    // Katman 2: Masaüstü İkonları
    gui_draw_icons();
    
    // Katman 3: Aktif Pencere (Eğer varsa)
    if (active_window != 0) {
        int wx = 262, wy = 184; // Pencere konumu
        gui_draw_window_frame(wx, wy, 500, 350); // Çerçeve
        
        // Pencere içeriğini çiz
        if (active_window == 1)      run_exe_subsystem(wx, wy);
        else if (active_window == 2) run_deb_subsystem(wx, wy);
        else if (active_window == 3) run_sky_subsystem(wx, wy, tick);
    }
    
    // Katman 4: Taskbar (En üstte)
    gui_render_taskbar(tick);

    // Katman 5: Fare İmleci (Her şeyin üstünde)
    gui_draw_mouse(mx, my);
}

// ====================================================================
// 6. DONANIM I/O VE MOUSE SÜRÜCÜSÜ (DRIVER CORE)
// ====================================================================

// Assembly I/O Port Komutları
static inline uint8_t inb(uint16_t port) {
    uint8_t data;
    __asm__ __volatile__("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Gecikme fonksiyonu (Zamanlama için)
void sky_kernel_delay(int count) {
    for (volatile int i = 0; i < count * 900; i++);
}

// Masaüstü Tıklama ve Hitbox Kontrolü
void handle_desktop_click(void) {
    // Eğer bir pencere açıksa, kapatma butonunu kontrol et
    if (active_window != 0) {
        // Kapatma butonu hitbox'ı
        if (mouse_x >= 722 && mouse_x <= 754 && mouse_y >= 188 && mouse_y <= 212) {
            active_window = 0; // Pencereyi kapat
            gui_refresh_desktop(mouse_x, mouse_y, system_ticks);
            return;
        }
    }

    // Eğer masaüstündeysek, ikon tıklamalarını kontrol et
    if (active_window == 0) {
        // İkonların yatay hitbox'ı
        if (mouse_x >= 40 && mouse_x <= 76) {
            // Dikey hitbox'lar
            if (mouse_y >= 40 && mouse_y <= 76) {
                active_window = 1; // EXE aç
            }
            else if (mouse_y >= 100 && mouse_y <= 136) {
                active_window = 2; // DEB aç
            }
            else if (mouse_y >= 160 && mouse_y <= 196) {
                active_window = 3; // Sky-AI aç
            }
            gui_refresh_desktop(mouse_x, mouse_y, system_ticks);
        }
    }
}

// PS/2 Fare Başlatma
void init_mouse(void) {
    outb(0x64, 0xA8); // Fareyi etkinleştir
    while((inb(0x64) & 0x02));
    outb(0x64, 0x20); // Komut baytını oku
    while(!(inb(0x64) & 0x01));
    uint8_t status = (inb(0x60) | 0x02); 
    while((inb(0x64) & 0x02));
    outb(0x64, 0x60); // Komut baytını yaz
    while((inb(0x64) & 0x02));
    outb(0x60, status);
    while((inb(0x64) & 0x02));
    outb(0x64, 0xD4); // Fareye yazma
    while((inb(0x64) & 0x02));
    outb(0x60, 0xF4); // Fareyi etkinleştirme (akış)
    inb(0x60); // Ack
}

// Fare Polling (Okuma) İşleyicisi
void handle_mouse_polling(void) {
    uint8_t status = inb(0x64);
    if ((status & 0x01) && (status & 0x20)) {
        uint8_t data = inb(0x60);
        mouse_byte[mouse_cycle++] = data;
        
        if (mouse_cycle == 3) { 
            mouse_cycle = 0;
            
            // Sol tıklama kontrolü
            if (mouse_byte[0] & 0x01) {
                handle_desktop_click();
                sky_kernel_delay(10); // Debounce
                return;
            }
            
            // Hareket verilerini işle
            int8_t move_x = mouse_byte[1];
            int8_t move_y = mouse_byte[2];
            
            mouse_x += move_x;
            mouse_y -= move_y; // Y ekseni PS/2'de terstir
            
            // Ekran sınırlarını koru
            if (mouse_x < 0) mouse_x = 0;
            if (mouse_x > SCREEN_WIDTH - 5) mouse_x = SCREEN_WIDTH - 5;
            if (mouse_y < 0) mouse_y = 0;
            if (mouse_y > SCREEN_HEIGHT - 5) mouse_y = SCREEN_HEIGHT - 5;
            
            // Her fare hareketinde ekranı yenile (Dinamik)
            gui_refresh_desktop(mouse_x, mouse_y, system_ticks);
        }
    }
}

// ====================================================================
// 7. KERNEL ANA GİRİŞ NOKTASI (MAIN LOOP)
// ====================================================================

void kernel_main(void) {
    // Ekranı ilk kez cam gibi pürüzsüz aç
    gui_refresh_desktop(mouse_x, mouse_y, system_ticks); 
    
    // Fareyi uyandır
    init_mouse();          

    // Sonsuz Sistem Döngüsü (Real-Time OS Simülasyonu)
    while (1) {
        // Fareyi sürekli oku
        handle_mouse_polling();
        
        // Sistem tıkını artır ve animasyonları yenile
        system_ticks++;
        if (system_ticks % 15 == 0) { // Her 15 tıkta bir arayüz animasyonlarını güncelle
            gui_refresh_desktop(mouse_x, mouse_y, system_ticks);
        }
        
        sky_kernel_delay(1); // İşlemciyi aşırı yorma (Boş döngü)
    }
}
