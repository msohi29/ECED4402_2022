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

int Sensors_Expired = 0;

TimerHandle_t xTimer;


enum states {Start_Sensors, Parse_Sensor_Data, Disable_Sensors, Wait_};
char states_str[3][6] = {"EMPTY", "START", "RESET"};

static void ResetMessageStruct(struct CommMessage* currentRxMessage){

	static const struct CommMessage EmptyMessage = {0};
	*currentRxMessage = EmptyMessage;
}

void CheckEnableSensor( TimerHandle_t xTimer )
{
	Sensors_Expired = 1;

}

/******************************************************************************
This task is created from the main.
******************************************************************************/
void SensorControllerTask(void *params)
{
	//char HostPCMessage[50];
	struct CommMessage currentRxMessage = {0};
	int Acoustic_enabled = 0, Depth_enabled = 0, Disabled = 0;

	enum states state = Wait_;

	xTimer = xTimerCreate("Timer1",5000,pdTRUE,( void * ) 0, CheckEnableSensor);

	char str[60];

	do {
		switch(state)
		{
		case Wait_:

			enum HostPCCommands HostPCCommand = PC_Command_NONE;

			sprintf(str, "<<<<<<<<<<<<< Waiting for input from PC >>>>>>>>>>>>>\r\n");
			print_str(str);

			if(xQueueReceive(Queue_HostPC_Data, &HostPCCommand, 0) == pdPASS)
			{
				sprintf(str, "Command from PC: %s\r\n", states_str[HostPCCommand]);
				print_str(str);
				if(HostPCCommand == PC_Command_START) {
					state = Start_Sensors;
					Sensors_Expired = 0;
				}
			}
			else {
				state = Wait_;
				Sensors_Expired = 0;
			}

			break;

		case Start_Sensors:

			sprintf(str, "<<<<<<<<<<<<<<<<< Starting Sensors >>>>>>>>>>>>>>\r\n");
			print_str(str);

			send_sensorEnable_message(Acoustic, 1000);
			send_sensorEnable_message(Depth, 5000);

			// Start timer:
			xTimerStart(xTimer, 0);

			while(!Sensors_Expired) {
				if(xQueueReceive(Queue_Sensor_Data, &currentRxMessage, 0) == pdPASS) {
					if(currentRxMessage.messageId == 1) { // 1-> Ack Message
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
				Sensors_Expired = 0;
			} else {
				state = Start_Sensors;
				Sensors_Expired = 0;
			}
			break;

		case Parse_Sensor_Data:


			// Parse and display sensor data:
			sprintf(str, "<<<<<<<<<<<<< Parsing Sensor Data >>>>>>>>>>>>>\r\n");
			print_str(str);

			// FILL THIS TO DISPLAY SENSOR DATA

			// Start timer:
			xTimerStart(xTimer, 0);

			while(!Sensors_Expired) {
				if(xQueueReceive(Queue_Sensor_Data, &currentRxMessage, 0) == pdPASS) {
					if(currentRxMessage.messageId == 3) { // 3 -> data message
						if(currentRxMessage.SensorID == Acoustic) {
							sprintf(str, "Acoustic Sensor Data: %d\r\n", currentRxMessage.params);
						} else if (currentRxMessage.SensorID == Depth) {
							sprintf(str, "Depth Sensor Data: %d\r\n", currentRxMessage.params);
						}
					}
				}
				ResetMessageStruct(&currentRxMessage);

			}

			// Stop timer:
			xTimerStop(xTimer, 0);

			print_str(str);


			// Check for RESET from computer:
			sprintf(str, "<<<<<<<<<<<<< Checking for RESET from PC >>>>>>>>>>>>>\r\n");
			print_str(str);

			if(xQueueReceive(Queue_HostPC_Data, &HostPCCommand, 0) == pdPASS)
			{
				sprintf(str, "Command from PC: %s\r\n", states_str[HostPCCommand]);
				print_str(str);
				if(HostPCCommand == PC_Command_RESET) {
					state = Disable_Sensors;
					Sensors_Expired = 0;
				}
			}
			else {
				state = Parse_Sensor_Data;
				Sensors_Expired = 0;
			}

			break;

		case Disable_Sensors:

			// Disabling sensors:
			sprintf(str, "<<<<<<<<<<<<< Disabling sensors >>>>>>>>>>>>>\r\n");
			print_str(str);

			send_sensorReset_message();

			// Start timer:
			xTimerStart(xTimer, 0);

			while(!Sensors_Expired) {
				if(xQueueReceive(Queue_Sensor_Data, &currentRxMessage, 0) == pdPASS) {
					if(currentRxMessage.messageId == 1 && currentRxMessage.SensorID == Controller) { // 1 -> Ack message
						Disabled = 1;
					}
				}
				ResetMessageStruct(&currentRxMessage);
			}

			// Stop timer:
			xTimerStop(xTimer, 0);

			if(Disabled) {
				state = Wait_;
				Sensors_Expired = 0;
			} else {
				state = Disable_Sensors;
				Sensors_Expired = 0;
			}

			break;

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
