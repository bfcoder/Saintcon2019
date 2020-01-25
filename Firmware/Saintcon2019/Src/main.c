/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "dma.h"
#include "i2c.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "usb_device.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdint.h"
#include "drv_is31fl.h"
#include "usbd_cdc_if.h"
#include "stm32l4xx_hal.h"
#include "errno.h"
#include "stm32l433xx.h"
#include "matrix.h"
#include "lampboard.h"
#include "keyboard.h"
#include "images.h"
#include "custommessage.h"
#include "eeprom_emul.h"
#include "stdlib.h"
#include "testmode.h"
#include "animations.h"
#include "menu.h"
#include "plugboard.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define LED_I2C_MASTER_NUM &hi2c1

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

//Uncomment to enable badge test mode
//#define BADGE_TEST_CODE

const char ERROR_COMMANDER[] = "You must have a commander to receive your debriefing. Invite one to join your ring.";
const uint8_t ERROR_COMMANDER_LENGTH = 83;
const char INITIAL_RING[] = "Helga has been captured! Your mission is to decode messages to enable Allied forces to save her from the Enemy. Intercepted messages can be picked up at Hut 6. Good luck!";
const uint8_t INITIAL_RING_LENGTH = 170;

const char RING1_MESSAGE[] = "Well done Agents! Helga has been rescued, but Trevor was recaptured. Your new mission is to stop the Enemy from extracting the vital intelligence that Trevor holds. Good luck!";
const char RING2_MESSAGE[] = "Trevor tragically paid the ultimate price, but hopefully the Enemy didn’t learn of our newest defense. Your new mission is to determine what the Enemy knows about Project Firewall.";
const char RING3_MESSAGE[] = "Mission Complete! Thanks to your decoding skills Helga was saved! Sadly Trevor gave his life to protect Project Firewall, but his sacrifice will never be forgotten #TrevorForget. Thanks for playing!";
const uint8_t RING1_MESSAGE_LENGTH = 175;
const uint8_t RING2_MESSAGE_LENGTH = 180;
const uint8_t RING3_MESSAGE_LENGTH = 198;

/* Virtual address Tab defined by the user: 0x0000 and 0xFFFF values are prohibited */
uint16_t VirtAddVarTab[NB_OF_VARIABLES];
uint32_t Index = 0;
__IO uint32_t ErasingOnGoing = 0;
uint32_t VarDataTab[NB_OF_VARIABLES] = {0};
uint32_t VarValue = 0;

uint8_t badgeLevel = 1; //Change to start on level 1 after the conference, when it's not possible to link up badges
uint8_t badgeNum = 1;

volatile uint8_t rbyte;

volatile uint8_t nextFrame = 0;

volatile int syncCounter = 0;

volatile int LeftRecvBufferHead = 0;
volatile int LeftRecvBufferTail = 0;
volatile uint8_t LeftRecvBuffer[UART_BUFFER_SIZE + 1];
volatile int RightRecvBufferHead = 0;
volatile int RightRecvBufferTail = 0;
volatile uint8_t RightRecvBuffer[UART_BUFFER_SIZE + 1];

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/*
 * Delay for n msec
 */
