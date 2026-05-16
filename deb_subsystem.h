#ifndef DEB_SUBSYSTEM_H
#define DEB_SUBSYSTEM_H

#include <stdint.h>

static inline void run_deb_subsystem(int win_x, int win_y) {
    gui_draw_rect(win_x + 15, win_y + 45, 470, 290, 0x212121);
    gui_draw_rect(win_x + 40, win_y + 70, 40, 40, 0xD70A53);
}

#endif
