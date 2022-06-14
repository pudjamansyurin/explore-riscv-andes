/*
 * Copyright (c) 2012-2018 Andes Technology Corporation
 * All rights reserved.
 *
 */
#include "ae210p.h"
#include "../afe/afe.h"
#include "../stdout/stdout.h"
#include "../segment/segment.h"

static volatile uint8_t Delay_tmp[16];
static void delay(uint64_t ms){
	uint64_t i;

	for (i = 0; i < (ms * CPUFREQ)/1000; ++i) {
		Delay_tmp[i % 16] = (i % 16);
	}
}

int main(void)
{
	uint8_t u8_counter = 0;

	// Initialize standard output
	stdout_init(38400);

	// Initialize seven segment
	segment_init();

	// Initialize I2C
	afe_i2c_init();

	while(1) {
		afe_power(1);
		afe_scan(DT_NOISE);
		afe_read(DT_NOISE);
		afe_print(DT_NOISE);

		afe_scan(DT_SELF_TX);
		afe_read(DT_SELF_TX);
		afe_print(DT_SELF_TX);

		afe_scan(DT_SELF_RX);
		afe_read(DT_SELF_RX);
		afe_print(DT_SELF_RX);

		afe_scan(DT_MUTUAL);
		afe_read(DT_MUTUAL);
		afe_print(DT_MUTUAL);

		afe_power(0);

		// Indicators
		segment_write(0, u8_counter);
		u8_counter++;
		delay(10);
	}

	return 0;
}