static void
msec_delay(int n)
{
	HAL_Delay(n);
}
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	EE_Status ee_status = EE_OK;
  /* USER CODE END 1 */
  

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_I2C2_Init();
  MX_SPI1_Init();
  MX_SPI2_Init();
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USB_DEVICE_Init();
  MX_TIM16_Init();
  /* USER CODE BEGIN 2 */

  /* Enable and set FLASH Interrupt priority */
    /* FLASH interrupt is used for the purpose of pages clean up under interrupt */
    HAL_NVIC_SetPriority(FLASH_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(FLASH_IRQn);

    /* Unlock the Flash Program Erase controller */
    HAL_FLASH_Unlock();
    /* Enable Power Control clock */
      __HAL_RCC_PWR_CLK_ENABLE();

      /* Set user List of Virtual Address variables: 0x0000 and 0xFFFF values are prohibited */
        for (VarValue = 0; VarValue < NB_OF_VARIABLES; VarValue++)
        {
          VirtAddVarTab[VarValue] = (uint16_t)(10*VarValue + 1);
        }

      /* Set EEPROM emulation firmware to erase all potentially incompletely erased
           pages if the system came from an asynchronous reset. Conditional erase is
           safe to use if all Flash operations where completed before the system reset */
        if(__HAL_PWR_GET_FLAG(PWR_FLAG_SB) == RESET)
        {


          /* System reset comes from a power-on reset: Forced Erase */
          /* Initialize EEPROM emulation driver (mandatory) */
          ee_status = EE_Init(VirtAddVarTab, EE_FORCED_ERASE);
          if(ee_status != EE_OK) {Error_Handler();}
        }
        else
        {
          /* Clear the Standby flag */
          __HAL_PWR_CLEAR_FLAG(PWR_FLAG_SB);


          /* System reset comes from a STANDBY wakeup: Conditional Erase*/
          /* Initialize EEPROM emulation driver (mandatory) */
          ee_status = EE_Init(VirtAddVarTab, EE_CONDITIONAL_ERASE);
          if(ee_status != EE_OK) {Error_Handler();}
        }





  HAL_TIM_Base_Start_IT(&htim16);

  FPGA_Init();

  HAL_GPIO_WritePin(GPIOE, GPIO_PIN_1,GPIO_PIN_SET);

  drv_is31fl_init();

  //uint8_t buffer[10];

  drv_is31fl_set_page(1);
  //HAL_I2C_Master_Transmit(&hi2c1, 0x50<<1, buffer,1,100);
  //drv_is31fl_send_value(10,128);

  //uint8_t x = 0;
  //uint8_t leddelay = 100;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
//matrix_fillRect(0,0,64,16,COLOR_WHITE);
//matrix_update();
  //while(1);
  //uint8_t address = 70;

  InitMinibadges();



  /*
  HAL_I2C_Master_Transmit(&hi2c2, address, data, 2, 100);

  data[0] = 0x01;
  for(int x=0; x<2; x++){
	  data[1] = 0x01;
	  HAL_I2C_Master_Transmit(&hi2c2, address, data, 2, 100);
	  msec_delay(100);
	  data[1] = 0x02;
	  HAL_I2C_Master_Transmit(&hi2c2, address, data, 2, 100);
	  msec_delay(100);
	  data[1] = 0x04;
	  HAL_I2C_Master_Transmit(&hi2c2, address, data, 2, 100);
	  msec_delay(100);
	  data[1] = 0x08;
	  HAL_I2C_Master_Transmit(&hi2c2, address, data, 2, 100);
	  msec_delay(100);
	  data[1] = 0x10;
	  HAL_I2C_Master_Transmit(&hi2c2, address, data, 2, 100);
	  msec_delay(100);
	  data[1] = 0x20;
	  HAL_I2C_Master_Transmit(&hi2c2, address, data, 2, 100);
	  msec_delay(100);
	  data[1] = 0x40;
	  HAL_I2C_Master_Transmit(&hi2c2, address, data, 2, 100);
	  msec_delay(100);
	  data[1] = 0x80;
	  HAL_I2C_Master_Transmit(&hi2c2, address, data, 2, 100);
	  msec_delay(100);
  }
  data[1] = 0xFF;
  HAL_I2C_Master_Transmit(&hi2c2, address, data, 2, 100);
*/

  //Character Test
  //matrix_drawChar("SAINTCON",0,0,COLOR_RED);
 // matrix_update();
  //msec_delay(500);

  //Drawing Test
  /*matrix_drawRect(0,0,64,16,COLOR_RED);
  matrix_drawRect(1,1,62,14,COLOR_GREEN);
  matrix_drawRect(2,2,60,12,COLOR_BLUE);
  matrix_drawRect(3,3,58,10,COLOR_WHITE);
  matrix_drawRect(4,4,56,8,COLOR_RED);
  matrix_drawRect(5,5,54,6,COLOR_GREEN);
  matrix_drawRect(6,6,52,4,COLOR_BLUE);
  matrix_drawRect(7,7,50,2,COLOR_WHITE);
  matrix_update();
  msec_delay(500);
   */

  uint8_t data;
  if(EE_ReadVariable8bits(VirtAddVarTab[EEP_MESSAGE_CODE_1], &data) != EE_OK){
	  TestMode();
  }

  EE_ReadVariable8bits(VirtAddVarTab[EEP_CHALLENGE_LEVEL], &badgeLevel);
  if(badgeLevel == 0) badgeLevel = 1; //Fix for after conference - Skip initial ring and start on level 1

  //Image Test
  //matrix_drawImage(sc,0,0,38,16);
  matrix_drawImage(sc4,0,0,64,16);
  matrix_update();
  msec_delay(1000);


  menu();

/*
  for(int x=64; x>0; x-=2){
	  matrix_clear();
	  matrix_drawImageScaled(sc4,(64-x)/2,0,64,16,x,16);
	  matrix_update();
	  msec_delay(500);
  }
*/
  /*
  for(int i=0; i<16; i++){
	  matrix_clear();
	  matrix_drawLine(0,0,63,i, COLOR_RED);
	  matrix_update();
	  msec_delay(200);
  }
  for(int i=63; i>0; i--){
  	  matrix_clear();
  	  matrix_drawLine(0,0,i,15, COLOR_RED);
  	  matrix_update();
  	  msec_delay(200);
    }*/
  /*for(int i=0; i<16; i++){
  	  matrix_clear();
  	  matrix_drawLine(0,0,63,i, COLOR_RED);
  	  matrix_update();
  	  msec_delay(500);
    }
  for(int i=0; i<16; i++){
  	  matrix_clear();
  	  matrix_drawLine(0,0,63,i, COLOR_RED);
  	  matrix_update();
  	  msec_delay(500);
    }*/

  while (1){

	  if(nextFrame == 1){ //Set by Timer16 - 30FPS
		  nextFrame = 0;
		  //Button Test Code
		  uint32_t buttons = 0;
		  buttons = keyboardUpdate();
		  for(int x=0; x<26; x++){
			  if((buttons>>x) & 1){
				  lampboard_setLamp(x, COLOR_RED);
				  HAL_UART_Transmit(&huart1, (unsigned char*)"A", 1, 500);
			  } else {
				  lampboard_setLamp(x, COLOR_GREEN);
			  }
		  }
		  if((buttons>>31)&1){//LongPress
			  for(int x=0; x<26; x++){
				  lampboard_setLamp(x, COLOR_BLUE);
			  }
		  }
		  lampboard_update();
		  //msec_delay(50);
	  }

  }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_MSI;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 40;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV7;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB busses clocks 
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_4) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2
                              |RCC_PERIPHCLK_I2C1|RCC_PERIPHCLK_I2C2
                              |RCC_PERIPHCLK_USB;
  PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
  PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_PCLK1;
  PeriphClkInit.I2c2ClockSelection = RCC_I2C2CLKSOURCE_PCLK1;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLLSAI1;
  PeriphClkInit.PLLSAI1.PLLSAI1Source = RCC_PLLSOURCE_MSI;
  PeriphClkInit.PLLSAI1.PLLSAI1M = 1;
  PeriphClkInit.PLLSAI1.PLLSAI1N = 24;
  PeriphClkInit.PLLSAI1.PLLSAI1P = RCC_PLLP_DIV7;
  PeriphClkInit.PLLSAI1.PLLSAI1Q = RCC_PLLQ_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1R = RCC_PLLR_DIV2;
  PeriphClkInit.PLLSAI1.PLLSAI1ClockOut = RCC_PLLSAI1_48M2CLK;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
  /** Configure the main internal regulator output voltage 
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin){
	//Called when an external interrupt pin is triggered
	if(GPIO_Pin == SYNC_Pin){
		//syncCounter ++;
	}
	//if(GPIO_Pin == GPIO_PIN_2){
		//dispDelay = 500;
		//for(int x=0; x<100; x++){
		//HAL_UART_Transmit(&huart1, (unsigned char*)"A", 1, 500);
		//msec_delay(10);
		//}
	//}
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim){
	if(htim == &htim16){ //Timer16 - 30fps timer
		//Set flag here to alert main loop to draw new frame
		nextFrame = 1;
	}
	/*static uint8_t state = 0;
	if(state == 0){
		state = 1;
		drv_is31fl_send_value(0,0);
	} else {
		state = 0;
		drv_is31fl_send_value(0,128);
	}*/
}

