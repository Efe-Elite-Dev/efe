#ifndef WIND_SUBSYSTEM_H
#define WIND_SUBSYSTEM_H

#include <stdint.h>

/* =============================================================================
   1. GRUB MULTIBOOT STANDART YAPISI (Grafik Modu Verilerini Almak İçin Şart)
   ============================================================================= */
struct multiboot_info {
    uint32_t flags;
    uint32_t mem_lower;
    uint32_t mem_upper;
    uint32_t boot_device;
    uint32_t cmdline;
    uint32_t mods_count;
    uint32_t mods_addr;
    uint32_t syms[4];
    uint32_t mmap_length;
    uint32_t mmap_addr;
    uint32_t drives_length;
    uint32_t drives_addr;
    uint32_t config_table;
    uint32_t boot_loader_name;
    uint32_t apm_table;
    uint32_t vbe_control_info;
    uint32_t vbe_mode_info;
    uint16_t vbe_mode;
    uint16_t vbe_interface_seg;
    uint16_t vbe_interface_off;
    uint16_t vbe_interface_len;
    uint64_t framebuffer_addr;
    uint32_t framebuffer_pitch;
    uint32_t framebuffer_width;
    uint32_t framebuffer_height;
    uint8_t  framebuffer_bpp;
    uint8_t  framebuffer_type;
};

/* =============================================================================
   2. SAF ASSEMBLY PORT GEÇİŞLERİ (Donanım Portlarını Koklayan Makine Kodları)
   ============================================================================= */
/* Dışarıdaki donanımsal bir porta (Klavye, Fare vb.) 1 byte veri yazar */
static inline void outb(uint16_t port, uint8_t data) {
    asm volatile("outb %0, %1" : : "a"(data), "Nd"(port));
}

/* Belirtilen donanımsal porttan gelen 1 byte'lık ham veriyi okur */
static inline uint8_t inb(uint16_t port) {
    uint8_t ret;
    asm volatile("inb %1, %0" : "=a"(ret) : "Nd"(port));
    return ret;
}

/* =============================================================================
   3. MERKEZİ ÇEKİRDEK VE GRAFİK SİSTEMLERİ PROTOTİPLERİ
   ============================================================================= */
void init_idt(void);              /* Donanımsal Kesme Tablosunu (IDT) ilklendirir */
void clear_text_screen(void);     /* Eski VGA metin modu kalıntı ekranını süpürür */
void gui_refresh_desktop(void);   /* Masaüstü arka planını arka arabelleğe çizer */
void run_exe_subsystem(void);     /* OOBE pencerelerini sırayla ekrana basar */
void swap_buffers(void);          /* RAM'deki gizli ekranı jilet gibi gerçek ekrana fırlatır */

/* =============================================================================
   4. AKILLI VE YAPAY ZEKALI SÜRÜCÜ ODALARI PROTOTİPLERİ
   ============================================================================= */
/* Fare Alt Sistemi Modülü (mouse.c) */
void init_mouse(void);
void handle_mouse_polling(void);
int ai_mouse_analyze_stress(void);

/* Klavye Alt Sistemi Modülü (keyboard.c) */
void init_keyboard(void);
void check_keyboard_pure(void);
int ai_keyboard_analyze_cadence(void);

/* =============================================================================
   5. MERKEZİ YAPAY ZEKA SİNİR MERKEZİ (ai_subsystem.c)
   ============================================================================= */
/* Fare ve klavyeden gelen katsayıları ağırlık matrisinde birleştirip karar verir */
int ai_core_predict_scheduler(int mouse_stress, int kb_cadence, int loop_count);

#endif /* WIND_SUBSYSTEM_H */
