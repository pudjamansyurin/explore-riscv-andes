/*
 * Copyright (c) 2012-2018 Andes Technology Corporation
 * All rights reserved.
 *
 */
#include "main.h"
#include "uart/terminal.h"
#include "nds-adp-gpio/adp_gpio.h"
#include "afe/afe.h"
#include "timer.h"
#include <string.h>
#include <stdio.h>

/* Private macros */
#define BUFFER_SZ 	512
#define TIMER 		0

/* External variables */
extern NDS_DRIVER_USART Driver_USART1;

/* Private variables */
static char term_buffer[BUFFER_SZ];
static volatile uint16_t u16_rxCnt;
static volatile uint8_t u8_scan;

/* Private function definitions */
static void usart_reader(void *p_buffer, uint16_t u16_cnt)
{
	/* replace with flag or queue message */
	/* as this is still in interrupt context call */
	u16_rxCnt = (BUFFER_SZ < u16_cnt) ? BUFFER_SZ : u16_cnt;

	memcpy(term_buffer, p_buffer, u16_rxCnt);
}

static void btn_reader(uint32_t event)
{
	static uint8_t u8_run;

	u8_run ? timer_stop(TIMER) : timer_start(TIMER);

	u8_run = !u8_run;
}

void timer_irq_handler(void)
{
	timer_irq_clear(0);

	u8_scan	= 1;
}

/* Public function definitions */
int main(void)
{
	uint8_t u8_cntr;

	// Initialize button
	adp_btnInit(btn_reader);

	// Initialize 7-segment
	adp_7segInit();
	adp_7segWrite(-1, 99);

	// Initialize terminal
	term_init(&Driver_USART1, 125000, usart_reader, NULL);

	// Initialize timer
	timer_init();
	timer_set_period(TIMER, sec_to_tick(1));
	timer_irq_enable(TIMER);

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

		/* process emulated scan request */
		if (u8_scan)
		{
			afe_scan();
			adp_7segWrite(-1, u8_cntr++);

			u8_scan = 0;
		}
	}

	return 0;
}
