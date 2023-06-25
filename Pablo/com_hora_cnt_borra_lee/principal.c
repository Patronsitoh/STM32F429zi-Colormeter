#include "cmsis_os2.h"                          // CMSIS RTOS header file
#include "hora.h"
#include "Joystick.h"
#include "LCD.h"
#include "com.h"
#include "color.h"
#include "principal.h"
#include "rgb.h"

#define INACTIVO 0
#define ACTIVO 1
#define MEDIDA 2
#define AUTOMATICA 3

#define MSGQUEUE_OBJECTS 16                    // number of Message Queue Objects

int aux_r = 0;
int aux_g = 0;
int aux_b = 0;
uint8_t aux_accion = 0;

char _buffer[13];
uint8_t puntero = 0;
uint8_t countdown;
bool flag = false;

MSGQUEUE_BACK_t almacen[50];
osMessageQueueId_t mid_BACKQueue;

int cuentaAtras;
int medir = 0;
int _segTarget;
int targetTiempo = 5;


int numMedidasAutomaticas = 0; int medidasRestantes = 0;
int tiempoEntreMedidasAutomaticas = 0;

GPIO_InitTypeDef Leds;

void Init_Leds(void){
  ////LEDS///////
  __HAL_RCC_GPIOB_CLK_ENABLE(); //habilita alimentacion al canal B
  Leds.Mode = GPIO_MODE_OUTPUT_PP; //Resistencia modo push pull
  Leds.Pull = GPIO_PULLUP;
  Leds.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
  
  Leds.Pin = GPIO_PIN_0; //LED1
  HAL_GPIO_Init(GPIOB, &Leds);
	Leds.Pin = GPIO_PIN_7; //LED2
  HAL_GPIO_Init(GPIOB, &Leds);	
}
/*------------------------------------------------------------------------------
 *      Thread 1 'Thread_Principal': Módulo principal del sistema que se encarga 
 *      de coordinar todos los demás.
 *			Es el módulo que decide las acciones a tomar en función del estado del
 *			sistema y de la información que reciba del resto de los módulos
 *-----------------------------------------------------------------------------*/

osThreadId_t tid_Principal;                        // thread id

void Thread_Principal(void *argument);                   // thread function

int Init_Principal(void) {

    mid_BACKQueue = osMessageQueueNew(MSGQUEUE_OBJECTS, sizeof(MSGQUEUE_BACK_t), NULL);
    if (mid_BACKQueue == NULL) {
        return (-1);
    }
    tid_Principal = osThreadNew(Thread_Principal, NULL, NULL);
    if (tid_Principal == NULL) {
        return (-1);
    }
    Init_Hora();
    Init_Joystick();
    Init_LCD();
    Init_Com();
    Init_Color();
    Init_RGB();
		Init_Leds();
    return (0);
}

