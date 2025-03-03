#ifndef LEDS_H
#define LEDS_H

#include "stm32f4xx_hal.h"

extern void Init_Led(void);
extern void salida_leds(uint8_t leds,uint8_t rgb);
extern void Init_Leds_RGB(void);
extern void Init_Full_Leds(void);
#endif // LEDS_H
