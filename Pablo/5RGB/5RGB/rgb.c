#include "rgb.h"
#include "stm32f4xx_hal.h"
#include "math.h"

#define RELAC_VOL_PULSE 6.6f

static osThreadId_t id_Th_rgb;
static osMessageQueueId_t id_MsgQueue_red, id_MsgQueue_green, id_MsgQueue_blue;
static osThreadId_t id_Th_rgb_test;

void Th_rgb(void* argument);
void Th_rgb_test(void* argument);
static void myPWM_Init(GPIO_InitTypeDef *sgpio, TIM_HandleTypeDef *htim_red, TIM_HandleTypeDef *htim_green, TIM_HandleTypeDef *htim_blue, TIM_OC_InitTypeDef *soc);
static void myPWM_New_Pulse(TIM_HandleTypeDef htim, TIM_OC_InitTypeDef soc, MSGQUEUE_OBJ_RGB msg);

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

// Add other init functions and helper functions here...

int Init_Th_rgb(void){
  id_Th_rgb = osThreadNew(Th_rgb, NULL, NULL);
  if(id_Th_rgb == NULL)
    return(-1);
}  



void Th_rgb(void *argument) {
    static MSGQUEUE_OBJ_RGB msg;
    static GPIO_InitTypeDef sgpio;
    static TIM_HandleTypeDef htim_red, htim_green, htim_blue;
    static TIM_OC_InitTypeDef soc_red, soc_green, soc_blue;
    
    Init_MsgQueue_red();
    Init_MsgQueue_green();
    Init_MsgQueue_blue();
    myPWM_Init(&sgpio, &htim_red, &htim_green, &htim_blue, &soc_red);

    
    while(1){
        // Handle red
        if(osMessageQueueGet(id_MsgQueue_red, &msg, NULL, 0U) == osOK)
            myPWM_New_Pulse(htim_red, soc_red, msg);
        // Handle green
        if(osMessageQueueGet(id_MsgQueue_green, &msg, NULL, 0U) == osOK)
            myPWM_New_Pulse(htim_green, soc_green, msg);
        // Handle blue
        if(osMessageQueueGet(id_MsgQueue_blue, &msg, NULL, 0U) == osOK)
            myPWM_New_Pulse(htim_blue, soc_blue, msg);
    }
}

static void myPWM_New_Pulse(TIM_HandleTypeDef htim, TIM_OC_InitTypeDef soc, MSGQUEUE_OBJ_RGB msg) {
    static uint8_t newValor;

    newValor = round(msg.pulse * RELAC_VOL_PULSE);

    HAL_TIM_PWM_Stop(&htim, TIM_CHANNEL_1);
    HAL_TIM_PWM_DeInit(&htim);

    soc.Pulse = (100 - newValor);
    HAL_TIM_PWM_Init(&htim);
    HAL_TIM_PWM_ConfigChannel(&htim, &soc, TIM_CHANNEL_1);
    HAL_TIM_PWM_Start(&htim, TIM_CHANNEL_1);
}



