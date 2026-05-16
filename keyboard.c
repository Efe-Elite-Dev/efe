#include "wind_subsystem.h"
#include "keyboard.h"

void init_keyboard(void) {
    /* Master PIC üzerinden IRQ1 (Klavye) Kesmesini Açma */
    outb(0x21, inb(0x21) & ~(1 << 1));
}
