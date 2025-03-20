#include "stm32f4xx_hal.h"
#ifndef __ADC_H_
#define __ADC_H_

#define RESOLUTION_12B 4096U
#define VREF 3.3f


  int ADC_init(void);
  int32_t ADC_getValue(uint32_t channel);
#endif
