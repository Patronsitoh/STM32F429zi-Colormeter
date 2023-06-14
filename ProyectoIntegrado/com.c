#include "com.h"
                         // CMSIS RTOS header file
#include <stdio.h>
#include <string.h>

 #define MSGQUEUE_OBJECTS 16                     // number of Message Queue Objects

osMessageQueueId_t mid_COMQueue;                // message queue id
 
osThreadId_t tid_Com;                        // thread id
 
void Thread_Com (void *argument);                   // thread function

/* USART Driver */
extern ARM_DRIVER_USART Driver_USART3;

void callbackCOM(uint32_t event) {}
 
int Init_Com (void) {
	
	mid_COMQueue = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_COM_t), NULL);
  if (mid_COMQueue == NULL) {
    return(-1);
  }
	
 
  tid_Com = osThreadNew(Thread_Com, NULL, NULL);
  if (tid_Com == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void Thread_Com (void *argument) {
	osStatus_t status;
	MSGQUEUE_COM_t com;
	
	static ARM_DRIVER_USART * USARTdrv = &Driver_USART3;
	USARTdrv->Initialize(callbackCOM); //Initialize the USART driver
	USARTdrv->PowerControl(ARM_POWER_FULL); //Power up the USART peripheral
	USARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS | 
													ARM_USART_DATA_BITS_8 | 
													ARM_USART_PARITY_NONE | 
													ARM_USART_STOP_BITS_1 | 
													ARM_USART_FLOW_CONTROL_NONE, 9600); //Configure the USART to 9600 Bits/sec
	USARTdrv->Control(ARM_USART_CONTROL_TX, 1);
	USARTdrv->Control(ARM_USART_CONTROL_RX, 1);
	
  while (1) {
		status = osMessageQueueGet(mid_COMQueue, &com, 0U, osWaitForever);   // wait for message
    if (status == osOK) {
			USARTdrv->Send(com.string, strlen(com.string));
    }
    osThreadYield();
  }
}