void UART_Right_IRQ(UART_HandleTypeDef *huart){ //huart1
	uint32_t isrflags   = READ_REG(huart->Instance->ISR);
	  uint32_t cr1its     = READ_REG(huart->Instance->CR1);
	  uint32_t cr3its     = READ_REG(huart->Instance->CR3);

	  uint32_t errorflags;
	  //uint32_t errorcode;

	  if (((isrflags & USART_ISR_RXNE) != 0U) //Receive Mode
			  && ((cr1its & USART_CR1_RXNEIE) != 0U)){
		  /* If no error occurs */
		  errorflags = (isrflags & (uint32_t)(USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE));
		  if (errorflags == 0U)
		  {
			  if(RightRecvBufferHead == ((RightRecvBufferTail - 1 + UART_BUFFER_SIZE) % UART_BUFFER_SIZE)){
				  //Buffer Full

				  rbyte = (uint8_t)huart->Instance->RDR;
			  } else {
				  RightRecvBuffer[RightRecvBufferHead] = (uint8_t)huart->Instance->RDR;
				  RightRecvBufferHead = (RightRecvBufferHead + 1) % UART_BUFFER_SIZE;
			  }
		  }
	  } else {
		  rbyte = (uint8_t)huart->Instance->RDR;
	  }


	  __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_PEF);
	  __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_FEF);
	  __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_NEF);
	  __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF);
	  __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_WUF);
}

