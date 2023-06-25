#ifndef __JOYSTICK_H
#define __JOYSTICK_H

#include "cmsis_os2.h"                          // CMSIS RTOS header file

#define PULSE_UP 1
#define PULSE_RIGHT 2
#define PULSE_DOWN 3
#define PULSE_LEFT 4
#define PULSE_CENTER 5
#define PULSE_UP_LONG 6


extern osThreadId_t tid_Joystick; 
extern osMessageQueueId_t mid_JoystickQueue;

typedef struct {                               
  uint8_t valor;
} MSGQUEUE_JOYSTICK_t;

int Init_Joystick (void);
int Init_Test_Joystick (void);

#endif /* __JOYSTICK_H */

