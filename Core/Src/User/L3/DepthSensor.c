/*
 * DepthSensorController.c
 *
 *  Created on: Oct. 21, 2022
 *      Author: Andre Hendricks / Dr. JF Bousquet
 */

#include <stdbool.h>

#include "User/L2/Comm_Datalink.h"
#include "User/L3/DepthSensor.h"

//Required FreeRTOS header files
#include "FreeRTOS.h"
#include "Timers.h"


/******************************************************************************
This is a software callback function.
******************************************************************************/
void RunDepthSensor(TimerHandle_t xTimer)
{
	static uint16_t depth = 10;
	static uint8_t down = true;
	if(down)
		depth += 10;
	else
		depth -=10;

	if(depth == 300)
		down = false;
	if(depth== 0)
		down = true;

	send_sensorData_message(Depth, depth);
}
