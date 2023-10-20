/*
 * main_user.c
 *
 *  Created on: Aug 8, 2022
 *      Author: Andre Hendricks
 *      Modified by: Jacob MacDonal, Manav Sohi
 *      ID: B00834513, B00844935
 *      Date: October 13th, 2023
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

void PrintTaskID(void * params) {
	char buff[50];

	/* Get the TaskID from vTaskCreate() */
	int id = *(int*)params;

	/* Variable to store the ticks the task has been running for */
	TickType_t ticks = 0;

	while(1) {
		sprintf(buff, "Task %d is printing\r\n", id);
		print_str(buff);

		/* Get the number of ticks the task has been running for */
		ticks = xTaskGetTickCount();

		/* delete the task if it has been running for more than 20 seconds or 20,000 ticks */
		if(ticks >= 20000 / portTICK_RATE_MS) {
			sprintf(buff, "Deleted Task %d\r\n", id);
			print_str(buff);

			/* When NULL is passed to vTaskDelete(), it deletes the Task that called it*/
			vTaskDelete(NULL);
		}

		/* Pauses for a period of one second */
		vTaskDelay(1000 / portTICK_RATE_MS);
	}
}

uint16_t Button;

static void main_task(void *params)
{
	char ibuff[50];
	TaskHandle_t xHandle[50];
	/*Variable used to keep track of sub tasks created with button*/
	int TaskID = 0;
	size_t val;

	do {
		print_str_ISR("Main task loop executing ");

		// Update hexadecimal 32-bit integer in string, and print it
		sprintf(main_string,"Main task iteration: 0x%08x\r\n",(unsigned int)main_counter++);
		print_str_ISR(main_string);

		//Create new task if button A was pressed
		if(Button == GPIO_PIN_10){

			if (xTaskCreate(&PrintTaskID,(const char*)"subtask",configMINIMAL_STACK_SIZE+100, (void *)&TaskID ,tskIDLE_PRIORITY+2,&xHandle[TaskID])!=pdPASS){
				sprintf(ibuff,"Failed creating new task %d \r\n", TaskID);
				print_str_ISR(ibuff);
			}else{
				sprintf(ibuff,"Created a new task %d \r\n", TaskID);
				print_str_ISR(ibuff);
				TaskID++;
			}

			/* Reset global variable button */
			Button = 0;
		}

		val = xPortGetFreeHeapSize() ;
		sprintf(ibuff,"Size = %d Bytes\r\n",val);
		print_str_ISR(ibuff);
		vTaskDelay(1000 / portTICK_RATE_MS);

	} while(1);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == GPIO_PIN_10){
		print_str_ISR("Button A Pressed\r\n");

		/* Update global variable button, which is used by main_task to check for button push */
		Button = GPIO_Pin;
	}
	else if(GPIO_Pin == GPIO_PIN_12){
		print_str_ISR("Button B Pressed\r\n");

		/* Update global variable button, which is used by main_task to check for button push */
		Button = GPIO_Pin;
	}
}

void main_user(){
	util_init();

	xTaskCreate(main_task,"Main Task", configMINIMAL_STACK_SIZE + 100, NULL, tskIDLE_PRIORITY + 2, NULL);

	vTaskStartScheduler();

	while(1);

}
