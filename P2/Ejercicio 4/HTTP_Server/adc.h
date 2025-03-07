#ifndef ADC_H
#define ADC_H
#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stm32f4xx_hal.h"
#define RESOLUTION_12B 4096U
#define VREF 3.3f
#include "math.h"

extern int32_t ADC_StartConversion (void) ;
extern int32_t ADC_ConversionDone (void) ;
extern void Init_ADC(void);
extern int ADC_Init_Single_Conversion(ADC_HandleTypeDef *, ADC_TypeDef  *);
extern float ADC_getVoltage(ADC_HandleTypeDef * , uint32_t );
extern int32_t ADC_GetValue (void);
#endif
