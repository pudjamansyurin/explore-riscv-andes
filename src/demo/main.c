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
#define SPI_BITRATE		(10*MHz)
#else
#define SPI_MODE		NDS_SPI_MODE_SLAVE
#define SPI_BITRATE		0
#endif
#define SPI_DATA_SIZE	255

extern void delay(uint64_t ms);
static volatile char gpio_pressed = 0;

uint16_t u16_txBuffer[SPI_DATA_SIZE];
uint16_t u16_rxBuffer[SPI_DATA_SIZE];

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
	uint16_t u16_i;

	// Initialize standard output
	uart_init(38400);

	// Initialize seven segment
	segment_init(gpio_callback);

	// Initialize SPI
	if (afe_spi_init(SPI_MODE, SPI_BITRATE) != NDS_DRIVER_OK) {
		error_handler();
	}

	// Generate data
	for (u16_i = 0; u16_i < SPI_DATA_SIZE; u16_i++) {
		if (0 == u16_i%2)
			u16_txBuffer[u16_i] = 0xAA;
		else
			u16_txBuffer[u16_i] = 0xCC;
	}

	while(1) {
#if 1
#ifdef SPI_MASTER
		while(0 == gpio_pressed)
		{};
		gpio_pressed = 0;

		if (afe_spi_transmit((uint8_t*) u16_txBuffer, sizeof(u16_txBuffer)) != NDS_DRIVER_OK) {
			error_handler();
		}
#else
		if (afe_spi_receive((uint8_t*) u16_rxBuffer, sizeof(u16_rxBuffer)) != NDS_DRIVER_OK) {
			error_handler();
		}
#endif
#else
		if (afe_spi_transfer((uint8_t*) u16_txBuffer, (uint8_t*) u16_rxBuffer, sizeof(u16_txBuffer)) != NDS_DRIVER_OK) {
			error_handler();
		}
#endif

#ifndef SPI_MASTER
        uint8_t u8_ok = 1;

        // compare data
        for (u16_i = 0; u16_i < SPI_DATA_SIZE; u16_i++) {
            if (u16_rxBuffer[u16_i] != u16_i) {
                u8_ok = 0;
                break;
            }
        }

        // indicator
        if (u8_ok) {
            segment_write(0, u8_counter++);
        }

        // clear data
        for (u16_i = 0; u16_i < SPI_DATA_SIZE; u16_i++) {
        	u16_rxBuffer[u16_i] = 0;
        }
#else
        segment_write(0, u8_counter++);
#endif
	}

	return 0;
}
