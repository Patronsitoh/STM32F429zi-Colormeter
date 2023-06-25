#include "Driver_I2C.h"  
#include "cmsis_os2.h" 
#ifndef _COLOR_H
#define _COLOR_H

#define TCS34725_ADDR 0x29
#define TCS34725_REG_ENABLE 0x00
#define TCS34725_REG_ATIME 0x01
#define TCS34725_ENABLE_AEN 0x02    /* RGBC Enable - Writing 1 actives the ADC, 0 disables it */
#define TCS34725_ENABLE_PON 0x01    /* Power on - Writing 1 activates the internal oscillator, 0 disables it */
#define WAIT_TIME_DISABLE 0x03
#define WTIME_REG 0x83
#define CONTROL_REG 0X8F
#define ID_REG 0X92
#define CDATAL 0x94
#define CDATAH 0x95
#define RDATAL 0x96
#define RDATAH 0x97
#define GDATAL 0x98
#define GDATAH 0x99
#define BDATAL 0x9A
#define BDATAH 0x9B

//Valor de color de una medida estandarizada en RGB
typedef struct{
	float red;
	float green;
	float blue;
} ColorRGB_Sample;



int init_COLOR (void);
void I2C_callback(uint32_t event);
void I2C_Color_Init(void);
void getRGBData(void);

void configurar_sensor(void);
void leer_registro(void); 
void iniciar_registros_sensor(void);
void addToQueue(ColorRGB_Sample sample);


#endif
