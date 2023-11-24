/*
 * DepthSensorController.c
 *
 *  Created on: Oct. 21, 2022
 *      Author: Andre Hendricks / Dr. JF Bousquet
 */

#include <stdbool.h>
#include <User/L3/OxygenSensor.h>

#include "User/L2/Comm_Datalink.h"
#include "FreeRTOS.h"
#include "Timers.h"


/******************************************************************************
This is a software callback function.
******************************************************************************/
void RunDepthSensor(TimerHandle_t xTimer)
{
	const uint16_t variance = 25;
	const uint16_t mean = 65;
	uint16_t o2 = 0;



	send_sensorData_message(Oxygen, o2);
}
