#include "lcd.h"
#include "Arial12x12.h"
#define posicionIni_L1  0
#define posicionIni_L2  256

ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;
ARM_SPI_STATUS state;

 TIM_HandleTypeDef htim7; 
 uint16_t positionL1=0;
uint16_t positionL2=0;


GPIO_InitTypeDef GPIO_InitStruct;
unsigned char buffer[512];

void delay(uint32_t n_microsegundos)
{
htim7.Instance = TIM7; //nombre de la instancia con la que trabajamos
	
	htim7.Init.Prescaler = 83; //valor del preescaler con el que vamos a trabajar-> 83 para que 84MHz/84=1MHz
	htim7.Init.Period = n_microsegundos-1; //valor del periodo con el que vamos a trabajar: en este caso queremos llegar a 1MHz (T=1us)
												   //por lo tanto 1MHz/n_us=tiempo que quieras
	__HAL_RCC_TIM7_CLK_ENABLE();
	HAL_TIM_Base_Init(&htim7); // configura el timer y lo inicializa
	HAL_TIM_Base_Start(&htim7); //pone al timer a correr 
	/*Esperar que se active el flag del registro Match correspondiente*/
	
	
	while( TIM7->CNT < n_microsegundos-1){}
	/*paro el reset*/	
	HAL_TIM_Base_Stop(&htim7);
	/*inicializo el timer de nuevo*/
	HAL_TIM_Base_DeInit(&htim7); // configura el timer
}

void LCD_reset(void)
{
SPIdrv->Initialize(NULL);
SPIdrv->PowerControl(ARM_POWER_FULL);
SPIdrv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB | ARM_SPI_DATA_BITS(8), 20000000);
 
//Configuración de los pines: RESET | A0 | CS
  __HAL_RCC_GPIOA_CLK_ENABLE(); //habilita el reloj del puerto A
  __HAL_RCC_GPIOF_CLK_ENABLE(); //habilita el reloj del puerto F
  __HAL_RCC_GPIOD_CLK_ENABLE(); //habilita el reloj del puerto D
  
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  // Configuración RESET
 GPIO_InitStruct.Pin = GPIO_PIN_6; 
HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
  // Configuración A0
   GPIO_InitStruct.Pin =  GPIO_PIN_13 ; 
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
  // Configuración CS
   GPIO_InitStruct.Pin = GPIO_PIN_14; 
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);

// Selecciona nivel alto
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);
HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);

// Genera el pulso de reset
	delay(1);//para respetar los tiempos para ver la señal en el analizador
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_RESET);
delay(5);
HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, GPIO_PIN_SET);
//retardo de 1ms
delay(1001);
}

void LCD_wr_data(unsigned char data)
{
 // Selecciona CS = 0;
HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
 // Selecciona A0 = 1;
HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_SET);

 // Escribe un dato (data) usando la función SPIDrv->Send(…);
SPIdrv->Send(&data, sizeof(data));
 // Espera a que se libere el bus SPI;
do{
		state = SPIdrv->GetStatus();
	}while(state.busy);
	// Selecciona CS = 1;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
}

