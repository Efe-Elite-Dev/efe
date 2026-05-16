#ifndef SKY_SUBSYSTEM_H
#define SKY_SUBSYSTEM_H

#include <stdint.h>

// === GLOBAL EKRAN YAPILANDIRMASI ===
#define SCREEN_WIDTH     800
#define SCREEN_HEIGHT    600
#define TOTAL_PIXELS     (SCREEN_WIDTH * SCREEN_HEIGHT)

// === KUTSAL DONANIM PORT KÖPRÜLERİ (APPLE HAL TARZI) ===
static inline uint8_t inb(uint16_t port) {
    uint8_t data;
    __asm__ __volatile__("inb %1, %0" : "=a"(data) : "Nd"(port));
    return data;
}

static inline void outb(uint16_t port, uint8_t data) {
    __asm__ __volatile__("outb %0, %1" : : "a"(data), "Nd"(port));
}

// === MULTIBOOT VE VRAM PAYLAŞIMI ===
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

extern uint32_t* vbe_vram;
extern uint32_t  vbe_pitch;
extern uint32_t  back_buffer[TOTAL_PIXELS];
extern int setup_completed;

// === GUI VE ALT SİSTEM KÖPRÜLERİ ===
extern int active_window;
void run_sky_subsystem(void);

#endif // SKY_SUBSYSTEM_H
