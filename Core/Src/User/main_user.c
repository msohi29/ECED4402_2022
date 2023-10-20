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
#include "semphr.h"

char main_string[50];
uint32_t main_counter = 0;

static SemaphoreHandle_t binary_semaphore;

/******************************************************************************
This is a task handler that prints periodically.
No changes are required.
******************************************************************************/
static void main_task(void *param){

	while(1){
		print_str("Main task loop executing\r\n");
		sprintf(main_string,"Main task iteration: 0x%08lx\r\n",main_counter++);
		print_str(main_string);
		vTaskDelay(1000/portTICK_RATE_MS);
	}
}

/******************************************************************************
This task handler needs to be completed.
It should toggle the LED on a rising edge.  Use the call to
"HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_A);
And send a message to the console of the host computer to indicate
that a new loop of the task_led has been run.
The message should show the number of times it has been run.
This task handler should not use any delays
******************************************************************************/
static void task_led(void *params)
{
	uint32_t LEDCount = 0;
	char ibuff[50];

	do {

		xSemaphoreTake(binary_semaphore, portMAX_DELAY);
		sprintf(ibuff, "LEDCount: 0x%08lx\r\n", ++LEDCount);
		print_str(ibuff);
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
//		xSemaphoreGive(binary_semaphore);
//		vTaskDelay(1000);
	}while(1);
}


/******************************************************************************
This is the callback function for onboard blue button.
To be completed.
******************************************************************************/
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){

	BaseType_t xHigherPriorityTaskWoken = pdFALSE;
	if(GPIO_Pin == GPIO_PIN_13){
//		print_str_ISR("Blue Button Pressed\r\n");
//		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		xSemaphoreGiveFromISR(binary_semaphore, &xHigherPriorityTaskWoken );
//		xSemaphoreGive(binary_semaphore);
		print_str_ISR("Blue Button Pressed\r\n");
	}
}

/******************************************************************************
main to be completed.
Make sure that the buttons and interrupts are configured.
Create a binary semaphore after initialization.
Additionally to the main_task, if the semaphore is created, create a second
task with the handler task_led.
Don't forget to start the scheduler!
******************************************************************************/
void main_user(){
	util_init();

	binary_semaphore = xSemaphoreCreateBinary();

	xTaskCreate(main_task,"Main Task", configMINIMAL_STACK_SIZE + 100, NULL, tskIDLE_PRIORITY + 2, NULL);
	xTaskCreate(task_led, "LED Task", configMINIMAL_STACK_SIZE + 100, NULL, tskIDLE_PRIORITY + 2, NULL);
	vTaskStartScheduler();

	while(1);

}
