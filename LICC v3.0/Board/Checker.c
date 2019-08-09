/******************************************************************************
* 	Checker.c
*
* 	This source file is used to contain all functionality and initialization for
* 	performing the failure checking of 74HCXX digital logic integrated circuits.
* 	Developed to work with IC Checker v1.5 Shield. Dependent on 2.097 MHz APB2
* 	timer clock to perform delayed output readings.
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
* 	1/25/2019:
* 	Updated IC probe I/O to match STM32L052R8 PCB port definitions.
* 	Consolidated setting and clearing functions into single function (removing
* 	two superfluous functions).
*
* 	08/04/2019:
* 	Ported module for use with LICC v3.0.
*
* 	Created on: 12/09/2018
* 	Author: Anthony Needles
******************************************************************************/
#include "stm32f030x8.h"
#include "Checker.h"

/******************************************************************************
* Private Definitions
******************************************************************************/
#define INPUT_A_LOOP_SKIP (loop_skip_field & 0x01)
#define INPUT_B_LOOP_SKIP ((loop_skip_field & 0x02) >> 1)
#define INPUT_C_LOOP_SKIP ((loop_skip_field & 0x04) >> 2)
#define INPUT_D_LOOP_SKIP ((loop_skip_field & 0x08) >> 3)
//	Defines for determining required number of for loops based on the number of
//	inputs per gate. A bit field of all 1s will be left shifted the number of inputs
//	per gate for the given IC, with zeros shifting in at LSB (this is
//	"loop_skip_field", where bits loop_skip_field[3:0] corresponds to whether [D:A]
//	loops should be skipped or not, respectively). If "INPUT_X_LOOP_SKIP"
//	evaluates to 1, then the loop will be skipped, but if evaluated to 0 will not.
//	For example: The resulting loop_skip_field[3:0] for a two input gate will be
//	0b1100, testing will require only the "gate_input_A" loop and the "gate_input_B"
//	loop, so the "gate_input_C" loop and the "gate_input_D" loop will be "bypassed"
//	by only executing once.

#define TRUE 1U
#define FAILED 0U
#define SET 1U

/******************************************************************************
* Private Function Prototypes
******************************************************************************/
static void checkerSetClrInputs(uint8_t*, uint8_t, uint8_t, uint8_t);
static uint8_t checkerReadICOutput(uint8_t);
static uint8_t checkerFailTest(IC_DESIGNATOR_T, uint8_t, uint8_t, uint8_t, uint8_t, uint8_t);

/******************************************************************************
* CheckerInit - Public Function
*
* 12/09/2018:	Anthony Needles
* 				Started and completed function.
*
* 08/04/2019:	Anthony Needles
* 				Ported function for use with LICC v3.0.
*
* Description:  Enables clocks for GPIO ports A and B. Enables
* 				TIM17 with count value of desired delays measured
* 				in cycles. This timer will be used for delaying
* 				small amounts to ensure any output gate change has
* 				time to propagate the system. One pulse mode enabled.
*
* Arguments:    None
*
* Return:		None
******************************************************************************/
void CheckerInit(void)
{
	// Enable GPIOA clock and GPIOB clock
	RCC->AHBENR |= (RCC_AHBENR_GPIOAEN | RCC_AHBENR_GPIOAEN);
	RCC->APB2ENR |= RCC_APB2ENR_TIM17EN;

	TIM17->CR1 |= TIM_CR1_OPM;
	TIM17->ARR = CYCLES_DELAY;
}

