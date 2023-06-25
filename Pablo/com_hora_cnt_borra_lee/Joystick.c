#include "joystick.h"
#include "stm32f4xx_hal.h"

/*----------------------------------------------------------------------------
 *      Thread_Joystick:
 *			Cada vez que se detecta una pulsación se envía un mensaje a una cola
 *			de mensajes con información de la tecla pulsada y si la pulsación ha
 *			sido normal o larga
 *---------------------------------------------------------------------------*/
#define MSGQUEUE_OBJECTS 8                  // number of Message Queue Objects
#define PULSACION_LARGA 0x10
#define PULSACION_CORTA 0x00
 
osMessageQueueId_t mid_JoystickQueue;                // message queue id

osThreadId_t tid_Joystick;                        // thread id

osTimerId_t tim_id_pulsacion_corta;                            // timer id
static uint32_t exec_pulsacion_corta;                          // argument for the timer call back function

osTimerId_t tim_id_pulsacion_larga;                            // timer id
static uint32_t exec_pulsacion_larga;                          // argument for the timer call back function

MSGQUEUE_JOYSTICK_t joy;
 
void Thread_Joystick (void *argument);                   // thread function

// One-Shoot Timer Function
static void Timer_Pulsacion_Corta_Callback (void const *arg) {
  if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10)){ // PULSACION ARRIBA
		joy.valor = PULSE_UP;
		osMessageQueuePut(mid_JoystickQueue, &joy, 0U, osWaitForever);
		osTimerStart(tim_id_pulsacion_larga,950);
	} else if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_11)){ // PULSACION DERECHA
		joy.valor = PULSE_RIGHT;
		osMessageQueuePut(mid_JoystickQueue, &joy, 0U, osWaitForever);
	} else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_12)){ // PULSACION ABAJO
		joy.valor = PULSE_DOWN;
		osMessageQueuePut(mid_JoystickQueue, &joy, 0U, osWaitForever);
	} else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_14)){ // PULSACION IZQUIERDA
		joy.valor = PULSE_LEFT;
		osMessageQueuePut(mid_JoystickQueue, &joy, 0U, osWaitForever);
	} else if(HAL_GPIO_ReadPin(GPIOE, GPIO_PIN_15)){ // PULSACION CENTRO
		joy.valor = PULSE_CENTER;
		osMessageQueuePut(mid_JoystickQueue, &joy, 0U, osWaitForever);
	}	
}

static void Timer_Pulsacion_Larga_Callback (void const *arg) {
  if(HAL_GPIO_ReadPin(GPIOB, GPIO_PIN_10)){ // PULSACION ARRIBA LARGA
		joy.valor = PULSE_UP_LONG;
		osMessageQueuePut(mid_JoystickQueue, &joy, 0U, osWaitForever);
	}	
}
 
int Init_Joystick (void) {
	static GPIO_InitTypeDef GPIO_InitStruct;
	__HAL_RCC_GPIOB_CLK_ENABLE();
	
	// ARRIBA
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	// DERECHA
	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	// ABAJO
	GPIO_InitStruct.Pin = GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	// IZQUIERDA
	GPIO_InitStruct.Pin = GPIO_PIN_14;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	// CENTRO
	GPIO_InitStruct.Pin = GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
	GPIO_InitStruct.Pull = GPIO_PULLDOWN;
	HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);	
	
	HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
	
	mid_JoystickQueue = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_JOYSTICK_t), NULL);
	if (mid_JoystickQueue == NULL) {
    return (-1);
  }
	// Create one-shoot timer
  exec_pulsacion_corta = 1U;
  tim_id_pulsacion_corta = osTimerNew((osTimerFunc_t)&Timer_Pulsacion_Corta_Callback, osTimerOnce, &exec_pulsacion_corta, NULL);
	
	exec_pulsacion_larga = 2U;
  tim_id_pulsacion_larga = osTimerNew((osTimerFunc_t)&Timer_Pulsacion_Larga_Callback, osTimerOnce, &exec_pulsacion_larga, NULL);
 
  tid_Joystick = osThreadNew(Thread_Joystick, NULL, NULL);
  if (tid_Joystick == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thread_Joystick (void *argument) {
  while (1) {
		osThreadFlagsWait(0x01,osFlagsWaitAll,osWaitForever);
		osTimerStart(tim_id_pulsacion_corta,50);
    osThreadYield();                                            // suspend thread
  }
}


//Código para testear el módulo

int arriba, abajo, derecha, izquierda, centro, larga = 0;
 
osThreadId_t tid_Test;                        // thread id
 
void Thread_Test (void *argument);                   // thread function
 
int Init_Test_Joystick (void) {
	
  tid_Test = osThreadNew(Thread_Test, NULL, NULL);
  if (tid_Test == NULL) {
    return(-1);
  }	
	
	Init_Joystick();
	
  return(0);
}
 
void Thread_Test (void *argument) {
	osStatus_t status;
	MSGQUEUE_JOYSTICK_t joystick;

	uint8_t aux_joy = 0;
	
	
  while (1) {
		status = osMessageQueueGet(mid_JoystickQueue, &joystick, 0U, 0U);
		if(status == osOK){
			aux_joy = joystick.valor;
		}
		
					switch(aux_joy){
						case PULSE_RIGHT:
							aux_joy = 0;
						derecha++;
							break;
						
						case PULSE_LEFT:
							aux_joy = 0;
						izquierda++;
							break;
						
						case PULSE_UP:
							aux_joy = 0;
						arriba++;
							break;
					
						case PULSE_DOWN:
							aux_joy = 0;
						abajo++;
							break;
		
						case PULSE_CENTER:
							aux_joy = 0;
						centro++;
							break;
						
						case PULSE_UP_LONG:
							aux_joy = 0;
						larga++;
							break;
					}

		    osThreadYield(); 
		}

  }
