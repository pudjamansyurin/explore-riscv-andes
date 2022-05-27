/*
 * Copyright (c) 2012-2018 Andes Technology Corporation
 * All rights reserved.
 *
 */
#include "ae210p.h"
#include "../stdout/stdout.h"
#include "../segment/segment.h"

static volatile uint8_t Delay_tmp[16] = {0};
static void delay(uint64_t ms){
	uint64_t i;

	for (i = 0; i < (ms * CPUFREQ)/1000; ++i) {
		Delay_tmp[i % 16] = (i % 16);
	}
}

volatile int global = 5;
volatile int global_bss;

int main(void)
{
	uint8_t u8_counter = 0;

	// Initialize standard output
	stdout_init(38400);

	// Initialize seven segment
	segment_init();

	if (global != 5) {
		printf("data section copy failed.\r\n");
		while (1) ;
	}
	printf("data section copy successfully.\r\n");

	if (global_bss != 0) {
		printf("bss section clear failed.\r\n");
		while (1) ;
	}
	printf("bss section clean successfully.\r\n");

	// Infinite loop
	while(1) {
		printf("counter = %d\r\n", u8_counter);
		segment_write(u8_counter);

		u8_counter += 1;
		u8_counter %= 100;
		delay(1);
	}

	return 0;
}
