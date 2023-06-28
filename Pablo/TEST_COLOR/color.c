#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "color.h"
 
extern ARM_DRIVER_I2C            Driver_I2C1;
static ARM_DRIVER_I2C *I2Cdrv = &Driver_I2C1;


osThreadId_t tid_COLOR;                       // thread id
void threadColor_callback (void *argument); // thread function 

ColorRGB_Sample lastestSamples[50]; //Aquí se guardarán las 50 utlimas medidas en formato RGB
uint16_t lastestSamplesPointer = 0; //Puntero que indica el siguiente  hueco a rellenar en la cola

float r, g, b = 0; //Valores de color para los Watches
 
int init_COLOR (void) {
  
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
		iniciar_registros_sensor();
		configurar_sensor();
		getRGBData();
			//a++;			
		
		osDelay(1000);		
		osThreadYield();		
  }
}


void I2C_callback(uint32_t event){

    osThreadFlagsSet(tid_COLOR, 0x02);
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
			osThreadFlagsWait(0x02, osFlagsWaitAny, osWaitForever);


}
void configurar_sensor(){		//Escritura del registro	control
			uint8_t cmd[2] = {0x8F,0x81};
			
			I2Cdrv->MasterTransmit(TCS34725_ADDR,cmd,2,true);			//Registro	addr 0x00
			osThreadFlagsWait(0x02, osFlagsWaitAny, osWaitForever);
			
		}

/**
 * @brief Obtiene las medidas del sensor
 * 
 */
void getRGBData(void){

  uint8_t buffer[8]; ///Aqui se guardan las medidas del sensor en RAW
	
	uint8_t colorRegistersQuery[8] = {CDATAL, CDATAH, RDATAL, RDATAH, GDATAL, GDATAH, BDATAH, BDATAL};
	I2Cdrv->MasterTransmit(TCS34725_ADDR,colorRegistersQuery,8,false); //Enviamos los registros donde queremos leer
	osThreadFlagsWait(0x02, osFlagsWaitAny, osWaitForever);	

	
	I2Cdrv->MasterReceive(TCS34725_ADDR,buffer,8,false); //Leemos la respuesta desde los registros solicitados y  almacenamos en buffer
	osThreadFlagsWait(0x02, osFlagsWaitAny, osWaitForever);
		
	int red, blue, green, clear = 0; 

	///Obtiene las medidas crudas del sensor con 16 bits, en 0 y 65535
  clear = (buffer[1] << 8) | buffer[0];
  red = (buffer[3] << 8) | buffer[2];
  green = (buffer[5] << 8) | buffer[4];
  blue = (buffer[7] << 8) | buffer[6];
	
	ColorRGB_Sample lastSample;
	
	r = lastSample.red = (float)red / clear*255;
	g = lastSample.green = (float)green / clear*255;
	b = lastSample.blue = (float)blue / clear*255;	
	
	addToQueue(lastSample);
	
	
	
}


/**
	@brief Añade un elemento a la cola de ultimas mediciones

*/
void addToQueue(ColorRGB_Sample sample){
	lastestSamples[lastestSamplesPointer] = sample;
	
	if(sizeof(lastestSamples) -1){
		lastestSamplesPointer = 0;
	}else{
		lastestSamplesPointer++;
	}		
	
}
