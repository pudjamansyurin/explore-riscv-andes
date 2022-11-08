/*
 * Copyright (c) 2012-2018 Andes Technology Corporation
 * All rights reserved.
 *
 */
#include "main.h"
#include "uart/terminal.h"
#include "nds-adp-gpio/adp_gpio.h"
#include <string.h>
#include <stdio.h>

/* Private macros */
#define BUFFER_SZ 	512

/* Private variables */
static char term_buffer[BUFFER_SZ];
static volatile uint16_t u16_rxCnt;

/* Private function definitions */
static void usart_reader(void *p_buffer, uint16_t u16_cnt)
{
	/* replace with flag or queue message */
	/* as this is still in interrupt context call */
	u16_rxCnt = (BUFFER_SZ < u16_cnt) ? BUFFER_SZ : u16_cnt;

	memcpy(term_buffer, p_buffer, u16_rxCnt);
}

/* Public function definitions */
int main(void)
{
	extern NDS_DRIVER_USART Driver_USART1;

	// Initialize 7-segment
	adp_7segInit();
	adp_7segWrite(-1, 77);

	// Initialize terminal
	term_init(&Driver_USART1, 625000, usart_reader, NULL);

	// Infinite loop
	while(1)
	{
		if (0 < u16_rxCnt)
		{
			/* handle received data (blocking) */
			term_in((void*) term_buffer, u16_rxCnt);

			/* clear data */
			memset(term_buffer, 0x0, u16_rxCnt);
			u16_rxCnt = 0;
		}
	}

	return 0;
}
