/*
 * AcousticSensor.c
 *
 *  Created on: Oct. 21, 2022
 *      Author: Andre Hendricks / Dr. JF Bousquet
 */
#include <stdlib.h>
#include <User/L3/pHSensor.h>

#include "User/L2/Comm_Datalink.h"
#include "FreeRTOS.h"
#include "Timers.h"


/******************************************************************************
This is a software callback function.
******************************************************************************/
void RunpHSensor(TimerHandle_t xTimer) //Default 1000 ms
{
	const uint16_t variance = 25;
	const uint16_t mean = 65;

	send_sensorData_message(pH, (rand() % variance) + mean);
}
