#include "wind_subsystem.h"
#include "ai_subsystem.h"

int ai_predict_hardware_load(int mouse_delta_x, int loop_count) {
    if (mouse_delta_x < 0) mouse_delta_x = -mouse_delta_x;
    int core_load = (mouse_delta_x * 3) + ((loop_count % 30) * 2);
    return (core_load > 250) ? 1 : 0;
}