/******************************************************************************
* CheckerTestIC - Public Function
*
* 12/09/2018:	Anthony Needles
* 				Started and completed function.
*
* 08/04/2019:	Anthony Needles
* 				Ported function for use with LICC v3.0.
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
* Arguments:    IC_PARAMETERS_T IC - Structure holding IC parameters
*
* Return:		Test pass or test failure
******************************************************************************/
uint8_t CheckerTestIC(IC_PARAMETERS_T IC)
{
	uint8_t num_gates = IC.num_outputs;
	uint8_t num_inputs_gate = IC.num_inputs/IC.num_outputs;
	uint8_t loop_skip_field = (0xFF << num_inputs_gate);
	uint8_t gate_start_index = 0;
	uint8_t test_output;
	uint8_t fail_result;
	uint8_t gate_num;

	for(gate_num = 0; gate_num < num_gates; gate_num++){
		for(uint8_t gate_input_A = INPUT_A_LOOP_SKIP; gate_input_A < 2; gate_input_A++){
			for(uint8_t gate_input_B = INPUT_B_LOOP_SKIP; gate_input_B < 2; gate_input_B++){
				for(uint8_t gate_input_C = INPUT_C_LOOP_SKIP; gate_input_C < 2; gate_input_C++){
					for(uint8_t gate_input_D = INPUT_D_LOOP_SKIP; gate_input_D < 2; gate_input_D++){
						if(INPUT_A_LOOP_SKIP != TRUE) checkerSetClrInputs(IC.input_pins, gate_start_index, 0, gate_input_A);
						if(INPUT_B_LOOP_SKIP != TRUE) checkerSetClrInputs(IC.input_pins, gate_start_index, 1, gate_input_B);
						if(INPUT_C_LOOP_SKIP != TRUE) checkerSetClrInputs(IC.input_pins, gate_start_index, 2, gate_input_C);
						if(INPUT_D_LOOP_SKIP != TRUE) checkerSetClrInputs(IC.input_pins, gate_start_index, 3, gate_input_D);

						test_output = checkerReadICOutput(IC.output_pins[gate_num]);

						fail_result = checkerFailTest(IC.ic_designator, test_output, gate_input_A, gate_input_B, gate_input_C, gate_input_D);

						if(fail_result == FAILED) return FAILED;
					}
				}
			}
		}
		gate_start_index += num_inputs_gate;
	}
	return PASSED;
}

/******************************************************************************
* checkerSetClrInputs - Private Function
*
* 12/09/2018:	Anthony Needles
* 				Started and completed function.
*
* 08/04/2019:	Anthony Needles
* 				Ported function for use with LICC v3.0.
*
* Description:  Handed input pin number array for given IC, whether to
* 				set or clear, the index the current gate input group is
* 				located in the pin number array, and which number input
* 				of the gate is specified. This function derives
* 				which specific IC pin is desired to set/clear based on
* 				passed specifications.
*
* Arguments:    uint8_t *input_pins - Input IC pin number array (in
* 				order) for tested IC.
*
* 				uint8_t gate_start - Index of pin number array that
* 				current input group starts.
*
* 				uint8_t input_offset - Offset of specific pin in input
* 				group that is desired to be set/cleared.
*
* 				uint8_t input_value - Desired value to give to input
* 				pin (1 for set, 0 for clear).
*
* Return:		None
******************************************************************************/
void checkerSetClrInputs(uint8_t *input_pins, uint8_t gate_start, uint8_t input_offset, uint8_t input_state)
{
	uint8_t ic_pin = input_pins[gate_start + input_offset];

	switch(ic_pin){
	case 1:
		GPIOB->MODER &= ~GPIO_MODER_MODER11_Msk;
		GPIOB->MODER |= GPIO_MODER_MODER11_0;
		(input_state == SET) ? (GPIOB->ODR |= GPIO_ODR_11) : (GPIOB->ODR &= ~GPIO_ODR_11);
	break;

	case 2:
		GPIOB->MODER &= ~GPIO_MODER_MODER10_Msk;
		GPIOB->MODER |= GPIO_MODER_MODER10_0;
		(input_state == SET) ? (GPIOB->ODR |= GPIO_ODR_10) : (GPIOB->ODR &= ~GPIO_ODR_10);
	break;

	case 3:
		GPIOB->MODER &= ~GPIO_MODER_MODER2_Msk;
		GPIOB->MODER |= GPIO_MODER_MODER2_0;
		(input_state == SET) ? (GPIOB->ODR |= GPIO_ODR_2) : (GPIOB->ODR &= ~GPIO_ODR_2);
	break;

	case 4:
		GPIOB->MODER &= ~GPIO_MODER_MODER1_Msk;
		GPIOB->MODER |= GPIO_MODER_MODER1_0;
		(input_state == SET) ? (GPIOB->ODR |= GPIO_ODR_1) : (GPIOB->ODR &= ~GPIO_ODR_1);
	break;

	case 5:
		GPIOB->MODER &= ~GPIO_MODER_MODER0_Msk;
		GPIOB->MODER |= GPIO_MODER_MODER0_0;
		(input_state == SET) ? (GPIOB->ODR |= GPIO_ODR_0) : (GPIOB->ODR &= ~GPIO_ODR_0);
	break;

	case 6:
		GPIOA->MODER &= ~GPIO_MODER_MODER7_Msk;
		GPIOA->MODER |= GPIO_MODER_MODER7_0;
		(input_state == SET) ? (GPIOA->ODR |= GPIO_ODR_7) : (GPIOA->ODR &= ~GPIO_ODR_7);
	break;

	case 8:
		GPIOA->MODER &= ~GPIO_MODER_MODER6_Msk;
		GPIOA->MODER |= GPIO_MODER_MODER6_0;
		(input_state == SET) ? (GPIOA->ODR |= GPIO_ODR_6) : (GPIOA->ODR &= ~GPIO_ODR_6);
	break;

	case 9:
		GPIOA->MODER &= ~GPIO_MODER_MODER5_Msk;
		GPIOA->MODER |= GPIO_MODER_MODER5_0;
		(input_state == SET) ? (GPIOA->ODR |= GPIO_ODR_5) : (GPIOA->ODR &= ~GPIO_ODR_5);
	break;

	case 10:
		GPIOA->MODER &= ~GPIO_MODER_MODER4_Msk;
		GPIOA->MODER |= GPIO_MODER_MODER4_0;
		(input_state == SET) ? (GPIOA->ODR |= GPIO_ODR_4) : (GPIOA->ODR &= ~GPIO_ODR_4);
	break;

	case 11:
		GPIOA->MODER &= ~GPIO_MODER_MODER3_Msk;
		GPIOA->MODER |= GPIO_MODER_MODER3_0;
		(input_state == SET) ? (GPIOA->ODR |= GPIO_ODR_3) : (GPIOA->ODR &= ~GPIO_ODR_3);
	break;

	case 12:
		GPIOA->MODER &= ~GPIO_MODER_MODER2_Msk;
		GPIOA->MODER |= GPIO_MODER_MODER2_0;
		(input_state == SET) ? (GPIOA->ODR |= GPIO_ODR_2) : (GPIOA->ODR &= ~GPIO_ODR_2);
	break;

	case 13:
		GPIOA->MODER &= ~GPIO_MODER_MODER1_Msk;
		GPIOA->MODER |= GPIO_MODER_MODER1_0;
		(input_state == SET) ? (GPIOA->ODR |= GPIO_ODR_1) : (GPIOA->ODR &= ~GPIO_ODR_1);
	break;

	default:
	break;
	}
}

