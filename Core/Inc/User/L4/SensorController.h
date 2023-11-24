/*
 * SensorController.h
 *
 *  Created on: Oct 24, 2022
 *      Author: kadh1
 */

#ifndef INC_USER_L4_SENSORCONTROLLER_H_
#define INC_USER_L4_SENSORCONTROLLER_H_


struct SensorStates{
	bool IspHAck;
	bool IsOxygenAck;
	bool IsTempAck;
	uint16_t pHData;
	uint16_t OxygenData;
	uint16_t TempData;
};
void HostPC_RX_Task();
void SensorPlatform_RX_Task();
void SensorControllerTask(void *params);
#endif /* INC_USER_L4_SENSORCONTROLLER_H_ */
