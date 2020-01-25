/*
 * testmode.c
 *
 *  Created on: Oct 12, 2019
 *      Author: compukidmike
 */


#include "testmode.h"
#include "eeprom_emul.h"
#include "keyboard.h"
#include "lampboard.h"
#include "matrix.h"
#include "stdlib.h"

extern volatile uint8_t nextFrame;
extern uint16_t VirtAddVarTab[];

void TestMode(){

	lampboard_clear();
	for(int x=0; x<26; x++){
		lampboard_setLamp(x, COLOR_WHITE); //Turn on all lamps
	}
	lampboard_update();

	matrix_clear();
	matrix_fillRect(0, 0, 64, 16, COLOR_WHITE); //Turn on all LEDs
	matrix_update();

	uint8_t loop = 1;
	uint32_t buttonStatus = 0x3FFFFFF; //26 lowest bits set to 1
	while (loop){
		if(nextFrame == 1){ //Set by Timer16 - 30FPS
			nextFrame = 0;
			//Button Test Code
			uint32_t buttons = 0;
			buttons = keyboardUpdate();
			for(int x=0; x<26; x++){
				if(buttons>>x & 1){
					lampboard_setLamp(x, COLOR_BLACK);
					lampboard_update();
					buttonStatus &= ~(1<<x);
				}
			}

			if(buttonStatus == 0){ //All buttons have been pressed
				EE_WriteVariable8bits(VirtAddVarTab[EEP_CHALLENGE_LEVEL], 0);
				uint32_t seed;
				seed = HAL_GetTick(); //Time since boot
				srand(seed);
				for(int x=0; x<6; x++){
					EE_WriteVariable8bits(VirtAddVarTab[EEP_MESSAGE_CODE_1 + x], rand() % 26);
					EE_WriteVariable8bits(VirtAddVarTab[EEP_MESSAGE_CODE_2 + x], rand() % 26);
					EE_WriteVariable8bits(VirtAddVarTab[EEP_MESSAGE_CODE_3 + x], rand() % 26);
					EE_WriteVariable8bits(VirtAddVarTab[EEP_MESSAGE_CODE_4 + x], rand() % 26);
					EE_WriteVariable8bits(VirtAddVarTab[EEP_MESSAGE_CODE_5 + x], rand() % 26);
					EE_WriteVariable8bits(VirtAddVarTab[EEP_MESSAGE_CODE_6 + x], rand() % 26);
					EE_WriteVariable8bits(VirtAddVarTab[EEP_MESSAGE_CODE_7 + x], rand() % 26);
					EE_WriteVariable8bits(VirtAddVarTab[EEP_MESSAGE_CODE_8 + x], rand() % 26);
					EE_WriteVariable8bits(VirtAddVarTab[EEP_MESSAGE_CODE_9 + x], rand() % 26);
				}
				matrix_clear();
				matrix_drawCharCentered("TEST DONE", 0, 0, COLOR_GREEN);
				matrix_update();
				HAL_Delay(1000);
			}
		}
	}
}
