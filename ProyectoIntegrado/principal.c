#include "principal.h"
#include "color.h"
 
osThreadId_t tid_Principal;                        // thread id
 
void principalCallback(void *argument);                   // thread function





typedef struct{
	char buffer[12];	
}Trama_TeraTerm;

float bufferGordo[3];



extern osThreadId_t tid_COLOR;
extern osMessageQueueId_t queueColor;
Trama_Color tramaColorRecibida;





typedef enum {
	INACTIVO, ACTIVO, MEDIDA_AUTO, MEDIDA_MANUAL
} SystemState;

SystemState estadoSistema;

	



 
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
			
			
			osMessageQueueGet(queueColor,&tramaColorRecibida,0U,0U);				
				
			addTimeStampColorQueue(tramaColorRecibida.latestSample);
			
				
				
				
				
			
			
			
			
				break;
			
			
			case MEDIDA_AUTO:				
							
				break;		
		
		}	
		
		
		
		
		
		
		
		
		
    osThreadYield();                            // suspend thread
  }
}




/**
	@brief Añade un elemento a la cola de ultimas mediciones

*/

void addTimeStampColorQueue(ColorRGB_Sample sample){
	bufferGordo[0] = sample.red;
	bufferGordo[1] = sample.green;
	bufferGordo[2] = sample.blue;
	

		
	}	



