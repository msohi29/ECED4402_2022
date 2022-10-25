/*
 * SensorController.h
 *
 *  Created on: Oct 24, 2022
 *      Author: kadh1
 */

#ifndef INC_USER_L4_SENSORCONTROLLER_H_
#define INC_USER_L4_SENSORCONTROLLER_H_


struct SensorStates{
	bool IsAcousticAck;
	bool IsDepthAck;
	uint16_t DepthData;
	uint16_t AcousticData;
};
void HostPC_RX_Task();
void SensorPlatform_RX_Task();
void SensorControllerTask(void *params);
#endif /* INC_USER_L4_SENSORCONTROLLER_H_ */
