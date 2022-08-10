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

static void main_task(void *params){

	while(1){
		print_str("Main task loop executing\r\n");
		sprintf(main_string,"Main task iteration: 0x%08lx\r\n",main_counter++);
		print_str(main_string);
		vTaskDelay(1000/portTICK_RATE_MS);
	}
}

/******************************************************************************
Function that will be called as a task.
Depending on the parameter value, the task will accomplish different
functionality.
******************************************************************************/
static void PrintStringTask(void *params){
	int *type = (int *)params;
	print_str("This task is created by me\r\n");
	switch (*type){
	case 0:
		do{
			print_str("This task is created with ID 0\r\n");
			for (int i = 0; i<1000000;i++);
			//vTaskDelay(1000/portTICK_RATE_MS);
		}while(1);
	case 1:
		do{
			print_str("This task is created with ID 1\r\n");
			for (int i = 0; i<1000000;i++);
			//vTaskDelay(1000/portTICK_RATE_MS);
		}while(1);

	}
}

void main_user(){
	util_init();

	static int param1 = 0, param2 =1;

	xTaskCreate(PrintStringTask,"Task 1", configMINIMAL_STACK_SIZE + 100, &param1, tskIDLE_PRIORITY + 1, NULL);
	xTaskCreate(PrintStringTask,"Task 2", configMINIMAL_STACK_SIZE + 100, &param2, tskIDLE_PRIORITY + 1, NULL);
	vTaskStartScheduler();

	while(1);

}
