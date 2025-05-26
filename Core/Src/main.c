/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <assert.h>
#include "stm32f1xx_hal.h"
#include "../../Systick/systick.h"
#include "../../GSM/gsm.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */


GSMModuleState gsmModuleState;
mqttObject mqttConnectionObject;
GSMModuleValue gsmvalue;


uint32_t intput_trig_time,intput_trig_time1,run_trig_time;
uint32_t First_trig=3000;
uint32_t Sec_trig=30000;



/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
uint8_t apn_netw[30] = {"airteliot.com"};
uint8_t apn_user[10] = {"none"};
uint8_t apn_pass[10] = {"none"};
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */




int sent_failed_count = 0;
uint8_t tcp_connection_failed_count = 0;
uint32_t sim_disconnected = 0;
uint8_t command_failed_count = 0;
int sim_module_on_counter = 0;



unsigned char mqtt_outbound_buffer[80];
unsigned char mqtt_inbound_buffer[80];
char mqttTopicName[100];

int publish_birth_message = 0;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int i=0;
uint8_t imeii[17];
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

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
  MX_USART1_UART_Init();
  MX_USART2_UART_Init();
  MX_USART3_UART_Init();
  /* USER CODE BEGIN 2 */

  UART1FIFOInit();
  UART2FIFOInit();
  serialPrint("\nStarting up\r\n");
  resetSIMModule(GSM_POWER_PORT, GSM_POWER_PIN, 3000);
  SysTick_Config(SystemCoreClock / 1000);
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */

	  run_trig_time=HAL_GetTick();
	  if (gsmModuleState == Off)
	      {
	        serialPrint("\nSIM State is Off.Initializing\r\n");
	        if (initializeSIMModule(&gsmvalue))
	        {
	          gsmModuleState = On;
	          serialPrint("\nSIM Module initilized\r\n");

	        }
	      }





	      if (gsmModuleState == On)
	      {
	        serialPrint("\nSIM module is on\r\n");
	        sim_module_on_counter++;
	        if (sim_module_on_counter > 300)
	        {
	          serialPrint("\nReseting...\r\n");
	          resetSIMModule(GSM_POWER_PORT, GSM_POWER_PIN, 3000);
	          sim_module_on_counter = 0;
	          gsmModuleState = Off;
	          sent_failed_count = 0;
	          tcp_connection_failed_count = 0;
	        }

	        if (checkSIMNetworkState())
	        { gsmModuleState = Registered;
	          if (setupTCP(&gsmvalue))
	          {
	            gsmModuleState = Registered;
	            serialPrint("\nNetwork registered\r\n");
	            setupMQTT();
//HAL_Delay(2000);


//	            if (setupTCPGET(&gsmvalue,"........................"))
//	          	          {
//	            	serialPrint("\nHTTP GET \r\n");
//	          	          }
	            //setupTCPGET
	          }
	        }
	      }



	      if (sent_failed_count > 10)
	      {
	        gsmModuleState = On;
	        sent_failed_count = 0;
	        tcp_connection_failed_count = 0;
	        serialPrint("\nGPRS Connection failed\r\n");
	      }
	      if (tcp_connection_failed_count > 20)
	      {
	        gsmModuleState = On;
	        tcp_connection_failed_count = 0;
	        sent_failed_count = 0;
	        serialPrint("\nTCP Connection failed\r\n");
	      }
	      if (gsmModuleState == Registered)
	      {
	    	  mqtt_check();
	        mqttConnectionObject.state = CONNECTED;

	        if (mqttConnectionObject.state == CONNECTED)
	        {

	        		 if(First_trig <(run_trig_time-intput_trig_time))
	        			 {
	        			 	int data_length;
	        			     uint8_t data_bucket[100];
	        			 	data_length = snprintf((char *)data_bucket, 120, "Gopaltest  %s: %d\r", imeii, i++);
	        			 	if(mqtt_pubmsg("test",(char *)data_bucket,data_length))
	        			 	{
	        			 		serialPrint("Triger 1  %d \r\n",(run_trig_time-intput_trig_time));


	        			 	}
	        			 	else
	        			 	{
	        			 		  sent_failed_count++;

	        			 	}
	        				  if(Gpscheck())
	        				  {
	        					  GPS_PRINT();
	        				  }
	        				  else
	        				  {

	        					  serialPrint("\nNot got GPS value\r\n");
	        				  }
	        				  HAL_Delay(2000);
	        			 intput_trig_time=run_trig_time;
	        			 }

	        }

	      }



  }
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

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */


int serialPrint(const char *string_format, ...)
{
  if (DEVEL)
  {
    return printf(string_format);
  }
  else
  {
    return 0;
  }
}

int __io_putchar(uint8_t ch)
{
  if (ch)
  {
    UART1PutChar(ch);
    return ch;
  }
  else
  {
    return 0;
  }
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
  __disable_irq();
  while (1)
  {
  }
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
