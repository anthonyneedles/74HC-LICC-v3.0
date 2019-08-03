/******************************************************************************
* 	SysTick.c
*
* 	This source file handles all SysTick initialization and functionality for
* 	time slice preemptive multitasking. Dependent on 48MHz Cortex system timer
* 	derived from 48MHz system clock via HSE and PLL.
*
* 	MCU: STM32F030C8Tx
*
* 	12/08/2018:
* 	Added initialization and handler increments. Added wait task.
*
* 	08/03/2019:
* 	Edited for LICC v3.0 use.
*
* 	Created on: 12/08/2018
* 	Author: Anthony Needles
******************************************************************************/
#include "stm32f030x8.h"
#include "SysTick.h"

/******************************************************************************
*	Private Definitions
******************************************************************************/
#define CLKCYCLES_PER_MS 48000U

/******************************************************************************
*	Private Global Variables
******************************************************************************/
static volatile uint32_t systickCurrentMSCount = 0U;
static uint32_t systickLastCount = 0U;
static uint8_t systickInitFlag = 0U;

/******************************************************************************
* SysTickInit - Public Function
*
* 12/08/2018:   Anthony Needles
* 				Started and completed function.
*
* 08/03/2018:   Anthony Needles
* 				Altered function for LICC v3.0
*
* Description:  Enables SysTick timer via CMSIS SysTick_Config with
* 				with required clock cycles to result in 1ms interrupts.
* 				Error value returned from SysTick_Config also passed back.
*
* Arguments:    None
*
* Return: 		None
******************************************************************************/
void SysTickInit(void)
{
	uint32_t st_error;

	st_error = SysTick_Config(CLKCYCLES_PER_MS);
	if (st_error == 0U) {
		// 0U = Systick timer successfully loaded
		systickInitFlag = 1;
	} else {
		// 1U = Reload value impossible
		systickInitFlag = 0;
	}
}

/******************************************************************************
* SysTickWaitTask - Public Function
*
* 12/08/2018:   Anthony Needles
* 				Started and completed function.
*
* 08/03/2018:   Anthony Needles
* 				Altered function for LICC v3.0.
*
* Description:  Upon first time pass saves current millisecond count
* 				value. Every following call will result in program idle
* 				until next time slice period.
*
* Arguments:    ts_period - Desired time slice period
*
* Return: 		None
******************************************************************************/
void SysTickWaitTask(const uint32_t ts_period)
{
    if (systickInitFlag == 1U) {
        while ((systickCurrentMSCount - systickLastCount) < ts_period) {}
        systickLastCount = systickCurrentMSCount;
    } else {
    }
}

/******************************************************************************
* SysTickHandler - Interrupt Handler
*
* 12/08/2018:   Anthony Needles
* 				Started and completed function.
*
* 08/03/2018:   Anthony Needles
* 				Altered function for LICC v3.0
*
* Description:  Interrupts will occur at 1 kHz (every millisecond).
* 				Increments millisecond count every entrance.
*
* Arguments:    None
*
* Return: 		None
******************************************************************************/
void SysTick_Handler(void)
{
	systickCurrentMSCount++;
}
