#ifndef GUI_H
#define GUI_H

#include <stdint.h>

void gui_draw_rect(int x, int y, int width, int height, uint32_t color);
void gui_refresh_desktop(void);

#endif /* GUI_H */
