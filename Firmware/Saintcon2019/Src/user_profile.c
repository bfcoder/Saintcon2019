/*
 * user_profile.c
 *
 *  Created on: Oct 13, 2019
 *      Author: bashNinja
 */

#include "keyboard.h"
#include "lampboard.h"
#include "matrix.h"
#include "string.h"
#include "eeprom_emul.h"
#include "user_profile.h"

const uint8_t envelope[294] = {
    0x05, 0x9c, 0x15, 0x05, 0x9c, 0x15, 0x05, 0x9c, 0x15, 0x05, 0x9c, 0x15, 0x05, 0x9c, 0x15, 0x05, 0x9c, 0x15, 0x05, 0x9c, 0x15, 0x05, 0x9c, 0x15, 0x05, 0x9c, 0x15, 0x05, 0x9c, 0x15, 0x05, 0x9c, 0x15, 0x05, 0x9c, 0x15, 0x05, 0x9c, 0x15, 0x05, 0x9c, 0x15,
    0x05, 0x9c, 0x15, 0x00, 0x00, 0x00, 0x05, 0x9c, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x9c, 0x15, 0x00, 0x00, 0x00, 0x05, 0x9c, 0x15,
    0x05, 0x9c, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x9c, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x9c, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x9c, 0x15,
    0x05, 0x9c, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x9c, 0x15, 0x05, 0x9c, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x9c, 0x15, 0x05, 0x9c, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x9c, 0x15,
    0x05, 0x9c, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x9c, 0x15, 0x05, 0x9c, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x9c, 0x15,
    0x05, 0x9c, 0x15, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0x9c, 0x15,
    0x05, 0x9c, 0x15, 0x05, 0x9c, 0x15, 0x05, 0x9c, 0x15, 0x05, 0x9c, 0x15, 0x05, 0x9c, 0x15, 0x05, 0x9c, 0x15, 0x05, 0x9c, 0x15, 0x05, 0x9c, 0x15, 0x05, 0x9c, 0x15, 0x05, 0x9c, 0x15, 0x05, 0x9c, 0x15, 0x05, 0x9c, 0x15, 0x05, 0x9c, 0x15, 0x05, 0x9c, 0x15
};



#define NUM_PROFILE_MENU_STATES 2

extern volatile uint8_t nextFrame;
const char PROFILE_LVL_TEXT[] = "Lvl:";
const char PROFILE_EXIT_TEXT[] = "EXIT";
const char PROFILE_NO_TEXT[] = "No";
const char PROFILE_MESSAGES_TEXT[] = "Messages";

extern const char INITIAL_RING[];
extern const uint8_t INITIAL_RING_LENGTH;

extern const char RING1_MESSAGE[];
extern const char RING2_MESSAGE[];
extern const char RING3_MESSAGE[];
extern const uint8_t RING1_MESSAGE_LENGTH;
extern const uint8_t RING2_MESSAGE_LENGTH;
extern const uint8_t RING3_MESSAGE_LENGTH;

int test;

