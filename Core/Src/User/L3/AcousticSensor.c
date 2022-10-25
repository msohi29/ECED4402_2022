/*
 * AcousticSensor.c
 *
 *  Created on: Oct. 21, 2022
 *      Author: Andre Hendricks / Dr. JF Bousquet
 */
#include <stdlib.h>

#include "User/L2/Comm_Datalink.h"
#include "User/L3/AcousticSensor.h"

//Required FreeRTOS header files
#include "FreeRTOS.h"
#include "Timers.h"


/******************************************************************************
This is a software callback function.
******************************************************************************/
void RunAcousticSensor(TimerHandle_t xTimer) //Default 1000 ms
{
	const uint16_t variance = 50;
	const uint16_t mean = 100;

	send_sensorData_message(Acoustic, (rand() % variance) + mean);
}
