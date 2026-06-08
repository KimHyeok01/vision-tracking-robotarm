/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "ir_remote.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
#define	SET_ZERO	0.0f
#define	SET_CENTER	135.0f
#define	SET_LIMIT	270.0f

float M1_Arr[5] = { 90.0, 110.0,135.0,150.0,170.0};
float M2_Arr[5] = { 0.0, 15.0,45.0,60.0,90.0};

char tx_buffer[50];

uint32_t ir_val;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim2;
TIM_HandleTypeDef htim3;

UART_HandleTypeDef huart2;

volatile uint32_t ir_raw_buf[100]; // 데이터를 순서대로 담을 큰 상자
volatile uint8_t ir_idx = 0;       // 현재 몇 번째 방에 넣고 있는지 가리키는 주소록
volatile uint8_t rx_done = 0;      // "리모컨 한 패킷 수신이 끝났다"를 메인에 알릴 플래그

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_TIM2_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_TIM3_Init(void);
/* USER CODE BEGIN PFP */
void Servo_Set_Angle(uint32_t channel, float angle)
{
    if (angle < 0.0f)   angle = 0.0f;
    if (angle > 270.0f) angle = 270.0f;

    uint32_t pulse_width = (uint32_t)(500.0f + (angle * 2000.0f / 270.0f));

    __HAL_TIM_SET_COMPARE(&htim2, channel, pulse_width);
}

void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim){




  if(htim == &htim3)
      {
          if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
          {
              uint32_t total_period = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_1);
              uint32_t high_duration = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_2);

              if (total_period > 8000 && high_duration > 4000)
                          {
                              ir_idx = 0;   // 새 신호가 시작되었으니 저장 위치를 처음(0)으로 리셋!
                              rx_done = 0;  // 메인 루프에게 아직 수신 중이라고 알림
                          }
                          // [케이스 B] 리더 펄스가 아닌 일반 데이터 비트(1120us, 2250us 등)가 들어온 경우
              else
                          {
                              // 조건: 인덱스가 배열 크기(100)를 넘지 않도록 안전장치를 걸고 저장
                              if (ir_idx < 100)
                              {
                        	  ir_raw_buf[ir_idx++] = total_period;


                              }

                          }
          }
      }

  /*
  if(htim==&htim3)
    {
      if (htim->Channel == HAL_TIM_ACTIVE_CHANNEL_1)
	{
	  if(start_flag == 0 && 8000<(ir_val = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_1)))
	      {
		if(htim->Channel == HAL_TIM_ACTIVE_CHANNEL_2 && (4000<(ir_val = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_2))))
		  int len =sprintf(tx_buffer, "start\r\n");
	      	  HAL_UART_Transmit(&huart2, (uint8_t *)tx_buffer, len, 100);

	      }
	  //int len =sprintf(tx_buffer, "falling var: %d\r\n", ir_val);
	  //HAL_UART_Transmit(&huart2, (uint8_t *)tx_buffer, len, 100);
	}
      else{
	  ir_val = HAL_TIM_ReadCapturedValue(&htim3, TIM_CHANNEL_2);
	  //int len = sprintf(tx_buffer, "raising var: %d\r\n", ir_val);
	  //HAL_UART_Transmit(&huart2, (uint8_t *)tx_buffer, len, 100);
      }
    }*/
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

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
  MX_TIM2_Init();
  MX_USART2_UART_Init();
  MX_TIM3_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_1);
  HAL_TIM_PWM_Start(&htim2, TIM_CHANNEL_2);
  HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_1);
  HAL_TIM_IC_Start_IT(&htim3, TIM_CHANNEL_2);

  Servo_Set_Angle(TIM_CHANNEL_1,SET_CENTER);
  Servo_Set_Angle(TIM_CHANNEL_2,SET_ZERO);

  uint32_t debug_counter = 0;	// set tera term

  uint32_t last_ir_tick = 0;   // 마지막으로 IR 인터럽트가 발생한 시점의 틱 저장
    uint8_t last_ir_idx = 0;
    uint8_t target_key = 0;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {
    /*for(int i=0; i < 5; i++)
      {
	Servo_Set_Angle(TIM_CHANNEL_1,M1_Arr[i]);
	Servo_Set_Angle(TIM_CHANNEL_2,M2_Arr[i]);

	//int len = sprintf(tx_buffer, "STM32F103RB Alive! Count: %lu\r\n", debug_counter++);
	//HAL_UART_Transmit(&huart2, (uint8_t *)tx_buffer, len, 100);
	HAL_Delay(2000);
      }*/
      if (ir_idx != last_ir_idx)
            {
                last_ir_idx = ir_idx;
                last_ir_tick = HAL_GetTick(); // 최신 인터럽트 발생 시간 백업
            }

            // 2. 버퍼에 데이터가 쌓이기 시작했고(ir_idx > 0),
            //    마지막 인터럽트가 발생한 지 40ms가 지났다면? -> [논블로킹 완료 판정]
            if (ir_idx > 0 && (HAL_GetTick() - last_ir_tick > 40))
            {
                // 분리해 둔 소스파일 함수를 호출하여 8비트 Hex 키값 추출
                target_key = IR_Decode_Packet(ir_raw_buf, ir_idx);

                // 디버깅용 로그 출력
                int len = sprintf(tx_buffer, "\r\n[Decoded Key]: 0x%02X (Bits: %d)\r\n", target_key, ir_idx);
                HAL_UART_Transmit(&huart2, (uint8_t *)tx_buffer, len, 100);

                // 케이스문 진입 및 명령어 검증 출력
                switch (target_key)
                {
                    case IR_KEY_POWER:
                        len = sprintf(tx_buffer, ">> COMMAND: POWER BUTTON DETECTED <<\r\n");
                        HAL_UART_Transmit(&huart2, (uint8_t *)tx_buffer, len, 100);
                        break;

                    case IR_KEY_UP:
                        len = sprintf(tx_buffer, ">> COMMAND: MOVE UP <<\r\n");
                        HAL_UART_Transmit(&huart2, (uint8_t *)tx_buffer, len, 100);
                        break;

                    case IR_KEY_DOWN:
                        len = sprintf(tx_buffer, ">> COMMAND: MOVE DOWN <<\r\n");
                        HAL_UART_Transmit(&huart2, (uint8_t *)tx_buffer, len, 100);
                        break;

                    case IR_KEY_RIGHT:
                        len = sprintf(tx_buffer, ">> COMMAND: MOVE RIGHT <<\r\n");
                        HAL_UART_Transmit(&huart2, (uint8_t *)tx_buffer, len, 100);
                        break;

                    case IR_KEY_LEFT:
                        len = sprintf(tx_buffer, ">> COMMAND: MOVE LEFT <<\r\n");
                        HAL_UART_Transmit(&huart2, (uint8_t *)tx_buffer, len, 100);
                        break;

                    default:
                        len = sprintf(tx_buffer, ">> COMMAND: UNKNOWN KEY (0x%02X) <<\r\n", target_key);
                        HAL_UART_Transmit(&huart2, (uint8_t *)tx_buffer, len, 100);
                        break;
                }

                // 다음 패킷 수신을 위한 변수들 원점 복귀
                ir_idx = 0;
                last_ir_idx = 0;
            }
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
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
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
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

/**
  * @brief TIM2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM2_Init(void)
{

  /* USER CODE BEGIN TIM2_Init 0 */

  /* USER CODE END TIM2_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};

  /* USER CODE BEGIN TIM2_Init 1 */

  /* USER CODE END TIM2_Init 1 */
  htim2.Instance = TIM2;
  htim2.Init.Prescaler = 71;
  htim2.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim2.Init.Period = 19999;
  htim2.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim2.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim2, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim2) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim2, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 1500;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim2, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM2_Init 2 */

  /* USER CODE END TIM2_Init 2 */
  HAL_TIM_MspPostInit(&htim2);

}

