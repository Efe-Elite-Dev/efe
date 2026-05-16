#include "wind_subsystem.h"

/* Masaüstünü pır-pır etmeden tamamen arka planda boyayan motor */
void gui_refresh_desktop(void) {
    /* Tüm ekranı asil Wind OS Gece Mavisi rengine boyuyoruz */
    for (int y = 0; y < 600; y++) {
        for (int x = 0; x < 800; x++) {
            /* Doğrudan draw_pixel_pure çağırarak back_buffer'a yazar */
            draw_pixel_pure(x, y, 0x000B1E36); 
        }
    }
    
    /* En alta şık bir görev çubuğu (Taskbar) alanı çekelim */
    for (int y = 560; y < 600; y++) {
        for (int x = 0; x < 800; x++) {
            draw_pixel_pure(x, y, 0x00111111);
        }
    }
}
