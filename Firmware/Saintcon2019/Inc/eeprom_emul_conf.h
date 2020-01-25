/**
  ******************************************************************************
  * @file    eeprom_emul_conf.h
  * @author  MCD Application Team
  * @brief   EEPROM emulation configuration file.
  *          This file should be copied to the application folder and renamed
  *          to eeprom_emul_conf.h.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2017 STMicroelectronics International N.V.
  * All rights reserved.</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without
  * modification, are permitted, provided that the following conditions are met:
  *
  * 1. Redistribution of source code must retain the above copyright notice,
  *    this list of conditions and the following disclaimer.
  * 2. Redistributions in binary form must reproduce the above copyright notice,
  *    this list of conditions and the following disclaimer in the documentation
  *    and/or other materials provided with the distribution.
  * 3. Neither the name of STMicroelectronics nor the names of other
  *    contributors to this software may be used to endorse or promote products
  *    derived from this software without specific written permission.
  * 4. This software, including modifications and/or derivative works of this
  *    software, must execute solely and exclusively on microcontroller or
  *    microprocessor devices manufactured by or for STMicroelectronics.
  * 5. Redistribution and use of this software other than as permitted under
  *    this license is void and will automatically terminate your rights under
  *    this license.
  *
  * THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT
  * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A
  * PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
  * RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT
  * SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
  * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
  * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,
  * OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
  * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
  * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
  * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */

/** @addtogroup EEPROM_Emulation
  * @{
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EEPROM_EMUL_CONF_H
#define __EEPROM_EMUL_CONF_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Private constants ---------------------------------------------------------*/
/** @addtogroup EEPROM_Private_Constants
  * @{
  */

/** @defgroup Private_Configuration_Constants Private Configuration Constants
  * @{
  */

/* Configuration of eeprom emulation in flash, can be custom */
#define START_PAGE_ADDRESS      0x08028000U /*!< Start address of the 1st page in flash, for EEPROM emulation */
#define CYCLES_NUMBER           1U   /*!< Number of 10Kcycles requested, minimum 1 for 10Kcycles (default),
                                        for instance 10 to reach 100Kcycles. This factor will increase
                                        pages number */
#define GUARD_PAGES_NUMBER      0U   /*!< Number of guard pages avoiding frequent transfers (must be multiple of 2): 0,2,4.. */

/* Configuration of crc calculation for eeprom emulation in flash */
#define CRC_POLYNOMIAL_LENGTH   LL_CRC_POLYLENGTH_16B /* CRC polynomial lenght 16 bits */
#define CRC_POLYNOMIAL_VALUE    0x8005U /* Polynomial to use for CRC calculation */

/**
  * @}
  */

/**
  * @}
  */

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/** @defgroup EEPROM_Exported_Constants EEPROM Exported Constants
  * @{
  */

/** @defgroup Exported_Configuration_Constants Exported Configuration Constants
  * @{
  */
#define NB_OF_VARIABLES         260U  /*!< Number of variables to handle in eeprom */

 //eeprom variable map
#define EEP_CHALLENGE_LEVEL 1 //1 byte
#define EEP_CUSTOM_MESSAGE_LENGTH 2 //Up to 255
#define EEP_CUSTOM_MESSAGE_START 3 //1-255
#define EEP_MINIBADGE_STATE 259 //1 byte
#define EEP_MESSAGE_CODE_1 258 //6 bytes
#define EEP_MESSAGE_CODE_2 264 //6 bytes
#define EEP_MESSAGE_CODE_3 270 //6 bytes
#define EEP_MESSAGE_CODE_4 276 //6 bytes
#define EEP_MESSAGE_CODE_5 282 //6 bytes
#define EEP_MESSAGE_CODE_6 288 //6 bytes
#define EEP_MESSAGE_CODE_7 294 //6 bytes
#define EEP_MESSAGE_CODE_8 300 //6 bytes
#define EEP_MESSAGE_CODE_9 306 //6 bytes
#define EEP_LEVEL_1_MESSAGE_LENGTH 336 //1 byte
#define EEP_LEVEL_1_MESSAGE_START 337 //Up to 255
#define EEP_LEVEL_2_MESSAGE_LENGTH 592 //1 byte
#define EEP_LEVEL_2_MESSAGE_START 593 //Up to 255
#define EEP_LEVEL_3_MESSAGE_LENGTH 848 //1 byte
#define EEP_LEVEL_3_MESSAGE_START 849 //Up to 255
#define EEP_FINAL_MESSAGE_LENGTH 1104 //1 byte
#define EEP_FINAL_MESSAGE_START 1105 //Up to 255



/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

/**
  * @}
  */

#endif /* __EEPROM_EMUL_CONF_H */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
