/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
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
#include "stm32h7xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LCD_DISP_Pin GPIO_PIN_15
#define LCD_DISP_GPIO_Port GPIOJ
#define OSC32_OUT_Pin GPIO_PIN_15
#define OSC32_OUT_GPIO_Port GPIOC
#define OSC32_IN_Pin GPIO_PIN_14
#define OSC32_IN_GPIO_Port GPIOC
#define LCD_DISPD7_Pin GPIO_PIN_7
#define LCD_DISPD7_GPIO_Port GPIOD
#define USB_OTG_FS2_ID_Pin GPIO_PIN_10
#define USB_OTG_FS2_ID_GPIO_Port GPIOA
#define B1_Pin GPIO_PIN_13
#define B1_GPIO_Port GPIOC
#define LD2_Pin GPIO_PIN_13
#define LD2_GPIO_Port GPIOI
#define LCD_INT_Pin GPIO_PIN_2
#define LCD_INT_GPIO_Port GPIOG
#define OSC_OUT_Pin GPIO_PIN_1
#define OSC_OUT_GPIO_Port GPIOH
#define OSC_IN_Pin GPIO_PIN_0
#define OSC_IN_GPIO_Port GPIOH
#define LCD_BL_Pin GPIO_PIN_0
#define LCD_BL_GPIO_Port GPIOK
#define VCP_TX_Pin GPIO_PIN_10
#define VCP_TX_GPIO_Port GPIOB
#define VCP_RX_Pin GPIO_PIN_11
#define VCP_RX_GPIO_Port GPIOB
#define USB_OTG_FS2_OverCurrent_Pin GPIO_PIN_11
#define USB_OTG_FS2_OverCurrent_GPIO_Port GPIOH
#define LD1_Pin GPIO_PIN_2
#define LD1_GPIO_Port GPIOJ
#define LCD_RST_Pin GPIO_PIN_12
#define LCD_RST_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
