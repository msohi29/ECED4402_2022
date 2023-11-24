#ifndef INC_USER_L3_TEMPSENSOR_H_
#define INC_USER_L3_TEMPSENSOR_H_

#include "FreeRTOS.h"
#include "timers.h"

void RunTempSensor(TimerHandle_t xTimer);

#endif /* INC_USER_L3_TEMPSENSOR_H_ */
