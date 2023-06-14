#include "Driver_USART.h"
#include "cmsis_os2.h" 
#ifndef __COM_H

typedef struct {                                // object data type
  char string[50];
} MSGQUEUE_COM_t;

extern osMessageQueueId_t mid_COMQueue;                // message queue id

	int Init_Com (void);
#endif
