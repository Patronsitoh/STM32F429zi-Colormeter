
#include "stm32f4xx_hal.h"
#include "hora.h"
#include "principal.h"
 
/*----------------------------------------------------------------------------
 *      Thread_Hora: 
 *			Actualiza las variables globales "horas", "minutos" y "segundos"
 *---------------------------------------------------------------------------*/

osThreadId_t tid_Hora;                        // thread id

/*----- Periodic Timer Example -----*/
osTimerId_t tim_id_hora;                            // timer id
static uint32_t exec_hora;                          // argument for the timer call back function

int segundos = 0, minutos = 0, horas = 0;

// Periodic Timer Function
static void TimerHora_Callback (void const *arg) {
  segundos++;
}
 
void Thread_Hora (void *argument);                   // thread function


int Init_Hora (void) {
  // Create periodic timer
  exec_hora = 2U;
  tim_id_hora = osTimerNew((osTimerFunc_t)&TimerHora_Callback, osTimerPeriodic, &exec_hora, NULL);
	 
  tid_Hora = osThreadNew(Thread_Hora, NULL, NULL);
  if (tid_Hora == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thread_Hora (void *argument) {
osTimerStart(tim_id_hora, 1000U);
	
  while (1) {
    if(segundos > 59){
			segundos = 0;
			minutos++;
		}
		
		if(minutos > 59){
			minutos = 0;
			horas++;
		}
		
		if(horas > 23){
			horas = 0;
		}
		
		if(segundos < 0){
			segundos = 59;
		}
		
		if(minutos < 0){
			minutos = 59;
		}
		
		if(horas < 0){
			horas = 23;
		}
		osThreadYield();
	}
}

//Código para testear el módulo

//osThreadId_t tid_Test;                        // thread id
// 
//void Thread_Test (void *argument);                   // thread function
// 
//int Init_Test_Hora (void) {
//	
//  tid_Test = osThreadNew(Thread_Test, NULL, NULL);
//  if (tid_Test == NULL) {
//    return(-1);
//  }	
//	
//	Init_Hora();

//  return(0);
//}
// 
//void Thread_Test (void *argument) {
//	
//	int aux_s = 60, aux_min = 60, aux_h = 24;
//	
//  while (1) {
//				if(aux_s != segundos || aux_min != minutos || aux_h != horas){
//					aux_s = segundos;
//					aux_min = minutos;
//					aux_h = horas;
//								
//					}
//		    osThreadYield(); 
//		}

//  }
