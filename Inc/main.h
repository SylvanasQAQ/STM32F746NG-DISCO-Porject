/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f7xx_hal.h"

#include "GUI.h"
#include "HW_Init.h"
#include "GUI_App.h"
#include "STemwin_wrapper.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include<stdio.h>

#include "GUIDRV_Lin.h"
#include "GUI.h"
#include "DIALOG.h"

#include "cmsis_os.h"
//#include "cmsis_os2.h"
#include "FreeRTOS.h"
#include "FreeRTOSConfig.h"
#include "task.h"
    

#include "stm32746g_discovery.h"
#include "stm32746g_discovery_sdram.h"

#include "arm_math.h"
#include "arm_const_structs.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
#define LCD_FRAME_BUFFER          ((uint32_t)SDRAM_DEVICE_ADDR)
#define LCD_FRAME_BUFFER_SIZE     0x40000

#define WAV_PLAYER_BUFFER         ((uint32_t)LCD_FRAME_BUFFER + LCD_FRAME_BUFFER_SIZE)
#define WAV_PLAYER_BUFFER_SIZE     0x20000

#define AUDIO_RECORD_BUFFER       ((uint32_t)WAV_PLAYER_BUFFER + WAV_PLAYER_BUFFER_SIZE)
#define AUDIO_RECORD_BUFFER_SIZE   0x200000

#define REBOOT_STORAGE_AREA       ((uint32_t)AUDIO_RECORD_BUFFER + AUDIO_RECORD_BUFFER_SIZE)
#define REBOOT_STORAGE_AREA_SIZE   0x8000
/* USER CODE END EM */

void HAL_TIM_MspPostInit(TIM_HandleTypeDef *htim);

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
/* USER CODE BEGIN Private defines */
#define CMSIS_V1
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
