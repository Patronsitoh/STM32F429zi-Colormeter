#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "com.h"
#include <stdio.h>
#include <string.h>

/*----------------------------------------------------------------------------
 *      Thread 1 'Thread_Com': Módulo encargado de la comunicación con 
 *      el PC a través de la línea RS232 integrada en el USB
 *---------------------------------------------------------------------------*/
 
 #define MSGQUEUE_OBJECTS 8                     // number of Message Queue Objects
 

osMessageQueueId_t mid_COM_MOSIQueue;                // message queue id
osMessageQueueId_t mid_COM_MISOQueue;                // message queue id
osMessageQueueId_t mid_COM_MISOHORAQueue;                // message queue id

MSGQUEUE_COM_MISO_HORA_t hora;


osThreadId_t tid_Com;                        // thread id
	
void Thread_Com (void *argument);                   // thread function

/* USART Driver */
extern ARM_DRIVER_USART Driver_USART3;
static ARM_DRIVER_USART * USARTdrv = &Driver_USART3;

void callbackCOM(uint32_t event) {
	uint32_t mask;
  mask = ARM_USART_EVENT_RECEIVE_COMPLETE  |
         ARM_USART_EVENT_TRANSFER_COMPLETE |
         ARM_USART_EVENT_SEND_COMPLETE     |
         ARM_USART_EVENT_TX_COMPLETE       ;
  if (event & mask) {
    osThreadFlagsSet(tid_Com, 0x08);
  }
}
 
int Init_Com (void) {
	
	mid_COM_MOSIQueue = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_COM_MOSI_t), NULL);
  if (mid_COM_MOSIQueue == NULL) {
    return(-1);
  }
	
	mid_COM_MISOQueue = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_COM_MISO_t), NULL);
  if (mid_COM_MISOQueue == NULL) {
    return(-1);
  }
	
		mid_COM_MISOHORAQueue = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_COM_MISO_HORA_t), NULL);
  if (mid_COM_MISOHORAQueue == NULL) {
    return(-1);
  }
  tid_Com = osThreadNew(Thread_Com, NULL, NULL);
  if (tid_Com == NULL) {
    return(-1);
  }
 
  return(0);
}
 
void myUART_Init(void){
	USARTdrv->Initialize(callbackCOM); //Initialize the USART driver
	USARTdrv->PowerControl(ARM_POWER_FULL); //Power up the USART peripheral
	USARTdrv->Control(ARM_USART_MODE_ASYNCHRONOUS | 
													ARM_USART_DATA_BITS_8 | 
													ARM_USART_PARITY_NONE | 
													ARM_USART_STOP_BITS_1 | 
													ARM_USART_FLOW_CONTROL_NONE, 115200); //Configure the USART to 9600 Bits/sec
	USARTdrv->Control(ARM_USART_CONTROL_TX, 1);
	USARTdrv->Control(ARM_USART_CONTROL_RX, 1);
}
void Thread_Com (void *argument) {
	MSGQUEUE_COM_MOSI_t com;
	osStatus_t status;
	myUART_Init();
	uint8_t  cmd[15];
  while (1) {
		
		status = osMessageQueueGet(mid_COM_MOSIQueue, &com, 0U, osWaitForever);   // wait for message
    if (status == osOK) {
			USARTdrv->Send(com.string, strlen(com.string));
			osThreadFlagsWait(0x08, osFlagsWaitAny, osWaitForever);
			osDelay(200);
    }else{
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
			valor2=cmd[1];		//COMANDO
			valor3=cmd[2];		//LONGITUD
			valor4=cmd[3];		//PAYLOAD
			valor5=cmd[4];
			valor6=cmd[5];
			valor7=cmd[6];
			valor8=cmd[7];
			valor9=cmd[8];
			valor10=cmd[9];
			valor11=cmd[10];
			valor12=cmd[11];		//FIN DE TRAMA
			
				switch (cmd[1]){
		//Se genera una trama diferente en funcion del COMANDO
		case PUESTA_HORA:
					//trama_puestahora(valor4,valor5,valor6,valor7,valor8,valor9,valor10,valor11);
					poner_hora(valor4,valor5,valor7,valor8,valor10,valor11);
		break;
		
		case CUENTA_ATRAS:
					trama_cuenta_atras(valor4);
		break;
		
		case LEER_CUENTA_ATRAS:
					//trama_resp_cuenta_atras(cuenta-2);  //porque para la representacion del display le sumo 1 teniendo el 0 como valor para enviar acciones
		break;
		
		case CICLO_MEDIDAS:
					crear_ciclo_medidas(valor5,valor6,valor8,valor9);
		break;
		
		case N_MEDIDAS_ALMACENADAS:
					//trama_total_medidas(fin);
		break;
		
		case ULTIMA_MEDIDA:
					//trama_ultima_medida();
		break;
		
		case TODAS_LAS_MEDIDAS:
					//mostrar_todas_medidas();
		break;
		
		case BORRAR_MEDIADAS:
					trama_borrar_medidas();
					//borrar_medidas();
		break;
		
		default:
					USARTdrv->Send("Fallo en el envio de la trama\n",31);
				osThreadFlagsWait(0x08, osFlagsWaitAny, osWaitForever);
		break;
	
	}//end switch
		}
    osThreadYield();
  }
}