/******************************************************************************
* checkerReadICOutput - Private Function
*
* 12/09/2018:	Anthony Needles
* 				Started and completed function.
*
* 08/04/2019:	Anthony Needles
* 				Ported function for use with LICC v3.0.
*
* Description:  Based on which pin number given from specific IC's
* 				output pin array, corresponding GPIO port is set to
* 				input mode (relative to MCU, but these pins will be
* 				outputs relative to the tested IC). TIM17 is enabled
* 				and has update interrupt flag polled in order to
* 				generate a delay of only a few clock cycles. This allows
* 				any gate output changes time to propagate so that
* 				readings are correct. The resulting read logic level
* 				is then returned.
*
* Arguments:    uint8_t ic_pin - IC pin desired to have logic level read
*
* Return:		Logic value read from GPIO input
******************************************************************************/
uint8_t checkerReadICOutput(uint8_t ic_pin)
{
	uint8_t ret_val;

	switch(ic_pin){
	case 1:
		GPIOB->MODER &= ~(GPIO_MODER_MODER11);
		TIM17->CR1 |= TIM_CR1_CEN;
		while((TIM17->SR & TIM_SR_UIF_Msk) == 0){}
		ret_val = ((GPIOB->IDR & GPIO_IDR_11) >> 10);
		break;

	case 2:
		GPIOB->MODER &= ~(GPIO_MODER_MODER10);
		TIM17->CR1 |= TIM_CR1_CEN;
		while((TIM17->SR & TIM_SR_UIF_Msk) == 0){}
		return ((GPIOB->IDR & GPIO_IDR_10) >> 3);
		break;

	case 3:
		GPIOB->MODER &= ~(GPIO_MODER_MODER2);
		TIM17->CR1 |= TIM_CR1_CEN;
		while((TIM17->SR & TIM_SR_UIF_Msk) == 0){}
		ret_val = ((GPIOB->IDR & GPIO_IDR_2) >> 5);
		break;

	case 4:
		GPIOB->MODER &= ~(GPIO_MODER_MODER1);
		TIM17->CR1 |= TIM_CR1_CEN;
		while((TIM17->SR & TIM_SR_UIF_Msk) == 0){}
		ret_val = ((GPIOB->IDR & GPIO_IDR_1) >> 4);
		break;

	case 5:
		GPIOB->MODER &= ~(GPIO_MODER_MODER0);
		TIM17->CR1 |= TIM_CR1_CEN;
		while((TIM17->SR & TIM_SR_UIF_Msk) == 0){}
		ret_val = ((GPIOB->IDR & GPIO_IDR_0) >> 10);
		break;

	case 6:
		GPIOA->MODER &= ~(GPIO_MODER_MODER7);
		TIM17->CR1 |= TIM_CR1_CEN;
		while((TIM17->SR & TIM_SR_UIF_Msk) == 0){}
		ret_val = ((GPIOA->IDR & GPIO_IDR_7) >> 8);
		break;

	case 8:
		GPIOA->MODER &= ~(GPIO_MODER_MODER6);
		TIM17->CR1 |= TIM_CR1_CEN;
		while((TIM17->SR & TIM_SR_UIF_Msk) == 0){}
		ret_val = ((GPIOA->IDR & GPIO_IDR_6) >> 9);
		break;

	case 9:
		GPIOA->MODER &= ~(GPIO_MODER_MODER5);
		TIM17->CR1 |= TIM_CR1_CEN;
		while((TIM17->SR & TIM_SR_UIF_Msk) == 0){}
		ret_val = ((GPIOA->IDR & GPIO_IDR_5) >> 7);
		break;

	case 10:
		GPIOA->MODER &= ~(GPIO_MODER_MODER4);
		TIM17->CR1 |= TIM_CR1_CEN;
		while((TIM17->SR & TIM_SR_UIF_Msk) == 0){}
		ret_val = ((GPIOA->IDR & GPIO_IDR_4) >> 6);
		break;

	case 11:
		GPIOA->MODER &= ~(GPIO_MODER_MODER3);
		TIM17->CR1 |= TIM_CR1_CEN;
		while((TIM17->SR & TIM_SR_UIF_Msk) == 0){}
		ret_val = ((GPIOA->IDR & GPIO_IDR_3) >> 7);
		break;

	case 12:
		GPIOA->MODER &= ~(GPIO_MODER_MODER2);
		TIM17->CR1 |= TIM_CR1_CEN;
		while((TIM17->SR & TIM_SR_UIF_Msk) == 0){}
		ret_val = ((GPIOA->IDR & GPIO_IDR_2) >> 6);
		break;

	case 13:
		GPIOA->MODER &= ~(GPIO_MODER_MODER1);
		TIM17->CR1 |= TIM_CR1_CEN;
		while((TIM17->SR & TIM_SR_UIF_Msk) == 0){}
		ret_val = ((GPIOA->IDR & GPIO_IDR_1) >> 5);
		break;

	default:
		ret_val = 0xFF;
		break;
	}
	return ret_val;
}

