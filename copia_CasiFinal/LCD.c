#include "stm32f4xx_hal.h"
#include "main.h"
#include "lcd.h"
#include "RTE_Device.h"
#include "Driver_SPI.h"
#include "Arial12x12.h"
 
/*----------------------------------------------------------------------------
 *      Thread_LCD:
 *			Thread leyendo la cola de mensajes con la informacion que debe
 *			representarse en el LCD y la linea en que debe representarse
 *---------------------------------------------------------------------------*/
 
#define MSGQUEUE_OBJECTS 8                     // number of Message Queue Objects
 
MSGQUEUE_LCD_t lcd;

osMessageQueueId_t mid_LCDQueue;                // message queue id
 
osThreadId_t tid_LCD;                        // thread id

static GPIO_InitTypeDef GPIO_InitStruct;

extern ARM_DRIVER_SPI Driver_SPI1;
ARM_DRIVER_SPI* SPIdrv = &Driver_SPI1;
ARM_SPI_STATUS stat;

uint16_t posicionL1 = 0, posicionL2 = 0;
bool invertirCaracter = false;

unsigned char buffer[512];
/* Private function prototypes -----------------------------------------------*/


void Thread_LCD (void *argument);                   // thread function
 
int Init_LCD (void) {
 
	mid_LCDQueue = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_LCD_t), NULL);
  if (mid_LCDQueue == NULL) {
    return(-1);
  }
  tid_LCD = osThreadNew(Thread_LCD, NULL, NULL);
  if (tid_LCD == NULL) {
    return(-1);
  } 
  return(0);
}
 
void Thread_LCD (void *argument) {
	osStatus_t status;
	LCD_reset();
	LCD_init();
	
  while (1) {
		status = osMessageQueueGet(mid_LCDQueue, &lcd, 0U, osWaitForever);   // wait for message
    if (status == osOK) {
			LCD_write(lcd.line,lcd.text);
    }
    osThreadYield();                            // suspend thread
  }
}

void SPI_callback(uint32_t event){
	uint32_t mask;
  mask = ARM_SPI_EVENT_TRANSFER_COMPLETE  |
         ARM_SPI_EVENT_DATA_LOST |
         ARM_SPI_EVENT_MODE_FAULT;
  if (event & mask) {
    osThreadFlagsSet(tid_LCD, 0x01);
  }
}


void LCD_reset(void){
	// Inicializaci?n y configuraci?n del driver SPI para gestionar el LCD
	SPIdrv->Initialize(SPI_callback);
	SPIdrv->PowerControl(ARM_POWER_FULL);
	SPIdrv->Control(ARM_SPI_MODE_MASTER | ARM_SPI_CPOL1_CPHA1 | ARM_SPI_MSB_LSB | ARM_SPI_DATA_BITS(8), 20000000);

	// Configurar los pines de IO que sean necesarios y programar su valor por defecto
	
  __HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOF_CLK_ENABLE();

  // 2 - Configure IO in outout push-pull mode to drive external LEDs

  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_VERY_HIGH;

  GPIO_InitStruct.Pin = GPIO_PIN_6; // SEÑAL DE RESET
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6,GPIO_PIN_SET);
	
  GPIO_InitStruct.Pin = GPIO_PIN_13; // SEÑAL DE AO
  HAL_GPIO_Init(GPIOF, &GPIO_InitStruct);
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13,GPIO_PIN_SET);
	
	
	GPIO_InitStruct.Pin = GPIO_PIN_14; // SEÑAL DE CS
  HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);	
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14,GPIO_PIN_SET);
	
	// Generar la señal de reset
	resetLCD();
}

void resetLCD(){
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6,GPIO_PIN_RESET);
	osDelay(1);
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6,GPIO_PIN_SET);
	osDelay(1);
}

