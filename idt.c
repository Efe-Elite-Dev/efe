#include "wind_subsystem.h"
#include "idt.h"

void init_idt(void) {
    /* Çakışma Önleyici Güvenli Boş IDT Kaydı */
    volatile uint16_t idt_pointer[3];
    idt_pointer[0] = 0;
    idt_pointer[1] = 0;
    idt_pointer[2] = 0;
    asm volatile("lidt (%0)" : : "r"(idt_pointer));
}