/********************************************************************
* checkerFailTest - Private Function
*
* 12/09/2018:	Anthony Needles
* 				Started and completed function.
*
* 08/04/2019:	Anthony Needles
* 				Ported function for use with LICC v3.0.
*
* Description:  Uses defined IC boolean functions to determine if given
* 				inputs and output match result as expected. IC boolean
* 				functions defined in Checker.h.
*
* Arguments:    IC_DESIGNATOR_T ic_id - Enumeration of IC's designation
* 				being tested
*
* 				uint8_t out - Read output for given inputs
*
* 				uint8_t in_A - Value given to gate_input_A
*
* 				uint8_t in_B - Value given to gate_input_B
*
* 				uint8_t in_C - Value given to gate_input_C
*
* 				uint8_t in_D - Value given to gate_input_D
*
* Return:		Pass or failure for specific IC tested
********************************************************************/
uint8_t checkerFailTest(IC_DESIGNATOR_T ic_id, uint8_t out, uint8_t in_A, uint8_t in_B, uint8_t in_C, uint8_t in_D)
{
	uint8_t ret_val = PASSED;

	switch(ic_id){
	case IC_74HC00:
		if(IC_74HC00_FAIL) ret_val = FAILED;
		break;

	case IC_74HC02:
		if(IC_74HC02_FAIL) ret_val =FAILED;
		break;

	case IC_74HC04:
		if(IC_74HC04_FAIL) ret_val = FAILED;
		break;

	case IC_74HC08:
		if(IC_74HC08_FAIL) ret_val = FAILED;
		break;

	case IC_74HC10:
		if(IC_74HC10_FAIL) ret_val = FAILED;
		break;

	case IC_74HC20:
		if(IC_74HC20_FAIL) ret_val = FAILED;
		break;

	case IC_74HC27:
		if(IC_74HC27_FAIL) ret_val = FAILED;
		break;

	case IC_74HC86:
		if(IC_74HC86_FAIL) ret_val = FAILED;
		break;

	default:
		ret_val = FAILED;
		break;
	}
	return ret_val;
}
