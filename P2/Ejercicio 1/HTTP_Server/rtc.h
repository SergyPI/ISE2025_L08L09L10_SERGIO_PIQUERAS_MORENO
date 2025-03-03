#ifndef RTC_H
#define RTC_H
#include "main.h"
#include "stdio.h"
#include "stm32f4xx_hal.h"
#include "stm32f4xx_it.h"
#include "string.h"
#include "stdbool.h"

/* Defines related to Clock configuration */
#define RTC_ASYNCH_PREDIV  0x7F   /* LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* LSE as RTC clock */

extern RTC_HandleTypeDef RtcHandle;
extern volatile bool AlarmaFlag;

void initialize_RTC(void);
void show_RTC_Calendar(uint8_t *showtime, uint8_t *showdate);
void RTC_alarma(RTC_HandleTypeDef *hrtc);
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *RtcHandle);
void Alarma (void);
#endif // RTC_H