void UART_Left_IRQ(UART_HandleTypeDef *huart){ //huart2
	uint32_t isrflags   = READ_REG(huart->Instance->ISR);
		  uint32_t cr1its     = READ_REG(huart->Instance->CR1);
		  uint32_t cr3its     = READ_REG(huart->Instance->CR3);

		  uint32_t errorflags;
		  uint32_t errorcode;

		  if ((isrflags & USART_ISR_RXNE) != 0U){ //Receive Mode
				  //&& ((cr1its & USART_CR1_RXNEIE) != 0U)){
			  /* If no error occurs */
			  errorflags = (isrflags & (uint32_t)(USART_ISR_PE | USART_ISR_FE | USART_ISR_ORE | USART_ISR_NE));
			  if (errorflags == 0U)
			  {
				  if(LeftRecvBufferHead == ((LeftRecvBufferTail - 1 + UART_BUFFER_SIZE) % UART_BUFFER_SIZE)){
					  //Buffer Full
					  rbyte = (uint8_t)huart->Instance->RDR;
				  } else {
					  LeftRecvBuffer[LeftRecvBufferHead] = (uint8_t)huart->Instance->RDR;
					  LeftRecvBufferHead = (LeftRecvBufferHead + 1) % UART_BUFFER_SIZE;
				  }
			  }
		  } else {
			  rbyte = (uint8_t)huart->Instance->RDR;
		  }

		  __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_PEF);
		  __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_FEF);
		  __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_NEF);
		  __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_OREF);
		  __HAL_UART_CLEAR_FLAG(huart, UART_CLEAR_WUF);

}

