/*
 * Comm_Datalink.c
 *
 *  Created on: Oct. 21, 2022
 *      Author: Andre Hendricks / Dr. JF Bousquet
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "User/L1/USART_Driver.h"
#include "User/L2/Comm_Datalink.h"
#include "User/util.h"

enum ParseMessageState_t {Waiting_S, SensorID_S, MessageID_S, ParamsID_S, Star_S, CS_S};

static void sendStringSensor(char* tx_string);

/******************************************************************************
******************************************************************************/
void initialize_sensor_datalink(void)
{
	configure_usart_extern();
}

void initialize_hostPC_datalink(void){
	configure_usart_hostPC();
}

/******************************************************************************
this function calculates the checksum and sends a given input string to the uart.
******************************************************************************/
static void sendStringSensor(char* tx_string)
{
	uint8_t checksum;
	uint16_t str_length;

	// Check the length of the command
	str_length = strlen((char *)tx_string)-1;

	// Compute the checksum
	checksum = tx_string[0];
	for (int idx = 1; idx < str_length-2; idx++) {
		checksum ^= tx_string[idx];
	}

	sprintf(&tx_string[str_length-2], "%02x\n", checksum);
	printStr_extern(tx_string);
}


/******************************************************************************
This function parses the queue to identify received messages.
******************************************************************************/
void parse_sensor_message(struct CommMessage* currentRxMessage)
{
	static enum ParseMessageState_t currentState = Waiting_S;
	uint8_t CurrentChar;
	static uint16_t sensorIdIdx = 0, MessageIdIdx = 0, ParamIdx = 0, checksumIdx = 0;
	static char sensorId[6],CSStr[3];
	static uint8_t checksum_val;
	static const struct CommMessage EmptyMessage = {0};

	while(xQueueReceive(Queue_extern_UART, &CurrentChar, portMAX_DELAY) == pdPASS && currentRxMessage->IsMessageReady == false)  // as long as there are characters in the queue.
	{
		if (CurrentChar == '$'){ //Reset State Machine
			checksum_val = CurrentChar;
			sensorIdIdx = 0;
			MessageIdIdx = 0;
			ParamIdx = 0;
			checksumIdx = 0;
			currentState = SensorID_S;
			*currentRxMessage = EmptyMessage;
			continue;
		}

		// TO DO: we must calculate the received checksum!
		switch (currentState)
		{
			case Waiting_S: // Do nothing
				break;
			case SensorID_S: //Get Sensor ID Code
				checksum_val ^= CurrentChar;
				if(CurrentChar == ','){
					currentState = MessageID_S;
					break;
				}
				else if (sensorIdIdx < 5){
					sensorId[sensorIdIdx++] = CurrentChar;
				}
				if(sensorIdIdx == 5){
					//Add NULL Terminator
					sensorId[sensorIdIdx] = '\0';
					if(strcmp(sensorId, "CNTRL") == 0)//Sensor ID: Controller
						currentRxMessage->SensorID = Controller;
					else if(strcmp(sensorId, "ACSTC") == 0)//Sensor ID: Acoustic
						currentRxMessage->SensorID = Acoustic;
					else if(strcmp(sensorId, "DEPTH") == 0)//Sensor ID: Depth
						currentRxMessage->SensorID = Depth;
					else{//Sensor ID: None
						currentRxMessage->SensorID = None;
						currentState = Waiting_S;
					}
				}
				break;

			case MessageID_S: //Get Message Type
				checksum_val ^= CurrentChar;
				if(CurrentChar == ','){
					currentState = ParamsID_S;
				}
				else{
					if(MessageIdIdx < 2){
						currentRxMessage->messageId = currentRxMessage->messageId * 10;
						currentRxMessage->messageId += CurrentChar -  '0';
					}
					MessageIdIdx++;
				}
				break;

			case ParamsID_S: //Get Message Parameter (Period/Data)
				checksum_val ^= CurrentChar;

				if(CurrentChar == ','){
					currentState = Star_S;
				}
				else if(ParamIdx < 8){
					currentRxMessage->params = currentRxMessage->params * 10;
					currentRxMessage->params += CurrentChar -  '0';
				}
				break;

			case Star_S:
				checksum_val ^= CurrentChar;
				if(CurrentChar == ','){
					currentState = CS_S;
				}
				break;

			case CS_S:
				if(checksumIdx < 2){
					CSStr[checksumIdx++] = CurrentChar;
				}
				if(checksumIdx == 2){
					currentState = Waiting_S;
					CSStr[checksumIdx] = '\0';
					currentRxMessage->checksum = strtol(CSStr, NULL, 16);
					if(currentRxMessage->checksum == checksum_val){
						currentRxMessage->IsMessageReady = true;
						currentRxMessage->IsCheckSumValid = true;
					}else{
						currentRxMessage->IsCheckSumValid = false;
					}
				}
					break;
			}
		}
}



enum HostPCCommands parse_hostPC_message(){

	uint8_t CurrentChar;
	static char HostPCMessage[10];
	static uint16_t HostPCMessage_IDX = 0;


	while (xQueueReceive(Queue_hostPC_UART, &CurrentChar, portMAX_DELAY) == pdPASS){
		if(CurrentChar == '\n' || CurrentChar == '\r'|| HostPCMessage_IDX >=6){
			HostPCMessage[HostPCMessage_IDX++] = '\0';
			HostPCMessage_IDX = 0;
			if(strcmp(HostPCMessage, "START") == 0)
				return PC_Command_START;
			else if(strcmp(HostPCMessage, "RESET") == 0)
				return PC_Command_RESET;
		}else{
			HostPCMessage[HostPCMessage_IDX++] = CurrentChar;
		}

	}
	return PC_Command_NONE;
}


void send_sensorData_message(enum SensorId_t sensorType, uint16_t data){
	char tx_sensor_buffer[50];

	switch(sensorType){
	case Acoustic:
		sprintf(tx_sensor_buffer, "$ACSTC,03,%08u,*,00\n", data);
		break;
	case Depth:
		sprintf(tx_sensor_buffer, "$DEPTH,03,%08u,*,00\n", data);
		break;
	default:
		break;
	}
	sendStringSensor(tx_sensor_buffer);
}

void send_sensorEnable_message(enum SensorId_t sensorType, uint16_t TimePeriod_ms){
	char tx_sensor_buffer[50];

	switch(sensorType){
	case Acoustic:
		sprintf(tx_sensor_buffer, "$ACSTC,00,%08u,*,00\n", TimePeriod_ms);
		break;
	case Depth:
		sprintf(tx_sensor_buffer, "$DEPTH,00,%08u,*,00\n", TimePeriod_ms);
		break;
	default:
		break;
	}
	sendStringSensor(tx_sensor_buffer);
}

void send_sensorReset_message(void){
	char tx_sensor_buffer[50];

	sprintf(tx_sensor_buffer, "$CNTRL,00,,*,00\n");

	sendStringSensor(tx_sensor_buffer);
}

void send_ack_message(enum AckTypes AckType){
	char tx_sensor_buffer[50];

	switch(AckType){
	case RemoteSensingPlatformReset:
		sprintf(tx_sensor_buffer, "$CNTRL,01,,*,00\n");
		break;
	case AcousticSensorEnable:
		sprintf(tx_sensor_buffer, "$ACSTC,01,,*,00\n");
		break;
	case DepthSensorEnable:
		sprintf(tx_sensor_buffer, "$DEPTH,01,,*,00\n");
		break;
	}

	sendStringSensor(tx_sensor_buffer);
}
