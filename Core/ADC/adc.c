
#include "adc.h"

ADC_HandleTypeDef hadc1;	// Экземпляр объекта АЦП.

void ADC1_MspInit(ADC_HandleTypeDef *hadc);


/* Инициализация АЦП */
void ADC1_Init(void)
{
	ADC_ChannelConfTypeDef sConfig;


	if(HAL_ADC_RegisterCallback(&hadc1, HAL_ADC_MSPINIT_CB_ID, ADC1_MspInit) != HAL_OK)
	{
		__DBG_LOG("Error: ADC1 registration MspInitCallback failed.\n");
		Error_Handler();
	}


	/* Параметры модуля */
	hadc1.Instance = ADC1;
	hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
	hadc1.Init.ScanConvMode = ADC_SCAN_ENABLE;
	hadc1.Init.ContinuousConvMode = DISABLE;
	//hadc1.Init.NbrOfConversion = 2;
	hadc1.Init.DiscontinuousConvMode = DISABLE;
	hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;


	if (HAL_ADC_Init(&hadc1) != HAL_OK)
	{
		__DBG_LOG("Error: ADC init failed.\n");
		Error_Handler();
	}


	if (HAL_ADC_RegisterCallback(&hadc1, HAL_ADC_CONVERSION_COMPLETE_CB_ID, DMA1_Cplt_Callback))
	{
		__DBG_LOG("Error: ADC Registration DMA1_Cplt_Callback failed.\n");
		Error_Handler();
	}



	/* Настройка канала IN0 (AI1) */
	sConfig.Channel = ADC_CHANNEL_0;
	//sConfig.Rank = ADC_RANK_CHANNEL_NUMBER;
	sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		__DBG_LOG("Error: Chanel 0 configure failed.\n");
		Error_Handler();
	}

	/* Настройка канала IN1 (AI2) */
	sConfig.Channel = ADC_CHANNEL_1;
	//sConfig.Rank = ADC_REGULAR_RANK_2;
	sConfig.SamplingTime = ADC_SAMPLETIME_55CYCLES_5;
	if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
	{
		__DBG_LOG("Error: Chanel 1 configure failed.\n");
		Error_Handler();
	}

	__DBG_LOG("Info: ADC configured successfully.\n");
}

void ADC1_MspInit(ADC_HandleTypeDef *hadc)
{
	if (hadc->Instance == ADC1)
	{
		__HAL_RCC_ADC1_CLK_ENABLE();
		//RCC_PeriphCLKInitTypeDef PeriphClkInit;
		//PeriphClkInit.PeriphClockSelection = RCC;
		//PeriphClkInit.AdcClockSelection = RCC_ADCPCLK2_DIV6;			// ADC_CLK = 12MHz
		//if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
		//{
		//	__DBG_LOG("Error: ADC clock configure failed.\n");
		//	Error_Handler();
		//}

		HAL_NVIC_SetPriority(ADC1_IRQn, 0, 0);
		HAL_NVIC_EnableIRQ(ADC1_IRQn);

		/* ADC1 DMA Init */
		hdma_adc1.Instance = DMA1_Channel1;
		hdma_adc1.Init.Direction = DMA_PERIPH_TO_MEMORY;
		hdma_adc1.Init.PeriphInc = DMA_PINC_DISABLE;
		hdma_adc1.Init.MemInc = DMA_MINC_ENABLE;
		hdma_adc1.Init.PeriphDataAlignment = DMA_PDATAALIGN_WORD;
		hdma_adc1.Init.MemDataAlignment = DMA_MDATAALIGN_WORD;
		hdma_adc1.Init.Mode = DMA_CIRCULAR;
		hdma_adc1.Init.Priority = DMA_PRIORITY_MEDIUM;
		if (HAL_DMA_Init(&hdma_adc1) != HAL_OK)
		{
			__DBG_LOG("Error: DMA for ADC configure failed.\n");
			Error_Handler();
		}

		// if (HAL_DMA_RegisterCallback(&hdma_adc1, HAL_DMA_XFER_CPLT_CB_ID, DMA1_Cplt_Callback) != HAL_OK)
		// {
		// 	__DBG_LOG("Error: DMA registration DMA1_Cplt_Callback failed.\n");
		// 	Error_Handler();
		// }

		__HAL_LINKDMA(hadc,DMA_Handle,hdma_adc1);
		__DBG_LOG("Info: DMA linked to ADC successfully.\n");
	}
}

void ADC1_2_IRQHandler()
{
	HAL_ADC_IRQHandler(&hadc1);
}



