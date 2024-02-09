#ifndef _DEBUGGING_H_
#define _DEBUGGING_H_


#ifdef __cplusplus
extern "C" {
#endif


// Отладочные сообщения через UART.
#ifdef DEBUGGING_ENABLE
#include <stdio.h>
#define __DBG_UART_LOG(str, ...) printf(str, ##__VA_ARGS__)
#define __DBG_LOG(str, ...) printf(str, ##__VA_ARGS__)
#else
#define __DBG_UART_LOG(str, ...)
#define __DBG_LOG(str, ...)
#endif

extern UART_HandleTypeDef huart1;

extern void prvGetRegistersFromStack(uint32_t *pulFaultStackAddress);
extern void Error_Handler(void);

#ifdef __cplusplus
}
#endif

#endif /* _DEBUGGING_H_ */