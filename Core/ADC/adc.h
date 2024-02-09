#ifndef _ADC_H_
#define _ADC_H_


#ifdef __cplusplus
extern "C" {
#endif

extern ADC_HandleTypeDef hadc1;
extern DMA_HandleTypeDef hdma_adc1;

void ADC1_Init(void);
void ADC1_2_IRQHandler();
extern void DMA1_Cplt_Callback( ADC_HandleTypeDef *hadc);

#ifdef __cplusplus
}
#endif


#endif /* _ADC_H_ */