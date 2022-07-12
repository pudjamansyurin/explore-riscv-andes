/*
 * Copyright (c) 2012-2018 Andes Technology Corporation
 * All rights reserved.
 *
 */
#include "ae210p.h"
#include "dma_ae210p.h"

#include "../nds-stdout/uart.h"
#include "../nds-pfm/pfm.h"
#include "../nds-7segment/segment.h"
#include <string.h>

extern void delay(uint64_t ms);

#define DMA_CH			(4U)
#define DMA_REQID 		(1U)
#define BUF_SZ 			(256U)

static uint8_t	u8_srcBuf[BUF_SZ];
static uint8_t	u8_dstBuf[BUF_SZ];
static volatile uint8_t u8_dmaComplete;

static void fill_buffer(uint8_t *u8_buffer, uint16_t u16_size)
{
	uint16_t u16_i;

	for(u16_i=0; u16_i<u16_size; u16_i++) {
		u8_buffer[u16_i] = u16_i%UINT8_MAX;
	}
}

static uint8_t benchmark(uint32_t* u32_tick, uint8_t (*fn)(uint8_t*, uint8_t*, uint16_t))
{
	uint8_t u8_ok;

	// prepare data
	fill_buffer(u8_srcBuf, BUF_SZ);
	memset(u8_dstBuf, 0, BUF_SZ);

	// benchmarking
	pfm_start();
	u8_ok = fn(u8_dstBuf, u8_srcBuf, BUF_SZ);
	*u32_tick = pfm_read();
	pfm_stop();

	// compare
	if (u8_ok) {
		u8_ok = (0 == memcmp(u8_srcBuf, u8_dstBuf, BUF_SZ));
	}

	return (u8_ok);
}

static uint8_t memcpy_test(uint8_t* u8_dst, uint8_t* u8_src, uint16_t u16_size)
{
	memcpy(u8_dst, u8_src, u16_size);

	return (1);
}

static void dma_event_handler(uint32_t event)
{
	switch (event) {
		case DMA_EVENT_TERMINAL_COUNT_REQUEST:
			u8_dmaComplete = 1;
			break;
		case DMA_EVENT_ERROR:
		default:
			break;
	}
}

static uint8_t dma_test(uint8_t* u8_dst, uint8_t* u8_src, uint16_t u16_size)
{
	uint32_t u32_xfer;

	if (NDS_DRIVER_OK != dma_channel_configure(DMA_CH,
				    (uint32_t)(long) u8_srcBuf,
					(uint32_t)(long) u8_dstBuf,
				    BUF_SZ,
				    DMA_CH_CTRL_SBSIZE(DMA_BSIZE_1)          |
				    DMA_CH_CTRL_SWIDTH(DMA_WIDTH_BYTE)       |
				    DMA_CH_CTRL_DWIDTH(DMA_WIDTH_BYTE)       |
//				    DMA_CH_CTRL_SMODE_HANDSHAKE              |
				    DMA_CH_CTRL_SRCADDR_INC                  |
				    DMA_CH_CTRL_DSTADDR_INC                  |
//					DMA_CH_CTRL_SRCREQ(DMA_REQID)			 |
				    DMA_CH_CTRL_INTABT                       |
				    DMA_CH_CTRL_INTERR                       |
				    DMA_CH_CTRL_INTTC,
					dma_event_handler))
	{
		return (0);
	}

	u8_dmaComplete = 0;
	if (NDS_DRIVER_OK != dma_channel_enable(DMA_CH))  {
		return (0);
	}

	while(0 == u8_dmaComplete) {};

	if (NDS_DRIVER_OK != dma_channel_disable(DMA_CH))  {
		return (0);
	}

	u32_xfer = dma_channel_get_count(DMA_CH);
	if (u32_xfer != u16_size) {
		return (0);
	}

	return (1);
}

/* Main program */
int main(void)
{
	uint32_t u32_tick;

	// Initialize standard output
	uart_init(38400);

	// Initialize 7-segment
	segment_init(NULL);
	segment_write(0, 0);
	segment_write(1, 0);

	// Copy (software)
	if (benchmark(&u32_tick, memcpy_test)) {
		printf("memcpy(): %u ticks\r\n", (unsigned int) u32_tick);
		segment_write(0, 1);
	} else {
		printf("memcpy(): failed\r\n");
		segment_write(1, 1);
	}

	// Copy (hardware)
	dma_initialize();
	if (benchmark(&u32_tick, dma_test)) {
		printf("DMA: %u ticks\r\n", (unsigned int) u32_tick);
		segment_write(0, 2);
	} else {
		printf("DMA: failed\r\n");
		segment_write(1, 2);
	}
	dma_uninitialize();

	// Infinite loop
	while(1) {
		printf(".");
		delay(1);
	}

	return 0;
}
