#ifndef BUTTON_H
#define BUTTON_H

#include "stm32f4xx.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"

void Init_Peripherals(void);

extern void handle_left_button(void);
extern void  handle_right_button(void); 

#endif
