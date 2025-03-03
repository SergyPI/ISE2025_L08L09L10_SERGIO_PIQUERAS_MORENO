#ifndef LCD_H
#define LCD_H

#include "DRIVER_SPI.h"
#include <string.h>
#include <stdio.h>
#include "main.h"
#include "RTE_Device.h"

#define posicionIni_L1  0 //inicio de la pagina 0
#define posicionIni_L2  256 //inicio de la pagina 2

void LCD_reset(void);
void delay(uint32_t n_microsegundos);
void LCD_GPIO_init (void);
void LCD_wr_data(unsigned char data);
void LCD_wr_cmd(unsigned char cmd);
void LCD_init (void);
void LCD_update(void);
extern void LCD_symbolToLocalBuffer_L1(uint8_t symbol);
extern void LCD_symbolToLocalBuffer_L2(uint8_t symbol);
void LCD_symbolToLocalBuffer(uint8_t line, uint8_t symbol);
void LCD_stringToLocateBuffer(const char* string, int page, int desp);
void LCD_ProcessOff(const char* string);
void LCD_DrawUnderline(uint8_t x_start, uint8_t y, uint8_t length);
void L1(void);
void LCD_Clean(void);
void escribir_lineasWeb(void);
void texto_SBM(void);
void escribir_hora(void);
void Thlcd (void *argument);
int Init_Thlcd (void);
//int Init_MsgQueue (void); 

void escribir_temperatura(void);
void pagina3(void);
void cuadrado(void);
#endif // LEDS_H
