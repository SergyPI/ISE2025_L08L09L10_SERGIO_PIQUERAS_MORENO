#include "rtc.h"
  extern osThreadId_t TID_Led;
  RTC_HandleTypeDef RtcHandle;
  RTC_AlarmTypeDef  RtcAlarm;
  RTC_TimeTypeDef stimestructureget;
  RTC_DateTypeDef sdatestructureget;
  bool bit_alarm = false;
  uint8_t segundos;           // variable util para contar segundos hasta alcanzar  mi alarma
 uint8_t periodoAlarma=20;
  void Init_RTC(void){
  RCC_OscInitTypeDef        RCC_OscInitStruct;
  RCC_PeriphCLKInitTypeDef  PeriphClkInitStruct;

  /*##-1- Enables the PWR Clock and Enables access to the backup domain ###################################*/
  /* To change the source clock of the RTC feature (LSE, LSI), You have to:
     - Enable the power clock using __HAL_RCC_PWR_CLK_ENABLE()
     - Enable write access using HAL_PWR_EnableBkUpAccess() function before to 
       configure the RTC clock source (to be done once after reset).
     - Reset the Back up Domain using __HAL_RCC_BACKUPRESET_FORCE() and 
       __HAL_RCC_BACKUPRESET_RELEASE().
     - Configure the needed RTc clock source */
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
  /*Configuracion RTC--------------------------------------------------------*/
  RtcHandle.Instance = RTC; 
  RtcHandle.Init.HourFormat = RTC_HOURFORMAT_24;
  RtcHandle.Init.AsynchPrediv = RTC_ASYNCH_PREDIV;
  RtcHandle.Init.SynchPrediv = RTC_SYNCH_PREDIV;
  RtcHandle.Init.OutPut = RTC_OUTPUT_DISABLE;
  RtcHandle.Init.OutPutPolarity = RTC_OUTPUT_POLARITY_HIGH;
  RtcHandle.Init.OutPutType = RTC_OUTPUT_TYPE_OPENDRAIN;
  __HAL_RTC_RESET_HANDLE_STATE(&RtcHandle);
  HAL_RTC_Init(&RtcHandle);
  


}
void Set_Alarma(void){
  segundos=0;
  /*Configuracion alarma RTC---------------------------------------------------*/
  RtcAlarm.Alarm = RTC_ALARM_A;
  RtcAlarm.AlarmTime.Hours = 0x00;
  RtcAlarm.AlarmTime.Minutes = 13;
  RtcAlarm. AlarmTime.Seconds = 5;                   // Cuando los segundos lleguen a 0 salta alarma
  RtcAlarm.AlarmTime.TimeFormat = RTC_HOURFORMAT_24;
  RtcAlarm.AlarmTime.DayLightSaving = RTC_DAYLIGHTSAVING_NONE;
  RtcAlarm.AlarmTime.StoreOperation = RTC_STOREOPERATION_RESET;
  RtcAlarm.AlarmMask = RTC_ALARMMASK_ALL;
  RtcAlarm.AlarmDateWeekDaySel = RTC_ALARMDATEWEEKDAYSEL_DATE;
  HAL_RTC_SetAlarm_IT(&RtcHandle, &RtcAlarm, RTC_FORMAT_BIN);
   HAL_NVIC_SetPriority(RTC_Alarm_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(RTC_Alarm_IRQn);
}
  fechayhora Obtener_FechaYHora(void){//obtencion de la hora y la fecha del RTC

  fechayhora tiempo;
  HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
  sprintf(tiempo.time,"Time: %.2d:%.2d:%.2d ",stimestructureget.Hours+1,stimestructureget.Minutes,stimestructureget.Seconds);
  HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);
  sprintf(tiempo.date,"Date: %02d / %02d / %02d",sdatestructureget.Date,sdatestructureget.Month+1,sdatestructureget.Year-100);
  return tiempo;
  }


 void RTC_CalendarConfig(uint8_t sec,uint8_t min,uint8_t hour,uint8_t date,uint8_t month,uint8_t year,uint8_t wday)
{
  RTC_TimeTypeDef stimestructure;
  RTC_DateTypeDef sdatestructure;

    /*##-1- Configure the Time #################################################*/
  stimestructure.Hours = hour;
  stimestructure.Minutes = min;
  stimestructure.Seconds = sec;
  stimestructure.TimeFormat = RTC_HOURFORMAT_24;
  stimestructure.DayLightSaving = RTC_DAYLIGHTSAVING_NONE ;
  stimestructure.StoreOperation = RTC_STOREOPERATION_RESET;
  HAL_RTC_SetTime(&RtcHandle, &stimestructure, RTC_FORMAT_BIN);
  
  /*##-2- Configure the Date #################################################*/
  sdatestructure.Year = year;
  sdatestructure.Month = month;
  sdatestructure.Date = date;
  sdatestructure.WeekDay = wday;
  HAL_RTC_SetDate(&RtcHandle,&sdatestructure,RTC_FORMAT_BIN);

  /*##-3- Writes a data in a RTC Backup data Register1 #######################*/
  HAL_RTCEx_BKUPWrite(&RtcHandle, RTC_BKP_DR1, 0x32F2);
}
 void RTC_CalendarShow(char *showtime, char *showdate)
{
    /* Get the RTC current Time */
  HAL_RTC_GetTime(&RtcHandle, &stimestructureget, RTC_FORMAT_BIN);
  /* Get the RTC current Date */
  HAL_RTC_GetDate(&RtcHandle, &sdatestructureget, RTC_FORMAT_BIN);
  /* Display date Format : mm-dd-yy in STDOUT depurador*/
  sprintf(showdate, "%02d-%02d-%02d", sdatestructureget.Month, sdatestructureget.Date, 2000 + sdatestructureget.Year);
  /* Display time Format : hh:mm:ss in STDOUT depurador*/
  sprintf(showtime, "%02d:%02d:%02d", stimestructureget.Hours + 1, stimestructureget.Minutes, stimestructureget.Seconds);

}
void RTC_Alarm_IRQHandler(void){
  HAL_RTC_AlarmIRQHandler(&RtcHandle);

}
void HAL_RTC_AlarmAEventCallback(RTC_HandleTypeDef *RtcHandle){
  segundos++;      //importante incrementarlo fuera del if o me hará las 2 (if y else) en el límite de condicion if
  if(segundos<180){ //genera alarma cada minuto, pues pasados 60 seg se envia señal de encender led al hilo led
  }
  else{
    segundos=0;
    osThreadFlagsSet(TID_Led,ALARMA_RTC);  //envio señal al hilo led para que ejecute funcion una vez salta la alarma
    bit_alarm = true;
  }
}