#ifndef RTC_H
#define RTC_H

#include "stm32f4xx_hal.h"
#include <time.h>
#include <stdio.h>
#include "main.h"
#include <stdbool.h>

/*Como el reloj LSE es de 32768 Hz es necesario dividir por 128 y 256 para tener 1 Hz de frecuencia*/
#define RTC_ASYNCH_PREDIV  0x7F   /* 127 LSE as RTC clock */
#define RTC_SYNCH_PREDIV   0x00FF /* 255 LSE as RTC clock */
#define ALARMA_RTC 0x04

typedef struct {
	char time[30];
	char date[30];
 } fechayhora;

extern fechayhora Obtener_FechaYHora(void);
extern RTC_TimeTypeDef stimestructureget;
extern RTC_DateTypeDef sdatestructureget;
extern RTC_HandleTypeDef RtcHandle;
extern RTC_AlarmTypeDef  RtcAlarm;
extern bool bit_alarm;
extern uint8_t segundos;

extern void Init_RTC(void);
extern void RTC_CalendarShow(char *showtime, char *showdate);
extern void RTC_CalendarConfig(uint8_t sec,uint8_t min,uint8_t hour,uint8_t date,uint8_t month,uint8_t year,uint8_t wday);
extern void RTC_Alarm_IRQHandler(void);
extern void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *RtcHandle);
extern void Set_Alarma(void);

#endif