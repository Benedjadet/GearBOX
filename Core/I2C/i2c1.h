#ifndef __I2C1_H__
#define __I2C1_H__


#ifdef __cplusplus
extern "C" {
#endif



extern I2C_HandleTypeDef hi2c1;


void I2C1_Init(void);
void I2C1_EV_IRQHandler(void);
void I2C1_ER_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __I2C1_H__ */