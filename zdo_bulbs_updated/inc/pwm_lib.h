#ifndef PWM_LIB
#define PWM_LIB
#include "stm32f4xx.h"
#include "stm32f4xx_tim.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
/**
 *
 * @brief A pwm library functions.
 *
 * functions to allow pwm.
 */

/** @brief Initialization.
 *
 *
 * A function that you use for initing Timers and leds.
 */
void InitLib(void);

/** @brief Switch color.
 *
 * Function switchs active leds.
 * @param order Takes 1 or -1 to change colors forward or backwards.
 */
void change_color(int);

/** @brief Change color intensity.
 *
 * Changing color intensity forward if order > 0 and backwards if < 0.
 * @param intensity Takes values from 0 to 100.
 */
void change_intensity(int);
#endif
