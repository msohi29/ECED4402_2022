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

char main_string[50];
uint32_t main_counter = 0;

char LED_string[50];
uint32_t LEDCount = 0;

static void main_task(void *params){

	while(1){
		print_str("Main task loop executing\r\n");
		sprintf(main_string,"Main task iteration: 0x%08lx\r\n",main_counter++);
		print_str(main_string);
		vTaskDelay(1000/portTICK_RATE_MS);
	}
}

static void task_led(void *params)
{
	do{
		//Toggle onboard LED
		HAL_GPIO_TogglePin(/*To be completed*/);

		print_str("LED executing\r\n");
		sprintf(LED_string,"LED task iteration: 0x%08lx\r\n",LEDCount++);
		print_str(LED_string);
		vTaskDelay(/*To be completed*/);
	}while(1);
}


void main_user(){
	util_init();

	xTaskCreate(main_task,"Main Task", configMINIMAL_STACK_SIZE + 100, NULL, tskIDLE_PRIORITY + 2, NULL);

	xTaskCreate(task_led,"LED task", configMINIMAL_STACK_SIZE + 100, NULL, tskIDLE_PRIORITY + 2, NULL);

	vTaskStartScheduler();

	while(1);

}
