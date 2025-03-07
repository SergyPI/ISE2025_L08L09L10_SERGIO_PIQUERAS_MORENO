#include <stdio.h>
#include "main.h"
#include "cmsis_os2.h"                  // ::CMSIS:RTOS2
#include "rl_net.h"                     // Keil.MDK-Pro::Network:CORE
#include "leds.h"
#include "lcd.h"
#include "adc.h"
#include "rtc.h"
#include "pulsadores.h"
#include "stm32f4xx_hal_eth.h"
#include <time.h>

#define LCD 0x01
#define SINCRONIZAR 0x02
#define ALARMA_RTC 0x04



// Main stack size must be multiple of 8 Bytes
#define APP_MAIN_STK_SZ (1024U)
uint64_t app_main_stk[APP_MAIN_STK_SZ / 8];
const osThreadAttr_t app_main_attr = {
  .stack_mem  = &app_main_stk[0],
  .stack_size = sizeof(app_main_stk)
  };

/*Main function declaration*/
__NO_RETURN void app_main (void *arg);

/*Declaracion externa funciones pulsador*/
extern void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);
extern void EXTI15_10_IRQHandler(void);

/*Declaracion externa variables*/
extern bool LEDrun;                              //booleano que me permite correr en automatico si true
extern bool pulsacion;
extern char lcd_text[2][20+1];
extern uint16_t AD_in (uint32_t ch);
extern bool noRepetir;
  extern int periodoAlarma;

/*Declaración externa hilos*/
extern osThreadId_t TID_Display;
extern osThreadId_t TID_Led;
extern osThreadId_t TID_Rtc;
extern osThreadId_t TID_Sntp;
extern osThreadId_t TID_Pulsador;

/*variables hilo LED*/
bool LEDrun;
bool parpadeoSinc;
uint32_t flags;                                               //variable que almacena la señal enviada por los hilos

/*variables ADC*/
int32_t val ;
ADC_HandleTypeDef adchandle;

/*variables hilo LCD*/
char lcd_text[2][20+1];
int i;

/*variables y funciones hilo RTC y SNTP*/
RTC_HandleTypeDef rtc;
char aShowTime[50];
char aShowDate[50] ;
RTC_TimeTypeDef sTime;
RTC_DateTypeDef sDate;
netStatus status;
const NET_ADDR4 ntp_server = { NET_ADDR_IP4,  217, 79, 179, 106 };
static void time_callback (uint32_t seconds, uint32_t seconds_fraction);
time_t tiempo;                                         //variable de tipo time_t. Este tipo esta definido en la libreria time.h
struct tm info;                                        // estructura info de tipo tm definida en la libreria time.h

/* Thread IDs */
osThreadId_t TID_Display;
osThreadId_t TID_Led;
osThreadId_t TID_Rtc;
osThreadId_t TID_Sntp;
osThreadId_t TID_Pulsador;

/* Thread declarations */
 void BlinkLed (void *arg);
 void Display  (void *arg);
 void Rtc      (void *arg);
 void Sntp     (void *arg);
 void Pulsador (void *arg);

/*Funcion parpadeo Led verde alarma RTC y funcion Sincronizacion Led rojo con SNTP*/
void Led_Blink_5seg(void){
  for(uint8_t i = 0; i<25; i++){            //con delay de 200 ms y 25 iteraciones tenemos 200*25= 5000 ms= 5 segundos
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);
    osDelay(200);
  }
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0,GPIO_PIN_RESET);
  //bit_alarm=false;//borra flag. Lo comento por que como uso señales, no necesito usar estos flags globales
  }
void Parpadeo_Sinc(void){
  for(uint8_t i = 0; i<10; i++){            //con delay de 200 ms y 25 iteraciones tenemos 200*25= 5000 ms= 5 segundos
    HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_14);
    osDelay(200);
    }
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14,GPIO_PIN_RESET);
  //parpadeoSinc=false;  //borra flag. Lo comento por que como uso señales, no necesito usar estos flags globales
  }

/*Funcion callBack que devuelve los segundos totales desde 1970 y permite calcular fecha actual mediante funcion localtime*/
static void time_callback (uint32_t seconds, uint32_t seconds_fraction) {
    if (seconds == 0) {
      printf ("Server not responding or bad response.\n");
      }
    else{
      printf ("%d seconds elapsed since 1.1.1970\n", seconds);
      osThreadFlagsSet(TID_Led,SINCRONIZAR);  //sustituye el booleano parpadeoSinc y evito variables globales por señales
    // parpadeoSinc=true;//flag que activa el parpadeo una vez obtengo el tiempo del servidor
      pulsacion=false; //necesario para no entrar mas en el bucle de seteo rtc a 0 y devolver la hora del server 
      tiempo=seconds;
      info=*localtime(&tiempo);
      RTC_CalendarConfig(info.tm_sec,info.tm_min,info.tm_hour+1,info.tm_mday,info.tm_mon+1,info.tm_year-100,info.tm_wday);
      
      }
    }

