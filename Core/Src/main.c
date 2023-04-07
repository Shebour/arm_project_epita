/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 ******************************************************************************
 * @attention
 *
 * Copyright (c) 2023 STMicroelectronics.
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
#include "mbedtls.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "mbedtls/aes.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "stm32f4xx_ll_dma.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define BUFFER_SIZE 512
#define KEY_SIZE_BITS 256
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
TIM_HandleTypeDef htim10;
TIM_HandleTypeDef htim11;

UART_HandleTypeDef huart2;
DMA_HandleTypeDef hdma_usart2_rx;
DMA_HandleTypeDef hdma_usart2_tx;

/* USER CODE BEGIN PV */
enum STATE { IDLE = 0, WAITING, COMMUNICATING };

enum Command { GEN = 0, ENC, DEC };

struct __attribute__((__packed__)) header {
  uint8_t cmd;
  size_t payload_length;
};

enum STATE state = IDLE;
UART_HandleTypeDef *huart_cb;
uint8_t header_str[5] = {0};
uint8_t data[BUFFER_SIZE] = {0};
uint8_t crypted_data[BUFFER_SIZE] = {0};
struct header *head = {0};
__attribute__((section(".reserved"))) unsigned char key[32];
int key_present = 0;
unsigned char key_tmp[32] = {0};
unsigned char iv[16] = {0};
char iv_data[16 + BUFFER_SIZE] = {0};

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_TIM10_Init(void);
static void MX_TIM11_Init(void);
static void MX_USART2_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

char *_strncpy(char *dest, const char *src, size_t n) {
  if (n == 0)
    return dest;
  char *d = dest;
  const char *s = src;
  while (n > 0) {
    *d++ = *s++;
    n--;
  }
  return dest;
}

int generate_key() {
  mbedtls_ctr_drbg_context ctr_drbg;
  mbedtls_entropy_context entropy;

  mbedtls_entropy_init(&entropy);

  mbedtls_ctr_drbg_init(&ctr_drbg);

  mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0);

  if (mbedtls_ctr_drbg_random_with_add(&ctr_drbg, key_tmp, 32,
                                       (const unsigned char *)HAL_GetTick(),
                                       1) != 0) {
    return 0;
  }
  if (mbedtls_ctr_drbg_random_with_add(
          &ctr_drbg, iv, 16, (const unsigned char *)HAL_GetTick(), 1) != 0)
    return 0;
  return 1;
}

int encrypt() {
  if (!key_present)
    return 0;
  mbedtls_aes_context aes;
  if (mbedtls_aes_setkey_enc(&aes, key_tmp, KEY_SIZE_BITS)) {
    return 0;
  }
  _strncpy(iv_data, (char *)iv, 16);
  // HAL_UART_Transmit_DMA(&huart2, (uint8_t *)iv, 16);
  if (mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, BUFFER_SIZE, iv, data,
                            crypted_data))
    return 0;
  // mbedtls_aes_encrypt(&aes, buffer, crypted_tmp);
  _strncpy(&iv_data[16], (char *)crypted_data, BUFFER_SIZE);
  return 1;
}

int decrypt() {
  if (!key_present)
    return 0;
  mbedtls_aes_context aes;
  if (mbedtls_aes_setkey_dec(&aes, key_tmp, KEY_SIZE_BITS)) {
    return 0;
  }
  _strncpy((char *)iv, iv_data, 16);
  _strncpy((char *)data, &iv_data[16], BUFFER_SIZE);
  if (mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, BUFFER_SIZE, iv, data,
                            crypted_data))
    return 0;
  return 1;
}

void parse_header() {
  if (head->cmd == GEN) {
    if (!generate_key()) {
      key_present = 0;
      HAL_UART_Transmit_DMA(&huart2, (uint8_t *)"KO", 2);
    } else {
      key_present = 1;
      HAL_UART_Transmit_DMA(&huart2, (uint8_t *)"OK", 2);
    }
    state = IDLE;
    HAL_UART_Receive_DMA(&huart2, header_str, 5);
    HAL_TIM_Base_Start_IT(&htim10);

  } else if (head->cmd == ENC) {
    state = COMMUNICATING;
    HAL_UART_Receive_DMA(&huart2, data, BUFFER_SIZE);
  } else if (head->cmd == DEC) {
    state = COMMUNICATING;
    HAL_UART_Receive_DMA(&huart2, (uint8_t *)iv_data, BUFFER_SIZE + 16);
  }
}

