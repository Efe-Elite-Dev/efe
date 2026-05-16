#include "wind_subsystem.h"

#define KEYBOARD_DATA_PORT 0x60
#define KEYBOARD_STATUS_PORT 0x64

/* KLAVYE AI İÇ DEĞİŞKENLERİ */
static int keyboard_stroke_count = 0;
static int keyboard_ai_cadence_score = 0;

void init_keyboard(void) {
    /* İleride PIC kesme maskeleri buraya eklenecektir */
}

/* KLAVYE ODASI YAPAY ZEKASI: Yazma sıklığına göre işlemci zamanlayıcısına tempo puanı üretir */
int ai_keyboard_analyze_cadence(void) {
    if (keyboard_stroke_count > 15) {
        keyboard_ai_cadence_score = 3; /* Yoğun veri girişi var, zamanlayıcıyı daralt */
    } else if (keyboard_stroke_count > 5) {
        keyboard_ai_cadence_score = 2; /* Akıcı yazma temposu */
    } else {
        keyboard_ai_cadence_score = 1; /* Klavye dinlenmede */
    }
    return keyboard_ai_cadence_score;
}

char scancode_to_ascii_core(unsigned char scancode) {
    switch(scancode) {
        case 0x1E: return 'A'; case 0x30: return 'B'; case 0x2E: return 'C';
        case 0x20: return 'D'; case 0x12: return 'E'; case 0x21: return 'F';
        case 0x22: return 'G'; case 0x23: return 'H'; case 0x17: return 'I';
        case 0x24: return 'J'; case 0x25: return 'K'; case 0x26: return 'L';
        case 0x32: return 'M'; case 0x31: return 'N'; case 0x18: return 'O';
        case 0x19: return 'P'; case 0x10: return 'Q'; case 0x13: return 'R';
        case 0x1F: return 'S'; case 0x14: return 'T'; case 0x16: return 'U';
        case 0x2F: return 'V'; case 0x11: return 'W'; case 0x2D: return 'X';
        case 0x15: return 'Y'; case 0x2C: return 'Z';
        case 0x39: return ' ';  
        case 0x1C: return '\n'; 
        default: return 0;
    }
}

void check_keyboard_pure(void) {
    static unsigned char last_kb_scancode = 0;
    
    if (inb(KEYBOARD_STATUS_PORT) & 1) {
        unsigned char scancode = inb(KEYBOARD_DATA_PORT);
        
        if (scancode != last_kb_scancode && scancode < 0x80) {
            char ascii = scancode_to_ascii_core(scancode);
            if (ascii != 0) {
                /* AI verisini besle */
                keyboard_stroke_count += 3;
            }
        }
        last_kb_scancode = scancode;
    } else {
        if (keyboard_stroke_count > 0) keyboard_stroke_count--; /* AI sönümlendirme */
    }
}
