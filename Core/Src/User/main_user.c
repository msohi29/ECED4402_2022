/*
 * main_user.c
 *
 *  Created on: Aug 8, 2022
 *      Author: Andre Hendricks
 */

#include <stdio.h>

//STM32 generated header files
#include "main.h"

//User generated header files
#include "User/main_user.h"
#include "User/util.h"
#include "User/L1/USART_Driver.h"
#include "User/L2/Comm_Datalink.h"
#include "User/L4/SensorPlatform.h"
#include "User/L4/SensorController.h"

//Required FreeRTOS header files
#include "FreeRTOS.h"
#include "task.h"

void main_user(){
	util_init();

	initialize_sensor_datalink();

	initialize_hostPC_datalink();

	xTaskCreate(HostPC_RX_Task,"HostPC_RX_Task", configMINIMAL_STACK_SIZE + 100, NULL, tskIDLE_PRIORITY + 2, NULL);

	xTaskCreate(SensorPlatform_RX_Task,"SensorPlatform_RX_Task", configMINIMAL_STACK_SIZE + 100, NULL, tskIDLE_PRIORITY + 2, NULL);

	xTaskCreate(SensorControllerTask,"Sensor_Controller_Task", configMINIMAL_STACK_SIZE + 100, NULL, tskIDLE_PRIORITY + 2, NULL);

	vTaskStartScheduler();

	while(1);

}
