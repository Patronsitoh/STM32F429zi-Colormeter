#ifndef __RGB_H
#define __RGB_H

#include "cmsis_os2.h"
#include "stm32f4xx_hal.h"
#include "math.h"
#define RELAC_TCS_PULSE 1

typedef struct {
	uint8_t pulse_r;
	uint8_t pulse_g;
	uint8_t pulse_b;
} MSGQUEUE_RGB_t;

extern osMessageQueueId_t mid_RGBQueue;

int Init_RGB(void);
int Init_Test_RGB(void);
osMessageQueueId_t get_id_MsgQueue_rgb(void);

void Thread_RGB(void* argument);
void Thread_RGB_test(void* argument);
static void myPWM_Init(GPIO_InitTypeDef *sgpio, TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef *soc);
static void myPWM_New_Pulse_R(TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef *soc, MSGQUEUE_RGB_t rgb, uint32_t channel);
static void myPWM_New_Pulse_G(TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef *soc, MSGQUEUE_RGB_t rgb, uint32_t channel);
static void myPWM_New_Pulse_B(TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef *soc, MSGQUEUE_RGB_t rgb, uint32_t channel);

#endif
