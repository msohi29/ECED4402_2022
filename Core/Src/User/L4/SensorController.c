/*
 * SensorController.c
 *
 *  Created on: Oct 24, 2022
 *      Author: kadh1
 */


#include <stdio.h>

#include "main.h"
#include "User/L2/Comm_Datalink.h"
#include "User/L3/AcousticSensor.h"
#include "User/L3/DepthSensor.h"
#include "User/L4/SensorPlatform.h"
#include "User/L4/SensorController.h"
#include "User/util.h"

//Required FreeRTOS header files
#include "FreeRTOS.h"
#include "Timers.h"
#include "semphr.h"

QueueHandle_t Queue_Sensor_Data;
QueueHandle_t Queue_HostPC_Data;

int Sensors_Enable_Expired = 0;

TimerHandle_t xTimer;


enum states {Start_Sensors, Parse_Sensor_Data, Disable_Sensors, Wait_};

static void ResetMessageStruct(struct CommMessage* currentRxMessage){

	static const struct CommMessage EmptyMessage = {0};
	*currentRxMessage = EmptyMessage;
}

void CheckEnableSensor( TimerHandle_t xTimer )
{
	Sensors_Enable_Expired = 1;

}

/******************************************************************************
This task is created from the main.
******************************************************************************/
void SensorControllerTask(void *params)
{
	//char HostPCMessage[50];
	struct CommMessage currentRxMessage = {0};
	int Acoustic_enabled = 0, Depth_enabled = 0;

	enum states state = Wait_;

	xTimer = xTimerCreate("Timer1",5000,pdTRUE,( void * ) 0, CheckEnableSensor);

	char str[60];
	sprintf(str, "Waiting for input, State: %d\n", state);
	print_str(str);

	do {
		switch(state)
		{
		case Wait_:

			enum HostPCCommands HostPCCommand = PC_Command_NONE;

			sprintf(str, "Waiting for input, State: %d\r\n", state);
			print_str(str);

			if(xQueueReceive(Queue_HostPC_Data, &HostPCCommand, 0) == pdPASS && HostPCCommand == PC_Command_START)
			{
				state = Start_Sensors;
			}
			else {
				state = Wait_;
			}

			break;

		case Start_Sensors:

			send_sensorEnable_message(Acoustic, 5000);
			send_sensorEnable_message(Depth, 5000);

			// Start timer:
			xTimerStart(xTimer, 0);

			while(!Sensors_Enable_Expired) {
				if(xQueueReceive(Queue_Sensor_Data, &currentRxMessage, 0) == pdPASS) {
					if(currentRxMessage.messageId == 1) {
						if(currentRxMessage.SensorID == Acoustic) {
							Acoustic_enabled = 1;
						} else if (currentRxMessage.SensorID == Depth) {
							Depth_enabled = 1;
						}
					}
				}
				ResetMessageStruct(&currentRxMessage);
			}

			// Stop timer:
			xTimerStop(xTimer, 0);

			if(Acoustic_enabled && Depth_enabled) {
				state = Parse_Sensor_Data;
			} else {
				state = Start_Sensors;
				Sensors_Enable_Expired = 0;
			}
			break;

		case Parse_Sensor_Data:
		default:
			break;
		}

		vTaskDelay(1000 / portTICK_RATE_MS);
	} while(1);
}




/*
 * This task reads the queue of characters from the Sensor Platform when available
 * It then sends the processed data to the Sensor Controller Task
 */
void SensorPlatform_RX_Task(){
	struct CommMessage currentRxMessage = {0};
	Queue_Sensor_Data = xQueueCreate(80, sizeof(struct CommMessage));

	request_sensor_read();  // requests a usart read (through the callback)

	while(1){
		parse_sensor_message(&currentRxMessage);

		if(currentRxMessage.IsMessageReady == true && currentRxMessage.IsCheckSumValid == true){

			xQueueSendToBack(Queue_Sensor_Data, &currentRxMessage, 0);
			ResetMessageStruct(&currentRxMessage);
		}
	}
}



/*
 * This task reads the queue of characters from the Host PC when available
 * It then sends the processed data to the Sensor Controller Task
 */
void HostPC_RX_Task(){

	enum HostPCCommands HostPCCommand = PC_Command_NONE;

	Queue_HostPC_Data = xQueueCreate(80, sizeof(enum HostPCCommands));

	request_hostPC_read();

	while(1){
		HostPCCommand = parse_hostPC_message();

		if (HostPCCommand != PC_Command_NONE){
			xQueueSendToBack(Queue_HostPC_Data, &HostPCCommand, 0);
		}

	}
}