void Thread_Principal(void *argument) {
    osStatus_t status;
		osStatus_t accion;
    uint8_t modo = INACTIVO;
    MSGQUEUE_JOYSTICK_t joystick;
    MSGQUEUE_LCD_t lcd;
    MSGQUEUE_COLOR_t color;
    MSGQUEUE_COM_RX_t comrx;
		MSGQUEUE_COM_TX_t comtx;
    MSGQUEUE_RGB_t rgb;

    int aux_s = 60, aux_min = 60, aux_h = 24;
    int s = 0, m = 0, h = 0;
    uint8_t aux_joy = 0;

    while (1) {
        status = osMessageQueueGet(mid_JoystickQueue, &joystick, 0U, 0U);
        if (status == osOK) {
            aux_joy = joystick.valor;
        }
				
				accion = osMessageQueueGet(mid_COM_RXQueue, &comrx, 0U, 0U);
				if (accion == osOK && flag) {
						aux_accion = comrx.act;
				}
				
        switch (modo) {
            case INACTIVO: // Espera pulsacion UP LONG para pasar a activo
                if (aux_s != segundos || aux_min != minutos || aux_h != horas) {
                    aux_s = segundos;
                    aux_min = minutos;
                    aux_h = horas;

                    sprintf(lcd.text, "INACTIVO %02d:%02d:%02d", horas, minutos, segundos);
                    lcd.line = 1;
                    osMessageQueuePut(mid_LCDQueue, &lcd, 0U, 0U);
                }

                if (aux_joy == PULSE_DOWN) {
                    aux_joy = 0;
                    modo = ACTIVO;
										flag = true;
                }
                break;

            case ACTIVO: // Espera a trama para medir
                if (aux_s != segundos || aux_min != minutos || aux_h != horas) {
                    aux_s = segundos;
                    aux_min = minutos;
                    aux_h = horas;

                    sprintf(lcd.text, "ACTIVO %02d:%02d:%02d", horas, minutos, segundos);
                    lcd.line = 1;
                    osMessageQueuePut(mid_LCDQueue, &lcd, 0U, 0U);
                }
								HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0, GPIO_PIN_SET);//ENCIENDO
								osDelay(250);
                sprintf(lcd.text, "ACTIVO %02d:%02d:%02d", horas, minutos, segundos);
                lcd.line = 1;								
                osMessageQueuePut(mid_LCDQueue, &lcd, 0U, 0U);
								osDelay(250);
								HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0, GPIO_PIN_RESET);//APAGO
								sprintf(lcd.text, "ACTIVO %02d:%02d:%02d", horas, minutos, segundos);
                lcd.line = 1;
                osMessageQueuePut(mid_LCDQueue, &lcd, 0U, 0U);
								osDelay(250);
								sprintf(lcd.text, "ACTIVO %02d:%02d:%02d", horas, minutos, segundos);
                lcd.line = 1;
                osMessageQueuePut(mid_LCDQueue, &lcd, 0U, 0U);
								osDelay(250);
								sprintf(lcd.text, "ACTIVO %02d:%02d:%02d", horas, minutos, segundos);
                lcd.line = 1;
                osMessageQueuePut(mid_LCDQueue, &lcd, 0U, 0U);
								osDelay(250);	
                sprintf(lcd.text, "ACTIVO %02d:%02d:%02d", horas, minutos, segundos);
                lcd.line = 1;
                osMessageQueuePut(mid_LCDQueue, &lcd, 0U, 0U);								
								osDelay(250);
										
                if (aux_accion == PUESTA_HORA) {
                    aux_accion = 0;
                    modo = MEDIDA;
										horas = comrx.dh;
										minutos = comrx.dm;
										segundos = comrx.ds;
										sprintf(comtx.string, "\r\n01 DF 0C \"%d:%d:%d\" FE", comrx.dh, comrx.dm, comrx.ds);
										osMessageQueuePut(mid_COM_TXQueue, &comtx, 0U, 0U);
										aux_joy = 0;
                }
								
								if(aux_accion == CUENTA_ATRAS){
									aux_accion = 0;
									targetTiempo = comrx.cnt;
									sprintf(comtx.string, "\r\n01 DA 05 \"%d\" FE", comrx.cnt);
									osMessageQueuePut(mid_COM_TXQueue, &comtx, 0U, 0U);
								}
								
								if(aux_accion == LEER_CUENTA_ATRAS){
									aux_accion = 0;
									sprintf(comtx.string, "\r\n01 CA 05 \"%d\" FE", targetTiempo);
									osMessageQueuePut(mid_COM_TXQueue, &comtx, 0U, 0U);
								}
								
								if(aux_accion == N_MEDIDAS_ALMACENADAS){
									aux_accion = 0;
									sprintf(comtx.string, "\r\n01 BF 06 \"%d\" FE", puntero);
									osMessageQueuePut(mid_COM_TXQueue, &comtx, 0U, 0U);
								}
								
								if(aux_accion == BORRAR_MEDIDAS){
									aux_accion = 0;
									memset(almacen, 0, sizeof(almacen));
									puntero = 0;
									sprintf(comtx.string, "\r\n01 9F 04 FE");
									osMessageQueuePut(mid_COM_TXQueue, &comtx, 0U, 0U);
								}						
																
								if (aux_joy == PULSE_UP_LONG) { //Vuelta a inactivo
										aux_joy = 0;
										aux_accion = 0;
										flag = false;
										modo = INACTIVO;  
                }
								
								if(aux_accion == CICLO_MEDIDAS){
										aux_accion = 0;
									
										numMedidasAutomaticas	= comrx.cicloMedidas;
										tiempoEntreMedidasAutomaticas = comrx.tiempoMedidas;						
									
										modo = AUTOMATICA;	
								}	
								
								
								
                break;
								

            case MEDIDA: // Recoge una muestra de RGB con pulsacion DOWN para replicar en el RGB
                if (aux_s != segundos || aux_min != minutos || aux_h != horas) {
                    aux_s = segundos;
                    aux_min = minutos;
                    aux_h = horas;
                    sprintf(lcd.text, "MEDIDA %02d:%02d:%02d", horas, minutos, segundos);
                    lcd.line = 1;
                    osMessageQueuePut(mid_LCDQueue, &lcd, 0U, 0U);
                }
							  if (aux_joy == PULSE_DOWN) { //MEDIDA MANUAL
										aux_joy = 0;
										for(int i = targetTiempo; i > 0; i--){ //Cuenta atrás en modo manual
											if (aux_s != segundos || aux_min != minutos || aux_h != horas) {
												aux_s = segundos;
												aux_min = minutos;
												aux_h = horas;
												sprintf(lcd.text, "MEDIDA %02d:%02d:%02d", horas, minutos, segundos);
												lcd.line = 1;
												osMessageQueuePut(mid_LCDQueue, &lcd, 0U, 0U);
											}
											sprintf(lcd.text, "Cuenta: %2d",i);
                      lcd.line = 2;
                      osMessageQueuePut(mid_LCDQueue, &lcd, 0U, 0U);
											osDelay(1000);
										}
										HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7, GPIO_PIN_SET);
										osDelay(350);
										HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7, GPIO_PIN_RESET);	
											
                    osThreadFlagsSet(tid_COLOR, 0x10); // Mando señal a colorimetro para que me devuelva valores RGB entre 0 y 255
                    if (osMessageQueueGet(mid_COLORQueue, &color, 0U, osWaitForever) == osOK) {
                        guardaMedidas(color.red, color.green, color.blue);

                        rgb.pulse_r = aux_r;
                        rgb.pulse_g = aux_g;
                        rgb.pulse_b = aux_b;

                        sprintf(lcd.text, "   R:%02d G:%02d B:%02d", aux_r, aux_g, aux_b);
                        lcd.line = 2;
                        osMessageQueuePut(mid_LCDQueue, &lcd, 0U, 0U);
                        osMessageQueuePut(mid_RGBQueue, &rgb, 0U, 0U);
                        aux_joy = 0;
                    }
                }
								
								if (aux_joy == PULSE_UP_LONG) { //Vuelta a inactivo
                    aux_joy = 0;
                    rgb.pulse_r = 0;
                    rgb.pulse_g = 0;
                    rgb.pulse_b = 0;
                    osMessageQueuePut(mid_RGBQueue, &rgb, 0U, 0U);
                    sprintf(lcd.text, "");
                    lcd.line = 2;
                    osMessageQueuePut(mid_LCDQueue, &lcd, 0U, 0U);
										flag = false;
										modo = INACTIVO;
                }
								break;
								
								
							
								
								
								// si recibo trama correspondiente ir a automatica
                
								/*
								tramasDesdePC = escucharTramas();
								
								if(colaCOM == cicloMedidas){
									modo = AUTOMATICA;
								}elseif(Puesta en hora){
									
								}elseif(Establecer cuentaAtras){
								targetTiempo = COMcolaRecibida.cuentaAtras;
								
								
								}elseif(leerCuentaAtrás){
									COMcolaEnviar.cuentaAtras = targetTiempo;	
								
								}elseif(numeroMedidasalmacenas){
									COMcolaEnviar.Medidasalmacenadas = puntero;
															
								
								}elseif(ultimaMedia){
									COMcolaEnviar.Medidasalmacenadas = almacen[puntero];
								
								}elseif(todasLasMedidas){
									putamierda
								
								elseif(borrarMedidas){
									 memset(almacen, 0, sizeof(alamacen));
								
								}
								
								
								
								
								*/

            case AUTOMATICA://Ciclo de medidas si recibe trama correspondiente
							medidasRestantes = numMedidasAutomaticas;
							
								for(int n = numMedidasAutomaticas; n > 0; n--){
																
										for(int t = tiempoEntreMedidasAutomaticas; t > 0; t--){ ////Cuenta atrás en modo manual
										
											if (aux_s != segundos || aux_min != minutos || aux_h != horas) {
													aux_s = segundos;
													aux_min = minutos;
													aux_h = horas;
													sprintf(lcd.text, "MEDIDA %02d:%02d:%02d  C ", horas, minutos, segundos);
													lcd.line = 1;
													osMessageQueuePut(mid_LCDQueue, &lcd, 0U, 0U);
												}	
													
												sprintf(lcd.text, "Next: %02d",t);
												lcd.line = 2;
												osMessageQueuePut(mid_LCDQueue, &lcd, 0U, 0U);
												osDelay(1000);
										}
										
										 medidasRestantes--;
										 HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7, GPIO_PIN_SET);
										 osDelay(350);
										 HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7, GPIO_PIN_RESET);	
												
										 osThreadFlagsSet(tid_COLOR, 0x10); // Mando señal a colorimetro para que me devuelva valores RGB entre 0 y 255
										 if (osMessageQueueGet(mid_COLORQueue, &color, 0U, osWaitForever) == osOK) {
													guardaMedidas(color.red, color.green, color.blue);

													rgb.pulse_r = aux_r;
													rgb.pulse_g = aux_g;
													rgb.pulse_b = aux_b;

													sprintf(lcd.text, "   R:%02d G:%02d B:%02d", rgb.pulse_r, rgb.pulse_g, rgb.pulse_b);
													lcd.line = 2;
													osMessageQueuePut(mid_LCDQueue, &lcd, 0U, 0U);
													osMessageQueuePut(mid_RGBQueue, &rgb, 0U, 0U);                        
										 }
									}
									
									sprintf(lcd.text, " ");
									lcd.line = 2;
									osMessageQueuePut(mid_LCDQueue, &lcd, 0U, 0U);
									modo = ACTIVO;
								 
						            
							break;
        }
        osThreadYield();
    }
}

