/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body - Music + LED Effects
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
	C4, C5, D5, E5, F5, G5, A5, B5, C6, D6, E6, F6, G6, A6, B6,
	Ds5, Fs5, As5, Cs6, Ds6, Fs6, As6,
	MAX_VALUE
} soundToneType;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define CS43L22_I2C_ADDRESS		0x94
#define I2C_TIMEOUT				10
/* USER CODE END PD */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;
I2S_HandleTypeDef hi2s3;
DMA_HandleTypeDef hdma_spi3_tx;

/* USER CODE BEGIN PV */
int16_t dataI2S[100] = {0};
const uint32_t full_note_duration = 500;
const uint32_t half_note_duration = full_note_duration >> 1;
const uint32_t quarter_note_duration = full_note_duration >> 2;
const uint32_t eighth_note_duration = full_note_duration >> 3;
uint8_t play_count = 0;
const uint8_t max_plays = 3;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_I2S3_Init(void);

/* USER CODE BEGIN PFP */
void CS43L22_Init(void);
void CS43L22_Beep(soundToneType pitch, uint32_t duration_ms);
/* USER CODE END PFP */

/* USER CODE BEGIN 0 */

void CS43L22_Init(void)
{
	// Enable chip
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_SET);

	uint8_t TxBuffer[2];

	TxBuffer[0] = 0x0D; TxBuffer[1] = 0x01;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x00; TxBuffer[1] = 0x99;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x47; TxBuffer[1] = 0x80;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x32; TxBuffer[1] = 0xFF;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x32; TxBuffer[1] = 0x7F;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x00; TxBuffer[1] = 0x00;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x04; TxBuffer[1] = 0xAF;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x0D; TxBuffer[1] = 0x70;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x05; TxBuffer[1] = 0x81;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x06; TxBuffer[1] = 0x07;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x0A; TxBuffer[1] = 0x00;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x27; TxBuffer[1] = 0x00;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x1A; TxBuffer[1] = 0x0A;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x1B; TxBuffer[1] = 0x0A;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x1F; TxBuffer[1] = 0x0F;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	TxBuffer[0] = 0x02; TxBuffer[1] = 0x9E;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);
}

void CS43L22_Beep(soundToneType pitch, uint32_t duration_ms)
{
	uint8_t TxBuffer[2];

	// Set volume and off time
	TxBuffer[0] = 0x1D;
	TxBuffer[1] = 0x00;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	// Set sound frequency
	TxBuffer[0] = 0x1C;
	TxBuffer[1] = (pitch << 4);
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	// Enable continuous mode
	TxBuffer[0] = 0x1E;
	TxBuffer[1] = 0xC0;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);

	// Playing...
	HAL_Delay(duration_ms);

	// Disable continuous mode
	TxBuffer[0] = 0x1E;
	TxBuffer[1] = 0x00;
	HAL_I2C_Master_Transmit(&hi2c1, CS43L22_I2C_ADDRESS, (uint8_t*) &TxBuffer, 2, I2C_TIMEOUT);
}

/* USER CODE END 0 */

