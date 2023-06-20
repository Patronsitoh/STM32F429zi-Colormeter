#ifndef __LCD_H
#define __LCD_H

#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "stdio.h" 
#include "stdlib.h"
#include "string.h"

typedef struct {                                // object data type
  char text[24];
  uint8_t line;
} MSGQUEUE_LCD_t;

extern MSGQUEUE_LCD_t lcd;

extern osMessageQueueId_t mid_LCDQueue;

int Init_LCD (void);
int Init_Test_LCD (void);

void SPI_callback(uint32_t event);

void LCD_reset(void);
void resetLCD(void);
void LCD_config(void);
void LCD_InvertirPantalla(void);
void LCD_update(void);
void LCD_init(void);

void LCD_write(uint8_t line, char text[]);
void clear(void);

void LCD_off(void);
void LCD_on(void);
void LCD_clear(void);
void LCD_init(void);

uint8_t LCD_symbolToLocalBuffer_L1(uint8_t symbol);
uint8_t LCD_symbolToLocalBuffer_L2(uint8_t symbol);
uint8_t LCD_symbolToLocalBuffer(uint8_t line, uint8_t symbol);


#endif /* __LCD_H */
