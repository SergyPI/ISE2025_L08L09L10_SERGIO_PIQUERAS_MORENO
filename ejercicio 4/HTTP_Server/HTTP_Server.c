/*------------------------------------------------------------------------------
 * MDK Middleware - Component ::Network
 * Copyright (c) 2004-2019 Arm Limited (or its affiliates). All rights reserved.
 *------------------------------------------------------------------------------
 * Name:    HTTP_Server.c
 * Purpose: HTTP Server example
 *----------------------------------------------------------------------------*/

#include <stdio.h>

#include "main.h"

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
#define LCD 0x01
// Main stack size must be multiple of 8 Bytes
#define APP_MAIN_STK_SZ (1024U)
uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
};

//extern GLCD_FONT GLCD_Font_6x8;
//extern GLCD_FONT GLCD_Font_16x24;

extern uint16_t AD_in          (uint32_t ch);
extern uint8_t  get_button     (void);
extern void     netDHCP_Notify (uint32_t if_num, uint8_t option, const uint8_t *val, uint32_t len);

extern bool LEDrun;
extern char lcd_text[2][20+1];

//extern osThreadId_t TID_Display;
extern osThreadId_t TID_Led;

bool LEDrun;
char lcd_text[2][20+1] = { "LCD line 1",
                          "LCD line 2" };

/* Thread IDs */
osThreadId_t TID_Display;
osThreadId_t TID_Led;

///* Thread declarations */
static void BlinkLed (void *arg);
static void Display  (void *arg);

__NO_RETURN void app_main (void *arg);

/* Read analog inputs */
uint16_t AD_in (uint32_t ch) {
  int32_t val = 0;

  if (ch == 0) {
//    ADC_StartConversion();
//    while (ADC_ConversionDone () < 0);
//    val = ADC_GetValue();
  }
  return ((uint16_t)val);
}

/* Read digital inputs */
uint8_t get_button (void) {
//  return ((uint8_t)Buttons_GetState ());
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
     n= sprintf (buf, "Inicio de programa");

      for(i=0; i<n; i++){
        
    LCD_symbolToLocalBuffer_L1(buf[i]);
  }

    n=sprintf (buf, "Bienvenido Sergio");
          for(i=0; i<n; i++){
    LCD_symbolToLocalBuffer_L2(buf[i]);
  }
   LCD_update();
  while(1){
    osThreadFlagsWait (LCD, osFlagsWaitAll, osWaitForever); //espera que le llegue la señal LCD
    LCD_Clean();
     n= sprintf (buf, "%-10s", lcd_text[0]);
     int positionL1=0;
      for(i=0; i<n; i++){
        
    LCD_symbolToLocalBuffer_L1(buf[i]);
  }
    int positionL2=0;
    n=sprintf (buf, "%-10s", lcd_text[1]);
          for(i=0; i<n; i++){
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

  LEDrun = true;
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

/*----------------------------------------------------------------------------
  Main Thread 'main': Run Network
 *---------------------------------------------------------------------------*/
__NO_RETURN void app_main (void *arg) {
  (void)arg;

   Init_Full_Leds(); // inicializacion de los led y rgb
//  Buttons_Initialize();
//  ADC_Initialize();

  netInitialize (); //inicializacio pagina web

  TID_Led     = osThreadNew (BlinkLed, NULL, NULL);
  TID_Display = osThreadNew (Display,  NULL, NULL);

  osThreadExit();
}
