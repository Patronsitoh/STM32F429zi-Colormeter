#include "principal.h"
#include "color.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
 
osThreadId_t tid_Principal;                        // thread id
 
void principalCallback(void *argument);                   // thread function

char _buffer[12];



typedef struct{
	char buffer[12];	
}Trama_TeraTerm;

Trama_TeraTerm latestSamples[50];
uint16_t pointerLatestSamples = 0;


extern osThreadId_t tid_COLOR;
extern osMessageQueueId_t queueColor;
Trama_Color tramaColorRecibida;

extern 	uint16_t segundos, minutos, horas;





typedef enum {
	INACTIVO, ACTIVO, MEDIDA_AUTO, MEDIDA_MANUAL
} SystemState;

SystemState estadoSistema;

	
typedef struct{
	char string_p[100];
} TRAMA_COM;





 
int init_Principal(void) {
	
	estadoSistema = MEDIDA_MANUAL;
 
  tid_Principal = osThreadNew(principalCallback, NULL, NULL);
  if (tid_Principal == NULL) {
    return(-1);
  }
	
  return(0);
}
 
void principalCallback(void *argument) {
 
  while (1) {
   
		switch(estadoSistema){
			
			case INACTIVO:
					break;
			
			case ACTIVO:
				break;
			
			
			case MEDIDA_MANUAL:					
				
			osThreadFlagsSet(tid_COLOR,0x10); ///Haz una medida bro				
			
			
				//osMessageQueueGet(queueColor,&tramaColorRecibida,0U,0U);				
				
				if(osMessageQueueGet(queueColor,&tramaColorRecibida,0U,0U) == osOK){		
				
					addTimeStampColorQueue(tramaColorRecibida.latestSample);		
				}				
				
			
				break;
			
			
			case MEDIDA_AUTO:				
							
				break;		
		
		}	
		
		
		
		
		
		
		
		
		
    osThreadYield();                            // suspend thread
  }
}




/**
	@brief Almacena el valor recibido y lo añade a la cola de 50 ultimas medidas sitauada en RAM-
		Setea el valor RGB entre 0-100% y añade el timeStamp a cada medida.
		-> timeStamp = hhmmssrrggbb
		

*/

void addTimeStampColorQueue(ColorRGB_Sample sample){	
	
	uint16_t scaledR = (uint16_t)((sample.red / 255.0) * 99);
	uint16_t scaledG = (uint16_t)((sample.green / 255.0) * 99);
	uint16_t scaledB = (uint16_t)((sample.blue / 255.0) * 99);
	
	//Función snpfintf para proteger desbordamientos
	snprintf(_buffer, 7, "%02u%02u%02u", horas, minutos, segundos); // Limitamos la escritura a los primeros 6 caracteres
	snprintf(_buffer + 6, 7, "%02u%02u%02u", scaledR, scaledG, scaledB); // Limitamos la escritura a los primeros 6 caracteres
	
	strcpy(latestSamples[pointerLatestSamples].buffer, _buffer);
		
	if(pointerLatestSamples < 49){
		pointerLatestSamples++;
	}else{
		pointerLatestSamples = 0;		
	}		

		
}	