void LCD_wr_data(unsigned char data){
	// Seleccionar CS = 0;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14,GPIO_PIN_RESET);
	// Seleccionar A0 = 1;
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13,GPIO_PIN_SET);
	// Escribir un dato (data) usando la funci?n SPIDrv->Send(...);
	SPIdrv->Send(&data, sizeof(data));
	// Esperar a que se libere el bus SPI;
	osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);
	osThreadFlagsClear(0x01);
	// Seleccionar CS = 1;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14,GPIO_PIN_SET);
}

void LCD_wr_cmd(unsigned char cmd){
	// Seleccionar CS = 0;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14,GPIO_PIN_RESET);
	// Seleccionar A0 = 0;
	HAL_GPIO_WritePin(GPIOF, GPIO_PIN_13,GPIO_PIN_RESET);
	// Escribir un comando (cmd) usando la funci?n SPIDrv->Send(...);
	SPIdrv->Send(&cmd, sizeof(cmd));
	// Esperar a que se libere el bus SPI;
	osThreadFlagsWait(0x01, osFlagsWaitAny, osWaitForever);
	osThreadFlagsClear(0x01);
	// Seleccionar CS = 1;
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14,GPIO_PIN_SET);
}

void LCD_config(void){
	LCD_wr_cmd(0xAE); // Display off
	LCD_wr_cmd(0xA2); // Fija el valor de la relaci?n de tensi?n de polarizaci?n del LCD a 1/9
	LCD_wr_cmd(0xA0); // El direccionamiento de la RAM de datos del display es la normal
	LCD_wr_cmd(0xC8); // El scan en las salidas COM es el normal
	LCD_wr_cmd(0x22); // Fija la relaci?n de resistencias interna a 2
	LCD_wr_cmd(0x2F); // Power on
	LCD_wr_cmd(0x40); // Display empieza en la l?nea 0
	LCD_wr_cmd(0xAF); // Display ON
	LCD_wr_cmd(0x81); // Contraste
	LCD_wr_cmd(0x17); // Valor Contraste
	LCD_wr_cmd(0xA4); // Display all points normal
	LCD_wr_cmd(0xA6); // LCD Display normal
	memset(buffer, 0, 512);
}

void LCD_InvertirPantalla(void){
	invertirCaracter = !invertirCaracter;
}

void LCD_update(void)
{
	int i;
	char c;
	LCD_wr_cmd(0x00); 		// 4 bits de la parte baja de la direcci�n a 0
	LCD_wr_cmd(0x10); 		// 4 bits de la parte alta de la direcci�n a 0
	LCD_wr_cmd(0xB0);		 	// Pagina 0
	for(i=0;i<128;i++){
		if(!invertirCaracter){
			c = buffer[i];
		}else{
			c = ~buffer[i];
		}
		LCD_wr_data(c);
	}
	LCD_wr_cmd(0x00); 		// 4 bits de la parte baja de la direcci�n a 0
	LCD_wr_cmd(0x10); 		// 4 bits de la parte alta de la direcci�n a 0
	LCD_wr_cmd(0xB1); 		// Pagina 1
	for(i=128;i<256;i++){
		if(!invertirCaracter){
			c = buffer[i];
		}else{
			c = ~buffer[i];
		}
		LCD_wr_data(c);
	}
	LCD_wr_cmd(0x00);
	LCD_wr_cmd(0x10);
	LCD_wr_cmd(0xB2); 			//Pagina 2
	for(i=256;i<384;i++){
		if(!invertirCaracter){
			c = buffer[i];
		}else{
			c = ~buffer[i];
		}
		LCD_wr_data(c);
	}
	LCD_wr_cmd(0x00);
	LCD_wr_cmd(0x10);
	LCD_wr_cmd(0xB3); 			// Pagina 3
	for(i=384;i<512;i++){
		if(!invertirCaracter){
			c = buffer[i];
		}else{
			c = ~buffer[i];
		}
		LCD_wr_data(c);
	}
}

