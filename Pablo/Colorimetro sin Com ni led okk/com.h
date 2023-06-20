#include "Driver_USART.h"
#include "cmsis_os2.h" 
#ifndef __COM_H

#define PUESTA_HORA 0X20
#define CUENTA_ATRAS	0X25
#define LEER_CUENTA_ATRAS 0x35
#define N_MEDIDAS_ALMACENADAS 0X40
#define ULTIMA_MEDIDA 0x50
#define TODAS_LAS_MEDIDAS 0x55
#define BORRAR_MEDIADAS 0X60
#define CICLO_MEDIDAS 0x70


typedef struct {                                // object data type
  char string[512];
} MSGQUEUE_COM_MOSI_t;

typedef struct {                                // object data type
  char string[512];
} MSGQUEUE_COM_MISO_t;

typedef struct {                                // object data type
  uint8_t dh;
	uint8_t uh;
	uint8_t dm;
	uint8_t um;
	uint8_t ds;
	uint8_t us;
} MSGQUEUE_COM_MISO_HORA_t;

extern osMessageQueueId_t mid_COM_MOSIQueue;                // message queue id
extern osMessageQueueId_t mid_COM_MISOQueue;                // message queue id
extern osMessageQueueId_t mid_COM_MISOHORAQueue;                // message queue id


int Init_Com (void);
int Init_Test_Com (void);
void poner_hora(uint8_t valor1,uint8_t valor2,uint8_t valor3,uint8_t valor4,uint8_t valor5,uint8_t valor6);

void trama_puestahora(uint8_t valor1,uint8_t valor2,uint8_t valor3 , uint8_t valor4 ,uint8_t valor5,uint8_t valor6,uint8_t valor7,uint8_t valor8);

void trama_cuenta_atras(uint8_t valor1);

void trama_resp_cuenta_atras(uint8_t valor1);

void trama_borrar_medidas(void);

void trama_total_medidas(uint8_t valor);

//void trama_ultima_medida(void);

void	crear_ciclo_medidas(uint8_t valor5,uint8_t valor6,uint8_t valor8,uint8_t valor9);

//void 	mostrar_todas_medidas(void);

void myUART_Init(void);
#endif
