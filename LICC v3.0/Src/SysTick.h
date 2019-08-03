/******************************************************************************
* 	SysTick.h
*
* 	Header for SysTick.c
*
* 	MCU: STM32L052R8
*
* 	12/08/2018:
* 	Added initialization and handler increments.
*
* 	Created on: 12/08/2018
* 	Author: Anthony Needles
******************************************************************************/
#ifndef SYSTICK_H_
#define SYSTICK_H_

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
void SysTickInit(void);

/******************************************************************************
* SysTickWaitTask - Public Function
*
* 12/08/2018:   Anthony Needles
* 				Started and completed function.
*
* 08/03/2018:   Anthony Needles
* 				Altered function for LICC v3.0
*
* Description:  Upon first time pass saves current millisecond count
* 				value. Every following call will result in program idle
* 				until next time slice period.
*
* Arguments:    ts_period - Desired time slice period
*
* Return: 		None
******************************************************************************/
void SysTickWaitTask(const uint32_t);

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
void SysTick_Handler(void);

#endif /* SYSTICK_H_ */
