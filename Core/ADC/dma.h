#ifndef _DMA_H_
#define _DMA_H_


#ifdef __cplusplus
extern "C" {
#endif

extern DMA_HandleTypeDef hdma_adc1;

void DMA1_Init(void);
void DMA1_Channel1_IRQHandler(void);


#ifdef __cplusplus
}
#endif


#endif /* _DMA_H_ */