/******************************************************************************
* 	Checker.h
*
* 	Header for Checker.c
*
* 	MCU: STM32L052R8
*
* 	12/09/2018:
* 	Completed initialization function and non-generic test looping.
*
* 	12/10/2018:
* 	Completed bit set/clear functions, as well as read output function.
*
* 	12/12/2018:
* 	Completed set/clear handler function, as well as test pass fail function
*
* 	12/13/2018:
* 	Completed test loop generic to IC, only needing IC parameters. Defines
* 	for loop skipping created.
*
* 	Created on: 12/09/2018
* 	Author: Anthony Needles
******************************************************************************/
#ifndef CHECKER_H_
#define CHECKER_H_

/********************************************************************
* Public Definitions
********************************************************************/
#define CYCLES_DELAY 240U
// 5us delay determined to by oscilloscope testing for minimum time of assertion
// for accurate output (48MHz * 5us = 240)

#define PASSED 1U

#define IC_74HC00_FAIL (in_A & in_B) != !out
#define IC_74HC02_FAIL (in_A | in_B) != !out
#define IC_74HC04_FAIL in_A != !out
#define IC_74HC08_FAIL (in_A & in_B) != out
#define IC_74HC10_FAIL (in_A & in_B & in_C) != !out
#define IC_74HC20_FAIL (in_A & in_B & in_C & in_D) != !out
#define IC_74HC27_FAIL (in_A | in_B | in_C) != !out
#define IC_74HC86_FAIL (in_A ^ in_B) != out
// IC failure boolean functions. Used to determine if test set of inputs
// and read output matches expected results

typedef enum {IC_74HC00,
			  IC_74HC02,
			  IC_74HC04,
			  IC_74HC08,
			  IC_74HC10,
			  IC_74HC20,
			  IC_74HC27,
			  IC_74HC86
} IC_DESIGNATOR_T;
// Unique identifier for each IC enumeration

typedef struct {
	IC_DESIGNATOR_T ic_designator;
	uint8_t num_inputs;
	uint8_t num_outputs;
	uint8_t input_pins[9];
	uint8_t output_pins[6];
} IC_PARAMETERS_T;
// Structure to hold various parameters for a given IC necessary
// for testing

/******************************************************************************
* Public Constants
******************************************************************************/
const IC_PARAMETERS_T IC_74HC00_PARAM = {IC_74HC00, 8, 4,
										{1, 2, 4, 5, 9, 10, 12, 13},
										{3, 6, 8, 11} };

const IC_PARAMETERS_T IC_74HC02_PARAM = {IC_74HC02, 8, 4,
										{2, 3, 5, 6, 8, 9, 11, 12},
										{1, 4, 10, 13} };

const IC_PARAMETERS_T IC_74HC04_PARAM = {IC_74HC04, 6, 6,
										{1, 3, 5, 9, 11, 13},
										{2, 4, 6, 8, 10, 12} };

const IC_PARAMETERS_T IC_74HC08_PARAM = {IC_74HC08, 8, 4,
										{1, 2, 4, 5, 9, 10, 12, 13},
										{3, 6, 8, 11} };

const IC_PARAMETERS_T IC_74HC10_PARAM = {IC_74HC10, 9, 3,
										{1, 2, 13, 3, 4, 5, 9, 10, 11},
										{12, 6, 8} };

const IC_PARAMETERS_T IC_74HC20_PARAM = {IC_74HC20, 8, 2,
										{1, 2, 4, 5, 9, 10, 12, 13},
										{6, 8} };

const IC_PARAMETERS_T IC_74HC27_PARAM = {IC_74HC27, 9, 3,
										{1, 2, 13, 3, 4, 5, 9, 10, 11},
										{12, 6, 8} };

const IC_PARAMETERS_T IC_74HC86_PARAM = {IC_74HC86, 8, 4,
										{1, 2, 4, 5, 9, 10, 12, 13},
										{3, 6, 8, 11} };
// 74HCXX Parameters: IC Designator, # of inputs, # of outputs, list of input
// pins, and list of output pins
// Note: Input lists shall have all input pin(s) for a certain gate grouped
// together, and their corresponding output pin shall be placed accordingly in
// the output list

/********************************************************************
* CheckerInit - Initializes required checker peripherals
*
* Description:  Enables clocks for GPIO ports A, B, and C. Enables
* 				TIM22 with count value of desired delays measured
* 				in cycles. This timer will be used for delaying
* 				small amounts to ensure any output gate change has
* 				time to propagate the system. One pulse mode enabled.
*
* Return value:	None
*
* Arguments:    None
********************************************************************/
void CheckerInit(void);

/********************************************************************
* CheckerTestIC - Task for reading and debouncing current button press
*
* Description:  Main test structure. Performs testing by creating all
* 				possible input combinations and reading resulting outputs.
* 				Made generically for any boolean logic 74HCXX IC with
* 				four or less inputs per gate. Gate inputs are labeled
* 				A, B, C, and D. For gates with less than four inputs,
* 				the loops for unused inputs are bypassed (e.g. two
* 				input gates will only use A and B loops). Only required
* 				input pins are set/cleared. If tests fails at any point
* 				failure result is immediately sent.
*
*
* Return value:	Test pass or test failure
*
* Arguments:    IC_PARAMETERS_T IC - Structure holding IC parameters
********************************************************************/
uint8_t CheckerTestIC(IC_PARAMETERS_T);

#endif /* CHECKER_H_ */