/**
  * @brief TIM3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM3_Init(void)
{

  /* USER CODE BEGIN TIM3_Init 0 */

  /* USER CODE END TIM3_Init 0 */

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_SlaveConfigTypeDef sSlaveConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_IC_InitTypeDef sConfigIC = {0};

  /* USER CODE BEGIN TIM3_Init 1 */

  /* USER CODE END TIM3_Init 1 */
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 71;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 65535;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_IC_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sSlaveConfig.SlaveMode = TIM_SLAVEMODE_RESET;
  sSlaveConfig.InputTrigger = TIM_TS_TI1FP1;
  sSlaveConfig.TriggerPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sSlaveConfig.TriggerFilter = 0;
  if (HAL_TIM_SlaveConfigSynchro(&htim3, &sSlaveConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_FALLING;
  sConfigIC.ICSelection = TIM_ICSELECTION_DIRECTTI;
  sConfigIC.ICPrescaler = TIM_ICPSC_DIV1;
  sConfigIC.ICFilter = 0;
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigIC.ICPolarity = TIM_INPUTCHANNELPOLARITY_RISING;
  sConfigIC.ICSelection = TIM_ICSELECTION_INDIRECTTI;
  if (HAL_TIM_IC_ConfigChannel(&htim3, &sConfigIC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM3_Init 2 */

  /* USER CODE END TIM3_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : B1_Pin */
  GPIO_InitStruct.Pin = B1_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(B1_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LD2_Pin */
  GPIO_InitStruct.Pin = LD2_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LD2_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
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
