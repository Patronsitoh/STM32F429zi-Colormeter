#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "com.h"
#include <stdio.h>
#include <string.h>

/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Name': Sample thread
 *---------------------------------------------------------------------------*/
 
#define MSGQUEUE_OBJECTS 16                     // number of Message Queue Objects
uint8_t  cmd[15];
extern ARM_DRIVER_USART Driver_USART3;	
static ARM_DRIVER_USART * USARTdrv = &Driver_USART3;
osMessageQueueId_t mid_COMQueue;                // message queue id
 
osThreadId_t tid_Com;                        // thread id

osThreadId_t tid_Test;                        // thread id
 
void Thread_Test (void *argument);                   // thread function
 
void Thread_Com (void *argument);                   // thread function
int cuenta = 4;
/* USART Driver */
extern ARM_DRIVER_USART Driver_USART3;

void callbackCOM(uint32_t event) {
	uint32_t mask;
  mask = ARM_USART_EVENT_RECEIVE_COMPLETE  |
         ARM_USART_EVENT_TRANSFER_COMPLETE |
         ARM_USART_EVENT_SEND_COMPLETE     |
         ARM_USART_EVENT_TX_COMPLETE       ;
  if (event & mask) {
    osThreadFlagsSet(tid_Test, 0x08);
  }
}
 
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
			osThreadFlagsWait(0x08, osFlagsWaitAny, osWaitForever);
    }
    osThreadYield();
  }
}


//Código para testear el módulo


 
int Init_Test_Com (void) {
	
  tid_Test = osThreadNew(Thread_Test, NULL, NULL);
  if (tid_Test == NULL) {
    return(-1);
  }	
	Init_Com();
  return(0);
}
 
void Thread_Test (void *argument) {
	osDelay(50U);
	USARTdrv->Initialize(callbackCOM); //Initialize the USART driver
	USARTdrv->PowerControl(ARM_POWER_FULL); //Power up the USART peripheral
	USARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS | 
													ARM_USART_DATA_BITS_8 | 
													ARM_USART_PARITY_NONE | 
													ARM_USART_STOP_BITS_1 | 
													ARM_USART_FLOW_CONTROL_NONE, 9600); //Configure the USART to 9600 Bits/sec
	USARTdrv->Control(ARM_USART_CONTROL_TX, 1);
	USARTdrv->Control(ARM_USART_CONTROL_RX, 1);
	
	USARTdrv->Send("\r\nExiste conexion con UART0\r\n",26);
	osThreadFlagsWait(0x08, osFlagsWaitAny, osWaitForever);
		
  while (1) {
		uint8_t valor1;
		uint8_t valor2;
		uint8_t valor3;
		uint8_t valor4;
		uint8_t valor5;
		uint8_t valor6;
		uint8_t valor7;
		uint8_t valor8;
		uint8_t valor9;
		uint8_t valor10;
		uint8_t valor11;
		uint8_t valor12;
		
		USARTdrv->Receive(cmd,12);
		osThreadFlagsWait(0x08, osFlagsWaitAny, osWaitForever);
		valor1=cmd[0];		//CABECERA
		valor2=cmd[1];		//CABECERA
		valor3=cmd[2];		//
		valor4=cmd[3];		//COMANDO
		valor5=cmd[4];		//COMANDO
		valor6=cmd[5];
		valor7=cmd[6];		//LONG
		valor8=cmd[7];		//LONG
		valor9=cmd[8];		
		valor10=cmd[9];
		valor11=cmd[10];		//FIN DE TRAMA
		valor12=cmd[11];		//FIN DE TRAMA
	osDelay(1000);
	switch (cmd[1]){
		//Se genera una trama diferente en funcion del COMANDO
		case PUESTA_HORA:
					trama_puestahora(valor4,valor5,valor6,valor7,valor8,valor9,valor10,valor11);
					poner_hora(valor4,valor5,valor7,valor8,valor10,valor11);
		break;
		
		case CUENTA_ATRAS:
					trama_cuenta_atras(valor4);
		break;
		
		case LEER_CUENTA_ATRAS:
					trama_resp_cuenta_atras(cuenta-2);  //porque para la representacion del display le sumo 1 teniendo el 0 como valor para enviar acciones
		break;
		
//		default:
//					USARTdrv->Send("Fallo en el envio de la trama\n",31);
//					osThreadFlagsWait(0x08, osFlagsWaitAny, osWaitForever);
//		break;
	
	}//end switch

    osThreadYield ();                                          
  }//end while
}

