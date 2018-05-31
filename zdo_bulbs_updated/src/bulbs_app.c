#include "bulbs_app.h"
#define ORDER 1
#define COUNT 3
#define INTESITY_LEVELS 10
#define MAX_INTENSITY 100

typedef struct toggle_s {
    uint8_t intensity;
    int on;    
}toggle_s;

static toggle_s toggles[COUNT] = {
                                    {.intensity = 0, .on = 1},
                                    {.intensity = 0, .on = 1},
                                    {.intensity = 0, .on = 1}
                                };

static int current_channel;
//static uint32_t *ccr_arr[] = { &(TIM1->CCR1), &(TIM1->CCR2), &(TIM1->CCR3) };

void InitApp() {
    InitLib();
    current_channel = 0;
}


void switch_channel() {
    current_channel = (current_channel + ORDER + COUNT) % COUNT;
    change_color(current_channel); 
}

void increase_intensity() {
    if(toggles[current_channel].on) {
        toggles[current_channel].intensity = 
            (toggles[current_channel].intensity + INTESITY_LEVELS) % MAX_INTENSITY;
        change_intensity(toggles[current_channel].intensity);
    }
}

void toggle_channel() {
    if(toggles[current_channel].on) {
        change_intensity(0);
    } else {
        change_intensity(toggles[current_channel].intensity);
    }
    toggles[current_channel].on = !toggles[current_channel].on;
}
