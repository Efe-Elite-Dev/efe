#ifndef MOUSE_H
#define MOUSE_H

#include <stdint.h>

// Ekran çözünürlük sınırları (mouse.c içindeki taşma kontrolleri için gerekli)
#define SCREEN_WIDTH     800
#define SCREEN_HEIGHT    600

// mouse.c içindeki global fare değişkenleri
extern int mouse_x;
extern int mouse_y;
extern int mouse_left_click;
extern int mouse_right_click;
extern int mouse_middle_click;

// Fare okunun görsel matrisi (Sprite)
extern const uint8_t mouse_pointer_sprite[19][12];

// kernel.c tarafından çağrılacak fonksiyon prototipleri
void init_mouse(void);
void handle_mouse_polling(void);

// Alt seviye port fonksiyonları
unsigned char inb(unsigned short port);
void outb(unsigned short port, unsigned char data);

#endif // MOUSE_H
