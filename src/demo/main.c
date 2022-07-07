/*
 * Copyright (c) 2012-2018 Andes Technology Corporation
 * All rights reserved.
 *
 */
#include "ae210p.h"

#include "../nds-stdout/uart.h"
#include "../nds-7segment/segment.h"
#include "../afe/afe_spi.h"

#define SPI_MASTER
#ifdef  SPI_MASTER
#define SPI_MODE		NDS_SPI_MODE_MASTER
#define SPI_BITRATE		(1400*KHz)
#else
#define SPI_MODE		NDS_SPI_MODE_SLAVE
#define SPI_BITRATE		0
#endif
#define SPI_DATA_SIZE	255
#define SPI_DUMMY_SIZE	2

extern void delay(uint64_t ms);
static volatile char gpio_pressed = 0;

uint8_t u8_txBuffer[SPI_DATA_SIZE];
uint8_t u8_rxBuffer[SPI_DATA_SIZE];
uint8_t u8_outBuffer[SPI_DATA_SIZE+SPI_DUMMY_SIZE];

void transform_data(uint8_t *u8_dst, const uint8_t *u8_src, uint16_t u16_len) {
	uint16_t u16_i;

	u8_dst[0] = 0x51;
	u8_dst[1] = 0xFF;

	for(u16_i=0; u16_i<u16_len; u16_i++) {
		u8_dst[u16_i+SPI_DUMMY_SIZE] = u8_src[u16_i];
	}
}

void error_handler() {
    segment_write(0, 7);
    segment_write(1, 7);
    while(1){};
}

void gpio_callback(uint32_t event) {
	gpio_pressed = 1;
}

int main(void)
{
	uint8_t u8_counter = 0;
	uint8_t u8_i;

	// Initialize standard output
	uart_init(38400);

	// Initialize seven segment
	segment_init(gpio_callback);

	// Initialize SPI
	if (afe_spi_init(SPI_MODE, SPI_BITRATE) != NDS_DRIVER_OK) {
		error_handler();
	}

	// Generate data
	for (u8_i = 0; u8_i < SPI_DATA_SIZE; u8_i++) {
		u8_txBuffer[u8_i] = u8_i;
	}

	while(1) {
#ifdef SPI_MASTER
		while(0 == gpio_pressed)
		{};
		gpio_pressed = 0;

		//transform_data(u8_outBuffer, u8_txBuffer, sizeof(u8_txBuffer));

		if (afe_spi_transmit(u8_txBuffer, sizeof(u8_txBuffer)) != NDS_DRIVER_OK) {
			error_handler();
		}
#else
		if (afe_spi_receive(u8_rxBuffer, sizeof(u8_rxBuffer)) != NDS_DRIVER_OK) {
			error_handler();
		}
#endif

#ifdef SPI_MASTER
        segment_write(0, u8_counter++);
#else
        uint8_t u8_ok = 1;

		for (u8_i = 0; u8_i < SPI_DATA_SIZE; u8_i++) {
			if (u8_rxBuffer[u8_i] != u8_i) {
				u8_ok = 0;
			}
		}

		for (u8_i = 0; u8_i < SPI_DATA_SIZE; u8_i++) {
			u8_rxBuffer[u8_i] = 0;
		}

        if (u8_ok) {
            segment_write(0, u8_counter++);
        }
#endif
	}

	return 0;
}
