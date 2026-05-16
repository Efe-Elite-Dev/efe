#ifndef MOUSE_H
#define MOUSE_H

#include "sky_subsystem.h"

#define MOUSE_BIT_SYNC    0x08
#define MOUSE_X_OVERFLOW  0x40
#define MOUSE_Y_OVERFLOW  0x80

extern int mouse_x;
extern int mouse_y;
extern int mouse_left_click;
extern int mouse_right_click;
extern int mouse_middle_click;

extern const uint8_t mouse_pointer_sprite[19][12];

void init_mouse(void);
void handle_mouse_polling(void);

#endif // MOUSE_H
