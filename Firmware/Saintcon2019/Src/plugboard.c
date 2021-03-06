/*
 * plugboard.c
 *
 *  Created on: Aug 25, 2019
 *      Author: compukidmike
 */

#include "plugboard.h"
#include "main.h"
#include "gpio.h"

#define gpio_ishigh(pin)	(HAL_GPIO_ReadPin(pin##_GPIO_Port, pin##_Pin) == GPIO_PIN_SET)
#define gpio_ispressed(pin)	!(HAL_GPIO_ReadPin(pin##_GPIO_Port, pin##_Pin) == GPIO_PIN_SET)

uint16_t plugboardPins[26] = {PlugA_Pin, PlugB_Pin, PlugC_Pin, PlugD_Pin, PlugE_Pin,
		PlugF_Pin, PlugG_Pin, PlugH_Pin, PlugI_Pin, PlugJ_Pin, PlugK_Pin, PlugL_Pin,
		PlugM_Pin, PlugN_Pin, PlugO_Pin, PlugP_Pin, PlugQ_Pin, PlugR_Pin, PlugS_Pin,
		PlugT_Pin, PlugU_Pin, PlugV_Pin, PlugW_Pin, PlugX_Pin, PlugY_Pin, PlugZ_Pin };

GPIO_TypeDef * plugboardPorts[26] = {PlugA_GPIO_Port, PlugB_GPIO_Port, PlugC_GPIO_Port, PlugD_GPIO_Port, PlugE_GPIO_Port,
		PlugF_GPIO_Port, PlugG_GPIO_Port, PlugH_GPIO_Port, PlugI_GPIO_Port, PlugJ_GPIO_Port, PlugK_GPIO_Port, PlugL_GPIO_Port,
		PlugM_GPIO_Port, PlugN_GPIO_Port, PlugO_GPIO_Port, PlugP_GPIO_Port, PlugQ_GPIO_Port, PlugR_GPIO_Port, PlugS_GPIO_Port,
		PlugT_GPIO_Port, PlugU_GPIO_Port, PlugV_GPIO_Port, PlugW_GPIO_Port, PlugX_GPIO_Port, PlugY_GPIO_Port, PlugZ_GPIO_Port };

GPIO_InitTypeDef GPIO_InitStruct = {0};

extern char plugboard[26];

//Returns char array of alphabet with any connected letters swapped
void plugboardUpdate(){
	char plugboardSettings[26];

	//Reset plugboard array to A-Z
	for(int x=0; x<26; x++){
		plugboardSettings[x] = x+65;
	}

	//Set all plugboard pins to input with pullup
	for(int x=0; x<26; x++){
		GPIO_InitStruct.Pin = plugboardPins[x];
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(plugboardPorts[x], &GPIO_InitStruct);
	}

	//Check for plugboard connections and swap values in array
	for(int x=0; x<26; x++){
		GPIO_InitStruct.Pin = plugboardPins[x];
		GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_OD;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
		HAL_GPIO_Init(plugboardPorts[x], &GPIO_InitStruct);
		HAL_GPIO_WritePin(plugboardPorts[x], plugboardPins[x], GPIO_PIN_RESET); //Set pin LOW
		for(int y=0; y<26; y++){
			if(y != x){
				if(HAL_GPIO_ReadPin(plugboardPorts[y], plugboardPins[y]) == GPIO_PIN_RESET){
					plugboardSettings[x] = y+65;
					plugboardSettings[y] = x+65;
				}
			}
		}
		GPIO_InitStruct.Pin = plugboardPins[x];
		GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
		GPIO_InitStruct.Pull = GPIO_PULLUP;
		HAL_GPIO_Init(plugboardPorts[x], &GPIO_InitStruct);
	}

	for(int x=0; x<26; x++){
		plugboard[x] = plugboardSettings[x];
	}
}
