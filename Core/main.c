
#include "main.h"
#include "usb_device.h"

#include "usbd_hid.h"
#include "uart1.h"
#include "adc.h"
#include "dma.h"
#include "i2c1.h"
#include "i2c2.h"
#include <stdbool.h>

// H-shifter mode analog axis thresholds
#define AXIS_X_LEFT     1560
#define AXIS_X_RIGHT    1480
#define AXIS_Y_UP       720
#define AXIS_Y_DOWN     1000

#define BUTTON_NONE     -1
#define BUTTON_1         0
#define BUTTON_2         1
#define BUTTON_3         2
#define BUTTON_4         3
#define BUTTON_5         4
#define BUTTON_6         5
#define BUTTON_7         6
#define BUTTON_8		 7

#define FILTRATION_LEVEL 100

extern USBD_HandleTypeDef hUsbDeviceFS;

uint32_t adcBuf[2];
uint32_t FiltrationValue[2];
void SystemClock_Config(void);
//int filtration(uint8_t pin, uint8_t count);
static void MX_GPIO_Init(void);


uint8_t AxisXraw[2] = {0};
uint8_t AxisYraw[2] = {0};

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	HAL_Init();
	HAL_IncTick();
	SystemClock_Config();
	MX_GPIO_Init();
	UART1_Init();
	DMA1_Init();
	//ADC1_Init();
	I2C1_Init();
	I2C2_Init();
	MX_USB_DEVICE_Init();

	//HAL_ADC_Start_DMA(&hadc1, adcBuf, 2);

	while (1)
	{
		//HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

		// Адрес 0x36 нужно смещать в лево на 1.
		HAL_I2C_Mem_Read(&hi2c2, 0x6c, 0x0C, 1, AxisXraw, 2, 1000);
		HAL_I2C_Mem_Read(&hi2c1, 0x6c, 0x0C, 1, AxisYraw, 2, 1000);

		bool Reverse = !HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2);
		//uint32_t AxisX = FiltrationValue[0];
		//uint32_t AxisY = FiltrationValue[1];

		// Младший байт приходит вторым.
		uint32_t AxisX = AxisXraw[1];
		AxisX |= AxisXraw[0] << 8;

		uint32_t AxisY = AxisYraw[1];
		AxisY |= AxisYraw[0] << 8;


		uint8_t Gear = BUTTON_NONE;
		static uint8_t GearLast = BUTTON_NONE;

		// Положение оси Х.
		bool AxisXLeft = (AxisX >= AXIS_X_LEFT);
		bool AxisXMiddle = (AxisX < AXIS_X_LEFT) && (AxisX > AXIS_X_RIGHT);
		bool AxisXRight = (AxisX <= AXIS_X_RIGHT);

		// Положение оси Y.
		bool AxisYUp = (AxisY <= AXIS_Y_UP);
		//bool AxisYMiddle = (AxisY < AXIS_Y_UP) && (AxisY > AXIS_Y_DOWN);
		bool AxisYDown = (AxisY >= AXIS_Y_DOWN);

		// Передача.
		if (AxisXLeft && AxisYUp) {
			Gear = BUTTON_1;
		} else if (AxisXLeft && AxisYDown) {
			Gear = BUTTON_2;
		} else if (AxisXMiddle && AxisYUp) {
			Gear = BUTTON_3;
		} else if (AxisXMiddle && AxisYDown) {
			Gear = BUTTON_4;
		} else if (AxisXRight && AxisYUp) {
			Gear = BUTTON_5;
		} else if (AxisXRight && AxisYDown && !Reverse) {
			Gear = BUTTON_6;
		} else if (AxisXRight && AxisYDown && Reverse){
			Gear = BUTTON_7;
		} else {
			Gear = BUTTON_8;
		}

		if (Gear != GearLast ) {
			GearLast = Gear;

			uint8_t GearBit = 1 << Gear;

			USBD_HID_SendReport(&hUsbDeviceFS, &GearBit, 1);


			printf("Gear %d\n", Gear);
		}

		printf("X %ld Y %ld\n\r", AxisX, AxisY);
		HAL_Delay(100);

	}

}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
  RCC_CRSInitTypeDef RCC_CRSInitStruct = {0};

  __HAL_RCC_HSI48_ENABLE();
  while(__HAL_RCC_GET_FLAG(RCC_FLAG_HSI48RDY) == RESET );

  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
    |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI48;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1);

  __HAL_RCC_USB_CONFIG( RCC_USBCLKSOURCE_HSI48 );

  /* CRS */
  __HAL_RCC_CRS_CLK_ENABLE();
  
  RCC_CRSInitStruct.Prescaler = RCC_CRS_SYNC_DIV1;
  RCC_CRSInitStruct.Source = RCC_CRS_SYNC_SOURCE_USB;
  RCC_CRSInitStruct.Polarity = RCC_CRS_SYNC_POLARITY_RISING;
  RCC_CRSInitStruct.ReloadValue = __HAL_RCC_CRS_RELOADVALUE_CALCULATE( 48000000, 1000 );
  RCC_CRSInitStruct.ErrorLimitValue = RCC_CRS_ERRORLIMIT_DEFAULT;
  RCC_CRSInitStruct.HSI48CalibrationValue = RCC_CRS_HSI48CALIBRATION_DEFAULT;

  HAL_RCCEx_CRSConfig( &RCC_CRSInitStruct );
}

/**
 * @brief GPIO Initialization Function
 * @param None
 * @retval None
 */
static void MX_GPIO_Init(void)
{

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOC_CLK_ENABLE();
	__HAL_RCC_GPIOD_CLK_ENABLE();

	GPIO_InitTypeDef gpioInit = {0};

	gpioInit.Pin = GPIO_PIN_13;
	gpioInit.Mode = GPIO_MODE_OUTPUT_PP;
	gpioInit.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOC, &gpioInit);


	gpioInit.Pin = GPIO_PIN_2;
	gpioInit.Mode = GPIO_MODE_INPUT;
	gpioInit.Pull = GPIO_PULLUP;
	gpioInit.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOA, &gpioInit);
}

/* USER CODE BEGIN 4 */

void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  //__HAL_RCC_AFIO_CLK_ENABLE();
  //__HAL_RCC_PWR_CLK_ENABLE();

  /* System interrupt init*/

	//__HAL_AFIO_REMAP_SWJ_NOJTAG();

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */


	__HAL_RCC_SYSCFG_CLK_ENABLE();
	__HAL_RCC_PWR_CLK_ENABLE();

	__HAL_RCC_GPIOF_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();


}


void DMA1_Cplt_Callback( ADC_HandleTypeDef *hadc)
{
	//vMainAdcInerrupt();

	static uint32_t sum[2] = {0, 0};
	static uint32_t counter = 0;

	sum[0] += adcBuf[0];
	sum[1] += adcBuf[1];

	counter++;
	if(counter > FILTRATION_LEVEL)
	{
		FiltrationValue[0] = sum[0]/FILTRATION_LEVEL;
		FiltrationValue[1] = sum[1]/FILTRATION_LEVEL;

		sum[0] = 0;
		sum[1] = 0;
		counter = 0;
	}

	HAL_ADC_Start_DMA(hadc, adcBuf, 2);
}



// int filtration(uint8_t pin, uint8_t count)
// {
// 	uint32_t summ = 0;

// 	for(uint8_t i = 0; i<count; i++)
// 	{
// 		summ += analogRead(pin);
// 	}

// 	return summ / count;
// }



#ifdef USE_FULL_ASSERT
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
