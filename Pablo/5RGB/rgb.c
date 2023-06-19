#include "rgb.h"
#include "stm32f4xx_hal.h"
#include "math.h"

#define RELAC_VOL_PULSE 6.6f

static osThreadId_t id_Th_rgb;
static osMessageQueueId_t id_MsgQueue_red, id_MsgQueue_green, id_MsgQueue_blue;
static osThreadId_t id_Th_rgb_test;

void Th_rgb(void* argument);
void Th_rgb_test(void* argument);
static void myPWM_Init(GPIO_InitTypeDef *sgpio, TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef *soc);
static void myPWM_New_Pulse(TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef *soc, MSGQUEUE_OBJ_RGB msg, uint32_t channel);

uint8_t cnt = 0;
uint8_t brightness;

osMessageQueueId_t get_id_MsgQueue_red(void){
    return id_MsgQueue_red;
}

osMessageQueueId_t get_id_MsgQueue_green(void){
    return id_MsgQueue_green;
}

osMessageQueueId_t get_id_MsgQueue_blue(void){
    return id_MsgQueue_blue;
}

static int Init_MsgQueue_red(void){
    id_MsgQueue_red = osMessageQueueNew(MSGQUEUE_OBJECTS_RGB, sizeof(MSGQUEUE_OBJ_RGB), NULL);
    if(id_MsgQueue_red == NULL)
        return (-1); 
    return(0);
}

static int Init_MsgQueue_green(void){
    id_MsgQueue_green = osMessageQueueNew(MSGQUEUE_OBJECTS_RGB, sizeof(MSGQUEUE_OBJ_RGB), NULL);
    if(id_MsgQueue_green == NULL)
        return (-1); 
    return(0);
}

static int Init_MsgQueue_blue(void){
    id_MsgQueue_blue = osMessageQueueNew(MSGQUEUE_OBJECTS_RGB, sizeof(MSGQUEUE_OBJ_RGB), NULL);
    if(id_MsgQueue_blue == NULL)
        return (-1); 
    return(0);
}

int Init_Th_rgb(void){
  id_Th_rgb = osThreadNew(Th_rgb, NULL, NULL);
  if(id_Th_rgb == NULL)
    return(-1);
  return(0);
}  

void Th_rgb(void *argument) {
    static MSGQUEUE_OBJ_RGB msg;
    static GPIO_InitTypeDef sgpio;
    static TIM_HandleTypeDef htim;
    static TIM_OC_InitTypeDef soc;
    
    Init_MsgQueue_red();
    Init_MsgQueue_green();
    Init_MsgQueue_blue();
    myPWM_Init(&sgpio, &htim, &soc);

    while(1){
        // Handle red
        if(osMessageQueueGet(id_MsgQueue_red, &msg, NULL, 0U) == osOK)
            myPWM_New_Pulse(&htim, &soc, msg, TIM_CHANNEL_2);
        // Handle green
        if(osMessageQueueGet(id_MsgQueue_green, &msg, NULL, 0U) == osOK)
            myPWM_New_Pulse(&htim, &soc, msg, TIM_CHANNEL_1);
        // Handle blue
        if(osMessageQueueGet(id_MsgQueue_blue, &msg, NULL, 0U) == osOK)
            myPWM_New_Pulse(&htim, &soc, msg, TIM_CHANNEL_4);
    }
}

static void myPWM_New_Pulse(TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef *soc, MSGQUEUE_OBJ_RGB msg, uint32_t channel) {
    uint8_t newValor;

    newValor = round(msg.pulse * RELAC_VOL_PULSE);

    HAL_TIM_PWM_Stop(htim, channel);
    HAL_TIM_PWM_DeInit(htim);

    soc->Pulse = (100 - newValor);
    HAL_TIM_PWM_Init(htim);
    HAL_TIM_PWM_ConfigChannel(htim, soc, channel);
    HAL_TIM_PWM_Start(htim, channel);
}

static void myPWM_Init(GPIO_InitTypeDef *sgpio, TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef *soc) {
    /* TIM4 CH1 (Green LED) - PD12, CH2 (Red LED) - PD13, CH4 (Blue LED) - PD15*/
    __HAL_RCC_GPIOD_CLK_ENABLE();

    sgpio->Mode = GPIO_MODE_AF_PP;
    sgpio->Pull = GPIO_NOPULL;
    sgpio->Speed = GPIO_SPEED_FREQ_VERY_HIGH;
    
    /* Configure PD12 (GREEN), PD13 (RED), PD15 (BLUE) */
    sgpio->Pin = GPIO_PIN_12 | GPIO_PIN_13 | GPIO_PIN_15;
		//sgpio->Pin = GPIO_PIN_15;
    sgpio->Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init(GPIOD, sgpio);

    __HAL_RCC_TIM4_CLK_ENABLE();
    htim->Instance = TIM4;
    htim->Init.Prescaler = 839;
    htim->Init.Period = 99;    // 1 kHz blinking
    htim->Init.CounterMode = TIM_COUNTERMODE_UP;
    htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(htim);

    soc->OCMode = TIM_OCMODE_PWM1;
    soc->Pulse = 49;
    soc->OCPolarity = TIM_OCPOLARITY_HIGH;
    soc->OCFastMode = TIM_OCFAST_DISABLE;
    
    HAL_TIM_PWM_ConfigChannel(htim, soc, TIM_CHANNEL_1); // Green
    HAL_TIM_PWM_ConfigChannel(htim, soc, TIM_CHANNEL_2); // Red
    HAL_TIM_PWM_ConfigChannel(htim, soc, TIM_CHANNEL_4); // Blue
}

int Init_Th_rgb_test(void){
	id_Th_rgb_test = osThreadNew(Th_rgb_test, NULL, NULL);
	if(id_Th_rgb_test == NULL)
		return(-1);
	return(0);
}

void Th_rgb_test(void* arg) {
    MSGQUEUE_OBJ_RGB msg;

    Init_Th_rgb();

    while(1) {
        osDelay(50U);

        if(cnt < 15)
            cnt++;
        else
            cnt = 0;

        
				msg.pulse = cnt;   // Assigns pulse for color

        osMessageQueuePut(get_id_MsgQueue_red(), &msg, 0U, 0U);
				
				msg.pulse = 5;   // Assigns pulse for color
        osMessageQueuePut(get_id_MsgQueue_green(), &msg, 0U, 0U);
				
				msg.pulse = 2;   // Assigns pulse for color
        osMessageQueuePut(get_id_MsgQueue_blue(), &msg, 0U, 0U);
    }
}
