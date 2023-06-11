                  
#include "clock.h"
 
osThreadId_t tid_clock;               
void clock_Callback(void *argument);   


osTimerId_t timer_Reloj; 																	
static uint32_t exec2 = 2U; 



static void timer_Reloj_Callback (void const *arg) {
		segundos++;
	
	 if(segundos > 59){
		 segundos = 0;
		 minutos++;
	 }
	 
	 if(minutos > 59){
	  minutos = 0;
		horas++;
	 }

	 if(horas > 23){
		 horas =0;
	 }	
	
} 



 
int initiClock(void) {
 
  tid_clock = osThreadNew(clock_Callback, NULL, NULL);
	timer_Reloj = osTimerNew((osTimerFunc_t)&timer_Reloj_Callback, osTimerPeriodic, &exec2, NULL); //Se crea el objeto timer Peridodico
	
  if (tid_clock == NULL) {
    return(-1);
  }
	
	
 
  return(0);
}
 
void clock_Callback (void *argument) {
	osTimerStart(timer_Reloj, 1000U);
 
  while (1) {
    ; // Insert thread code here...
    osThreadYield();                            // suspend thread
  }
}
