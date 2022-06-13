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
	uint8_t u8_arrTxBuffer[3];

	// Initialize standard output
	stdout_init(38400);

	// Initialize seven segment
	segment_init();

	// Initialize I2C
	i2c_master_init();

	while(1) {
		u8_arrTxBuffer[0] = AFE_CMD_HEADER;
		u8_arrTxBuffer[1] = AFE_CMD_WAKEUP;
		u8_arrTxBuffer[2] = AFE_CMD_STOP;
		i2c_master_tx(u8_arrTxBuffer, sizeof(u8_arrTxBuffer));

		delay(10);
	}

	return 0;
}
