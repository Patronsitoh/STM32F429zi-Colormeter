#include "Driver_USART.h"
#include "cmsis_os2.h" 
#include "commads.h"
#ifndef __COM_H

#define PUESTA_HORA 0x20
#define CUENTA_ATRAS	0x25
#define LEER_CUENTA_ATRAS 0x35

typedef struct {                                // object data type
  char string[50];//REVISAR
} MSGQUEUE_COM_t;

extern osMessageQueueId_t mid_COMQueue;                // message queue id

	int Init_Com (void);
	int Init_Test_Com (void);

void poner_hora(uint8_t valor1,uint8_t valor2,uint8_t valor3,uint8_t valor4,uint8_t valor5,uint8_t valor6);

void trama_puestahora(uint8_t valor1,uint8_t valor2,uint8_t valor3 , uint8_t valor4 ,uint8_t valor5,uint8_t valor6,uint8_t valor7,uint8_t valor8);

void trama_cuenta_atras(uint8_t valor1);

void trama_resp_cuenta_atras(uint8_t valor1);
#endif
