/*
 * main_user.c
 *
 *  Created on: Aug 8, 2022
 *      Author: Andre Hendricks
 */

#include <stdio.h>

//STM32 generated header files
#include "main.h"

//User generated header files
#include "User/main_user.h"
#include "User/util.h"

//Required FreeRTOS header files
#include "FreeRTOS.h"
#include "task.h"

#include "timers.h"

char main_string[50];
uint32_t main_counter = 0;

TimerHandle_t xTimers[2];
enum StopLightStates {STATE_GREEN,STATE_ORANGE,STATE_RED};

void StopLightController( TimerHandle_t xTimer )
{
    volatile static enum StopLightStates StopLightController_State = STATE_GREEN;

    uint32_t TimerID;
    TimerID = ( uint32_t ) pvTimerGetTimerID( xTimer );

    if (StopLightController_State == STATE_GREEN && TimerID == 0){
        //turn on green light
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_SET); //green
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET); //orange
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET); //red
        /*Fill: Turn on crossing light (white)*/
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_SET); // White

        StopLightController_State = STATE_ORANGE;

    }else if (StopLightController_State == STATE_ORANGE && TimerID == 0){

        /*Fill: Blink crossing light (white) by calling a 2nd software timer*/
		//turn on orange light
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET); //green
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_SET); //orange
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_RESET); //red
		StopLightController_State = STATE_RED;


    }else if (StopLightController_State == STATE_RED && TimerID == 0){
        //turn on red light
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_0, GPIO_PIN_RESET); //green
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_1, GPIO_PIN_RESET); //orange
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_2, GPIO_PIN_SET); //red
        /*Fill: Turn off crossing light (white) and stop the 2nd software timer*/
        HAL_GPIO_WritePin(GPIOC, GPIO_PIN_3, GPIO_PIN_RESET); // White
        StopLightController_State = STATE_GREEN;
    }

    if(TimerID == 1 && StopLightController_State == STATE_RED) {
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_3);

   }
}

static void main_task(void *param){

	while(1){
		print_str("Main task loop executing\r\n");
		sprintf(main_string,"Main task iteration: 0x%08lx\r\n",main_counter++);
		print_str(main_string);
		vTaskDelay(1000/portTICK_RATE_MS);
	}
}


void main_user(){
  util_init();

  xTimers[0] = xTimerCreate("Timer1",3000,pdTRUE,( void * ) 0,StopLightController);
  xTimers[1] = xTimerCreate("Timer2",100,pdTRUE,( void * ) 1,StopLightController);
  xTimerStart( xTimers[0], 0 );
  xTimerStart( xTimers[1], 0 );

  xTaskCreate(main_task,"Task 1", configMINIMAL_STACK_SIZE + 100, NULL, tskIDLE_PRIORITY + 2, NULL);
  vTaskStartScheduler();
  while(1);
}
