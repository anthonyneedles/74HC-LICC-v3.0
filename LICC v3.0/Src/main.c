/******************************************************************************
* 	74HHC Logic IC Checker (LICC) v3.0
*
* 	MCU: STM32F030C8Tx
*
* 	08/02/2019:
*	LICC v3.0.0 - Empty project generated via CubeMX
*
*	08/02/2019:
*	LICC v3.1.0 - Added Clock Config and SysTick config
*
* 	Created on: 08/02/2019
* 	Author: Anthony Needles
******************************************************************************/
#include "stm32f030x8.h"
#include "ClockConfig.h"
#include "SysTick.h"
#include "Checker.h"

/******************************************************************************
* Public Definitions
******************************************************************************/
#define MASK_FAILURE 0x00000000
#define MASK_74HC00 0x00000001
#define MASK_74HC02	0x00000002
#define MASK_74HC04 0x00000004
#define MASK_74HC08 0x00000008
#define MASK_74HC10 0x00000010
#define MASK_74HC20 0x00000020
#define MASK_74HC27 0x00000040
#define MASK_74HC86 0x00000080
// Bit field mask for setting a single bit for a specific test pass, one per IC

typedef enum{IDLE, CHECK_74HC00, CHECK_74HC02, CHECK_74HC04, CHECK_74HC08,
			 CHECK_74HC10, CHECK_74HC20, CHECK_74HC27, CHECK_74HC86,
			 DISPLAY_RESULT
} CONTROL_STATE_T;

int main(void)
{
	ClkCfgInit();
	SysTickInit();

	while (1){
//		SysTickWaitTask();
	}
}