/**
    @brief Almacena el valor recibido y lo añade a la cola de 50 ultimas medidas sitauada en RAM-
            Setea el valor RGB entre 0-100% y añade el timeStamp a cada medida.
            -> timeStamp = hhmmssrrggbb
*/
void guardaMedidas(float r, float g, float b) {
    aux_r = (int)((r / 255.0) * 99);
    aux_g = (int)((g / 255.0) * 99);
    aux_b = (int)((b / 255.0) * 99);

    if (aux_r < 0) {
        aux_r = 0; // Establecer en 0 si es menor que 0
    } else if (aux_r > 99) {
        aux_r = 99; // Establecer en 100 si es mayor que 100
    }

    if (aux_g < 0) {
        aux_g = 0; // Establecer en 0 si es menor que 0
    } else if (aux_g > 99) {
        aux_g = 99; // Establecer en 100 si es mayor que 100
    }

    if (aux_b < 0) {
        aux_b = 0; // Establecer en 0 si es menor que 0
    } else if (aux_b > 99) {
        aux_b = 99; // Establecer en 100 si es mayor que 100
    }

    sprintf(_buffer, "%02u%02u%02u%02u%02u%02u", horas, minutos, segundos, aux_r, aux_g, aux_b);

    strcpy(almacen[puntero].info, _buffer);
    if (puntero < 49) {
        puntero++;
    } else {
        puntero = 0;
    }
}
