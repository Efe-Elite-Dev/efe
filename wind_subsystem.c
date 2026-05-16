#include "wind_subsystem.h"

/* Wind OS Alt Sistem Durum Belirteci (1: Kurulum, 2: Canlı Masaüstü) */
int wind_system_status = 1;

int get_wind_status(void) {
    return wind_system_status;
}
