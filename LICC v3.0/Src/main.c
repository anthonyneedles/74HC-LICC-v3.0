/******************************************************************************
* 	74HHC Logic IC Checker (LICC) v3.0
*
* 	MCU: STM32F030C8Tx
*
* 	08/02/2019:
*	Created empty project framework.
*
* 	Created on: 08/02/2019
* 	Author: Anthony Needles
******************************************************************************/
#include "stm32f030x8.h"
#include "ClockConfig.h"
#include "SysTick.h"

/******************************************************************************
*	Private Definitions
******************************************************************************/
#define TIMESLICE_PERIOD_MS 20U

int main(void)
{
	ClkCfgInit();
	SysTickInit();

	while (1){
		SysTickWaitTask(TIMESLICE_PERIOD_MS);
	}
}
