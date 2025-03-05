#include "rtc.h"
#include <stdio.h>

/* RTC handler declaration */
RTC_HandleTypeDef RtcHandle;

RTC_TimeTypeDef stimestructure;
volatile bool AlarmaFlag = false;


void initialize_RTC(void)
{
    RTC_DateTypeDef sdatestructure;
 
	//-----RTC config----//
RCC_OscInitTypeDef        RCC_OscInitStruct;
RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;
  __HAL_RCC_PWR_CLK_ENABLE();
  HAL_PWR_EnableBkUpAccess();

  
  /*##-2- Configure LSE as RTC clock source ###################################*/
  RCC_OscInitStruct.OscillatorType =  RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
    if(HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  { 
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_RTC;
  PeriphClkInitStruct.RTCClockSelection = RCC_RTCCLKSOURCE_LSE;
    if(HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  { 
  }
  /*##-3- Enable RTC peripheral Clocks #######################################*/
  /* Enable RTC Clock */
  __HAL_RCC_RTC_ENABLE();
    RtcHandle.Instance = RTC; 
    RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
    RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
    RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
    RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
    RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
    RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
    __HAL_RTC_RESET_HANDLE_STATE(&RtcHandle);

    if (HAL_RTC_Init(&RtcHandle) != HAL_OK)
    {
        /* Initialization Error */
        //Error_Handler();
    }
		
    uint32_t backupVal = HAL_RTCEx_BKUPRead(&RtcHandle, RTC_BKP_DR1);
		
		if (backupVal != 0x32F2) {
        // => Primera vez que iniciamos: ponemos fecha/hora
        RTC_DateTypeDef sdatestructure;
        RTC_TimeTypeDef stimestructure;
    /*##-1- Configure the Date #################################################*/
    /* Set Date: Tuesday February 18th 2014 */
        sdatestructure.Year = 0x14;
        sdatestructure.Month = RTC_MONTH_FEBRUARY;
        sdatestructure.Date = 0x18;
        sdatestructure.WeekDay = RTC_WEEKDAY_TUESDAY;
			
        HAL_RTC_SetDate(&RtcHandle, &sdatestructure, RTC_FORMAT_BCD);
			
    /*##-2- Configure the Time #################################################*/
    /* Set Time: 02:00:00 */
        stimestructure.Hours = 0x02;
        stimestructure.Minutes = 0x00;
        stimestructure.Seconds = 0x55;
        stimestructure.TimeFormat = RTC_HOURFORMAT12_AM;
        stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
        stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;
			
			
        HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BCD);

        // Escribimos el valor mágico para decir "Ya configurado"
        HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x32F2);
    }
}

void show_RTC_Calendar(uint8_t *showtime, uint8_t *showdate)
{
    RTC_DateTypeDef sdatestructureget;
    RTC_TimeTypeDef stimestructureget;

    /* Get the RTC current Time */
    HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
    /* Get the RTC current Date */
    HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);
    /* Display time Format : hh:mm:ss */
    sprintf((char *)showtime, "%02d:%02d:%02d", stimestructureget.Hours, stimestructureget.Minutes, stimestructureget.Seconds);
    /* Display date Format : mm-dd-yyyy */
    sprintf((char *)showdate, "%02d-%02d-%04d", sdatestructureget.Month, sdatestructureget.Date, 2000 + sdatestructureget.Year);
}

void RTC_alarma(RTC_HandleTypeDef *hrtc) {
  RTC_AlarmTypeDef sAlarm;
  memset(&sAlarm, 0, sizeof(sAlarm));

  // Configurar Alarma A
  sAlarm.Alarm                = RTC_ALARM_A;
  sAlarm.AlarmTime.Hours      = stimestructure.Hours ;  // Ignorados por Mask
  sAlarm.AlarmTime.Minutes    = (stimestructure.Minutes + 1)%60 ;  // Ignorados por Mask
  sAlarm.AlarmTime.Seconds    = 0;  // Cuando "segundos = 0"
  sAlarm.AlarmTime.TimeFormat = RTC_HOURFORMAT_24;
  sAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  sAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;

  // Ignoramos horas y fecha => suena cada vez que segundos = 0 (cada minuto)
  sAlarm.AlarmMask = RTC_ALARMMASK_HOURS 
                   | RTC_ALARMMASK_MINUTES 
                   | RTC_ALARMMASK_DATEWEEKDAY
                   | RTC_ALARMDATEWEEKDAYSEL_DATE ;
  sAlarm.AlarmSubSecondMask = RTC_ALARMSUBSECONDMASK_ALL;

  sAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  sAlarm.AlarmDateWeekDay    = 1; // se ignora con la Mask
	
  // Programar la alarma en hardware:
  if (HAL_RTC_SetAlarm_IT(hrtc, &sAlarm, RTC_FORMAT_BIN) != HAL_OK) {
    // Manejo de error (Error_Handler, printf, etc.)
  }
  // Habilitar la interrupción del RTC en el NVIC
  HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
	
}
void set_rtc_year(int year) {
  // Ajustar la fecha/hora usando HAL
  //  1) Leer la fecha/hora actual
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;
  HAL_RTC_GetTime(&RtcHandle, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&RtcHandle, &sDate, RTC_FORMAT_BIN);

  //  2) Cambiar solo el campo year (restando 2000 si trabajas en binario)
  sDate.Year = (uint8_t)(year - 2000);

  //  3) Volver a programar
  HAL_RTC_SetDate(&RtcHandle, &sDate, RTC_FORMAT_BIN);
}

void set_rtc_month(int month) {
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;
  HAL_RTC_GetTime(&RtcHandle, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&RtcHandle, &sDate, RTC_FORMAT_BIN);

  sDate.Month = (uint8_t)month;
  HAL_RTC_SetDate(&RtcHandle, &sDate, RTC_FORMAT_BIN);
}

void set_rtc_day(int day) {
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;
  HAL_RTC_GetTime(&RtcHandle, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&RtcHandle, &sDate, RTC_FORMAT_BIN);

  sDate.Date = (uint8_t)day;
  HAL_RTC_SetDate(&RtcHandle, &sDate, RTC_FORMAT_BIN);
}

void set_rtc_hour(int hour) {
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;
  HAL_RTC_GetTime(&RtcHandle, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&RtcHandle, &sDate, RTC_FORMAT_BIN);

  sTime.Hours = (uint8_t)hour;
  HAL_RTC_SetTime(&RtcHandle, &sTime, RTC_FORMAT_BIN);
}

void set_rtc_minute(int minute) {
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;
  HAL_RTC_GetTime(&RtcHandle, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&RtcHandle, &sDate, RTC_FORMAT_BIN);

  sTime.Minutes = (uint8_t)minute;
  HAL_RTC_SetTime(&RtcHandle, &sTime, RTC_FORMAT_BIN);
}

void set_rtc_second(int second) {
  RTC_TimeTypeDef sTime;
  RTC_DateTypeDef sDate;
  HAL_RTC_GetTime(&RtcHandle, &sTime, RTC_FORMAT_BIN);
  HAL_RTC_GetDate(&RtcHandle, &sDate, RTC_FORMAT_BIN);

  sTime.Seconds = (uint8_t)second;
  HAL_RTC_SetTime(&RtcHandle, &sTime, RTC_FORMAT_BIN);
}