void CheckSerial(){

	uint8_t leftBadgeLevel = 0;
	uint8_t leftBadgeNum = 0;
	//uint8_t leftBadgeNum = 25; //for testing
	//Left Serial
	if(LeftRecvBufferHead == LeftRecvBufferTail){
		//Buffer Empty
	} else {
		uint8_t data = LeftRecvBuffer[LeftRecvBufferTail];
		//LeftRecvBufferTail = (LeftRecvBufferTail + 1) % UART_BUFFER_SIZE;
		if(data == SYNC){
			DisplayAnimation();
		} else {
			LeftRecvBufferTail = (LeftRecvBufferTail + 1) % UART_BUFFER_SIZE;
		}

	}

	uint8_t rightBadgeLevel = 0;
	//Right Serial
	if(RightRecvBufferHead == RightRecvBufferTail){
		//Buffer Empty
	} else {
		uint8_t data = RightRecvBuffer[RightRecvBufferTail];

		if(data == badgeNum){
			DisplayAnimation();
		} else {
			RightRecvBufferTail = (RightRecvBufferTail + 1) % UART_BUFFER_SIZE;
		}

		if(data == SYNC + 1){ //201
			uint8_t out[8];
			uint8_t data = 0;
			out[0] = SYNC;
			out[1] = badgeLevel;
			if(badgeLevel < 1){
				for(int x=0; x<6; x++){
					out[x + 2] = 'A';
				}
			} else if(badgeLevel < 2){
				for(int x=0; x<6; x++){
					EE_ReadVariable8bits(EEP_MESSAGE_CODE_1 + x, &data);
					out[x + 2] = data+65;
				}
			} else if(badgeLevel < 3){
				for(int x=0; x<6; x++){
					EE_ReadVariable8bits(EEP_MESSAGE_CODE_2 + x, &data);
					out[x + 2] = data+65;
				}
			} else if(badgeLevel < 4){
				for(int x=0; x<6; x++){
					EE_ReadVariable8bits(EEP_MESSAGE_CODE_3 + x, &data);
					out[x + 2] = data+65;
				}
			} else if(badgeLevel < 5){
				for(int x=0; x<6; x++){
					out[x + 2]='A';
				}
			} else if(badgeLevel < 6){
				for(int x=0; x<6; x++){
					EE_ReadVariable8bits(EEP_MESSAGE_CODE_4 + x, &data);
					out[x + 2] = data+65;
				}
			} else if(badgeLevel < 7){
				for(int x=0; x<6; x++){
					EE_ReadVariable8bits(EEP_MESSAGE_CODE_5 + x, &data);
					out[x + 2] = data+65;
				}
			} else if(badgeLevel < 8){
				for(int x=0; x<6; x++){
					EE_ReadVariable8bits(EEP_MESSAGE_CODE_6 + x, &data);
					out[x + 2] = data+65;
				}
			} else if(badgeLevel < 9){
				for(int x=0; x<6; x++){
					out[x + 2]='A';
				}
			} else if(badgeLevel < 10){
				for(int x=0; x<6; x++){
					EE_ReadVariable8bits(EEP_MESSAGE_CODE_7 + x, &data);
					out[x + 2] = data+65;
				}
			} else if(badgeLevel < 11){
				for(int x=0; x<6; x++){
					EE_ReadVariable8bits(EEP_MESSAGE_CODE_8 + x, &data);
					out[x + 2] = data+65;
				}
			} else if(badgeLevel < 12){
				for(int x=0; x<6; x++){
					EE_ReadVariable8bits(EEP_MESSAGE_CODE_9 + x, &data);
					out[x + 2] = data+65;
				}
			} else if(badgeLevel < 13){
				for(int x=0; x<6; x++){
					out[x + 2] = 'A';
				}
			} else if(badgeLevel < 14){
				for(int x=0; x<6; x++){
					out[x + 2] = 'Z';
				}
			}
			HAL_UART_Transmit(&huart1, out, 8, 100); //Send Code to Printer
			//matrix_clear();
			//matrix_drawCharCentered("PRINTING", 0, 4, COLOR_GREEN);
			//matrix_update();
			//HAL_Delay(1000);
		}
				/*if(data == SYNC){
					while(RightRecvBufferHead == RightRecvBufferTail);
					rightBadgeLevel = RightRecvBuffer[RightRecvBufferTail];
					RightRecvBufferTail = (RightRecvBufferTail + 1) % UART_BUFFER_SIZE;
					while(RightRecvBufferHead == RightRecvBufferTail);
					uint8_t rightBadgeNum = RightRecvBuffer[RightRecvBufferTail];
					RightRecvBufferTail = (RightRecvBufferTail + 1) % UART_BUFFER_SIZE;
					while(RightRecvBufferHead == RightRecvBufferTail);
					uint8_t rightBadgeCommand = RightRecvBuffer[RightRecvBufferTail];
					RightRecvBufferTail = (RightRecvBufferTail + 1) % UART_BUFFER_SIZE;

					uint8_t ack[5];
					switch(rightBadgeCommand){
					case 2: //Ack
						ack[0] = SYNC;
						ack[1] = badgeLevel;
						//ack[2] = leftBadgeNum + 1;
						ack[2] = 26;
						ack[3] = 4;
						ack[4] = 0;
						HAL_UART_Transmit(&huart1, ack, 5, 100); //Send Display Rotor Animation
						DisplayAnimation(leftBadgeLevel, leftBadgeNum, 4, badgeLevel);
						break;

					case 5: //Request Level Code
						;
						uint8_t code[10] = {SYNC, badgeLevel, 1, 6};
						if(badgeLevel == 1){
							for(int x=4; x<10; x++){
								EE_ReadVariable8bits(EEP_MESSAGE_CODE_1 + x, code[x]);
							}
						}
						if(badgeLevel == 2){
							for(int x=4; x<10; x++){
								EE_ReadVariable8bits(EEP_MESSAGE_CODE_2 + x, code[x]);
							}
						}
						if(badgeLevel == 3){
							for(int x=4; x<10; x++){
								EE_ReadVariable8bits(EEP_MESSAGE_CODE_3 + x, code[x]);
							}
						}
						if(badgeLevel == 4){
							for(int x=4; x<10; x++){
								EE_ReadVariable8bits(EEP_MESSAGE_CODE_4 + x, code[x]);
							}
						}
						if(badgeLevel == 5){
							for(int x=4; x<10; x++){
								EE_ReadVariable8bits(EEP_MESSAGE_CODE_5 + x, code[x]);
							}
						}
						if(badgeLevel == 6){
							for(int x=4; x<10; x++){
								EE_ReadVariable8bits(EEP_MESSAGE_CODE_6 + x, code[x]);
							}
						}
						if(badgeLevel == 7){
							for(int x=4; x<10; x++){
								EE_ReadVariable8bits(EEP_MESSAGE_CODE_7 + x, code[x]);
							}
						}
						if(badgeLevel == 8){
							for(int x=4; x<10; x++){
								EE_ReadVariable8bits(EEP_MESSAGE_CODE_8 + x, code[x]);
							}
						}
						if(badgeLevel == 9){
							for(int x=4; x<10; x++){
								EE_ReadVariable8bits(EEP_MESSAGE_CODE_9 + x, code[x]);
							}
						}
						HAL_UART_Transmit(&huart1, code, 10, 100); //Send Level Code
						break;
					}
				}*/
	}

	//uint8_t outData[4] = {SYNC, badgeLevel, leftBadgeNum + 1, 1};
	//uint8_t outData[4] = {SYNC, badgeLevel, 26, 1};
	//HAL_UART_Transmit(&huart1, outData, 4, 100); //Send Hello to Right side
	uint8_t outData[5] = {SYNC, badgeLevel, 1, 4, 0};
	HAL_UART_Transmit(&huart1, outData, 5, 100); //Send Rotor Command to Right
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */

  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(char *file, uint32_t line)
{ 
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
