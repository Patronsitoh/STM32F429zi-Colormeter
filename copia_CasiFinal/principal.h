#include "Driver_USART.h"
#include "cmsis_os2.h"
#include "stdio.h"
#include "string.h"
#include "stdlib.h"

#ifndef __PRINCIPAL_H
	int Init_Principal (void);
#endif

typedef struct {                                // object data type
char info[12];
} MSGQUEUE_BACK_t;


extern osMessageQueueId_t mid_BACKQueue;

void guardaMedidas(float r, float g, float b);
void newMedida(float r, float g, float b);
