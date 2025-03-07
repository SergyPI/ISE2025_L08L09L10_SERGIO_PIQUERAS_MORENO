#include "lcd.h"
#include "Arial12x12.h"


extern osStatus_t status_lcd;
static GPIO_InitTypeDef GPIO_InitStruct;

  extern ARM_DRIVER_SPI Driver_SPI1;
  ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;
  ARM_SPI_STATUS SPIstatus;
	
 TIM_HandleTypeDef htim7;
		
  unsigned char buffer[512];
   
  
  uint16_t positionL1;
  uint16_t positionL2;
  uint16_t positionL3;
  uint32_t valor1 = 1234; 
  char linea1[19];
  
  int i;
/*----------------------------------------------------------------------------
  * COLA DEL SERVER -> Se crea y se rellena en el módulo principal (en este caso, en el test)            
*---------------------------------------------------------------------------*/
 typedef struct {                                // object data type
	char texto1[30];
  char texto2[30];
} MSGQUEUE_OBJ_t_ServerLcd;  
  
MSGQUEUE_OBJ_t_ServerLcd msg_TimeDataLcd_recibido;
osMessageQueueId_t mid_MsgQueue_ServerLcd;
  
void LCD_wr_data(unsigned char data){
	
	// Seleccionar CS = 0;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);

	// Seleccionar A0 = 1;
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);


 // Escribir un dato (data) usando la funci?n SPIDrv->Send(?);
	//SPIdrv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE);
	
	SPIdrv->Send(&data, sizeof(data));
	
 // Esperar a que se libere el bus SPI;
	do{
		SPIstatus = SPIdrv->GetStatus();
	}while(SPIstatus.busy);
	
	//SPIdrv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
	
 // Seleccionar CS = 1;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);

}

void LCD_wr_cmd(unsigned char cmd){
	
	// Seleccionar CS = 0;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);

	// Seleccionar A0 = 0;
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);


	// Escribir un comando (cmd) usando la funci?n SPIDrv->Send(?);
	//SPIdrv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_ACTIVE);
	
	SPIdrv->Send(&cmd, sizeof(cmd));
	// Esperar a que se libere el bus SPI;
	do{
		SPIstatus = SPIdrv->GetStatus();
	}while(SPIstatus.busy);
	
	//SPIdrv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);

	// Seleccionar CS = 1;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
}
	
 void LCD_reset(void){
	/*Clock enable*/
	
	SPIdrv->Initialize(NULL);
	
	SPIdrv->PowerControl(ARM_POWER_FULL);
	
	/*--Configure the SPI to Master, 8-bit mode @20000 kBits/sec--*/
	SPIdrv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB | ARM_SPI_DATA_BITS(8), 20000000);
	/*--SS line = INACTIVE = HIGH--*/
	//SPIdrv->Control(ARM_SPI_CONTROL_SS, ARM_SPI_SS_INACTIVE);
	
	LCD_GPIO_init();
	
	  /*  VALOR INICIAL DE LOS PINES GPIO */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);  // RESET
  HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET); // CS
  HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET); // A0
	

	/*--Proceso de reset explicado con tiempos en OneNote*/
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
	
	//delay(5); //Delay Trw (min=1us) luego por ej ponemos 5us
	delay(2);
	
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
	
	//delay(1001); //Delay Tr
	delay(1001);
	
}

void LCD_init(void) {
	
	LCD_wr_cmd(0xAE);
	LCD_wr_cmd(0xA2);
	LCD_wr_cmd(0xA0);
	LCD_wr_cmd(0xC8);
	LCD_wr_cmd(0x22);
	LCD_wr_cmd(0x2F);
	LCD_wr_cmd(0x40);
	LCD_wr_cmd(0xAF);
	LCD_wr_cmd(0x81);
	LCD_wr_cmd(0x17);
	LCD_wr_cmd(0xA4);
	LCD_wr_cmd(0xA6);
}

void LCD_Clean(void) {
	memset(buffer, 0 , 512u);	//Necesita #include "stdio.h"
	positionL1 = 0;
	positionL2 = 0;
	LCD_update();
}

void LCD_update(void) {
	int i;
	 LCD_wr_cmd(0x00); // 4 bits de la parte baja de la direcci?n a 0
	 LCD_wr_cmd(0x10); // 4 bits de la parte alta de la direcci?n a 0
	 LCD_wr_cmd(0xB0); // Pagina 0

	 for(i=0;i<128;i++){
		LCD_wr_data(buffer[i]);
	 }

	 LCD_wr_cmd(0x00); // 4 bits de la parte baja de la direcci?n a 0
	 LCD_wr_cmd(0x10); // 4 bits de la parte alta de la direcci?n a 0
	 LCD_wr_cmd(0xB1); // Pagina 1

	 for(i=128;i<256;i++){
		LCD_wr_data(buffer[i]);
	 }

	 LCD_wr_cmd(0x00);
	 LCD_wr_cmd(0x10);
	 LCD_wr_cmd(0xB2); //Pagina 2
	 
	 for(i=256;i<384;i++){
		LCD_wr_data(buffer[i]);
	 }

	 LCD_wr_cmd(0x00);
	 LCD_wr_cmd(0x10);
	 LCD_wr_cmd(0xB3); // Pagina 3


	 for(i=384;i<512;i++){
		LCD_wr_data(buffer[i]);
	 }
}

