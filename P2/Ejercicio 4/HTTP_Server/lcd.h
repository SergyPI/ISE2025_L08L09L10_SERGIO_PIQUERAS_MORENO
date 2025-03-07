#ifndef _LCD_H_
#define _LCD_H_

#include "DRIVER_SPI.h"
#include <string.h>
#include <stdio.h>
#include "main.h"
#include "RTE_Device.h"
#define posicionIni_L1  0
#define posicionIni_L2  256


extern ARM_DRIVER_SPI* SPIdrv;
extern ARM_DRIVER_SPI Driver_SPI1;
extern ARM_SPI_STATUS state;
extern TIM_HandleTypeDef htim7; 
extern uint16_t positionL1 ;
extern uint16_t positionL2 ;
extern GPIO_InitTypeDef GPIO_InitStruct;
extern unsigned char buffer[512];
extern void delay(uint32_t n_microsegundos);
extern void LCD_reset(void);
extern void LCD_wr_data(unsigned char data);
extern void LCD_wr_cmd(unsigned char data);
extern void LCD_init(void);
extern void LCD_update(void);
extern void delay(uint32_t n_microsegundos);
extern void symbolToLocalBuffer(uint8_t line, uint8_t symbol);
extern void symbolToLocalBuffer_L1(uint8_t symbol);
extern void symbolToLocalBuffer_L2(uint8_t symbol);
extern void bufferReset(void);
extern void borraL1(void);
extern void borraL2(void);
extern int wrLCD(char chain [], int linea);
extern void copy_to_lcd1(void);
extern void copy_to_lcd2(void);
extern void pintar_raya(int posicion);
extern void borrar_raya(int posicion);

#endif
