/*
 * DepthSensor.h
 *
 *  Created on: Oct 22, 2022
 *      Author: kadh1
 */

#ifndef INC_USER_L3_DEPTHSENSOR_H_
#define INC_USER_L3_DEPTHSENSOR_H_

#include "FreeRTOS.h"
#include "timers.h"

void RunDepthSensor(TimerHandle_t xTimer);

#endif /* INC_USER_L3_DEPTHSENSOR_H_ */