static void myPWM_Init(GPIO_InitTypeDef *sgpio, TIM_HandleTypeDef *htim_red, TIM_HandleTypeDef *htim_green, TIM_HandleTypeDef *htim_blue, TIM_OC_InitTypeDef *soc) {
    /* TIM4 CH1 (Red LED) - PD12*/
    __HAL_RCC_GPIOD_CLK_ENABLE();

    sgpio->Mode = GPIO_MODE_AF_PP;
    sgpio->Pull = GPIO_NOPULL;
    sgpio->Speed = GPIO_SPEED_FREQ_VERY_HIGH;

    /* Configure PD12 (RED) */
    sgpio->Pin = GPIO_PIN_12;
    sgpio->Alternate = GPIO_AF2_TIM4;
    HAL_GPIO_Init(GPIOD, sgpio);

    __HAL_RCC_TIM4_CLK_ENABLE();
    htim_red->Instance = TIM4;
    htim_red->Init.Prescaler = 839;
    htim_red->Init.Period = 99;    // 1 kHz blinking
    htim_red->Init.CounterMode = TIM_COUNTERMODE_UP;
    htim_red->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(htim_red);

    soc->OCMode = TIM_OCMODE_PWM1;
    soc->Pulse = 49;
    soc->OCPolarity = TIM_OCPOLARITY_HIGH;
    soc->OCFastMode = TIM_OCFAST_DISABLE;

    /* TIM2 CH1 (Green LED) - PD13 */
    sgpio->Pin = GPIO_PIN_13;
    sgpio->Alternate = GPIO_AF1_TIM2;
    HAL_GPIO_Init(GPIOD, sgpio);

    __HAL_RCC_TIM2_CLK_ENABLE();
    htim_green->Instance = TIM2;
    htim_green->Init.Prescaler = 839;
    htim_green->Init.Period = 99;
    htim_green->Init.CounterMode = TIM_COUNTERMODE_UP;
    htim_green->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(htim_green);

    /* TIM3 CH1 (Blue LED) - PD11 */
    sgpio->Pin = GPIO_PIN_11;
    sgpio->Alternate = GPIO_AF2_TIM3;
    HAL_GPIO_Init(GPIOD, sgpio);

    __HAL_RCC_TIM3_CLK_ENABLE();
    htim_blue->Instance = TIM3;
    htim_blue->Init.Prescaler = 839;
    htim_blue->Init.Period = 99;
    htim_blue->Init.CounterMode = TIM_COUNTERMODE_UP;
    htim_blue->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    HAL_TIM_PWM_Init(htim_blue);
}





/*TEST*/
int Init_Th_rgb_test(void){
	id_Th_rgb_test = osThreadNew(Th_rgb_test, NULL, NULL);
	if(id_Th_rgb_test != osOK)
		return(-1);
	return(0);
}

void Th_rgb_test(void* arg) {
    MSGQUEUE_OBJ_RGB msg;
    
		
		uint32_t tiempo = 50;
	
    Init_Th_rgb();

    while(1) {
        // Test red LED
        for(brightness = 0; brightness <= 100; brightness++){
            msg.pulse = brightness;
            osMessageQueuePut(id_MsgQueue_red, &msg, 0U, 0U);
            osDelay(tiempo);
        }
        for(brightness = 100; brightness > 0; brightness--){
            msg.pulse = brightness;
            osMessageQueuePut(id_MsgQueue_red, &msg, 0U, 0U);
            osDelay(tiempo);
        }

        // Test green LED
        for(brightness = 0; brightness <= 100; brightness++){
            msg.pulse = brightness;
            osMessageQueuePut(id_MsgQueue_green, &msg, 0U, 0U);
            osDelay(tiempo);
        }
        for(brightness = 100; brightness > 0; brightness--){
            msg.pulse = brightness;
            osMessageQueuePut(id_MsgQueue_green, &msg, 0U, 0U);
            osDelay(tiempo);
        }

        // Test blue LED
        for(brightness = 0; brightness <= 100; brightness++){
            msg.pulse = brightness;
            osMessageQueuePut(id_MsgQueue_blue, &msg, 0U, 0U);
            osDelay(tiempo);
        }
        for(brightness = 100; brightness > 0; brightness--){
            msg.pulse = brightness;
            osMessageQueuePut(id_MsgQueue_blue, &msg, 0U, 0U);
            osDelay(tiempo);
        }

        // Test all LEDs at once
        for(brightness = 0; brightness <= 100; brightness++){
            msg.pulse = brightness;
            osMessageQueuePut(id_MsgQueue_red, &msg, 0U, 0U);
            osMessageQueuePut(id_MsgQueue_green, &msg, 0U, 0U);
            osMessageQueuePut(id_MsgQueue_blue, &msg, 0U, 0U);
            osDelay(tiempo);
        }
        for(brightness = 100; brightness > 0; brightness--){
            msg.pulse = brightness;
            osMessageQueuePut(id_MsgQueue_red, &msg, 0U, 0U);
            osMessageQueuePut(id_MsgQueue_green, &msg, 0U, 0U);
            osMessageQueuePut(id_MsgQueue_blue, &msg, 0U, 0U);
            osDelay(tiempo);
        }
    }
}








