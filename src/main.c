/*
 * Copyright (c) 2012-2018 Andes Technology Corporation
 * All rights reserved.
 *
 */
#include "main.h"
#include "nds-terminal/terminal.h"
#include "nds-adp-gpio/adp_gpio.h"
#include <stdio.h>

/* Private macros */

/* External variables */
extern void delay(uint64_t ms);
extern NDS_DRIVER_USART Driver_USART1;

/* Private variables */

/* Private function definitions */

/* Public function definitions */
int main(void)
{
	uint8_t u8_cntr;

	u8_cntr = 0;

	// Initialize 7-segment
	adp_7segInit();
	adp_7segWrite(-1, 99);

	// Initialize terminal
	term_init(&Driver_USART1, 38400, NULL, NULL);

	// Infinite loop
	while(1)
	{
        printf("u8_cntr=%d\r\n", u8_cntr);
		adp_7segWrite(-1, u8_cntr++);
		delay(10);
	}

	return 0;
}
