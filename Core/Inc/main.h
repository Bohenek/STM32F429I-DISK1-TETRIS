/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
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
#define ADC_IN_Pin GPIO_PIN_3
#define ADC_IN_GPIO_Port GPIOF
#define CS_Pin GPIO_PIN_2
#define CS_GPIO_Port GPIOC
#define BTN_Pin GPIO_PIN_0
#define BTN_GPIO_Port GPIOA
#define RES_Pin GPIO_PIN_12
#define RES_GPIO_Port GPIOD
#define DC_Pin GPIO_PIN_13
#define DC_GPIO_Port GPIOD
#define LED4_Pin GPIO_PIN_14
#define LED4_GPIO_Port GPIOD
#define LED3_Pin GPIO_PIN_15
#define LED3_GPIO_Port GPIOD
#define LED2_Pin GPIO_PIN_2
#define LED2_GPIO_Port GPIOG
#define LED1_Pin GPIO_PIN_3
#define LED1_GPIO_Port GPIOG
#define BTN_LEFT_Pin GPIO_PIN_5
#define BTN_LEFT_GPIO_Port GPIOG
#define BTN_LEFT_EXTI_IRQn EXTI9_5_IRQn
#define BTN_RIGHT_Pin GPIO_PIN_6
#define BTN_RIGHT_GPIO_Port GPIOG
#define BTN_RIGHT_EXTI_IRQn EXTI9_5_IRQn
#define BTN_DOWN_Pin GPIO_PIN_7
#define BTN_DOWN_GPIO_Port GPIOG
#define BTN_DOWN_EXTI_IRQn EXTI9_5_IRQn
#define BTN_START_Pin GPIO_PIN_8
#define BTN_START_GPIO_Port GPIOG
#define BTN_START_EXTI_IRQn EXTI9_5_IRQn
#define BTN_UP_Pin GPIO_PIN_9
#define BTN_UP_GPIO_Port GPIOC
#define BTN_UP_EXTI_IRQn EXTI9_5_IRQn

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
