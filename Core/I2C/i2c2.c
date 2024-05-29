#include "i2c2.h"


I2C_HandleTypeDef hi2c2;

void I2C2_MspInit(I2C_HandleTypeDef *hi2c);

void I2C2_Init(void)
{
	if(HAL_I2C_RegisterCallback(&hi2c2, HAL_I2C_MSPINIT_CB_ID, I2C2_MspInit) != HAL_OK)
	{
		__DBG_LOG("Error: I2C2 registration MspInitCallback failed.\n");
		Error_Handler();
	}

	hi2c2.Instance = I2C2;
	hi2c2.Init.ClockSpeed = 100;
	hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;

	if (HAL_I2C_Init(&hi2c2) != HAL_OK)
	{
		__DBG_LOG("Error: I2C2 configure failed.\n");
		Error_Handler();
	}

	__DBG_LOG("Info: I2C2 configure successfully.\n");
}



void I2C2_MspInit(I2C_HandleTypeDef *hi2c)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};

	__I2C2_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();

	// PB10 - SCL alternate function open-drain.
	GPIO_InitStruct.Pin = GPIO_PIN_10;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	//GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	// PB11 - SDA alternate function open-drain.
	GPIO_InitStruct.Pin = GPIO_PIN_11;
	GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

	HAL_NVIC_SetPriority(I2C2_ER_IRQn, 0, 0);
	HAL_NVIC_EnableIRQ(I2C2_ER_IRQn);

	HAL_NVIC_SetPriority(I2C2_EV_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(I2C2_EV_IRQn);

}


void I2C2_EV_IRQHandler(void)
{
	HAL_I2C_EV_IRQHandler(&hi2c2);
}

void I2C2_ER_IRQHandler(void)
{
	HAL_I2C_ER_IRQHandler(&hi2c2);
}


