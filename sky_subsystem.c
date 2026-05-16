#include "sky_subsystem.h"

uint32_t* vbe_vram = (uint32_t*)0xE0000000;
uint32_t  vbe_pitch = SCREEN_WIDTH * 4;
uint32_t  back_buffer[TOTAL_PIXELS];
int setup_completed = 0;

// Linker çakışmalarını bitiren asıl tanımlamalar
int active_window = 0;

void run_sky_subsystem(void) {
    // Okul versiyonu için ileride alt sistem alt yapısını burası tetikleyecek
    setup_completed = 1;
}
