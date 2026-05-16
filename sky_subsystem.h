#ifndef SKY_SUBSYSTEM_H
#define SKY_SUBSYSTEM_H

#include <stdint.h>

void gui_draw_rect(int start_x, int start_y, int width, int height, uint32_t color);

void run_sky_subsystem(int win_x, int win_y, uint32_t current_tick);

#endif
