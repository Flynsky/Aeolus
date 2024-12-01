/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
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
#include "stm32l4xx_hal.h"

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
#define PIN_IRQ_NRF_Pin GPIO_PIN_13
#define PIN_IRQ_NRF_GPIO_Port GPIOC
#define PIN_CS_NRF_Pin GPIO_PIN_14
#define PIN_CS_NRF_GPIO_Port GPIOC
#define PIN_CE_NRF_Pin GPIO_PIN_15
#define PIN_CE_NRF_GPIO_Port GPIOC
#define PIN_NTC_0_Pin GPIO_PIN_0
#define PIN_NTC_0_GPIO_Port GPIOA
#define PIN_NTC_1_Pin GPIO_PIN_1
#define PIN_NTC_1_GPIO_Port GPIOA
#define PIN_V_BAT_DIV_Pin GPIO_PIN_4
#define PIN_V_BAT_DIV_GPIO_Port GPIOA
#define PIN_NTC_2_Pin GPIO_PIN_5
#define PIN_NTC_2_GPIO_Port GPIOA
#define PIN_TOF_X_SHUNT_Pin GPIO_PIN_6
#define PIN_TOF_X_SHUNT_GPIO_Port GPIOA
#define PIN_TOF_INT_Pin GPIO_PIN_1
#define PIN_TOF_INT_GPIO_Port GPIOB
#define PIN_MAG_INT_Pin GPIO_PIN_2
#define PIN_MAG_INT_GPIO_Port GPIOB
#define PIN_MAG_CS_Pin GPIO_PIN_12
#define PIN_MAG_CS_GPIO_Port GPIOB
#define PIN_STATUS_LED_Pin GPIO_PIN_8
#define PIN_STATUS_LED_GPIO_Port GPIOA
#define PIN_IMU_INT1_Pin GPIO_PIN_14
#define PIN_IMU_INT1_GPIO_Port GPIOA
#define PIN_IMU_INT2_Pin GPIO_PIN_8
#define PIN_IMU_INT2_GPIO_Port GPIOB
#define PIN_IMU_CS_Pin GPIO_PIN_9
#define PIN_IMU_CS_GPIO_Port GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
