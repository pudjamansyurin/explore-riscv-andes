/*
 * Copyright (c) 2012-2018 Andes Technology Corporation
 * All rights reserved.
 *
 */
#include "ae210p.h"
#include <string.h>
#include <stdio.h>

#include "usart_ae210p.h"
#include "../uart/terminal.h"

extern void delay(uint64_t ms);

/* Private variables */
static char shell_buffer[512];
static volatile uint16_t u16_rxCnt;

/* Private function definitions */
static void usart_reader(void *p_buffer, uint16_t u16_cnt)
{
	/* replace with flag or queue message */
	/* as this is still in interrupt context call */
	if (sizeof(shell_buffer) < u16_cnt)
	{
		u16_rxCnt = sizeof(shell_buffer);
	}
	else
	{
		u16_rxCnt = u16_cnt;
	}

	memcpy(shell_buffer, p_buffer, u16_rxCnt);
}

/* Public function definitions */
int main(void)
{
	extern NDS_DRIVER_USART Driver_USART1;

	// Initialize terminal
	terminal_init(&Driver_USART1, 250000, usart_reader, NULL);

	// Infinite loop
	while(1)
	{
		if (0 < u16_rxCnt)
		{
			/* handle received data (blocking) */
			terminal_in((void*) shell_buffer, u16_rxCnt);

			/* clear data */
			memset(shell_buffer, 0x0, u16_rxCnt);
			u16_rxCnt = 0;
		}

		delay(1);
	}

	return 0;
}
