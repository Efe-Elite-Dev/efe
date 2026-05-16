#include "screen.h"
#include <stdint.h>

char cmd_buffer[256];
int cmd_index = 0;

// Windows OOBE Adımları (State Machine)
int setup_state = 0;

// GEÇİCİ KULLANICI VERİLERİ
char user_time[16] = "12:00";
int time_idx = 0;

// FARE (MOUSE) SÜRÜCÜSÜ DEĞİŞKENLERİ
int mouse_x = 40; 
int mouse_y = 12; 
uint8_t mouse_cycle = 0;
int8_t mouse_byte[3];

// VGA Bellek Adresi ve Renk Tanımları
#define VGA_START 0xB8000
#define COLOR_WINDOWS_BLUE 0x1F  
#define COLOR_BUTTON_ACTIVE 0x70 

// 1. ÖNCE EN MARJİNAL YARDIMCI FONKSİYONLARI TANIMLIYORUZ (GCC HATA VERMESİN)
int sky_strcmp(const char* s1, const char* s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return *(unsigned char*)s1 - *(unsigned char*)s2;
}

// Port İşlemleri
static inline uint8_t inb(uint16_t port) {
    uint8_t data;
    __asm__ __volatile__("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

static inline void outb(uint16_t port, uint8_t val) {
    __asm__ __volatile__("outb %0, %1" : : "a"(val), "Nd"(port));
}

// Zaman oyalama fonksiyonu
void sky_ui_delay(int count) {
    for (volatile int i = 0; i < count * 500; i++);
}

// Fare Çipini Aktif Etme
void init_mouse(void) {
    uint8_t status;
    outb(0x64, 0xA8);
    while((inb(0x64) & 0x02));
    outb(0x64, 0x20); 
    while(!(inb(0x64) & 0x01));
    status = (inb(0x60) | 0x02); 
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

// Ekranı Windows Mavisine Boyayan Grafik Motoru
void paint_windows_bg(void) {
    uint16_t *vga_buffer = (uint16_t*)VGA_START;
    for (int i = 0; i < 80 * 25; i++) {
        vga_buffer[i] = (COLOR_WINDOWS_BLUE << 8) | ' '; 
    }
    sky_clear_screen(); 
}

// Windows OOBE Arayüz Çizicisi
void draw_windows_ui(void) {
    paint_windows_bg();
    
    if (setup_state == 0) {
        sky_print("\n\n                  W I N D O W S   S T Y L E   O O B E\n");
        sky_print("               -------------------------------------------\n\n\n");
        sky_print("         Bolge secimiyle baslayalim. Dogru yer mi?\n\n");
        sky_print("                     [  Turkiye  ]\n\n\n\n\n\n");
        sky_print("         -----------------------------------------------------\n");
        sky_print("         | FARE ILE TIKLA ->    [ ILERI ] (X:40-48, Y:15)    |\n");
        sky_print("         -----------------------------------------------------\n");
    }
    else if (setup_state == 1) {
        sky_print("\n\n                  W I N D O W S   S T Y L E   O O B E\n");
        sky_print("               -------------------------------------------\n\n\n");
        sky_print("         Bir aga baglanma zamani. Internetiniz aktif mi?\n\n");
        sky_print("                     Ag: [ Sky_Net_5G_Fiber ]\n\n\n\n");
        sky_print("         -----------------------------------------------------\n");
        sky_print("         | FARE ILE SECIN ->    [ ATLA ]  (X:40-48, Y:14)    |\n");
        sky_print("         -----------------------------------------------------\n");
    }
    else if (setup_state == 2) {
        sky_print("\n\n                  W I N D O W S   S T Y L E   O O B E\n");
        sky_print("               -------------------------------------------\n\n\n");
        sky_print("         Zaman ve Tarih Esitlemesi yapiliyor...\n\n");
        sky_print("         Saat otomatik senkronize edilsin mi?\n\n\n\n");
        sky_print("         -----------------------------------------------------\n");
        sky_print("         | FARE ILE SECIN ->    [ ONAYLA ] (X:40-48, Y:14)   |\n");
        sky_print("         -----------------------------------------------------\n");
    }
    else if (setup_state == 3) {
        sky_print("\n\n\n\n\n               H A Z I R S I N I Z ! \n");
        sky_print("               -------------------------------------------\n\n");
        sky_print("         Windows tabanli Sky-OS kurulum sihirbazi bitti.\n\n\n\n\n");
        sky_print("         -----------------------------------------------------\n");
        sky_print("         | FARE ILE TIKLA ->    [ BASLAYIN ] (X:40-50, Y:15) |\n");
        sky_print("         -----------------------------------------------------\n");
    }
    else if (setup_state == 4) {
        sky_print("========================================================================\n");
        sky_print(" Windows OOBE Shell Loaded v0.9 | Donanim Kontrolu Tamamen Mouse Ile Aktif\n");
        sky_print("========================================================================\n\n");
        sky_print(" Kurulum bitti usta! Fareyi ekranda oynatabilirsin.\n");
        sky_print(" [ NOT: Fare imleci ekranda 'X' harfidir! ]\n\n");
        sky_print("SkyOS> ");
    }
}

// Fare Tıklaması Algılandığında Koordinat Kontrolü Yapan Hitbox Motoru
void handle_mouse_click(void) {
    if (setup_state == 0) {
        if (mouse_x >= 40 && mouse_x <= 52 && mouse_y >= 13 && mouse_y <= 17) {
            setup_state = 1;
            draw_windows_ui();
        }
    }
    else if (setup_state == 1) {
        if (mouse_x >= 40 && mouse_x <= 52 && mouse_y >= 12 && mouse_y <= 16) {
            setup_state = 2;
            draw_windows_ui();
        }
    }
    else if (setup_state == 2) {
        if (mouse_x >= 40 && mouse_x <= 52 && mouse_y >= 12 && mouse_y <= 16) {
            setup_state = 3;
            draw_windows_ui();
        }
    }
    else if (setup_state == 3) {
        if (mouse_x >= 40 && mouse_x <= 52 && mouse_y >= 13 && mouse_y <= 17) {
            setup_state = 4;
            draw_windows_ui();
        }
    }
}

// Ekrana Fare İmlecini ('X') Basan Grafiksel Yenileyici
void refresh_mouse_pointer(int old_x, int old_y) {
    uint16_t *vga_buffer = (uint16_t*)VGA_START;
    vga_buffer[old_y * 80 + old_x] = (COLOR_WINDOWS_BLUE << 8) | ' ';
    vga_buffer[mouse_y * 80 + mouse_x] = (0x1C << 8) | 'X';
}

// Klavye & Fare Çiftli Donanım Tarayıcı (Ultra Polling Combo)
void handle_hardware_polling(void) {
    uint8_t status = inb(0x64);
    
    // 1. FARE VERİSİ GELDİ Mİ?
    if ((status & 0x01) && (status & 0x20)) {
        uint8_t data = inb(0x60);
        mouse_byte[mouse_cycle++] = data;
        
        if (mouse_cycle == 3) { 
            mouse_cycle = 0;
            
            if (mouse_byte[0] & 0x01) {
                handle_mouse_click();
                return;
            }
            
            int old_x = mouse_x;
            int old_y = mouse_y;
            
            int8_t move_x = mouse_byte[1];
            int8_t move_y = mouse_byte[2];
            
            mouse_x += (move_x / 4);
            mouse_y -= (move_y / 4); 
            
            if (mouse_x < 0) mouse_x = 0;
            if (mouse_x > 79) mouse_x = 79;
            if (mouse_y < 0) mouse_y = 0;
            if (mouse_y > 24) mouse_y = 24;
            
            if (old_x != mouse_x || old_y != mouse_y) {
                refresh_mouse_pointer(old_x, old_y);
            }
        }
    }
    
    // 2. KLAVYE VERİSİ GELDİ Mİ?
    else if ((status & 0x01) && !(status & 0x20)) {
        uint8_t scancode = inb(0x60);
        if (!(scancode & 0x80) && setup_state == 4) { 
            static const char kbd_us[128] = {
                0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
              '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
                0,  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',   0,
             '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0,   '*',   0,
              ' ',   0
            };
            char key = kbd_us[scancode];
            if (key == '\n') {
                cmd_buffer[cmd_index] = '\0';
                sky_print("\n");
                
                // ARTIK SKY_STRCMP YUKARIDA OLDUĞU İÇİN GCC ASLA HATA VERMEZ!
                if (sky_strcmp(cmd_buffer, "neofetch") == 0) {
                    sky_print("   Sky-OS x86 Windows-OOBE Edition\n");
                    sky_print("   Donanim: Kesmesiz Saf Mouse Surucusu Aktif!\n");
                } else {
                    sky_print("Komut tespiti basarili.\n");
                }
                cmd_index = 0;
                sky_print("SkyOS> ");
            } else if (cmd_index < 255 && key != 0) {
                cmd_buffer[cmd_index++] = key;
                sky_put_char(key);
            }
        }
    }
}

void kernel_main(void) {
    setup_state = 0;
    draw_windows_ui();
    init_mouse();

    while (1) {
        handle_hardware_polling();
        sky_ui_delay(1); 
    }
}
