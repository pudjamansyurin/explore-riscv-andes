/*
 * Copyright (c) 2012-2018 Andes Technology Corporation
 * All rights reserved.
 *
 */
#include "ae210p.h"
#include "../nds-stdout/uart.h"
#include "../nds-pfm/pfm.h"

extern void delay(uint64_t ms);

static uint32_t u32fn_factorial(uint16_t u16_num)
{
	uint32_t u32_result = 1;
	uint32_t u32_i;

	for (u32_i = 1; u32_i <= u16_num; u32_i++) {
		u32_result *= u32_i;
	}
	return (u32_result);
}

int main(void)
{
	uint16_t u16_num;
	uint32_t u32_result;
//	uint32_t u32_cycle;

	// Test function
	u16_num = 20;
//	pfm_start();
	u32_result = u32fn_factorial(u16_num);
//	u32_cycle = pfm_read();
//	pfm_stop();

	// Initialize stdout
	uart_init(38400);
	printf("%u! = %lu\r\n", u16_num, u32_result);
//	printf("cycle = %lu\r\n", u32_cycle);

	// Infinite loop
	while(1) {}

	return 0;
}
