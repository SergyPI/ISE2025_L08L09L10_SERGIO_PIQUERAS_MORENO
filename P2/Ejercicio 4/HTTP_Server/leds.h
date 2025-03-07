#ifndef LEDS_H
#define LEDS_H

#include "stm32f4xx_hal.h"

extern void Init_All_Leds(void);
extern void salida_leds(uint8_t leds,uint8_t rgb);
extern void reset_leds(void);
#endif // LEDS_H