/*----------------------------------------------------------------------------
 *      Display hardware GPIO resources
 *---------------------------------------------------------------------------*/

void LCD_GPIO_init(void){
	
	GPIO_InitTypeDef GPIO_InitStruct;
	
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	
	//RST PA6
	
	GPIO_InitStruct.Pin = GPIO_PIN_6;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  
	//A0 PF13
	
	GPIO_InitStruct.Pin = GPIO_PIN_13;
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
  
	//CS PD14
	
	GPIO_InitStruct.Pin = GPIO_PIN_14;
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

/*----------------------------------------------------------------------------
 *      Delay function
 *				-Se usa para esperar el tiempo que necesita el display en su reset
 *
 *			@param micro segundos 
 *---------------------------------------------------------------------------*/
void delay(uint32_t n_us) {
	// Configurar y arrancar el timer para generar un evento
	// pasados n_microsegundos

	__HAL_RCC_TIM7_CLK_ENABLE();
	
	htim7.Instance = TIM7;
	htim7.Init.Prescaler = 9;
	htim7.Init.Period = n_us;
	
	HAL_TIM_Base_Init(&htim7);
	HAL_TIM_Base_Start(&htim7);
	
	
 // Esperar a que se active el flag del registro de Match
 // correspondiente
 // Borrar el flag,
 // Parar el Timer y ponerlo a 0 para la siguiente llamada a
 // la funci?n
	while((TIM7->CNT)<(n_us-1)){}
	HAL_TIM_Base_Stop(&htim7);
	HAL_TIM_Base_DeInit(&htim7);
}

/*----------------------------------------------------------------------------
 *      Varias funciones para pintar display
 *				-LCD_symbolToLocateBuffer_L1 y L2 usa el archivo arial12x12 para buscar un simbolo
 *				-string hace printf sobre el buffer de cada caracter de la cadena
 *
 *---------------------------------------------------------------------------*/
void write_lcd(char cadena[], int linea, bool reset, int desp){
  	
	if(reset) LCD_Clean();
	LCD_stringToLocateBuffer(cadena,linea,desp);
	
  LCD_update();
}

void LCD_symbolToLocalBuffer_L1(uint8_t symbol) {
	uint8_t i, value1, value2;
	uint16_t offset=0;
	
	offset=25*(symbol - ' ');
	
	for(i=0; i<12; i++){
		value1 = Arial12x12[offset+i*2+1];
		value2 = Arial12x12[offset+i*2+2];
		
		buffer[i  + positionL1] = value1;
		buffer[i  + 128 +positionL1] = value2;
	}
	positionL1 += Arial12x12[offset];
}


void LCD_symbolToLocalBuffer_L2(uint8_t symbol) {
	uint8_t i, value1, value2;
	uint16_t offset=0;
	
	offset=25*(symbol - ' ');
	
	for(i=0; i<12; i++){
		value1 = Arial12x12[offset+i*2+1];
		value2 = Arial12x12[offset+i*2+2];
		
		buffer[i + 256  + positionL2] = value1;
		buffer[i+384 +positionL2] = value2;
	}
	
	if(positionL2 < 128){
		positionL2 += Arial12x12[offset];	
	}else{
		positionL2 = 0;
	}
}


void LCD_stringToLocateBuffer(const char* string, int page, int desp) {
	uint8_t i;
	uint8_t offsetMid;
	uint8_t x;
	positionL3 = 0;
	
	offsetMid = (128-positionL3)/2;
	
	switch(page){
		case 1: default:
			positionL1 = 0;

			for(i=0; string[i] != '\0'; i++){
				LCD_symbolToLocalBuffer_L1(string[i]);
		
			}
		break;
		case 2: 
			positionL2 = 0;

			for(i=0; string[i] != '\0'; i++){
				
				LCD_symbolToLocalBuffer_L2(string[i]);
				if(string[i] == ' ')x=positionL2;
			}
			if(desp != 0) LCD_DrawUnderline(x+offsetMid+desp,26,5);

		break;
	}
		
}
void LCD_DrawUnderline(uint8_t x_start, uint8_t y, uint8_t length) {
	uint8_t i;
	uint16_t byteIndex;
	uint8_t bitMask;
    if (y >= 32 || x_start >= 128) return; // Validar límites del display

    byteIndex = (y / 8) * 128 + x_start; // Índice en el buffer
    bitMask = 1 << (y % 8);                         // Máscara para la línea

    for (i = 0; i < length; i++) {
        if (x_start + i < 128) {
            buffer[byteIndex + i] |= bitMask; // Encender los píxeles de la línea
        }
    }
}
///*----------------------------------------------------------------------------
//  *                         Thread : HILO DEL LCD                    
//*---------------------------------------------------------------------------*/
 
	osThreadId_t tid_Thlcd;                        // thread id
	 
	void Thlcd (void *argument);                   // thread function
	 
	int Init_Thlcd (void) {
	 
		tid_Thlcd = osThreadNew(Thlcd, NULL, NULL);
		if (tid_Thlcd == NULL) {
			return(-1);
		}

		LCD_Clean();

    
		return(0);
	}
 


	void Thlcd (void *argument) {
		LCD_GPIO_init();
    int32_t val1 = 0;

		
		while (1) {
			osMessageQueueGet(mid_MsgQueue_ServerLcd, &msg_TimeDataLcd_recibido, NULL, osWaitForever);
      LCD_reset();
      LCD_init();
      LCD_Clean();
      osDelay(100);
    }
  }
