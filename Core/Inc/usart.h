/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    usart.h
  * @brief   This file contains all the function prototypes for
  *          the usart.c file
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
#ifndef __USART_H__
#define __USART_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern UART_HandleTypeDef huart3;

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

void MX_USART1_UART_Init(void);
void MX_USART2_UART_Init(void);
void MX_USART3_UART_Init(void);

/* USER CODE BEGIN Prototypes */
/**
 * @brief Initalize and enable UART1
 *
 */
void UART1FIFOInit(void);

/**
 * @brief Probe UART1
 *
 * @return int
 */
int UART1Probe(void);

/**
 * @brief Return a char array from FIFO buffer
 *
 * @return uint8_t
 */
uint8_t UART1GetChar(void);

/**
 * @brief Insert character array for UART transmission
 *
 * @param data
 */
void UART1PutChar(uint8_t data);

/**
 * @brief IRQ for handling serial printing
 *
 */
void UART1IRQHandler(void);

/**
 * @brief Transmit data of given size to the serial
 *
 * @param data
 * @param data_size
 * @return uint32_t
 */
uint32_t UART1Send(uint8_t *data, int data_size);

/**
 * @brief Initalize UART2
 *
 */
void UART2FIFOInit(void);

/**
 * @brief Insert char array into UART2 FiFo buffer
 *
 * @param data
 */
void UART2PutChar(uint8_t data);
void UART2clear();
/**
 * @brief Probe UART2
 *
 * @return int
 */
int  UART2Probe(void);

/**
 * @brief Get a character array from UART2 buffer
 *
 * @return uint8_t
 */
uint8_t UART2GetChar(void);

/**
 * @brief Insert a char array into to UART2 buffer
 *
 * @param data
 */
void UART2PutChar(uint8_t data);

/**
 * @brief UART2 IRQ Handler
 *
 */
void UART2IRQHandler(void);

/**
 * @brief High-level method for sending data of a
 * limited size to the UART2 buffer
 *
 * @param data
 * @param data_size
 * @return uint32_t
 */
uint32_t UART2Send(uint8_t *data, int data_size);
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __USART_H__ */

