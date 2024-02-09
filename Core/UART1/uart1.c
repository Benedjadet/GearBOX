
#include "uart1.h"

UART_HandleTypeDef huart1;

void UART1_MspInit(UART_HandleTypeDef *uartHandle);

void UART1_Init(void)
{

	//test_uart_for_gd32f30x();

	if(HAL_UART_RegisterCallback(&huart1, HAL_UART_MSPINIT_CB_ID, UART1_MspInit) != HAL_OK)
	{
		__DBG_LOG("Error: UART1 registration MspInitCallback failed.\n");
		Error_Handler();
	}

	huart1.Instance = USART1;	// TODO: Сделать чтобы это значение бралось из конфигурационного файла модуля.
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
		__DBG_LOG("Error: UART1 initialization failed.\n");
		Error_Handler();
	}

	__DBG_LOG("Info: UART1 initialized successfilly.\n");
}

void UART1_MspInit(UART_HandleTypeDef *uartHandle)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__HAL_RCC_USART1_CLK_ENABLE();
	__HAL_RCC_GPIOA_CLK_ENABLE();

	// PA9 - TX alternate function pull-up
	GPIO_InitStruct.Pin = GPIO_PIN_9;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
	//GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	// PA10 - RX alternate function input
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(USART1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(USART1_IRQn);
}