/*Funcion get_time que devuelve netOk si se ha obtenido correctamente el tiempo*/
void get_time (void) {
  if (netSNTPc_GetTime (NULL, time_callback) == netOK) {
    printf ("SNTP request sent.\n");
  }
  else {
    printf ("SNTP not ready or bad parameters.\n");
  }
}

/*----------------------------------------------------------------------------
  Thread 'Pulsador': 
 *---------------------------------------------------------------------------*/
//__NO_RETURN void Pulsador(void *arg){

//  pulsacion=false;
//  while(1){
//    if(pulsacion){
//      RTC_CalendarConfig(0,0,0,1,0,0,0);
//      pulsacion=false;
//      }
//    }
//  }

/*----------------------------------------------------------------------------
  Thread 'SNTP': Recoge la hora de los servidores web
 *---------------------------------------------------------------------------*/
__NO_RETURN void Sntp(void *arg){

  //osDelay(5000);         // espera de 5 segundos tras inicializar los servicios de red antes de primera sincronizacion
  get_time();              //primera sincronizacion
  while(1){
    osDelay(30000);
    //osDelay(180000);     //resincronizacion cada 3 minutos
    get_time();
  }
}

/*----------------------------------------------------------------------------
  Thread 'RTC': Muestra la hora con una frecuencia de 1Hz= 1 segundo
 *---------------------------------------------------------------------------*/
__NO_RETURN void Rtc(void *arg){

  char LcdShowTime[30];
  char LcdShowDate[30];
  int n;
  Init_RTC();
  Set_Alarma();
  LCD_reset();
  LCD_init();
  bufferReset();
  pulsacion=false;
  noRepetir=false;
   while(1){
     RTC_CalendarShow(aShowTime,aShowDate);
     bufferReset();
     if(pulsacion){//solo entra si se ha producido pulsacion.
       if(noRepetir){//solo entra si tambien se ha producido pulsacion
       RTC_CalendarConfig(0,0,0,1,1,100,0);//seg,min,hour,day,month,year,weekday
       noRepetir=false;
     }
       RTC_CalendarShow(aShowTime,aShowDate);
       n= sprintf (LcdShowTime, "%02d:%02d:%02d", stimestructureget.Hours ,stimestructureget.Minutes,stimestructureget.Seconds);
       positionL1=0;
       for(i=0; i<n; i++){
         symbolToLocalBuffer_L1(LcdShowTime[i]);
       }
       positionL2=0;
       n=sprintf (LcdShowDate, "%02d/%02d/%02d",sdatestructureget.Date ,sdatestructureget.Month ,1900+ sdatestructureget.Year);
       for(i=0; i<n; i++){
         symbolToLocalBuffer_L2(LcdShowDate[i]);
       }
       LCD_update();
     }
     else{
       n= sprintf (LcdShowTime, "%02d:%02d:%02d", stimestructureget.Hours ,stimestructureget.Minutes,stimestructureget.Seconds);
       positionL1=0;
       for(i=0; i<n; i++){
         symbolToLocalBuffer_L1(LcdShowTime[i]);
       }
       positionL2=0;
       n=sprintf (LcdShowDate, "%02d/%02d/%02d",sdatestructureget.Date ,sdatestructureget.Month, sdatestructureget.Year);
       for(i=0; i<n; i++){
         symbolToLocalBuffer_L2(LcdShowDate[i]);
       }
       LCD_update();
     }
     osDelay (100);
  }
}

/* -----------------------------------------------------------------------------------------------------------------
  Read analog inputs.
  Potenciometro 1 manejado como entrada analogica. Su pin en appBoard puede ser DIP 19->Pot 1 ::::: DIP 20 -> Pot 2
-------------------------------------------------------------------------------------------------------------------*/
uint16_t AD_in (uint32_t ch) {
  if (ch == 0){
    val = ADC_getVoltage(&adchandle, 10); //Usar canal 13 si se usa pin PC3 (A2), si PC0(A1) usar canal 10
    }
  return ((uint16_t)val);
  }
/*-------------------------------------------------------------------------------------------------------------------------------
  Thread 'Display': LCD display handler
  
  El comando %-10s usado en sprintf me indica que mi variable buf tiene que ocupar si o si 10 caracteres, de modo que 
  si finalmente lcd_text[i] ocupa menos, se rellenará con caracteres vacios(espacios)por la izquierda hasta llegar a 10 caracteres
  En caso de que fuera -%10, los espacios se rellenarían por la izquierda.Originalmente este programa venia con codigo que ponia
  %-20 pero eso son excesivos espacios, lo que hacia que la frase no cupiese en el LCD y de hecho, colgaba la pagina web. CUIDADO!!!

 *----------------------------------------------------------------------------------------------------------------------------------*/
