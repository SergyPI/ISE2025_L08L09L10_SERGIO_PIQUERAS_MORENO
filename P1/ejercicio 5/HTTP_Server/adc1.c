#include "adc1.h"


static volatile uint8_t  AD_done;       /* AD conversion done flag            */
static volatile uint16_t AD_last;       /* Last converted value               */
static ADC_HandleTypeDef hadc;


//Funcion que configura el ADC para la lectura del POT 1:
void Init_ADC()
{
	  GPIO_InitTypeDef GPIO_InitStruct = {0};
	__HAL_RCC_ADC1_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
		/*PC0     ------> ADC1_IN10
			PC3     ------> ADC1_IN13
    */
    GPIO_InitStruct.Pin = GPIO_PIN_0;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
}

//Funcion que arranca la conversion del POT 1:
int ADC_Init_Single_Conversion(ADC_HandleTypeDef *hadc, ADC_TypeDef  *ADC_Instance)
{
  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)*/
  hadc->Instance = ADC_Instance;
  hadc->Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV2;
  hadc->Init.Resolution = ADC_RESOLUTION_12B;
  hadc->Init.ScanConvMode = DISABLE;
  hadc->Init.ContinuousConvMode = DISABLE;
  hadc->Init.DiscontinuousConvMode = DISABLE;
  hadc->Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc->Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc->Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc->Init.NbrOfConversion = 1;
  hadc->Init.DMAContinuousRequests = DISABLE;
  hadc->Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(hadc) != HAL_OK)
  {
    return -1;
  }
	return 0;
}
    uint32_t valor = 0;

//Funcion que proporciona el valor de la lectura del POT 1:
float ADC_getVoltage(ADC_HandleTypeDef *hadc, uint32_t Channel)
{
    ADC_ChannelConfTypeDef sConfig = {0};
    HAL_StatusTypeDef status;
    float voltage = 0;
    
    // Configurar el canal del ADC
    sConfig.Channel = Channel;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
    if (HAL_ADC_ConfigChannel(hadc, &sConfig) != HAL_OK) {
        return -1;  // Error en la configuración
    }

    // Iniciar la conversión
    HAL_ADC_Start(hadc);
    
    // Esperar a que termine la conversión
    do {
        status = HAL_ADC_PollForConversion(hadc, HAL_MAX_DELAY); // Espera indefinidamente
    } while(status != HAL_OK);

    // Leer el valor convertido
    valor = HAL_ADC_GetValue(hadc);

    // Convertir a voltaje
    voltage = valor ;
    
    return voltage;
}

int32_t ADC_ConversionDone (void) {
  HAL_StatusTypeDef status;

  status = HAL_ADC_PollForConversion(&hadc, 0);
  if (status == HAL_OK) {
    AD_done = 1;
    return 0;
  } else {
    AD_done = 0;
    return -1;
  }
}
int32_t ADC_StartConversion (void) {
  __HAL_ADC_CLEAR_FLAG(&hadc, ADC_FLAG_EOC);
  HAL_ADC_Start(&hadc);

  AD_done = 0;

  return 0;
}
int32_t ADC_GetValue (void) {
  HAL_StatusTypeDef status;
  int32_t val;

  if (AD_done == 0) {
    status = HAL_ADC_PollForConversion(&hadc, 0);
    if (status != HAL_OK) return -1;
  } else {
    AD_done = 0;
  }

  val = (int32_t)HAL_ADC_GetValue(&hadc);

  return val;
}
