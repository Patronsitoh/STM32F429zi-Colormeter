#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "color.h"

/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Color': Thread que mediante el sensor TCS34725
 *			recoge valores RGB y de luminancia. Tras normarlizarlo entre 0 y 255
 *			manda una cola de mensajes a Principal
 *---------------------------------------------------------------------------*/
 #define MSGQUEUE_OBJECTS 8                    // number of Message Queue Objects

extern ARM_DRIVER_I2C            Driver_I2C1;
static ARM_DRIVER_I2C *I2Cdrv = &Driver_I2C1;

osThreadId_t tid_COLOR;                       // thread id
void threadColor_callback (void *argument); // thread function 

osMessageQueueId_t mid_COLORQueue;
MSGQUEUE_COLOR_t color; //una medida

 
int Init_Color (void) {
	
	mid_COLORQueue = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_COLOR_t), NULL);
  if (mid_COLORQueue == NULL) {
    return(-1);
  }
	
  tid_COLOR = osThreadNew(threadColor_callback, NULL, NULL);
  if (tid_COLOR == NULL) {
    return(-1);
  }
  return(0);
}
 
void threadColor_callback (void *argument) {
	I2C_Color_Init();
	iniciar_registros_sensor();
	configurar_sensor();
	
  while (1) {
		uint32_t event = osThreadFlagsWait(0x10, osFlagsWaitAny, osWaitForever);
		if(event == 0x10){
			getRGBData();
			osMessageQueuePut(mid_COLORQueue, &color, 0U, 0U);
		}
		osThreadYield();		
  }
}

void I2C_callback(uint32_t event){
    osThreadFlagsSet(tid_COLOR, 0x30);
}

void I2C_Color_Init(void){		
	I2Cdrv->Initialize(I2C_callback);
	I2Cdrv->PowerControl(ARM_POWER_FULL);
	I2Cdrv->Control(ARM_I2C_BUS_SPEED,ARM_I2C_BUS_SPEED_FAST);
  I2Cdrv-> Control(ARM_I2C_BUS_CLEAR, 0);
}

/**
 * @brief Configuración inicial del sensor
 * 
 */
void iniciar_registros_sensor(){		//Escribimos en el registro enable
			uint8_t buf[2] = {0x80,0x83};
			I2Cdrv->MasterTransmit(TCS34725_ADDR,buf,2,true);			//Registro	addr 0x00
			osThreadFlagsWait(0x30, osFlagsWaitAny, osWaitForever);
}
void configurar_sensor(){		//Escritura del registro	control
			uint8_t cmd[2] = {0x8F,0x81};
			I2Cdrv->MasterTransmit(TCS34725_ADDR,cmd,2,true);			//Registro	addr 0x00
			osThreadFlagsWait(0x30, osFlagsWaitAny, osWaitForever);
		}

/**
 * @brief Obtiene las medidas del sensor
 * 
 */
void getRGBData(void){
	int red, blue, green, clear = 0;
  uint8_t buffer[8]; ///Aqui se guardan las medidas del sensor
	uint8_t colorRegistros[8] = {CDATAL, CDATAH, RDATAL, RDATAH, GDATAL, GDATAH, BDATAH, BDATAL};
	
	I2Cdrv->MasterTransmit(TCS34725_ADDR,colorRegistros,8,false);
	osThreadFlagsWait(0x30, osFlagsWaitAny, osWaitForever);

	I2Cdrv->MasterReceive(TCS34725_ADDR,buffer,8,false); //Leemos la respuesta desde los registros solicitados y  almacenamos en buffer
	osThreadFlagsWait(0x30, osFlagsWaitAny, osWaitForever);

	//Medidas de 16 bits, entre 0 y 65535
  clear = (buffer[1] << 8) | buffer[0];
  red = (buffer[3] << 8) | buffer[2];
  green = (buffer[5] << 8) | buffer[4];
  blue = (buffer[7] << 8) | buffer[6];
	
	//Normalización de los valores entre 0 y 255
	color.red = ((float)red/clear)*255;
	color.green = ((float)green/clear)*255;
	color.blue = ((float)blue/clear)*255;
}

////Codigo para testear el modulo
//osThreadId_t tid_COLOR_test;
//void Thread_COLOR_test(void* argument);
//float r, g, b = 0;

//int Init_Test_COLOR(void){
//    tid_COLOR_test = osThreadNew(Thread_COLOR_test, NULL, NULL);
//    if(tid_COLOR_test == NULL){
//        return(-1);
//		}
//		Init_Color();
//    return(0);
//	}
//		
//void Thread_COLOR_test(void *argument){
//	I2C_Color_Init();
//	iniciar_registros_sensor();
//	configurar_sensor();
//	int red, blue, green, clear = 0;
//	
//  while (1) {
//		iniciar_registros_sensor();
//		configurar_sensor();
//		uint32_t event = osThreadFlagsWait(0x10, osFlagsWaitAny, 0);
//		if(event == 0x10){
//			getRGBData();
//			r = color.red = (float)red / clear*255;
//			g = color.green = (float)green / clear*255;
//			b = color.blue = (float)blue / clear*255;	
//			osMessageQueuePut(mid_COLORQueue, &color, 0U, 0U);
//			//osDelay(50U);
//		}
//		osThreadYield();		
//  }
//}
