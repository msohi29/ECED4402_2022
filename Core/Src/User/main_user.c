/*
 * main_user.c
 *
 *
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
#include "queue.h"

QueueHandle_t xQueue;
char receiver_string[] = "Received = 000\r\n";

/******************************************************************************
******************************************************************************/
static void vSenderTask( void *pvParameters )
{
	int32_t lValueToSend;
	BaseType_t xStatus;


	lValueToSend = ( int32_t ) pvParameters;

	for( ;; )
	{
		xStatus = xQueueSendToBack( xQueue, &lValueToSend, 0 );
		if( xStatus != pdPASS )
		{
			print_str( "Could not send to the queue.\r\n" );
		}
		vTaskDelay(50);
	}
}


/******************************************************************************
******************************************************************************/
static void vReceiverTask( void *pvParameters )
{
	int32_t lReceivedValue;
	BaseType_t xStatus;
	const TickType_t xTicksToWait = pdMS_TO_TICKS( 100 );

	for( ;; )
	{
		/* Verify if queue is empty. */
		if( uxQueueMessagesWaiting( xQueue ) != 0 )
		{
			print_str( "Queue is not empty!\r\n" );
		}
		/* Receive data from the queue. */
		xStatus = xQueueReceive( xQueue, &lReceivedValue, xTicksToWait );
		if( xStatus == pdPASS )
		{
			/* Data was successfully received from the queue, print out the received
			value. */
			sprintf(&receiver_string[11], "%ld", lReceivedValue);
			receiver_string[14] = '\r';
			receiver_string[15] = '\n';
			print_str(receiver_string);
		}
		else
		{
			// Data was not received from the queue even after waiting
			print_str( "Could not receive from the queue.\r\n" );
		}
	}
}


void main_user(){
	util_init();

	// The queue is created
	xQueue = xQueueCreate( 5, sizeof( int32_t ) );
	if( xQueue != NULL )
	{
		// sender tasks.
		xTaskCreate( vSenderTask, "Sender1", 1000, ( void * ) 100, 1, NULL );
		xTaskCreate( vSenderTask, "Sender2", 1000, ( void * ) 200, 1, NULL );

		// Receiver task
		xTaskCreate( vReceiverTask, "Receiver", 1000, NULL, 2, NULL );

		// Start the scheduler so the created tasks start executing.
		vTaskStartScheduler();
	}

	while(1);

}
