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
#include "stm32f4xx_hal.h"

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
#define KEY_Pin GPIO_PIN_0
#define KEY_GPIO_Port GPIOA
#define SCK_ADS1118_Pin GPIO_PIN_1
#define SCK_ADS1118_GPIO_Port GPIOA
#define CS_AD1118_Pin GPIO_PIN_3
#define CS_AD1118_GPIO_Port GPIOA
#define MISO_ADS1118_Pin GPIO_PIN_5
#define MISO_ADS1118_GPIO_Port GPIOA
#define MOSI_ADS1118_Pin GPIO_PIN_7
#define MOSI_ADS1118_GPIO_Port GPIOA
#define LED_RED_Pin GPIO_PIN_5
#define LED_RED_GPIO_Port GPIOC
#define LED_BULE_Pin GPIO_PIN_2
#define LED_BULE_GPIO_Port GPIOB
#define AD7616_RD_Pin GPIO_PIN_10
#define AD7616_RD_GPIO_Port GPIOE
#define AD7616_WR_Pin GPIO_PIN_11
#define AD7616_WR_GPIO_Port GPIOE
#define AD7616_CHS0_Pin GPIO_PIN_12
#define AD7616_CHS0_GPIO_Port GPIOE
#define AD7616_CS_Pin GPIO_PIN_13
#define AD7616_CS_GPIO_Port GPIOE
#define AD7616_CHS2_Pin GPIO_PIN_14
#define AD7616_CHS2_GPIO_Port GPIOE
#define AD7616_CHS1_Pin GPIO_PIN_15
#define AD7616_CHS1_GPIO_Port GPIOE
#define AD7616_CONV_Pin GPIO_PIN_10
#define AD7616_CONV_GPIO_Port GPIOB
#define AD7616_BUSY_Pin GPIO_PIN_11
#define AD7616_BUSY_GPIO_Port GPIOB
#define AD7616_RST_Pin GPIO_PIN_12
#define AD7616_RST_GPIO_Port GPIOB
#define AD7616_RNG1_Pin GPIO_PIN_13
#define AD7616_RNG1_GPIO_Port GPIOB
#define AD7616_RNG0_Pin GPIO_PIN_14
#define AD7616_RNG0_GPIO_Port GPIOB
#define AD7616_S_P_Pin GPIO_PIN_15
#define AD7616_S_P_GPIO_Port GPIOB
#define AD7616_D15_Pin GPIO_PIN_8
#define AD7616_D15_GPIO_Port GPIOD
#define AD7616_D14_Pin GPIO_PIN_9
#define AD7616_D14_GPIO_Port GPIOD
#define AD7616_D13_Pin GPIO_PIN_10
#define AD7616_D13_GPIO_Port GPIOD
#define AD7616_D12_Pin GPIO_PIN_11
#define AD7616_D12_GPIO_Port GPIOD
#define AD7616_D11_Pin GPIO_PIN_12
#define AD7616_D11_GPIO_Port GPIOD
#define AD7616_D10_Pin GPIO_PIN_13
#define AD7616_D10_GPIO_Port GPIOD
#define AD7616_D9_Pin GPIO_PIN_14
#define AD7616_D9_GPIO_Port GPIOD
#define AD7616_D8_Pin GPIO_PIN_15
#define AD7616_D8_GPIO_Port GPIOD
#define AD7616_D7_Pin GPIO_PIN_6
#define AD7616_D7_GPIO_Port GPIOC
#define AD7616_D6_Pin GPIO_PIN_7
#define AD7616_D6_GPIO_Port GPIOC
#define AD7616_D5_Pin GPIO_PIN_8
#define AD7616_D5_GPIO_Port GPIOC
#define AD7616_D4_Pin GPIO_PIN_9
#define AD7616_D4_GPIO_Port GPIOC
#define AD7616_D3_Pin GPIO_PIN_8
#define AD7616_D3_GPIO_Port GPIOA
#define AD7616_D2_Pin GPIO_PIN_11
#define AD7616_D2_GPIO_Port GPIOA
#define AD7616_D1_Pin GPIO_PIN_12
#define AD7616_D1_GPIO_Port GPIOA
#define AD7616_D0_Pin GPIO_PIN_15
#define AD7616_D0_GPIO_Port GPIOA
#define BLK_Pin GPIO_PIN_3
#define BLK_GPIO_Port GPIOD
#define FS_Pin GPIO_PIN_4
#define FS_GPIO_Port GPIOD
#define CS_Pin GPIO_PIN_5
#define CS_GPIO_Port GPIOD
#define PS_Pin GPIO_PIN_6
#define PS_GPIO_Port GPIOD
#define DC_Pin GPIO_PIN_7
#define DC_GPIO_Port GPIOD
#define FSY_Pin GPIO_PIN_3
#define FSY_GPIO_Port GPIOB
#define RES_Pin GPIO_PIN_4
#define RES_GPIO_Port GPIOB
#define SCK_Pin GPIO_PIN_5
#define SCK_GPIO_Port GPIOB
#define SDA_Pin GPIO_PIN_6
#define SDA_GPIO_Port GPIOB
#define SDA_ad_Pin GPIO_PIN_7
#define SDA_ad_GPIO_Port GPIOB
#define SCL_Pin GPIO_PIN_8
#define SCL_GPIO_Port GPIOB
#define RST_Pin GPIO_PIN_9
#define RST_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