void communicate() {
  if (head->cmd == ENC) {
    if (!encrypt()) {
      HAL_UART_Transmit_DMA(&huart2, data, BUFFER_SIZE);
    } else {
      HAL_UART_Transmit_DMA(&huart2, (uint8_t *)iv_data, BUFFER_SIZE + 16);
    }
  } else if (head->cmd == DEC) {
    if (!decrypt()) {
      HAL_UART_Transmit_DMA(&huart2, (uint8_t *)iv_data, BUFFER_SIZE);
    } else {
      HAL_UART_Transmit_DMA(&huart2, crypted_data, BUFFER_SIZE);
    }
  }
}

// callback de reception sur l'UART
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  if (state == IDLE) {
    HAL_UART_Receive_DMA(&huart2, header_str, 5);
    return;
  }

  HAL_TIM_Base_Stop_IT(&htim11);
  huart_cb = huart; // save UART pour transmission
  if (state == WAITING) {
    head = (struct header *)header_str;
    parse_header();
  } else if (state == COMMUNICATING) {
    head->payload_length -= 1;
    communicate();
    if (head->payload_length == 0) {
      state = IDLE;
      HAL_UART_Receive_DMA(&huart2, header_str, 5);
      HAL_TIM_Base_Start_IT(&htim10);
    } else
      HAL_UART_Receive_DMA(&huart2, data, BUFFER_SIZE);
  }
}

// callback de fin de timer
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
  if (htim == &htim10 && state == IDLE) {
    HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
  }
  if (htim == &htim11 && state == WAITING) {
    HAL_TIM_Base_Start_IT(&htim10);
    HAL_TIM_Base_Stop_IT(&htim11);
    state = IDLE;
  }
}

/*
 * callback lors de l'appui du bouton
 * Stopper le timer du clignotement de led
 * Allumer la led
 * Demarrer le timer de 15sec pour la communication
 */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  if (GPIO_Pin == BUTTON_Pin && state == IDLE) {
    HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
    HAL_TIM_Base_Stop_IT(&htim10);
    HAL_TIM_Base_Start_IT(&htim11);
    state = WAITING;
  }
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
  MX_DMA_Init();
  MX_TIM10_Init();
  MX_TIM11_Init();
  MX_USART2_UART_Init();
  MX_MBEDTLS_Init();
  /* USER CODE BEGIN 2 */
  HAL_TIM_Base_Start_IT(&htim10);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  HAL_UART_Receive_DMA(&huart2, header_str,
                       5); // read header of fixed size
  while (1) {
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 84;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
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
  * @brief TIM10 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM10_Init(void)
{

  /* USER CODE BEGIN TIM10_Init 0 */

  /* USER CODE END TIM10_Init 0 */

  /* USER CODE BEGIN TIM10_Init 1 */

  /* USER CODE END TIM10_Init 1 */
  htim10.Instance = TIM10;
  htim10.Init.Prescaler = 2999;
  htim10.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim10.Init.Period = 6999;
  htim10.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim10.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim10) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM10_Init 2 */

  /* USER CODE END TIM10_Init 2 */

}

/**
  * @brief TIM11 Initialization Function
  * @param None
  * @retval None
  */
static void MX_TIM11_Init(void)
{

  /* USER CODE BEGIN TIM11_Init 0 */

  /* USER CODE END TIM11_Init 0 */

  /* USER CODE BEGIN TIM11_Init 1 */

  /* USER CODE END TIM11_Init 1 */
  htim11.Instance = TIM11;
  htim11.Init.Prescaler = 25199;
  htim11.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim11.Init.Period = 49999;
  htim11.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim11.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim11) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN TIM11_Init 2 */

  /* USER CODE END TIM11_Init 2 */

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
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Stream5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
  /* DMA1_Stream6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Stream6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(DMA1_Stream6_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : BUTTON_Pin */
  GPIO_InitStruct.Pin = BUTTON_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(BUTTON_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

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
  /* User can add his own implementation to report the HAL error return state
   */
  __disable_irq();
  while (1) {
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
  /* User can add his own implementation to report the file name and line
     number, ex: printf("Wrong parameters value: file %s on line %d\r\n",
     file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
