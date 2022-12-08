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
#include "nds-lmdma/lm.h"
#include "nds-lmdma/lmdma.h"
#include "ae210p.h"

/* Private macros */
#define BUFFER_SZ 	512
#define TIMER 		0

/* External variables */
extern NDS_DRIVER_USART Driver_USART1;

/* Private variables */
static char term_buffer[BUFFER_SZ];
static volatile uint16_t u16_rxCnt;
static volatile uint8_t u8_scan;
static volatile uint8_t u8_lmdma_busy;

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

static void lmdma_callback(unsigned int error)
{
	(void) error;
	u8_lmdma_busy = 0;
}

static int lmdma_test(void)
{
	static char *p_str __attribute__ ((aligned (4))) = "Hello World\r\n";
	unsigned int src_addr;
	unsigned int dst_addr;
	unsigned int count;
	int rc;

	do {
		// Initialize ILM
		rc = lm_initialize(LM_ILM, 0xA00000);
		if (0 > rc) {
			break;
		}

		// Initialize DLM
		rc = lm_initialize(LM_DLM, 0xC00000);
		if (0 > rc) {
			break;
		}

		// Initialize LM-DMA
		rc = lmdma_initialize(lmdma_callback);
		if (0 > rc) {
			break;
		}

		// Start LM-DMA transfer
		u8_lmdma_busy = 1;
		count = strlen(p_str);
		src_addr = (unsigned int)(p_str);
		dst_addr = (unsigned int)(&AE210P_UART2->THR);

		rc = lmdma_ch_configure(0,
			  src_addr,
			  dst_addr,
			  count,
			  (DMA_GCSW_FSM_FSTART_ESADDR | DMA_GCSW_SCMD_ENQ | DMA_GCSW_DMA_ENGINE_ON),
			  (DMA_SETUP_ILM | DMA_SETUP_LM_DDR | DMA_SETUP_BYTE | DMA_SETUP_ESTR(0) |
			  DMA_SETUP_CIE_EN | DMA_SETUP_SIE_EN | DMA_SETUP_EIE_EN | DMA_SETUP_UE_EN | DMA_SETUP_NONCACHE),
			  0,
			  0);
		if (0 > rc) {
			break;
		}

		/* blocking wait */
		// while(u8_lmdma_busy);

	} while(0);

	return (rc);
}

/* Interrupt handler */
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
	term_init(&Driver_USART1, 115200, usart_reader, NULL);

	// Initialize timer
	timer_init();
	timer_set_period(TIMER, sec_to_tick(1));
	timer_irq_enable(TIMER);

	// Test LMDMA
	if (-1 == lmdma_test())
	{
		printf("LM-DMA fail\n");
	}

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
			adp_7segWrite(-1, u8_cntr);

			u8_cntr++;
			u8_scan = 0;
		}
	}

	return 0;
}
