#include "leds.h"

   void Init_Leds(void){
   GPIO_InitTypeDef GPIO_InitStruct={0};
  __HAL_RCC_GPIOB_CLK_ENABLE();//habilita el reloj del pueto B para poder funcionar los distintos GPIOs localizados en el mismo
  GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP ; //modo push-pull indicado por el fabricante
  GPIO_InitStruct.Pull=GPIO_PULLUP; // pullpup, indicado por fabricante
  GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_VERY_HIGH; // indicado por fabricante
  GPIO_InitStruct.Pin=GPIO_PIN_0 |GPIO_PIN_7|GPIO_PIN_14; // indica que pines vamos a utilizar
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct); // inicializa la estructura que prepara los leds
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_11,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,GPIO_PIN_RESET);
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_RESET);
 }
    void Init_Leds_RGB(void)
 {
  GPIO_InitTypeDef GPIO_InitStruct={0};
__HAL_RCC_GPIOD_CLK_ENABLE();                                  //habilita el reloj del pueto D
  GPIO_InitStruct.Mode=GPIO_MODE_OUTPUT_PP ;                   //modo push-pull indicado por el fabricante
  GPIO_InitStruct.Pull=GPIO_NOPULL;                        // no es necesario pullup o pulldown pues push-pull ofrece niveles logicos 1 y 0
  GPIO_InitStruct.Speed=GPIO_SPEED_FREQ_VERY_HIGH;             // indicado por fabricante
  GPIO_InitStruct.Pin=GPIO_PIN_11|GPIO_PIN_12|GPIO_PIN_13;     // indica que pines vamos a utilizar
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);                      //Inicializa el GPIO con los parámtros indicados
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_11,GPIO_PIN_SET);           //led blue apagado tras reset(recordar que es ánodo común,SET para apagar)
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,GPIO_PIN_SET);           //led green apagado tras reset(recordar que es ánodo común,SET para apagar)
  HAL_GPIO_WritePin(GPIOD,GPIO_PIN_13,GPIO_PIN_SET);           //led red apagado tras reset(recordar que es ánodo común,SET para apagar)
 }

void salida_leds(uint8_t leds,uint8_t rgb){
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0,leds&0x02);//led1
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_7,leds&0x04);//led2
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_14,leds&0x08);//led3
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_11, (rgb & 0x10) ? GPIO_PIN_RESET : GPIO_PIN_SET); // Azul 
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, (rgb & 0x20) ? GPIO_PIN_RESET : GPIO_PIN_SET); // Verde
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, (rgb & 0x40) ? GPIO_PIN_RESET : GPIO_PIN_SET); // Rojo
}
 void Init_Full_Leds(void){
   Init_Leds_RGB();
   Init_Leds();
 }