void trama_puestahora(uint8_t valor1,uint8_t valor2,uint8_t valor3 , uint8_t valor4 ,uint8_t valor5,uint8_t valor6,uint8_t valor7,uint8_t valor8){
			
	uint8_t comando[21];
			comando[0]=48;		//0
			comando[1]=49;		//1
			comando[2]=32;		//espacio
			comando[3]=68;		//D
			comando[4]=65;		//F
			comando[5]=32;		//espacio
			comando[6]=48;		//0
			comando[7]=70;		//C
			comando[8]=32;		//espacio
			comando[9]=valor1;		//HORA1
			comando[10]=valor2;		//HORA2
			comando[11]=valor3;		//:
			comando[12]=valor4;		//MINUTOS1
			comando[13]=valor5;		//MINUTOS2
			comando[14]=valor6;		//:
			comando[15]=valor7;		//SEGUNDOS1
			comando[16]=valor8;		//SEGUNDOS2
			comando[17]=32;		//espacio
			comando[18]=70;		//F
			comando[19]=69;		//E
			comando[20]=10;		//SALTO DE LINEA
			USARTdrv->Send(comando,21);
			osThreadFlagsWait(0x08, osFlagsWaitAny, osWaitForever);
}
void poner_hora(uint8_t valor1,uint8_t valor2,uint8_t valor3,uint8_t valor4,uint8_t valor5,uint8_t valor6){
	hora.dh=valor1-0x30;
	hora.uh=valor2-0x30;
	hora.dm=valor3-0x30;
	hora.um=valor4-0x30;
	hora.ds=valor5-0x30;
	hora.us=valor6-0x30;
	osMessageQueuePut(mid_COM_MISOHORAQueue, &hora, 0U, 0U);

}
void trama_cuenta_atras(uint8_t valor1){
			uint8_t comando[15];
			
			uint8_t cuenta_atras=valor1-48;
			
//			establecer_cuenta=cuenta_atras+2;		//Sumamos 2 al valor porque en las operacion decremetamos este valor en 2
//			cuenta=establecer_cuenta;
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

void crear_ciclo_medidas(uint8_t valor5,uint8_t valor6,uint8_t valor8,uint8_t valor9){//Solamente obtien los valor de la trama para pasarlos de ascii a entero y luego generar	
																																											//los ciclos con estos valores
	
	uint8_t cantidad1=valor5-48;
	uint8_t cantidad2=valor6-48;
	uint8_t tiempo1=valor8-48;
	uint8_t tiempo2=valor9-48;
	
//	 num_ciclos=cantidad1*10+cantidad2;
//	 time_ciclo=tiempo1*10+tiempo2;
//	
//	osSignalSet(tid_Thread_control,S_CICLO_MEDIDAS);

}

void trama_total_medidas(uint8_t valor){
			uint8_t valor1;
			uint8_t valor2;
	
			valor1=valor/10;
			valor2=valor%10;
			uint8_t comando[16];
	
			comando[0]=48;		//0
			comando[1]=49;		//1
			comando[2]=32;		//ESPACIO
			comando[3]=66;		//B
			comando[4]=70;		//F
			comando[5]=32;		//ESPACIO
				if(valor<10){		//condicion que controla el numero de valores de la trama si tiene un digito 5 y con 2 sera 6
			comando[6]=48;		//0
			comando[7]=53;		//5
			}else{
			comando[6]=48;		//0
			comando[7]=54;		//6
			}
			comando[8]=32;		//ESPACIO
			comando[10]=valor1+48;		//el numero de mediadas
			comando[11]=valor2+48;		//el numero de mediadas
			comando[12]=32;		//ESPACIO
			comando[13]=70;		//F
			comando[14]=69;		//E
			comando[15]=10;		//SALTO DE LINEA

			USARTdrv->Send(comando,16);
			osThreadFlagsWait(0x08, osFlagsWaitAny, osWaitForever);
}



//void 	trama_ultima_medida(){
//				uint8_t comando[28];
//	
//			uint8_t hora1=medidas[fin-1].hour;
//			uint8_t minuto1=medidas[fin-1].min;
//			uint8_t segundo1=medidas[fin-1].seg;
//	
//			uint8_t buf1=medidas[fin-1].buff1-48;
//			uint8_t buf2=medidas[fin-1].buff2-48;
//			uint8_t buf3=medidas[fin-1].buff3;
//		
//			uint8_t buf100=buf1*10+buf2;

//		if (fin!=0){				//Si no hay mediadas ALMACENADAS no pasa la ultima medida
//			
//			if(buf100!=100){		//Controla si el valor que recoge el sensor es 100% para mostrarlo
//			comando[0]=48;		//0
//			comando[1]=49;		//1
//			comando[2]=32;		//ESPACIO
//			comando[3]=65;		//A
//			comando[4]=70;		//F
//			comando[5]=32;		//ESPACIO
//			comando[6]=49;		//1
//			comando[7]=50;		//2
//			comando[8]=32;		//ESPACIO
//			comando[10]=(hora1/10)+48;		//hora en ascii
//			comando[11]=(hora1%10)+48;		//hora	en ascii
//				comando[12]=58;							//:
//			comando[13]=(minuto1/10)+48;		//minuto en ascii
//			comando[14]=(minuto1%10)+48;		//minuto en ascii
//				comando[15]=58;									//:
//			comando[16]=(segundo1/10)+48;		//segundos en ascii
//			comando[17]=(segundo1%10)+48;		//segundos en ascii
//			comando[18]=45;								//-
//			comando[19]=buf1+48;		//buffer1 de temperatura
//			comando[20]=buf2+48;		//buffer2 de temperatura
//			comando[21]=46;					//.
//			comando[22]=buf3;			//buffer2 de temperatura
//			comando[23]=37;				//%
//			comando[24]=32;		//ESPACIO
//			comando[25]=70;		//F
//			comando[26]=69;		//E
//			comando[27]=10;		//SALTO DE LINEA
//		
//			USARTdrv->Send(comando,28);
//			osThreadFlagsWait(0x08, osFlagsWaitAny, osWaitForever);
//		}else{		//Solo para la representacion de 100%
//		
//			comando[0]=48;		//0
//			comando[1]=49;		//1
//			comando[2]=32;		//ESPACIO
//			comando[3]=65;		//A
//			comando[4]=70;		//F
//			comando[5]=32;		//ESPACIO
//			comando[6]=49;		//1
//			comando[7]=50;		//2
//			comando[8]=32;		//ESPACIO
//			comando[10]=(hora1/10)+48;		//hora en ascii
//			comando[11]=(hora1%10)+48;		//hora en ascii
//			comando[12]=58;									//:
//			comando[13]=(minuto1/10)+48;		//minuto en ascii
//			comando[14]=(minuto1%10)+48;		//minuto en ascii
//			comando[15]=58;									//:
//			comando[16]=(segundo1/10)+48;		//segundos en ascii
//			comando[17]=(segundo1%10)+48;		//segundos en ascii
//			comando[18]=45;							//-
//			comando[19]=1+48;			//REPRESENTA EN 100%
//			comando[20]=0+48;
//			comando[21]=0+48;		//
//			comando[22]=37;			//%
//			comando[23]=32;		//ESPACIO
//			comando[24]=70;		//F
//			comando[25]=69;		//E
//			comando[26]=10;		//SALTO DE LINEA
//		
//			USARTdrv->Send(comando,27);
//			osThreadFlagsWait(0x08, osFlagsWaitAny, osWaitForever);
//		
//		}	
//		
//	}
//}

//void 	mostrar_todas_medidas(){
//			uint8_t comando[28];
//			int i;
//			for(i=0;i<fin;i++){
//			uint8_t hora1=medidas[i].hour;
//			uint8_t minuto1=medidas[i].min;
//			uint8_t segundo1=medidas[i].seg;
//	
//			uint8_t buf1=medidas[i].buff1-48;
//			uint8_t buf2=medidas[i].buff2-48;
//			uint8_t buf3=medidas[i].buff3;

//			uint8_t buf100=buf1*10+buf2;
//			if(buf100!=100){								//Solamente controla si el valor de cuenta es 100% y lo representa 
//			comando[0]=48;		//0
//			comando[1]=49;		//1
//			comando[2]=32;		//ESPACIO
//			comando[3]=65;		//A
//			comando[4]=70;		//F
//			comando[5]=32;		//ESPACIO
//			comando[6]=49;		//1
//			comando[7]=50;		//2
//			comando[8]=32;		//ESPACIO
//			comando[10]=(hora1/10)+48;		//hora en ascii
//			comando[11]=(hora1%10)+48;		//hora en ascii
//				comando[12]=58;								//:
//			comando[13]=(minuto1/10)+48;		//minuto en ascii
//			comando[14]=(minuto1%10)+48;		//minuto en ascii
//				comando[15]=58;									//:
//			comando[16]=(segundo1/10)+48;		//segundos en ascii
//			comando[17]=(segundo1%10)+48;			//segundos en ascii
//			comando[18]=45;		//-
//			
//			comando[19]=buf1+48;		//buffer1 de temperatura
//			comando[20]=buf2+48;		//buffer2 de temperatura
//			comando[21]=46;					//.
//			comando[22]=buf3;		//buffer3 de temperatura
//			comando[23]=37;				//%
//			comando[24]=32;		//ESPACIO
//			comando[25]=70;		//F
//			comando[26]=69;		//E
//			comando[27]=10;		//SALTO DE LINEA
//				
//			USARTdrv->Send(comando,28);
//			osThreadFlagsWait(0x08, osFlagsWaitAny, osWaitForever);
//		}else{
//		
//			comando[0]=48;		//0
//			comando[1]=49;		//1
//			comando[2]=32;		//ESPACIO
//			comando[3]=65;		//A
//			comando[4]=70;		//F
//			comando[5]=32;		//ESPACIO
//			comando[6]=49;		//1
//			comando[7]=50;		//2
//			comando[8]=32;		//ESPACIO
//			comando[10]=(hora1/10)+48;		//hora en ascii
//			comando[11]=(hora1%10)+48;		//hora en ascii
//			comando[12]=58;								//:
//			comando[13]=(minuto1/10)+48;		//minuto en ascii
//			comando[14]=(minuto1%10)+48;		//minuto en ascii
//			comando[15]=58;								//:
//			comando[16]=(segundo1/10)+48;		//segundos en ascii
//			comando[17]=(segundo1%10)+48;		//segundos en ascii
//			comando[18]=45;							//-
//			comando[19]=1+48;				//REPRESENTA EL 100%
//			comando[20]=0+48;
//			comando[21]=0+48;
//			comando[22]=37;			//%
//			comando[23]=32;		//ESPACIO
//			comando[24]=70;		//F
//			comando[25]=69;		//E
//			comando[26]=10;		//SALTO DE LINEA
//				
//			USARTdrv->Send(comando,27);
//			osThreadFlagsWait(0x08, osFlagsWaitAny, osWaitForever);
//		
//		
//		}

//		
//		}

//}
void trama_borrar_medidas(){
uint8_t comando[15];
			comando[0]=48;		//0
			comando[1]=49;		//1
			comando[2]=32;		//ESPACIO
			comando[3]=57;		//9
			comando[4]=70;		//F
			comando[5]=32;		//ESPACIO
			comando[6]=48;		//0
			comando[7]=52;		//4
			comando[8]=32;		//ESPACIO
			comando[10]=70;		//F
			comando[11]=69;		//E
			comando[12]=10;		//SALTO DE LINEA

			USARTdrv->Send(comando,13);
			osThreadFlagsWait(0x08, osFlagsWaitAny, osWaitForever);
}


//Código para testear el módulo

//osThreadId_t tid_Test;                        // thread id
// 
//void Thread_Test (void *argument);                   // thread function
// 
//int Init_Test_Com (void) {
//	
//  tid_Test = osThreadNew(Thread_Test, NULL, NULL);
//  if (tid_Test == NULL) {
//    return(-1);
//  }	

//	Init_Com();
//	
//  return(0);
//}
// 
//void Thread_Test (void *argument) {

//	MSGQUEUE_COM_MOSI_t com;
//	int contador;
//	
//  while (1) {
//		contador++;
//			sprintf(com.string, "\r\nHOLA ESTO ES UNA PRUEBA\n\r");
//			osMessageQueuePut(mid_COM_MOSIQueue, &com, 0U, 0U);
//		
//	if(contador == 3){
//		contador = 0;
//			sprintf(com.string, "\r\nHAN PASADO 3 SEGUNDOS\n\r");
//			osMessageQueuePut(mid_COM_MOSIQueue, &com, 0U, 0U);
//		
//	}
//			osDelay(1000);
//		
//    osThreadYield(); 
//  }
//}
