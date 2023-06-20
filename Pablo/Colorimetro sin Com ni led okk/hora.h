#ifndef __HORA_H
#define __HORA_H

#include "cmsis_os2.h"                          // CMSIS RTOS header file

extern int segundos; 
extern int	minutos; 
extern int horas;


int Init_Hora (void);
int Init_Test_Hora (void);

#endif /* __HORA_H */
