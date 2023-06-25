#include "rgb.h"

/*----------------------------------------------------------------------------
 *      Thread_RGB:
 *			Thread leyendo la cola de mensajes con la informacion que debe
 *			representarse en el RGB
 *---------------------------------------------------------------------------*/
  #define MSGQUEUE_OBJECTS 4                    // number of Message Queue Objects
 
MSGQUEUE_RGB_t rgb;
osMessageQueueId_t mid_RGBQueue;
osThreadId_t tid_RGB;


int Init_RGB(void){
    tid_RGB = osThreadNew(Thread_RGB, NULL, NULL);
    if(tid_RGB == NULL){
        return(-1);
		}
		
		mid_RGBQueue = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_RGB_t), NULL);
    if(mid_RGBQueue == NULL){
        return (-1); 
		}
    return(0);
}  

void Thread_RGB(void *argument) {
    static GPIO_InitTypeDef sgpio;
    static TIM_HandleTypeDef htim;
    static TIM_OC_InitTypeDef soc;
    
    Init_RGB();
    myPWM_Init(&sgpio, &htim, &soc);

    while(1){
        // Handle R, G, B
        if(osMessageQueueGet(mid_RGBQueue, &rgb, NULL, 0U) == osOK){
					osDelay(10U);
					  myPWM_New_Pulse_R(&htim, &soc, rgb, TIM_CHANNEL_2);//R
					  myPWM_New_Pulse_G(&htim, &soc, rgb, TIM_CHANNEL_1);//G
            myPWM_New_Pulse_B(&htim, &soc, rgb, TIM_CHANNEL_4);//B
				}
				osThreadYield();
			}
}

static void myPWM_New_Pulse_R(TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef *soc, MSGQUEUE_RGB_t rgb, uint32_t channel) {
    uint8_t newValor;
    newValor = round(rgb.pulse_r * RELAC_TCS_PULSE);

    HAL_TIM_PWM_Stop(htim, channel);
    HAL_TIM_PWM_DeInit(htim);

    soc->Pulse = (100 - newValor);
    HAL_TIM_PWM_Init(htim);
    HAL_TIM_PWM_ConfigChannel(htim, soc, channel);
    HAL_TIM_PWM_Start(htim, channel);
}

static void myPWM_New_Pulse_G(TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef *soc, MSGQUEUE_RGB_t rgb, uint32_t channel) {
    uint8_t newValor;
    newValor = round(rgb.pulse_g * RELAC_TCS_PULSE);

    HAL_TIM_PWM_Stop(htim, channel);
    HAL_TIM_PWM_DeInit(htim);

    soc->Pulse = (100 - newValor);
    HAL_TIM_PWM_Init(htim);
    HAL_TIM_PWM_ConfigChannel(htim, soc, channel);
    HAL_TIM_PWM_Start(htim, channel);
}

static void myPWM_New_Pulse_B(TIM_HandleTypeDef *htim, TIM_OC_InitTypeDef *soc, MSGQUEUE_RGB_t rgb, uint32_t channel) {
    uint8_t newValor;
    newValor = round(rgb.pulse_b * RELAC_TCS_PULSE);

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

//Codigo para testear el modulo

//osThreadId_t tid_RGB_test;
//int Init_Test_RGB(void){
//    tid_RGB_test = osThreadNew(Thread_RGB_test, NULL, NULL);
//    if(tid_RGB_test == NULL)
//        return(-1);
//    return(0);
//}

//void Thread_RGB_test(void* arg) {
//    MSGQUEUE_RGB_t rgb;
//    int brightness;
//    uint32_t tiempo = 10U;

//    Init_RGB();
//    
//    while(1) {
//			
////				//Test para probar un unico color
////				rgb.pulse_r = 29;
////				rgb.pulse_g = 14;
////				rgb.pulse_b = 35;
////				osMessageQueuePut(mid_RGBQueue, &rgb, 0U, 0U);
////				osDelay(tiempo);
//			
//				//Test para probar cada LED de manera independiente de 0 a 100        
//        // Test red LED
//        for(brightness = 0; brightness <= 100; brightness++){
//            rgb.pulse_r = brightness;
//            osMessageQueuePut(mid_RGBQueue, &rgb, 0U, 0U);
//            osDelay(tiempo);
//        }
//        for(brightness = 100; brightness >= 0; brightness--){
//            rgb.pulse_r = brightness;
//            osMessageQueuePut(mid_RGBQueue, &rgb, 0U, 0U);
//            osDelay(tiempo);
//        }
//        // Test green LED
//        for(brightness = 0; brightness <= 100; brightness++){
//            rgb.pulse_g = brightness;
//            osMessageQueuePut(mid_RGBQueue, &rgb, 0U, 0U);
//            osDelay(tiempo);
//        }
//        for(brightness = 100; brightness >= 0; brightness--){
//            rgb.pulse_g = brightness;
//            osMessageQueuePut(mid_RGBQueue, &rgb, 0U, 0U);
//            osDelay(tiempo);
//        }
//        // Test blue LED
//        for(brightness = 0; brightness <= 100; brightness++){
//            rgb.pulse_b = brightness;
//            osMessageQueuePut(mid_RGBQueue, &rgb, 0U, 0U);
//            osDelay(tiempo);
//        }
//        for(brightness = 100; brightness >= 0; brightness--){
//            rgb.pulse_b = brightness;
//            osMessageQueuePut(mid_RGBQueue, &rgb, 0U, 0U);
//            osDelay(tiempo);
//        }
//        // Test all LEDs at once
//        for(brightness = 0; brightness <= 100; brightness++){
//						rgb.pulse_r = brightness;
//						rgb.pulse_g = brightness;
//						rgb.pulse_b = brightness;
//            osMessageQueuePut(mid_RGBQueue, &rgb, 0U, 0U);

//            osDelay(tiempo);
//        }
//        for(brightness = 100; brightness >= 0; brightness--){
//						rgb.pulse_r = brightness;
//						rgb.pulse_g = brightness;
//						rgb.pulse_b = brightness;
//            osMessageQueuePut(mid_RGBQueue, &rgb, 0U, 0U);

//            osDelay(tiempo);
//        }
//				osThreadYield();
//    }
//}