void trama_puestahora(uint8_t valor1,uint8_t valor2,uint8_t valor3 , uint8_t valor4 ,uint8_t valor5,uint8_t valor6,uint8_t valor7,uint8_t valor8){
			
		uint8_t comando[50];	
		
		/**
			convertir de char a int -> Valor1 = 2  49   
																 Valor2 = 1	 48   
							decenas   unidades 										
		  hora = (2*10)     +    (1) = 21												
	*/														
	
	
	
	
		 
		int _hora = 	((valor1 - '0')*10)+(valor2 - '0');
		int _minuto = ((valor4 - '0')*10)+(valor5 - '0');
		int _segundo = ((valor7 - '0')*10)+(valor8 - '0');
	
		sprintf(comando, "\r\nHora recibida-> 01 DF 0C \"%d:%d:%d\" FE", _hora, _minuto, _segundo);	
		//sprintf(comando, "\r\nHora recibida-> 01 DF 0C \"%c%c:%c%c:%c%c\" FE", valor1, valor2, valor4, valor5, valor7, valor8);			
				
		USARTdrv->Send(comando,sizeof(comando));
		osThreadFlagsWait(0x08, osFlagsWaitAny, osWaitForever);
			
}
void poner_hora(uint8_t valor1,uint8_t valor2,uint8_t valor3,uint8_t valor4,uint8_t valor5,uint8_t valor6){
	uint8_t hora1=valor1-0x30;
	uint8_t hora2=valor2-0x30;
	uint8_t minuto1=valor3-0x30;
	uint8_t minuto2=valor4-0x30;
	uint8_t segundo1=valor5-0x30;
	uint8_t	segundo2=valor6-0x30;

}
void trama_cuenta_atras(uint8_t valor1){
			uint8_t comando[15];
			
			uint8_t cuenta_atras=valor1-48;
			comando[0]=48;		//0
			comando[1]=49;		//1
			comando[2]=32;		//ESPACIO
			comando[3]=68;		//D
			comando[4]=65;		//A
			comando[5]=32;		//ESPACIO
			comando[6]=48;		//0
			comando[7]=53;		//5
			comando[8]=32;		//ESPACIO
			comando[9]=valor1;		//VALOR DE CONFIGURACION
			comando[10]=32;		//ESPACIO
			comando[11]=70;		//F
			comando[12]=69;		//E
			comando[13]=10;		//SALTO DE LINEA

			USARTdrv->Send(comando,14);
			osThreadFlagsWait(0x08, osFlagsWaitAny, osWaitForever);

}

void trama_resp_cuenta_atras(uint8_t valor1){

	uint8_t comando[15];
			comando[0]=48;		//0
			comando[1]=49;		//1
			comando[2]=32;		//ESPACIO
			comando[3]=67;		//C
			comando[4]=65;		//A
			comando[5]=32;		//ESPACIO
			comando[6]=48;		//0
			comando[7]=53;		//5
			comando[8]=32;		//ESPACIO
			comando[9]=valor1+48;		//valor1 QUE LEE EL ESTADO DE CUENTA Y LO PONE EN ASCII
			comando[10]=32;		//ESPACIO
			comando[11]=70;		//F
			comando[12]=69;		//E
			comando[13]=10;		//SALTO DE LINEA

			USARTdrv->Send(comando,14);
			osThreadFlagsWait(0x08, osFlagsWaitAny, osWaitForever);

}