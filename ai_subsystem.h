#ifndef AI_SUBSYSTEM_H
#define AI_SUBSYSTEM_H

#include <stdint.h>

void gui_draw_rect(int start_x, int start_y, int width, int height, uint32_t color);

int sky_ai_analyze_intent(uint32_t input_hash);
void run_ai_core_render(int win_x, int win_y, uint32_t current_tick, int intent);

#endif
