/*
 * DepthSensorController.c
 *
 *  Created on: Oct. 21, 2022
 *      Author: Andre Hendricks / Dr. JF Bousquet
 */
#include <stdlib.h>
#include <stdbool.h>
#include <User/L3/TempSensor.h>

#include "User/L2/Comm_Datalink.h"
#include "FreeRTOS.h"
#include "Timers.h"


bool TempUp = false;


/******************************************************************************
This is a software callback function.
******************************************************************************/
void RunTempSensor(TimerHandle_t xTimer)
{
	const uint16_t variance = 10;
	const uint16_t mean = 10;
	uint16_t temp = 0;
	uint16_t prev_temp = 0;

	// Calculate temp:
	temp = rand() % variance + mean;

	// Update Temp:
	TempUp = (temp > prev_temp);

	send_sensorData_message(Temperature, temp);
}