uint8_t LCD_symbolToLocalBuffer_L1(uint8_t symbol){
	uint8_t i, value1, value2;
	uint16_t offset = 0;
	
	offset = 25 * (symbol - ' ');
	for( i = 0; i< 12; i++){
		value1 = Arial12x12[offset+i*2+1];
		value2 = Arial12x12[offset+i*2+2];
		
		buffer[i + posicionL1] = value1;
		buffer[i + 128 + posicionL1] = value2;
	}
	posicionL1 = posicionL1 + Arial12x12[offset];
	if (posicionL1 > 127)
		return 0;
	return 1;
}

uint8_t LCD_symbolToLocalBuffer_L2(uint8_t symbol){
	uint8_t i, value1, value2;
	uint16_t offset = 0;
	
	offset = 25 * (symbol - ' ');
	
	for( i = 0; i< 12; i++){
		value1 = Arial12x12[offset+i*2+1];
		value2 = Arial12x12[offset+i*2+2];
		
		buffer[i + 256 + posicionL2] = value1;
		buffer[i + 384 + posicionL2] = value2;
	}
	posicionL2 = posicionL2 + Arial12x12[offset];
	if (posicionL2 > 127)
		return 0;
	return 1;
}

uint8_t LCD_symbolToLocalBuffer(uint8_t line, uint8_t symbol){
	uint8_t lleno = 0;
	switch(line){
		case 2:
			lleno = LCD_symbolToLocalBuffer_L2(symbol);
			break;
		default:
			lleno = LCD_symbolToLocalBuffer_L1(symbol);
	}
	return lleno;
}

void clear_L1(void)
{
	int i;
	for(i=0; i<sizeof(buffer)/2; i++){
		buffer[i] = 0x00;
  }
	posicionL1 = 0;
}

void clear_L2(void)
{
	int i;
	for(i=256; i<sizeof(buffer); i++){
		buffer[i] = 0x00;
  }
	posicionL2 = 0;
}

void LCD_clear(void){
	for(int i=0; i<sizeof(buffer); i++){
		buffer[i] = 0x00;
	}
	posicionL1 =0;
	posicionL2 =0;
}

void LCD_init(void){ //secuencia de arranque de la pantalla LCD
	LCD_reset();
	LCD_config();	
	LCD_clear();//borro caracteres residuos
}


void LCD_write(uint8_t line, char text[])
{
	uint8_t i, tam;
	tam = strlen(text);
	
	switch(line){
		case 1:
			clear_L1();
			break;
		case 2:
			clear_L2();
			break;
	}
	for (i = 0; i < tam; i++)
	{
		if(!LCD_symbolToLocalBuffer(line,text[i]))
			i = tam;
	}
	posicionL1 = 0;
	posicionL2 = 0;
	LCD_update();
}

void LCD_off(void){
	LCD_wr_cmd(0xAE);		//Mantenemos apagado el LCD 
}

void LCD_on(void){
	LCD_wr_cmd(0xAF);		//se enciende el LCD(LCD on)	
}


//Código para testear el módulo

//osThreadId_t tid_Pantalla;                        // thread id
// 
//void Thread_Pantalla (void *argument);                   // thread function
// 
//int Init_Test_LCD (void) {
//	
//  tid_Pantalla = osThreadNew(Thread_Pantalla, NULL, NULL);
//  if (tid_Pantalla == NULL) {
//    return(-1);
//  }	
//	
//	Init_LCD();
//	
//  return(0);
//}
// 
//void Thread_Pantalla (void *argument) {
//	osStatus_t status;
//	int contador = 0;

//	MSGQUEUE_LCD_t lcd;
//	
//  while (1) {

//						osDelay(500);
//						sprintf(lcd.text, "--prueba linea 1--  %d", contador);
//						lcd.line = 1;
//						osMessageQueuePut(mid_LCDQueue, &lcd, 0U, 0U);
//						
//		
//						sprintf(lcd.text, "--prueba linea 2--  %d", contador);
//						lcd.line = 2;
//						osMessageQueuePut(mid_LCDQueue, &lcd, 0U, 0U);
//		
//						contador++;
//		

//						
//		    osThreadYield(); 
//		}

//  }

