#ifndef AI_SUBSYSTEM_H
#define AI_SUBSYSTEM_H

#include <stdint.h>

void gui_draw_rect(int start_x, int start_y, int width, int height, uint32_t color);

static inline int sky_ai_analyze_intent(uint32_t input_hash) {
    if (input_hash % 3 == 0) return 1; 
    if (input_hash % 3 == 1) return 2; 
    return 0; 
}

static inline void run_ai_core_render(int win_x, int win_y, uint32_t current_tick, int intent) {
    gui_draw_rect(win_x + 20, win_y + 110, 460, 220, 0x1E1B4B); 

    int pulse = (current_tick / 5) % 8;
    
    if (intent == 1) {
        for (int i = 0; i < 4; i++) {
            gui_draw_rect(win_x + 40 + (i * 100), win_y + 150 + (pulse * 2), 80, 20, 0x10B981);
        }
    } 
    else if (intent == 2) {
        for (int i = 0; i < 6; i++) {
            gui_draw_rect(win_x + 60 + (i * 60) + pulse, win_y + 180 - (i * 5), 15, 15, 0x06B6D4); 
        }
    } 
    else {
        for (int i = 0; i < 10; i++) {
            int wave_height = 20 + (i * pulse) % 50;
            if (wave_height > 80) wave_height = 80;
            gui_draw_rect(win_x + 50 + (i * 40), win_y + 240 - wave_height, 15, wave_height, 0x8B5CF6); 
        }
    }
}

#endif
