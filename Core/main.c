
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
#define AXIS_X_LEFT     890
#define AXIS_X_RIGHT    910
#define AXIS_Y_UP       230
#define AXIS_Y_DOWN     130

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


uint8_t data1[2] = {0};
uint8_t data2[2] = {0};

/**
 * @brief  The application entry point.
 * @retval int
 */
int main(void)
{
	HAL_Init();
	SystemClock_Config();
	MX_GPIO_Init();
	UART1_Init();
	DMA1_Init();
	ADC1_Init();
	I2C1_Init();
	I2C2_Init();
	MX_USB_DEVICE_Init();

	HAL_ADC_Start_DMA(&hadc1, adcBuf, 2);

	while (1)
	{
		HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);

		// Адрес 0x36 нужно смещать в лево на 1.
		HAL_I2C_Mem_Read(&hi2c1, 0x6c, 0x0C, 1, data1, 2, 1000);
		HAL_I2C_Mem_Read(&hi2c2, 0x6c, 0x0C, 1, data2, 2, 1000);

		


		bool Reverse = HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_2);
		//uint32_t AxisX = FiltrationValue[0];
		//uint32_t AxisY = FiltrationValue[1];

		// Младший байт приходит вторым.
		uint32_t AxisX = data1[1];
		AxisX |= data1[0] << 8;

		uint32_t AxisY = data2[1];
		AxisY |= data2[0] << 8;


		uint8_t Gear = BUTTON_NONE;
		static uint8_t GearLast = BUTTON_NONE;

		// Положение оси Х.
		bool AxisXLeft = (AxisX <= AXIS_X_LEFT);
		bool AxisXMiddle = (AxisX > AXIS_X_LEFT) && (AxisX < AXIS_X_RIGHT);
		bool AxisXRight = (AxisX >= AXIS_X_RIGHT);

		// Положение оси Y.
		bool AxisYUp = (AxisY >= AXIS_Y_UP);
		//bool AxisYMiddle = (AxisY < AXIS_Y_UP) && (AxisY > AXIS_Y_DOWN);
		bool AxisYDown = (AxisY <= AXIS_Y_DOWN);

		// Передача.
		if (Reverse) {
			if (AxisXRight && AxisYDown){
				Gear = BUTTON_7;
			}
		} else if (AxisXLeft && AxisYUp) {
			Gear = BUTTON_1;
		} else if (AxisXLeft && AxisYDown) {
			Gear = BUTTON_2;
		} else if (AxisXMiddle && AxisYUp) {
			Gear = BUTTON_3;
		} else if (AxisXMiddle && AxisYDown) {
			Gear = BUTTON_4;
		} else if (AxisXRight && AxisYUp) {
			Gear = BUTTON_5;
		} else if (AxisXRight && AxisYDown) {
			Gear = BUTTON_6;
		}
		else
		{
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
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV2;
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
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USB;
  PeriphClkInit.UsbClockSelection = RCC_USBCLKSOURCE_PLL_DIV1_5;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
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
	gpioInit.Pull = GPIO_PULLDOWN;
	gpioInit.Speed = GPIO_SPEED_LOW;
	HAL_GPIO_Init(GPIOA, &gpioInit);
}

/* USER CODE BEGIN 4 */

void HAL_MspInit(void)
{
  /* USER CODE BEGIN MspInit 0 */

  /* USER CODE END MspInit 0 */

  __HAL_RCC_AFIO_CLK_ENABLE();
  __HAL_RCC_PWR_CLK_ENABLE();

  /* System interrupt init*/

	__HAL_AFIO_REMAP_SWJ_NOJTAG();

  /* USER CODE BEGIN MspInit 1 */

  /* USER CODE END MspInit 1 */
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
