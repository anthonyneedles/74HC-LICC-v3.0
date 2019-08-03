/******************************************************************************
* 	ClockConfig.h
*
* 	MCU: STM32F030C6Tx
*
* 	Configures MCU clock system for 48MHz system clock by use of 8MHz external
* 	crystal and internal PLL.
*
* 	08/02/2019:
* 	Created empty .c and .h files.
*
* 	08/03/2019:
* 	Completed clock configuration initialization for 8MHz external crystal
* 	providing 48MHz SYSCLK. Completed configuration for SYSCLK MCO.
*
* 	Created on: 08/02/2019
* 	Author: Anthony Needles
******************************************************************************/
#ifndef CLOCKCONFIG_H_
#define CLOCKCONFIG_H_

/******************************************************************************
* ClkCfgInit - Public Function
*
* 08/03/2019:	Anthony Needles
* 				Started and completed function.
*
* Description:  Runs initialization sequence for generating 48MHz system clock
* 				from external 8MHz crystal and internal PLL. Blocks program
* 				when ensuring stability from crystal oscillator and ensuring
* 				lock from PLL. Usually first function ran upon startup.
*
* Arguments:    None
*
* Return:		None
******************************************************************************/
void ClkCfgInit(void);

#endif /* CLOCKCONFIG_H_ */
