/*
 * ir.c
 *
 *  Created on: Nov 8, 2025
 *      Author: patry
 */

#include "ir.h"
#include "tim.h"
typedef enum {
	PULSE_9MS,
	PULSE_4MS,
	PULSE_2MS,
	PULSE_LONG,
	PULSE_SHORT,
	PULSE_ERROR,
}pulse_t;
static volatile uint32_t received_value;
static volatile uint32_t a;
static volatile uint32_t b;
volatile uint8_t frame_ready = 0;


static volatile int received_bits;
static pulse_t calc_pulse(uint32_t time)
{
	if (time < 250)
		return PULSE_ERROR;
	else if (time < 1200)
		return PULSE_SHORT;
	else if (time < 2000)
		return PULSE_LONG;
	else if (time < 3000)
		return PULSE_2MS;
	else if (time < 6000)
		return PULSE_4MS;
	else if (time < 12000)
		return PULSE_9MS;
	else
		return PULSE_ERROR;
}
void ir_tim_interrupt(void)
{
	pulse_t pulse;
	if (received_bits >= 32)
		return;
	pulse = calc_pulse(HAL_TIM_ReadCapturedValue(&htim2, TIM_CHANNEL_1));
	switch (pulse) {
	case PULSE_SHORT:
		received_value = received_value >> 1;
		received_bits++;
		break;
	case PULSE_LONG:
		received_value = (received_value >> 1) | 0x80000000;
		received_bits++;
		break;
	case PULSE_4MS:
		received_value = 0;
		received_bits = 0;
		break;
	case PULSE_2MS:
		if (received_bits == 0)
			received_bits = 32;
		break;
	default:
	//	received_bits = 0;
		break;
	}
}
void ir_init(void)
{
	  HAL_TIM_Base_Start(&htim2);
	  HAL_TIM_IC_Start_IT(&htim2, TIM_CHANNEL_1);
}
int ir_read(void)
{
	if (received_bits != 32)
		return -1;
//	uint8_t value = received_value >> 16;
	uint8_t addr = received_value >> 0;
	uint8_t addr_inv = received_value >> 8;
	uint8_t cmd = received_value >> 16;
	uint8_t cmd_inv = received_value >> 24;
	received_bits = 0;
	frame_ready = 0;
	if((addr ^ addr_inv) == 0xFF && (cmd ^ cmd_inv)== 0xFF){
		return cmd;
	}else {
		return -2;
	}
	}

int ir_get_received_bits(void)
{
    return received_bits;
}