void LCD_wr_cmd(unsigned char cmd)
{
 // Selecciona CS = 0;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
 // Selecciona A0 = 0;
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13, GPIO_PIN_RESET);

 // Escribe un comando (cmd) usando la función SPIDrv->Send(…);
	SPIdrv->Send(&cmd, sizeof(cmd));
 // Esperar a que se libere el bus SPI;
	do{
		state = SPIdrv->GetStatus();
	}while(state.busy);
	// Selecciona CS = 1;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
}

 void LCD_init(void)
{
	LCD_wr_cmd(0xAE); //Display off
	LCD_wr_cmd(0xA2); //Fija el valor de la relación de la tensión de polarización del LCD a 1/9
	LCD_wr_cmd(0xA0); //El direccionamiento de la RAM de datos del display es la normal
	LCD_wr_cmd(0xC8); //El scan en las salidas COM es el normal
	LCD_wr_cmd(0x22); //Fija la relación de resistencias interna a 2
	LCD_wr_cmd(0x2F); //Power on
	LCD_wr_cmd(0x40); //Display empieza en la línea 0
	LCD_wr_cmd(0xAF); //Display ON
	LCD_wr_cmd(0x81); //Contraste
	LCD_wr_cmd(0x17); //Valor Contraste
	LCD_wr_cmd(0xA4); //Display all points normal
	LCD_wr_cmd(0xA6); //LCD Display normal
 }
 
 void LCD_update(void)
{
 int i;
 LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
 LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
 LCD_wr_cmd(0xB0); // Página 0

 for(i=0;i<128;i++){
 LCD_wr_data(buffer[i]);
 }

 LCD_wr_cmd(0x00); // 4 bits de la parte baja de la dirección a 0
 LCD_wr_cmd(0x10); // 4 bits de la parte alta de la dirección a 0
 LCD_wr_cmd(0xB1); // Página 1

 for(i=128;i<256;i++){
 LCD_wr_data(buffer[i]);
 }

 LCD_wr_cmd(0x00);
 LCD_wr_cmd(0x10);
 LCD_wr_cmd(0xB2); //Página 2
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

void symbolToLocalBuffer_L1(uint8_t symbol){

uint8_t i, value1, value2;
uint16_t offset = 0;


offset = 25*(symbol - ' ');

for(i = 0; i<12; i++){

value1 = Arial12x12[offset+i*2+1];
value2 = Arial12x12[offset+i*2+2];

buffer[i + 0 + positionL1] = value1;
buffer[i + 128 + positionL1] = value2;
}
positionL1 = positionL1 + Arial12x12[offset];

if(positionL1>127){
  
  positionL1=0;
}

}

void symbolToLocalBuffer_L2(uint8_t symbol){

uint8_t i, value1, value2;
uint16_t offset = 0;
  

offset = 25*(symbol - ' ');

for(i = 0; i<12; i++){

value1 = Arial12x12[offset+i*2+1];
value2 = Arial12x12[offset+i*2+2];

buffer[i + 256 + positionL2] = value1;
buffer[i + 384 + positionL2] = value2;
}
positionL2 = positionL2 + Arial12x12[offset];

if(positionL2>127){
  
  positionL2=0;
}
}

void symbolToLocalBuffer(uint8_t line, uint8_t symbol){
uint8_t i, value1, value2;
uint16_t offset = 0;

offset = 25*(symbol - ' ');
for(i = 0; i<12; i++){
if(line == 1){
value1 = Arial12x12[offset+i*2+1];
value2 = Arial12x12[offset+i*2+2];

buffer[i + 0 + positionL1] = value1;
buffer[i + 128 + positionL1] = value2;

}else if(line == 2){
value1 = Arial12x12[offset+i*2+1];
 value2 = Arial12x12[offset+i*2+2];

buffer[i + 256 + positionL2] = value1;
  buffer[i + 384 + positionL2] = value2;
}
}
positionL1 = positionL1 + Arial12x12[offset];
positionL2 = positionL2 + Arial12x12[offset];
}
void bufferReset(void){
borraL1();
 borraL2();
}

int wrLCD(char chain [], int linea){
  int i;
  int cadena=strlen(chain);
  if(linea==1){
  borraL1();
  positionL1=0;
  for(i=0; i<cadena;i++){
    if(positionL1<128-11)
       symbolToLocalBuffer_L1(chain[i]);
    }
  }
  if (linea==2){
    borraL2();
    positionL2=0;	
  for(i=0; i<cadena;i++){
  if(positionL2<128-11)
  symbolToLocalBuffer_L2(chain[i]);
 }
 }
  LCD_update();
  return 0;
}
void borraL1(){
  for (int i = 0; i< 256; i ++){
    LCD_wr_data(buffer[i]=0x00);
 }
  LCD_update();
}
void borraL2(){
  for (int i = 256; i< 512; i ++){
    LCD_wr_data(buffer[i]=0x00);
 }
 LCD_update();
}

void copy_to_lcd1(void){
  int i;
  LCD_wr_cmd(0x00);      // 4 bits de la parte baja de la dirección a 0
  LCD_wr_cmd(0x10);      // 4 bits de la parte alta de la dirección a 0
  LCD_wr_cmd(0xB0);      // Página 0
  for(i=0;i<128;i++){
    LCD_wr_data(buffer[i]);
  }
  LCD_wr_cmd(0x00);      // 4 bits de la parte baja de la dirección a 0
  LCD_wr_cmd(0x10);      // 4 bits de la parte alta de la dirección a 0
  LCD_wr_cmd(0xB1);      // Página 1
  for(i=128;i<256;i++){
    LCD_wr_data(buffer[i]);
  }
}
void copy_to_lcd2(void){
  int i;
  LCD_wr_cmd(0x00);       
  LCD_wr_cmd(0x10);      
  LCD_wr_cmd(0xB2);      //Página 2
  for(i=256;i<384;i++){
    LCD_wr_data(buffer[i]);
  }
  LCD_wr_cmd(0x00);       
  LCD_wr_cmd(0x10);       
  LCD_wr_cmd(0xB3);      // Pagina 3
  for(i=384;i<512;i++){
    LCD_wr_data(buffer[i]);
  }
}
void pintar_raya(int posicion){
  
  
  buffer[posicion]=0xFF;
buffer[posicion+128]=0xFF;
}
void borrar_raya(int posicion){
  buffer[posicion]=0x00;
  buffer[posicion+128]=0x00;
}
  