int main(void)
{
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	MX_DMA_Init();
	MX_I2C1_Init();
	MX_I2S3_Init();

	CS43L22_Init();
	HAL_I2S_Transmit_DMA(&hi2s3, (uint16_t *)dataI2S, 100);


	while (1)
	{
	    if(play_count >= max_plays)
	    {
	        for(int dance = 0; dance < 6; dance++)
	        {
	            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_14, GPIO_PIN_SET);
	            HAL_Delay(80);
	            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_14, GPIO_PIN_RESET);
	            HAL_Delay(30);

	            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13|GPIO_PIN_15, GPIO_PIN_SET);
	            HAL_Delay(80);
	            HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13|GPIO_PIN_15, GPIO_PIN_RESET);
	            HAL_Delay(30);
	        }

	        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_SET);
	        HAL_Delay(300);
	        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);
	        HAL_Delay(2000);
	        continue;
	    }

	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
	    CS43L22_Beep(C5, quarter_note_duration);
	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
	    HAL_Delay(50);

	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_SET);
	    CS43L22_Beep(C5, quarter_note_duration);
	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12, GPIO_PIN_RESET);
	    HAL_Delay(50);

	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
	    CS43L22_Beep(G5, quarter_note_duration);
	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
	    HAL_Delay(50);

	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
	    CS43L22_Beep(G5, quarter_note_duration);
	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
	    HAL_Delay(50);

	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
	    CS43L22_Beep(A5, quarter_note_duration);
	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
	    HAL_Delay(50);

	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_SET);
	    CS43L22_Beep(A5, quarter_note_duration);
	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_14, GPIO_PIN_RESET);
	    HAL_Delay(50);

	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13|GPIO_PIN_14, GPIO_PIN_SET);
	    CS43L22_Beep(G5, half_note_duration);
	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13|GPIO_PIN_14, GPIO_PIN_RESET);
	    HAL_Delay(100);

	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
	    CS43L22_Beep(F5, quarter_note_duration);
	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
	    HAL_Delay(50);

	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
	    CS43L22_Beep(F5, quarter_note_duration);
	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
	    HAL_Delay(50);

	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_15, GPIO_PIN_SET);
	    CS43L22_Beep(E5, quarter_note_duration);
	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_15, GPIO_PIN_RESET);
	    HAL_Delay(50);

	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_15, GPIO_PIN_SET);
	    CS43L22_Beep(E5, quarter_note_duration);
	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_15, GPIO_PIN_RESET);
	    HAL_Delay(50);

	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
	    CS43L22_Beep(D5, quarter_note_duration);
	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
	    HAL_Delay(50);

	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
	    CS43L22_Beep(D5, quarter_note_duration);
	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
	    HAL_Delay(50);

	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_SET);
	    CS43L22_Beep(C5, half_note_duration);
	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);
	    HAL_Delay(200);

	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
	    CS43L22_Beep(F5, quarter_note_duration);
	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
	    HAL_Delay(50);

	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_SET);
	    CS43L22_Beep(F5, quarter_note_duration);
	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_15, GPIO_PIN_RESET);
	    HAL_Delay(50);

	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_15, GPIO_PIN_SET);
	    CS43L22_Beep(E5, quarter_note_duration);
	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_15, GPIO_PIN_RESET);
	    HAL_Delay(50);

	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_15, GPIO_PIN_SET);
	    CS43L22_Beep(E5, quarter_note_duration);
	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_15, GPIO_PIN_RESET);
	    HAL_Delay(50);

	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
	    CS43L22_Beep(D5, quarter_note_duration);
	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
	    HAL_Delay(50);

	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_SET);
	    CS43L22_Beep(D5, quarter_note_duration);
	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_13, GPIO_PIN_RESET);
	    HAL_Delay(50);

	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13, GPIO_PIN_SET);
	    CS43L22_Beep(C5, half_note_duration);
	    HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13, GPIO_PIN_RESET);
	    HAL_Delay(200);

	    for(int flash = 0; flash < 3; flash++)
	    {
	        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_SET);
	        HAL_Delay(80);
	        HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);
	        HAL_Delay(80);
	    }

	    play_count++;
	    HAL_Delay(1500);
	}
}

static void MX_GPIO_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_4, GPIO_PIN_RESET);
	HAL_GPIO_WritePin(GPIOD, GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15, GPIO_PIN_RESET);

	/*Configure GPIO pins : PD4, PD12, PD13, PD14, PD15 */
	GPIO_InitStruct.Pin = GPIO_PIN_4|GPIO_PIN_12|GPIO_PIN_13|GPIO_PIN_14|GPIO_PIN_15;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOD, &GPIO_InitStruct);
}

void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
	RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

	__HAL_RCC_PLL_PLLM_CONFIG(16);
	__HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSI);
	__HAL_RCC_PWR_CLK_ENABLE();
	__HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
	RCC_OscInitStruct.HSIState = RCC_HSI_ON;
	RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
		Error_Handler();
	}

	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
		Error_Handler();
	}
}

static void MX_I2C1_Init(void)
{
	hi2c1.Instance = I2C1;
	hi2c1.Init.ClockSpeed = 100000;
	hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hi2c1.Init.OwnAddress1 = 0;
	hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hi2c1.Init.OwnAddress2 = 0;
	hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	if (HAL_I2C_Init(&hi2c1) != HAL_OK)
	{
		Error_Handler();
	}
}

static void MX_I2S3_Init(void)
{
	hi2s3.Instance = SPI3;
	hi2s3.Init.Mode = I2S_MODE_MASTER_TX;
	hi2s3.Init.Standard = I2S_STANDARD_PHILIPS;
	hi2s3.Init.DataFormat = I2S_DATAFORMAT_16B;
	hi2s3.Init.MCLKOutput = I2S_MCLKOUTPUT_ENABLE;
	hi2s3.Init.AudioFreq = I2S_AUDIOFREQ_48K;
	hi2s3.Init.CPOL = I2S_CPOL_LOW;
	hi2s3.Init.ClockSource = I2S_CLOCK_PLL;
	hi2s3.Init.FullDuplexMode = I2S_FULLDUPLEXMODE_DISABLE;
	if (HAL_I2S_Init(&hi2s3) != HAL_OK)
	{
		Error_Handler();
	}
}

static void MX_DMA_Init(void)
{
	__HAL_RCC_DMA1_CLK_ENABLE();
	HAL_NVIC_SetPriority(DMA1_Stream5_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(DMA1_Stream5_IRQn);
}

void Error_Handler(void)
{
	__disable_irq();
	while (1) {}
}