__NO_RETURN void Display (void *arg) {
  int cnt_alarma=0;
  int n;
  int j;
  static char buf[24];
  static char alarma[30];
  LCD_reset();
  LCD_init();
  bufferReset();
  n= sprintf (buf, "Inicio de programa");
  for(i=0; i<n; i++){
    symbolToLocalBuffer_L1(buf[i]);
    }
  n=sprintf (buf, "Programa funciona");
  for(i=0; i<n; i++){
    symbolToLocalBuffer_L2(buf[i]);
    }
  LCD_update();
  while(1){

    osThreadFlagsWait (LCD, osFlagsWaitAll, osWaitForever);
    bufferReset();
    n= sprintf (buf, "%-10s", lcd_text[0]);
    positionL1=0;
    for(i=0; i<n; i++){
      symbolToLocalBuffer_L1(buf[i]);
      }
    positionL2=0;
    n=sprintf (buf, "%-10s", lcd_text[1]);
    for(i=0; i<n; i++){
      symbolToLocalBuffer_L2(buf[i]);
      }
    LCD_update();
      
    }
  }


/*----------------------------------------------------------------------------
  Thread 'BlinkLed': Blink the LEDs on an eval board
 *---------------------------------------------------------------------------*/
 __NO_RETURN void BlinkLed (void *arg)  {
   
  int cnt_led = 0;
  int cnt_rgb=0;
  int cnt=0;
  uint8_t led_val[3] = {0x02, 0x04, 0x08,};
  uint8_t rgb_val[3]={0x10,0x20,0x40};
  LEDrun = true;
  parpadeoSinc=false;
  flags=0;
  while(1) {
    flags=osThreadFlagsWait(SINCRONIZAR|ALARMA_RTC,osFlagsWaitAny,osWaitForever);
    if(flags==SINCRONIZAR){
      Parpadeo_Sinc();
    }
    if(flags==ALARMA_RTC){
      Led_Blink_5seg();//funcion parpadeo led verde durante 5s que se activa una vez indica el flag, puesto a 1 en la alarma
    }
    
/*-----FORMA 1 DE IMPLEMENTAR BARRIDO LEDS-----------------------------*/

//    if (LEDrun == true) {
//         salida_leds(led_val[cnt_led],rgb_val[cnt_rgb]);
//      if (++cnt_led >= sizeof(led_val)){
//        cnt_led = 0;}
//       if(++cnt_rgb>= sizeof(rgb_val)) {
//        cnt_rgb=0;}
//      }
//    osDelay(1000);
//    
/*-----FIN DE FORMA 1 DE INCREMENTAR BARRIDO LEDS----------------------*/

/*-----FORMA 2 DE IMPLEMENTAR BARRIDO LEDS--------------------*/
    
//    if (LEDrun == true) {//es decir, si estoy en modo automatico
//      if(cnt==0){
//  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0,GPIO_PIN_SET);
//  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7|GPIO_PIN_14,GPIO_PIN_RESET);
//  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11,GPIO_PIN_RESET);
//  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13,GPIO_PIN_SET);
//    }
//     else if(cnt==1){
//  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_14,GPIO_PIN_RESET);
//  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7,GPIO_PIN_SET);
//  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11|GPIO_PIN_13,GPIO_PIN_SET);
//  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12,GPIO_PIN_RESET);
//    }
//    else if(cnt==2){
//  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0|GPIO_PIN_7,GPIO_PIN_RESET);
//  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14,GPIO_PIN_SET);
//  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11|GPIO_PIN_12,GPIO_PIN_SET);
//  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13,GPIO_PIN_RESET);
//      }
//    }
//    ++cnt;
//    if(cnt>2){cnt=0;}
//    osDelay (1000);
//

/*-------FIN FORMA 2 IMPLEMENTAR BARRIDO LEDS-------------------------------*/
/*-------IMPLEMENTACION ALARMA RTC Y SINCRONIZACION DE 3 MIN------------*/
//    if(bit_alarm){  //flag puesto a 1 en alarma Rtc 
//      Led_Blink_5seg();//funcion parpadeo led verde durante 5s que se activa una vez indica el flag, puesto a 1 en la alarma
//      }
//    if(parpadeoSinc){ //flag que indica que es momento de señalizar sincronizacion mediante un parpadeo
//      Parpadeo_Sinc();
//    }
/*---FIN IMPLEMENTACION ALARMA RTC Y SINCRONIZACION DE 3 MIN------------*/
  }
}
 

/*----------------------------------------------------------------------------
  Main Thread 'main': Run Network
 *---------------------------------------------------------------------------*/
  
__NO_RETURN void app_main (void *arg) {
  
  (void)arg;
  netInitialize ();
  Init_ADC();
  ADC_Init_Single_Conversion(&adchandle,ADC1);
  Init_All_Leds();
  Init_Puls();
  
  TID_Led     = osThreadNew (BlinkLed, NULL, NULL);
//  TID_Display = osThreadNew (Display,  NULL, NULL);
  TID_Rtc     = osThreadNew (Rtc,  NULL, NULL);
  TID_Sntp    =osThreadNew (Sntp,  NULL, NULL);
  
  osThreadExit();
  
  }
