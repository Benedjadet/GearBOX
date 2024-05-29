#ifndef __I2C2_H__
#define __I2C2_H__


#ifdef __cplusplus
extern "C" {
#endif



extern I2C_HandleTypeDef hi2c2;


void I2C2_Init(void);
void I2C2_EV_IRQHandler(void);
void I2C2_ER_IRQHandler(void);

#ifdef __cplusplus
}
#endif

#endif /* __I2C2_H__ */