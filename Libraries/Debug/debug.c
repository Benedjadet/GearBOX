
#include "debug.h"

void prvGetRegistersFromStack(uint32_t *pulFaultStackAddress)
{
	/* These are volatile to try and prevent the compiler/linker optimising them
	away as the variables never actually get used.  If the debugger won't show the
	values of the variables, make them global my moving their declaration outside
	of this function. */
	__attribute__ ((unused)) volatile uint32_t cfsr  = SCB->CFSR;
	__attribute__ ((unused)) volatile uint32_t hfsr = SCB->HFSR;
	__attribute__ ((unused)) volatile uint32_t mmfar = SCB->MMFAR;
	__attribute__ ((unused)) volatile uint32_t bfar = SCB->BFAR;

	__attribute__ ((unused)) volatile uint32_t r0;
	__attribute__ ((unused)) volatile uint32_t r1;
	__attribute__ ((unused)) volatile uint32_t r2;
	__attribute__ ((unused)) volatile uint32_t r3;
	__attribute__ ((unused)) volatile uint32_t r12;
	__attribute__ ((unused)) volatile uint32_t lr;  // Link register.
	__attribute__ ((unused)) volatile uint32_t pc;  // Program counter.
	__attribute__ ((unused)) volatile uint32_t psr; // Program status register.
	// volatile uint32_t counter;

	r0 = pulFaultStackAddress[0];
	r1 = pulFaultStackAddress[1];
	r2 = pulFaultStackAddress[2];
	r3 = pulFaultStackAddress[3];

	r12 = pulFaultStackAddress[4];
	lr = pulFaultStackAddress[5];
	pc = pulFaultStackAddress[6];
	psr = pulFaultStackAddress[7];

	__DBG_LOG("\n");
	__DBG_LOG("> Hard Error\n");

	__DBG_LOG("\tSCB_CFSR: \t0x%08lX\n", cfsr);
	__DBG_LOG("\tSCB_HFSR: \t0x%08lX\n", hfsr);
	__DBG_LOG("\tSCB_MMFAR:\t0x%08lX\n", mmfar);
	__DBG_LOG("\tSCB_BFAR: \t0x%08lX\n", bfar);
	__DBG_LOG("\n");
	__DBG_LOG("\tR0:       \t0x%08lX\n", r0);
	__DBG_LOG("\tR1:       \t0x%08lX\n", r1);
	__DBG_LOG("\tR2:       \t0x%08lX\n", r2);
	__DBG_LOG("\tR3:       \t0x%08lX\n", r3);
	__DBG_LOG("\tR12:      \t0x%08lX\n", r12);
	__DBG_LOG("\n");
	__DBG_LOG("\tLR:       \t0x%08lX\n", lr);
	__DBG_LOG("\tPC:       \t0x%08lX\n", pc);
	__DBG_LOG("\tPSR:      \t0x%08lX\n", psr);

	volatile uint32_t cycleCounter = 0;

	// When the following line is hit, the variables contain the register values.
	for (;;)
	{
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_SET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(GPIOB, GPIO_PIN_5, GPIO_PIN_RESET);
		HAL_Delay(100);
		cycleCounter++;
	}
}

void Error_Handler(void)
{
	__disable_irq();
	while (1)
	{
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);
		HAL_Delay(100);
		HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET);
		HAL_Delay(100);
	}
}

#ifdef DEBUGGING_ENABLE
int _write(int file, char *ptr, int len)
{
	HAL_UART_Transmit(&huart1, (uint8_t *)ptr, len, HAL_MAX_DELAY);
	return len;
}
#endif