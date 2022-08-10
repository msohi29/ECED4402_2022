/*
 * main_user.c
 *
 *  Created on: Aug 8, 2022
 *      Author: Andre Hendricks
 */

#include <stdio.h>
#include <stdlib.h>

//STM32 generated header files
#include "main.h"

//User generated header files
#include "User/main_user.h"
#include "User/util.h"

//Required FreeRTOS header files
#include "FreeRTOS.h"
#include "task.h"
#include "event_groups.h"

char main_string[50];
uint32_t main_counter = 0;

int taskcount = 0;

EventGroupHandle_t xEventGroup;

static void PrintTaskID(void*params);

static void main_task(void *param)
{
	char ibuff[50];
	BaseType_t res = pdFAIL;
	TaskHandle_t xHandle[50];

	size_t val;
	do {
		print_str("Main task loop executing ");

		sprintf(main_string,"Main task iteration: 0x%08lx\r\n",main_counter++);
		print_str(main_string);

		// creation of tasks
		if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_10) == GPIO_PIN_RESET){  // create

			res = xTaskCreate(&PrintTaskID, (const char*)"subtask",
						configMINIMAL_STACK_SIZE+100,
						(void *) taskcount, tskIDLE_PRIORITY+2,
						&xHandle[taskcount]);

			if (res !=pdPASS){
				sprintf(ibuff,"Failed creating new task %d \r\n",taskcount);
				print_str(ibuff);
			}
			else{
				sprintf(ibuff,"Created a new task %d \r\n",taskcount++);
				print_str(ibuff);
			}

		} // deletion of tasks
		else if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_12) == GPIO_PIN_RESET && taskcount >0){ //delete
			taskcount--;

			vTaskDelete(xHandle[taskcount]);
			sprintf(ibuff,"Deleted task new task %d \r\n",taskcount);
			print_str(ibuff);
		}

		val = xPortGetFreeHeapSize() ;
		sprintf(ibuff,"Size = %d Bytes\r\n",val);
		print_str(ibuff);

		// at the end of the loop, in this task, we reinitialize.
		xEventGroupClearBits(xEventGroup, 0xFFFF ); // clears all bits
		xEventGroupSetBits( xEventGroup,  1<<0);  // this is the ID for task 0.

		vTaskDelay(1000 / portTICK_RATE_MS);

	} while(1);
}

/******************************************************************************
******************************************************************************/
static void PrintTaskID(void*params){
	char ibuff[50];
	int IDval = (int) params;

	do{
		// we wait for the bits associated to the current task id to be set.
		xEventGroupWaitBits( xEventGroup, 1<<IDval, pdTRUE, pdTRUE, portMAX_DELAY);

		sprintf(ibuff,"Task %d printing \r\n", IDval);
		print_str(ibuff);

		// After we print, we enable the printing of the next task id.
		xEventGroupSetBits( xEventGroup, 1<< (IDval+1));
		vTaskDelay(rand()%1000);

	}while(1);
}


void main_user(){
	util_init();

	/* Before an event group can be used it must first be created. */
	xEventGroup = xEventGroupCreate();

	xTaskCreate(main_task,"Main Task", configMINIMAL_STACK_SIZE + 100, NULL, tskIDLE_PRIORITY + 2, NULL);

	vTaskStartScheduler();

	while(1);

}
