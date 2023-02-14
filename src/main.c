/*
 * Copyright (c) 2012-2018 Andes Technology Corporation
 * All rights reserved.
 *
 */
#include "main.h"
#include "uart/terminal.h"
#include "nds-adp-gpio/adp_gpio.h"
#include "timer.h"
#include <string.h>
#include <stdio.h>
#include "ae210p.h"

/* Private macros */
#define BUFFER_SZ 	512
#define TIMER 		0

/* External variables */
extern void delay(uint64_t ms);
extern NDS_DRIVER_USART Driver_USART1;

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
    uint8_t u8_cntr;

    u8_cntr = 0;

	// Initialize 7-segment
	adp_7segInit();
	adp_7segWrite(-1, 99);

	// Initialize terminal
	term_init(&Driver_USART1, 115200, usart_reader, NULL);

	// Infinite loop
	while(1)
	{
		/* process new serial packet */
		if (0 < u16_rxCnt)
		{
			/* handle received data (blocking) */
			term_in((void*) term_buffer, u16_rxCnt);

			/* clear data */
			memset(term_buffer, 0x0, u16_rxCnt);
			u16_rxCnt = 0;
		}

        adp_7segWrite(-1, u8_cntr++);
        delay(10);
	}

	return 0;
}
