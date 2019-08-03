/******************************************************************************
* 	ClockConfig.c
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
#include "stm32f030x8.h"
#include "ClockConfig.h"

/******************************************************************************
*	Private Definitions
******************************************************************************/
#define RCC_CFGR_PLLSRC_HSE RCC_CFGR_PLLSRC_HSE_PREDIV
#define GPIO_MODER_MODER8_AF GPIO_MODER_MODER8_1
#define GPIO_AFRH_AFSEL8_AF0 (0x0U << GPIO_AFRH_AFSEL8_Pos)

/******************************************************************************
*	Private Function Prototypes
******************************************************************************/
void clkcfgEnableMCO(void);

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
void ClkCfgInit()
{
	clkcfgEnableMCO();

	// Enable Clock Security System and High Speed External clock
	RCC->CR |= (RCC_CR_CSSON | RCC_CR_HSEON);

	// Wait until HSE oscillator is stable (~512 HSE clock pulses, 1)
	while ((RCC->CR & RCC_CR_HSERDY_Msk) == 0){}

	// Set system clock mux to HSE (SYSCLK is now 8MHz)
	RCC->CFGR = ((RCC->CFGR & ~RCC_CFGR_SW_Msk) | RCC_CFGR_SW_HSE);

	// Turn PLL off to change parameters
	RCC->CR &= ~RCC_CR_PLLON;

	// Wait until PLL is unlocked (ofF, 0)
	while ((RCC->CR & RCC_CR_PLLRDY_Msk) != 0){}

	// Set PLL multiplication factor to 6 (6 x 8Mhz = 48MHz) MUST NOT EXCEED 6!
	// Select HSE as PLL source
	RCC->CFGR = ((RCC->CFGR & ~RCC_CFGR_PLLMUL_Msk) | RCC_CFGR_PLLMUL6
													| RCC_CFGR_PLLSRC_HSE);

	// Turn PLL on as parameters are changed
	RCC->CR |= RCC_CR_PLLON;

	// Wait until PLL is locked (on, 1)
	while ((RCC->CR & RCC_CR_PLLRDY_Msk) == 0){}

	// Set system clock mux to PLL (SYSCLK is now 48MHz)
	RCC->CFGR = ((RCC->CFGR & ~RCC_CFGR_SW_Msk) | RCC_CFGR_SW_PLL);
}

/******************************************************************************
* clkcfgEnableMCO - Private Function
*
* 08/03/2019:	Anthony Needles
* 				Started and completed function.
*
* Description:  Selects SYSCLK as MCO source. Enables MCO for MCU pin 29, PA8.
*
* Arguments:    None
*
* Return:		None
******************************************************************************/
void clkcfgEnableMCO()
{
	// Set MCO to SYSCLK
	RCC->CFGR = ((RCC->CFGR & ~RCC_CFGR_MCO_Msk) | RCC_CFGR_MCO_SYSCLK);

	// Enable GPIOA clock (MCO pin used is PA8)
	RCC->AHBENR |= RCC_AHBENR_GPIOAEN;

	// Set MCO pin to alternate function
	GPIOA->MODER = ((GPIOA->MODER & ~GPIO_MODER_MODER8_Msk)
				   | GPIO_MODER_MODER8_AF);

	// Set MCO pin to alternate function 0 (MCO)
	GPIOA->AFR[1] = ((GPIOA->AFR[1] & ~GPIO_AFRH_AFSEL8_Msk)
				    | GPIO_AFRH_AFSEL8_AF0);
}
