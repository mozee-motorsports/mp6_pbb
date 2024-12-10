/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "adc.h"
#include "fdcan.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "stm32g4xx_hal_uart.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
int __io_putchar(int ch)
{
    HAL_UART_Transmit(&hlpuart1, (uint8_t *)&ch, 1, HAL_MAX_DELAY);  // Send character via LPUART1
    return ch;  // Return the character (required by the standard)
}
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
  MX_ADC1_Init();
  MX_FDCAN1_Init();
  MX_LPUART1_UART_Init();
  /* USER CODE BEGIN 2 */
  // Need to modify MX_ADC1_Init function to accomodate two channels

  // Configure GPIO pins for ADC1_IN8 (PC2) and ADC1_IN9 (PC3)
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  __HAL_RCC_GPIOC_CLK_ENABLE();

  // Configure PC2 (ADC1_IN8) as analog input
  GPIO_InitStruct.Pin = GPIO_PIN_2;  // PC2
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  // Configure PC3 (ADC1_IN9) as analog input
  GPIO_InitStruct.Pin = GPIO_PIN_3;  // PC3
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  // FDCAN1 pins configuration for TX and RX
  __HAL_RCC_GPIOA_CLK_ENABLE();
  GPIO_InitStruct.Pin = GPIO_PIN_12; // FDCAN1_TX Pin
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  GPIO_InitStruct.Pin = GPIO_PIN_11; // FDCAN1_RX Pin
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF9_FDCAN1;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // PA2 (LPUART1_TX)
  GPIO_InitStruct.Pin = GPIO_PIN_2;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; // Alternate function push-pull mode
  GPIO_InitStruct.Pull = GPIO_NOPULL;     // No pull-up or pull-down resistors
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF8_LPUART1; // Alternate function for LPUART1 TX
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // PA3 (LPUART1_RX)
  GPIO_InitStruct.Pin = GPIO_PIN_3;
  GPIO_InitStruct.Mode = GPIO_MODE_AF_PP; // Alternate function push-pull mode
  GPIO_InitStruct.Pull = GPIO_NOPULL;     // No pull-up or pull-down resistors
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF8_LPUART1; // Alternate function for LPUART1 RX
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  // Start ADC
  HAL_ADC_Start(&hadc1);  // Start the ADC
  HAL_FDCAN_Start(&hfdcan1);  // Start FDCAN1 communication

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_ADC_Start(&hadc1);  // Start the ADC

  while (1)
  {
	  // declare variables
	  uint32_t adcValue1;
	  uint32_t adcValue2;
      // Configure and read from ADC1_IN8 (Pot 1)
    if (HAL_ADC_PollForConversion(&hadc1, 1000000) == HAL_OK)
    {
            adcValue1 = HAL_ADC_GetValue(&hadc1);
    }


            // Stop the ADC and configure it for ADC1_IN9 (Pot 2)
            HAL_ADC_Stop(&hadc1);

            ADC_ChannelConfTypeDef sConfig = {0};
            sConfig.Channel = ADC_CHANNEL_9; // ADC1_IN9
            sConfig.Rank = ADC_REGULAR_RANK_1;
            sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES_5;
            sConfig.SingleDiff = ADC_SINGLE_ENDED;
            HAL_ADC_ConfigChannel(&hadc1, &sConfig);

            // Start ADC conversion for ADC1_IN9 (Pot 2)
            HAL_ADC_Start(&hadc1);
            if (HAL_ADC_PollForConversion(&hadc1, 1000000) == HAL_OK)
            {
            //	uint32_t adcValue2;
                adcValue2 = HAL_ADC_GetValue(&hadc1); // Pot 2 value

                // inverting pot 2 value (assuming it's wired backwards)
                adcValue2 = 4095 - adcValue2;
            }

            // Convert ADC values to percentage
            float percentage1;
            float percentage2;

            percentage1 = ((float)adcValue1 / 4095) * 100;  // For ADC1_IN8 (PC2)
            percentage2 = ((float)adcValue2 / 4095) * 100;  // For ADC1_IN9 (PC3)


            // check if percentages are off?
            if((percentage1 >= percentage2 + 5) || (percentage1 <= percentage2 - 5))
            {
            	// call error message?
            	// Error_Handler();  ? Maybe???
            }


     /*       // output to CAN
            FDCAN_TxHeaderTypeDef TxHeader;
            uint8_t canData;  // CAN data payload (one byte)

            // CAN frame setup
            TxHeader.Identifier = 0x321;  // CAN identifier (can be any valid identifier)
            TxHeader.DLC = 1;  // Data length code (DLC), sending 1 byte
            TxHeader.TransmitGlobalTime = DISABLE;  // No global time

            // Prepare data for CAN: Convert percentage1 to an 8-bit integer
            int8_t percentage1Int = (int8_t)(percentage1 * 100);  // Convert percentage1 to an integer (range: -128 to 127)

            // Load the CAN data
            canData = percentage1Int;  // Store the 8-bit integer value of percentage1 in canData

            // Send the CAN message
            if (HAL_FDCAN_AddMessageToTxFifoQ(&hfdcan1, &TxHeader, &canData) != HAL_OK)
            {
                Error_Handler();  // Handle error if CAN message can't be added to FIFO
            }
	*/

            // output to LPUART -- No clue
            printf("Percentage 1: %.2f%%, Percentage 2: %.2f%%\r\n", percentage1, percentage2);

        }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  return 0;
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

  /** Configure the main internal regulator output voltage
  */
  HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV2;
  RCC_OscInitStruct.PLL.PLLN = 8;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
  RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;
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
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

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
