/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server.c
 * Purpose: HTTP Server example
 *----------------------------------------------------------------------------*/

#include <stdio.h>

#include "main.h"
#include "rtc.h"

#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE

#include "stm32f4xx_hal.h"              // Keil::Device:STM32Cube HAL:Common
//#include "Board_LED.h"                  // ::Board Support:LED
//#include "Board_Buttons.h"              // ::Board Support:Buttons
//#include "Board_ADC.h"                  // ::Board Support:A/D Converter
//#include "Board_GLCD.h"                 // ::Board Support:Graphic LCD
//#include "GLCD_Config.h"                // Keil.MCBSTM32F400::Board Support:Graphic LCD

//----------Librerias de mi placa---------//
#include "leds.h"
#include "lcd.h"
#include "adc1.h"
#define LCD 0x01

/* Buffers used for displaying Time and Date */
uint8_t aShowTime[10] = {0};
uint8_t aShowDate[10] = {0};

float val ;
ADC_HandleTypeDef adchandle;

// Main stack size must be multiple of 8 Bytes
#define APP_MAIN_STK_SZ (1024U)
uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
};

//extern GLCD_FONT GLCD_Font_6x8;
//extern GLCD_FONT GLCD_Font_16x24;

extern uint16_t AD_in          (uint32_t ch);//Potenciometro
extern uint8_t  get_button     (void);
extern void     netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len);

extern bool LEDrun;
extern char lcd_text[2][20+1];

//extern osThreadId_t TID_Display;
extern osThreadId_t TID_Led;
extern osThreadId_t TID_RTC;
extern osThreadId_t TID_Alarm;
bool LEDrun;
char lcd_text[2][20+1] = { "LCD line 1",
                          "LCD line 2" };

/* Thread IDs */
osThreadId_t TID_Display;
osThreadId_t TID_Led;
osThreadId_t TID_RTC;
osThreadId_t TID_Alarm;
													
///* Thread declarations */
static void BlinkLed (void *arg);
static void Display  (void *arg);
static void ShowTimeThread (void *arg);

__NO_RETURN void app_main (void *arg);

/* Read analog inputs */ //Potenciometros
uint16_t AD_in(uint32_t ch) {
    if (ch == 0) {
        val = ADC_getVoltage(&adchandle, ADC_CHANNEL_10); // PC0
    } else if (ch == 1) {
        val = ADC_getVoltage(&adchandle, ADC_CHANNEL_13); // PC3
    }
    return ((uint16_t)val);
}


/* Read digital inputs */
uint8_t get_button (void) {
 //return ((uint8_t)Buttons_GetState ());
	return 0;
}

/* IP address change notification */
void netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len) {

  (void)if_num;
  (void)val;
  (void)len;

  if (option == NET_DHCP_OPTION_IP_ADDRESS) {
    /* IP address change, trigger LCD update */
    osThreadFlagsSet (TID_Display, 0x01);
  }
}

/*----------------------------------------------------------------------------
  Thread 'Display': LCD display handler
 *---------------------------------------------------------------------------*/
static __NO_RETURN void Display (void *arg) {
  int n;
  int i;
  static char buf[24];
  LCD_reset();
  LCD_init();
  LCD_Clean();
  n = sprintf(buf, "Inicio de programa");

  for (i = 0; i < n; i++) {
    LCD_symbolToLocalBuffer_L1(buf[i]);
  }

  n = sprintf(buf, "Bienvenido Sergio");
  for (i = 0; i < n; i++) {
    LCD_symbolToLocalBuffer_L2(buf[i]);
  }
  LCD_update();
  while (1) {
    osThreadFlagsWait(LCD, osFlagsWaitAll, osWaitForever); // espera que le llegue la señal LCD
    LCD_Clean();
    n = sprintf(buf, "%-10s", lcd_text[0]);
    int positionL1 = 0;
    for (i = 0; i < n; i++) {
      LCD_symbolToLocalBuffer_L1(buf[i]);
    }
    int positionL2 = 0;
    n = sprintf(buf, "%-10s", lcd_text[1]);
    for (i = 0; i < n; i++) {
      LCD_symbolToLocalBuffer_L2(buf[i]);
    }
    LCD_update();
  }
}

/*----------------------------------------------------------------------------
  Thread 'BlinkLed': Blink the LEDs on an eval board
 *---------------------------------------------------------------------------*/
static __NO_RETURN void BlinkLed (void *arg) {
  int cuenta = 0;

  //(void)arg;

  LEDrun = false;
  while(1) {
    /* Every 100 ms */
    if (LEDrun == true) {
      if(cuenta ==0){
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0,GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7|GPIO_PIN_14,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13,GPIO_PIN_SET);
      }
      else if(cuenta==1){
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_14,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7,GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11|GPIO_PIN_13,GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12,GPIO_PIN_RESET);
      }
      else if(cuenta==2){
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_7,GPIO_PIN_RESET);
        HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14,GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11|GPIO_PIN_12,GPIO_PIN_SET);
        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13,GPIO_PIN_RESET);
      }
    }
    cuenta++;
    if (cuenta>2) {
      cuenta = 0U;
    }
    osDelay (100);
  }
}

/*----------------------------------------------------------------------
  Thread 'ShowTimeThread': Show the current date and time
 *---------------------------------------------------------------------------*/
static __NO_RETURN void ShowTimeThread (void *arg) {
  (void)arg;
 RTC_alarma(&RtcHandle); // INICIALIZACION ALARMA
	
  while (1) {
		
    // Display the current date and time
    show_RTC_Calendar(aShowTime, aShowDate);
		sprintf((char*)lcd_text[0], "%s", aShowTime); // Hora en la primera línea
    sprintf((char*)lcd_text[1], "%s", aShowDate); // Fecha en la segunda línea
		osThreadFlagsSet(TID_Display, LCD);
		osDelay(1000);

  }
}
/*----------------------------------------------------------------------
  Thread 'ShowTimeThread': Show the current date and time
 *---------------------------------------------------------------------------*/
__NO_RETURN void BlinkOnAlarmThread (void *arg) {
  (void)arg;
  while (1) {
    if (AlarmaFlag) {
      AlarmaFlag = false;
      // Parpadear LED
      for (int i = 0; i < 10; i++) {
        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
        osDelay(500);
      }
    }
  }
}
/*----------------------------------------------------------------------
  Main Thread 'main': Run Network
 *---------------------------------------------------------------------------*/
extern RTC_HandleTypeDef RtcHandle;
__NO_RETURN void app_main (void *arg) {
  (void)arg;

  Init_Full_Leds(); // inicializacion de los led y rgb
//  Buttons_Initialize();
//  ADC_Initialize();

  Init_ADC();
  ADC_Init_Single_Conversion(&adchandle, ADC1);
  initialize_RTC(); // inicializacion RTC
 
  netInitialize (); // inicializacio pagina web

  TID_Led     = osThreadNew (BlinkLed, NULL, NULL);
  TID_Display = osThreadNew (Display,  NULL, NULL);
  TID_RTC     = osThreadNew (ShowTimeThread, NULL, NULL);
  TID_Alarm   = osThreadNew (BlinkOnAlarmThread, NULL, NULL);
	 // Normalmente es la línea de interrupción RTC_Alarm_IRQn
  osThreadExit();
}