void user_profile(){
	lampboard_clear();
	lampboard_setLetter('W', COLOR_RED);
	lampboard_setLetter('A', COLOR_RED);
	lampboard_setLetter('S', COLOR_RED);
	lampboard_setLetter('D', COLOR_RED);
	lampboard_setLetter('K', COLOR_RED);
	lampboard_update();
	uint8_t loop = 1;

	// get level code
	uint8_t eeprom_level = 0;
	uint8_t eeprom_message_start=0;

	uint8_t availableMessages = 0;

	EE_ReadVariable8bits(EEP_CHALLENGE_LEVEL, &eeprom_level);
/*
	if(EE_ReadVariable8bits(EEP_LEVEL_1_MESSAGE_START, &eeprom_message_start) == EE_OK){
		availableMessages |= 1 << 0;
	}
	if(EE_ReadVariable8bits(EEP_LEVEL_2_MESSAGE_START, &eeprom_message_start) == EE_OK){
		availableMessages |= 1 << 1;
	}
	if(EE_ReadVariable8bits(EEP_LEVEL_3_MESSAGE_START, &eeprom_message_start) == EE_OK){
		availableMessages |= 1 << 2;
	}
	if(EE_ReadVariable8bits(EEP_FINAL_MESSAGE_START, &eeprom_message_start) == EE_OK){
		availableMessages |= 1 << 3;
	}
	*/
	if(eeprom_level > 0) availableMessages |= 1 << 0;
	if(eeprom_level > 4) availableMessages |= 1 << 1;
	if(eeprom_level > 8) availableMessages |= 1 << 2;
	if(eeprom_level > 12) availableMessages |= 1 << 3;

	// debug
	//availableMessages |= 1 << 0;
	//availableMessages |= 1 << 1;
	//availableMessages |= 1 << 2;
	//availableMessages |= 1 << 3;

	int profileMenuSelected = 0;
	/*
	 * Menu States:
	 * 0-Machine Type (3 or 4)
	 * 1-Set Rotors
	 * 2-Ring Setting
	 * 3-Initial Ring
	 */

	while (loop){
		  if(nextFrame == 1){ //Set by Timer16 - 30FPS
			  nextFrame = 0;
			  UpdateMinibadges();
			  //Button Test Code
			  uint32_t buttons = 0;
			  buttons = keyboardUpdate();
			  if(availableMessages != 0){
				  if(buttons>>22 & 1){ //W
					  if(profileMenuSelected == 0){
						  for (int i = 0; i < 4; i++){
							  if(availableMessages>>i & 1){ // if message 1-4 exists
								  profileMenuSelected = i + 1;
							  }
						  }
					  }
				  }
				  if(buttons>>0 & 1){ //A
					  if(profileMenuSelected > 1){
						  if(availableMessages>>(profileMenuSelected-2) & 1)
							  profileMenuSelected -=1;
						  else if (availableMessages>>(profileMenuSelected-3) & 1)
							  profileMenuSelected -=2;
						  else if (availableMessages>>(profileMenuSelected-4) & 1)
							  profileMenuSelected -=3;
					  }
				  }
				  if(buttons>>18 & 1){ //S
					  if(profileMenuSelected > 0){
						  profileMenuSelected = 0;
					  }
				  }
				  if(buttons>>3 & 1){//D
					  if(profileMenuSelected > 0){
						  if(availableMessages>>(profileMenuSelected) & 1)
							  profileMenuSelected +=1;
						  else if (availableMessages>>(profileMenuSelected+1) & 1)
							  profileMenuSelected +=2;
						  else if (availableMessages>>(profileMenuSelected+2) & 1)
							  profileMenuSelected +=3;

					  }
				  }
			  }
			  if(buttons>>10 & 1) { //K
				  int messageStart = -1;
				  int lengthLoc = -1;
				  switch(profileMenuSelected){
				  	  case 0:
				  		  loop = 0;
				  		  break;
				  	  case 1:
				  		  lengthLoc = INITIAL_RING_LENGTH;
				  		  messageStart = INITIAL_RING;
				  		  break;
				  	  case 2:
				  		  lengthLoc = RING1_MESSAGE_LENGTH;
				  		  messageStart = RING1_MESSAGE;
				  		  break;
				  	  case 3:
				  		  lengthLoc = RING2_MESSAGE_LENGTH;
				  		  messageStart = RING2_MESSAGE;
				  		  break;
				  	  case 4:
				  		  lengthLoc = RING3_MESSAGE_LENGTH;
				  		  messageStart = RING3_MESSAGE;
				  		  break;
				  }
				  if(messageStart != -1){
					  scroll_message(messageStart);
				  }
			  }

			  matrix_clear();
			  char lvl[2] = {};
			  lvl[0] = eeprom_level + 48; // switch decimal to ascii
			  matrix_drawChar(PROFILE_LVL_TEXT,1,8,COLOR_WHITE);
			  matrix_drawChar(lvl,25,8,COLOR_WHITE);
			  matrix_drawChar(PROFILE_EXIT_TEXT,39,8,COLOR_ORANGE);

			  if(availableMessages == 0){
				  matrix_drawChar(PROFILE_NO_TEXT, 0, 0, COLOR_GREEN);
				  matrix_drawChar(PROFILE_MESSAGES_TEXT, 16, 0, COLOR_GREEN);
			  } else {
				  for (int i = 0; i < 4; i++){
					  if(availableMessages>>i & 1){ // if message 1-4 exists
						  matrix_drawImage(envelope, (i*16) + 1, 0, 14, 7);
					  }
				  }
			  }

			  switch(profileMenuSelected){
			  case 0: //
				  matrix_drawHorizontalLine(38, 15, 62, COLOR_RED);
				  break;
			  case 1: //
				  matrix_drawHorizontalLine(0, 7, 15, COLOR_RED);
				  break;
			  case 2: //
				  matrix_drawHorizontalLine(16, 7, 31, COLOR_RED);
				  break;
			  case 3: //
				  matrix_drawHorizontalLine(32, 7, 47, COLOR_RED);
				  break;
			  case 4: //
				  matrix_drawHorizontalLine(48, 7, 63, COLOR_RED);
				  break;
			  }

			  matrix_update();
		  }

	  }
}

void scroll_message(char *message){
	uint8_t loop = 1;
	int scroll_loop_duration = 0 - (strlen(message)*6) ;
	int scrollCounter = 64;

	while (loop){
		  if(nextFrame == 1){ //Set by Timer16 - 30FPS
			  nextFrame = 0;
			  UpdateMinibadges();
			  uint32_t buttons = 0;
			  buttons = keyboardUpdate();
			  if(buttons > 0) loop = 0;
			  matrix_clear();
			  matrix_drawChar(message,scrollCounter,4,COLOR_GREEN);
			  scrollCounter--;
			  if(scrollCounter < scroll_loop_duration){
				  loop = 0;
			  }
			  matrix_update();

		  }
	}
}

