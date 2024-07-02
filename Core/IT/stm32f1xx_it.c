
#include "main.h"
#include "stm32f1xx_it.h"

extern PCD_HandleTypeDef hpcd_USB_FS;
extern UART_HandleTypeDef huart1;

/**
 * @brief This function handles Non maskable interrupt.
 */
void NMI_Handler(void)
{
	while (1)
	{
	}
}

/**
 * @brief This function handles Hard fault interrupt.
 */
void HardFault_Handler(void)
{
	/* __ASM volatile(
		" cpsid i				                                    \n"
		" tst lr, #4                                                \n"
		" ite eq                                                    \n"
		" mrseq r0, msp                                             \n"
		" mrsne r0, psp                                             \n"
		" ldr r1, [r0, #24]                                         \n"
		" ldr r2, handler2_address_const                            \n"
		" bx r2                                                     \n"
		" handler2_address_const: .word prvGetRegistersFromStack    \n"); */


		while (1)
	{
	}
}

/**
 * @brief This function handles Memory management fault.
 */
void MemManage_Handler(void)
{
	while (1)
	{
	}
}

/**
 * @brief This function handles Prefetch fault, memory access fault.
 */
void BusFault_Handler(void)
{
	while (1)
	{
	}
}

/**
 * @brief This function handles Undefined instruction or illegal state.
 */
void UsageFault_Handler(void)
{
	while (1)
	{
	}
}

/**
 * @brief This function handles System service call via SWI instruction.
 */
void SVC_Handler(void)
{
}

/**
 * @brief This function handles Debug monitor.
 */
void DebugMon_Handler(void)
{
}

/**
 * @brief This function handles Pendable request for system service.
 */
void PendSV_Handler(void)
{
}

/**
 * @brief This function handles System tick timer.
 */
void SysTick_Handler(void)
{
	HAL_IncTick();
}

/**
 * @brief This function handles USB low priority or CAN RX0 interrupts.
 */
void USB_IRQHandler(void)
{
	HAL_PCD_IRQHandler(&hpcd_USB_FS);
}

/**
 * @brief This function handles UART interrupts.
 */
void USART1_IRQHandler(void)
{
	HAL_UART_IRQHandler(&huart1);
}