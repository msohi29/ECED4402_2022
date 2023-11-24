/*
 * remoteSensingPlatform.c
 *
 *  Created on: Oct. 21, 2022
 *      Author: Andre Hendricks / Dr. JF Bousquet
 */
#include <stdio.h>
#include <User/L3/OxygenSensor.h>
#include <User/L3/pHSensor.h>
#include <User/L3/TempSensor.h>

#include "User/L2/Comm_Datalink.h"
#include "User/L4/SensorPlatform.h"
#include "User/util.h"

//Required FreeRTOS header files
#include "FreeRTOS.h"
#include "Timers.h"
#include "semphr.h"

static void ResetMessageStruct(struct CommMessage* currentRxMessage){

	static const struct CommMessage EmptyMessage = {0};
	*currentRxMessage = EmptyMessage;
}

/******************************************************************************
This task is created from the main.
It is responsible for managing the messages from the datalink.
It is also responsible for starting the timers for each sensor
******************************************************************************/
void SensorPlatformTask(void *params)
{
	const TickType_t TimerDefaultPeriod = 1000;
	TimerHandle_t TimerID_OxygenSensor,TimerID_pHSensor, TimerID_TempSensor;

	TimerID_pHSensor = xTimerCreate(
		"PH Sensor Task",
		TimerDefaultPeriod,		// Period: Needed to be changed based on parameter
		pdTRUE,		// Autoreload: Continue running till deleted or stopped
		(void*)0,
		RunpHSensor
		);

	TimerID_OxygenSensor = xTimerCreate(
		"Oxygen Sensor Task",
		TimerDefaultPeriod,		// Period: Needed to be changed based on parameter
		pdTRUE,		// Autoreload: Continue running till deleted or stopped
		(void*)1,
		RunOxygenSensor
		);

	TimerID_TempSensor = xTimerCreate(
		"Temp Sensor Task",
		TimerDefaultPeriod,		// Period: Needed to be changed based on parameter
		pdTRUE,		// Autoreload: Continue running till deleted or stopped
		(void*)2,
		RunTempSensor
		);

	request_sensor_read();  // requests a usart read (through the callback)

	struct CommMessage currentRxMessage = {0};

	do {

		parse_sensor_message(&currentRxMessage);

		if(currentRxMessage.IsMessageReady == true && currentRxMessage.IsCheckSumValid == true){

			switch(currentRxMessage.SensorID){
				case Controller:
					switch(currentRxMessage.messageId){
						case 0:
							xTimerStop(TimerID_pHSensor, portMAX_DELAY);
							xTimerStop(TimerID_OxygenSensor, portMAX_DELAY);
							xTimerStop(TimerID_TempSensor, portMAX_DELAY);
							send_ack_message(RemoteSensingPlatformReset);
							break;
						case 1: //Do Nothing
							break;
						case 3: //Do Nothing
							break;
						}
					break;
				case pH:
					switch(currentRxMessage.messageId){
						case 0:
							xTimerChangePeriod(TimerID_pHSensor, currentRxMessage.params, portMAX_DELAY);
							xTimerStart(TimerID_pHSensor, portMAX_DELAY);
							send_ack_message(pHSensorEnable);
							break;
						case 1: //Do Nothing
							break;
						case 3: //Do Nothing
							break;
					}
					break;
				case Oxygen:
					switch(currentRxMessage.messageId){
						case 0:
							xTimerChangePeriod(TimerID_OxygenSensor, currentRxMessage.params, portMAX_DELAY);
							xTimerStart(TimerID_OxygenSensor, portMAX_DELAY);
							send_ack_message(OxygenSensorEnable);
							break;
						case 1: //Do Nothing
							break;
						case 3: //Do Nothing
							break;
					}
					break;
				case Temperature:
					switch(currentRxMessage.messageId){
						case 0:
							xTimerChangePeriod(TimerID_TempSensor, currentRxMessage.params, portMAX_DELAY);
							xTimerStart(TimerID_TempSensor, portMAX_DELAY);
							send_ack_message(TemperatureSensorEnable);
							break;
						case 1: //Do Nothing
							break;
						case 3: //Do Nothing
							break;
					}
					break;
				default://Should not get here
					break;
			}
			ResetMessageStruct(&currentRxMessage);
		}
	} while(1);
}
