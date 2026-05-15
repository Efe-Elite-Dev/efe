#include "screen.h"

#define VIDEO_ADDRESS 0xB8000
#define MAX_ROWS 25
#define MAX_COLS 80
#define WHITE_ON_BLACK 0x0F

static int cursor_x = 0;
static int cursor_y = 0;

// Donanımsal imleci (yanıp sönen çizgi) işlemci portlarını kullanarak hareket ettirir
void update_cursor(void) {
    unsigned short position = (cursor_y * MAX_COLS) + cursor_x;

    // İşlemciye imleç pozisyonunun düşük byte'ını göndereceğimizi söylüyoruz (0x3D4)
    // Sonra veriyi (0x3D5) portuna yazıyoruz
    __asm__ __volatile__("outb %%al, %%dx" : : "a"(0x0F), "d"(0x3D4));
    __asm__ __volatile__("outb %%al, %%dx" : : "a"((unsigned char)(position & 0xFF)), "d"(0x3D5));
    
    // Yüksek byte için aynısı
    __asm__ __volatile__("outb %%al, %%dx" : : "a"(0x0E), "d"(0x3D4));
    __asm__ __volatile__("outb %%al, %%dx" : : "a"((unsigned char)((position >> 8) & 0xFF)), "d"(0x3D5));
}

void sky_clear_screen(void) {
    volatile char* video_memory = (volatile char*)VIDEO_ADDRESS;
    for (int i = 0; i < MAX_ROWS * MAX_COLS * 2; i += 2) {
        video_memory[i] = ' ';
        video_memory[i + 1] = WHITE_ON_BLACK;
    }
    cursor_x = 0;
    cursor_y = 0;
    update_cursor();
}

void sky_put_char(char c) {
    volatile char* video_memory = (volatile char*)VIDEO_ADDRESS;

    if (c == '\n') {
        cursor_x = 0;
        cursor_y++;
    } else {
        int offset = (cursor_y * MAX_COLS + cursor_x) * 2;
        video_memory[offset] = c;
        video_memory[offset + 1] = WHITE_ON_BLACK;
        cursor_x++;
    }

    // Satır sonuna gelindiyse alt satıra geç
    if (cursor_x >= MAX_COLS) {
        cursor_x = 0;
        cursor_y++;
    }

    // Ekranın en altına gelindiyse ekranı yukarı kaydır (Basit kaydırma)
    if (cursor_y >= MAX_ROWS) {
        sky_clear_screen();
    }

    update_cursor();
}

void sky_print(const char* str) {
    int i = 0;
    while (str[i] != '\0') {
        sky_put_char(str[i]);
        i++;
    }
}
