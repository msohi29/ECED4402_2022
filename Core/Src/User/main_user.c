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

static void main_task(void *params)
{
	char ibuff[50];
	TaskHandle_t xHandle[50];
	/*Variable used to keep track of sub tasks created with button*/

	size_t val;

	do {
		print_str("Main task loop executing ");

		// Update hexadecimal 32-bit integer in string, and print it
		sprintf(main_string,"Main task iteration: 0x%08x\r\n",(unsigned int)main_counter++);
		print_str(main_string);

		//Create new task if button A was pressed
		if( /*Check that button A was pressed*/){
			if (xTaskCreate(&PrintTaskID,(const char*)"subtask",configMINIMAL_STACK_SIZE+100,/*TaskID Parameter*/,tskIDLE_PRIORITY+2,&xHandle[/*fill this*/])!=pdPASS){
				sprintf(ibuff,"Failed creating new task %d \r\n",/*fill this*/);
				print_str(ibuff);
			}else{
				sprintf(ibuff,"Created a new task %d \r\n",/*fill this*/);
				print_str(ibuff);
				/*Increment taskid counter*/
			}
			//Delete new task if button B was pressed
		}else if( /*Check that button B was pressed*/ && /*atleast 1 subtask has been created*/){
			/*Decrement taskid counter*/
			vTaskDelete(xHandle[/*fill this*/]);
			sprintf(ibuff,"Deleted task new task %d \r\n",/*fill this*/);
			print_str(ibuff);
		}
		val = xPortGetFreeHeapSize() ;
		sprintf(ibuff,"Size = %d Bytes\r\n",val);
		print_str(ibuff);
		vTaskDelay(1000 / portTICK_RATE_MS);
	} while(1);
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	if(GPIO_Pin == GPIO_PIN_13){
		print_str_ISR("Blue Button Pressed\r\n");
		HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	}
}

void main_user(){
	util_init();

	xTaskCreate(main_task,"Main Task", configMINIMAL_STACK_SIZE + 100, NULL, tskIDLE_PRIORITY + 2, NULL);

	vTaskStartScheduler();

	while(1);

}
