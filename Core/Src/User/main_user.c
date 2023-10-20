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

uint32_t SharedCounter = 0;

static SemaphoreHandle_t mutex;


/******************************************************************************
******************************************************************************/
static void vSenderTask( void *pvParameters )
{
	uint32_t buffer =0;
	char ibuff[50];


	for( ;; )
	{
		/* The following set of operations is updating the SharedCounter
		 after a long delay. */

		xSemaphoreTake(mutex, portMAX_DELAY);
		buffer = SharedCounter;
		buffer++;
		vTaskDelay(1000);


		SharedCounter = buffer;



		// then, sending it to the USART.
		sprintf(ibuff,"%ld\r\n",SharedCounter);
		print_str(ibuff);

		xSemaphoreGive(mutex);
	}
}


void main_user(){
	util_init();

	mutex = xSemaphoreCreateMutex();

	// this task will increment by 1.
	xTaskCreate( vSenderTask, "Sender1", 1000, ( void * ) 1, 1, NULL );

	// so will this one.
	xTaskCreate( vSenderTask, "Sender2", 1000, ( void * ) 1, 1, NULL );

	vTaskStartScheduler();

	while(1); // we should never reach here.

}
