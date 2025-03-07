 #include "pulsadores.h"


void Init_Puls(void){
  GPIO_InitTypeDef Pulse_InitStruct={0};
   __HAL_RCC_GPIOC_CLK_ENABLE();
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  Pulse_InitStruct.Pin= GPIO_PIN_13;
  Pulse_InitStruct.Mode=GPIO_MODE_IT_RISING;// sensible a flancos de subida, segun fabricante
  Pulse_InitStruct.Pull=GPIO_PULLDOWN;//modo pulldown segun fabricante y además comprobable en el datasheet
  HAL_GPIO_Init(GPIOC, &Pulse_InitStruct);
  }