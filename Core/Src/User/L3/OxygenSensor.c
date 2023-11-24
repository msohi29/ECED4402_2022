/*
 * DepthSensorController.c
 *
 *  Created on: Oct. 21, 2022
 *      Author: Andre Hendricks / Dr. JF Bousquet
 */
#include <stdlib.h>
#include <stdbool.h>
#include <User/L3/OxygenSensor.h>

#include "User/L2/Comm_Datalink.h"
#include "FreeRTOS.h"
#include "Timers.h"

extern bool TempUp;

/******************************************************************************
This is a software callback function.
******************************************************************************/
void RunOxygenSensor(TimerHandle_t xTimer)
{
	const uint16_t variance = 25;
	const uint16_t mean = 65;
	uint16_t o2 = 0;
	uint16_t o2_prev = 0;

	if(TempUp) {
		while((o2 = (rand() % variance) + mean) > o2_prev);
	} else {
		while((o2 = (rand() % variance) + mean) < o2_prev);
	}

	send_sensorData_message(Oxygen, o2);
}
