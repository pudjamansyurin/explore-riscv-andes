/*
 * Copyright (c) 2012-2018 Andes Technology Corporation
 * All rights reserved.
 *
 */
#include "ae210p.h"

#include "../nds-stdout/uart.h"
#include "../nds-7segment/segment.h"

#include "../afe/afe.h"
#ifdef USE_TRANSPORT_SPI
#include "../afe/afe_spi.h"
const sAfeTransport_t *Transport = &SPI_Transport;
#else
#include "../afe/afe_i2c.h"
const sAfeTransport_t *Transport = &I2C_Transport;
#endif

#define DATA_SIZE	255

extern void delay(uint64_t ms);
static volatile char gpio_pressed = 0;

static uint8_t u8_buffer[DATA_SIZE];

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
	if (Transport->init() != NDS_DRIVER_OK) {
		error_handler();
	}

#ifdef USE_DIR_TRANSMIT
	// Generate data
	for (u8_i = 0; u8_i < DATA_SIZE; u8_i++) {
		u8_buffer[u8_i] = u8_i;
	}
#endif

	while(1) {
#ifdef USE_NODE_MASTER
		while(0 == gpio_pressed) {} ;
		gpio_pressed = 0;
#endif

#ifdef USE_DIR_TRANSMIT
		if (Transport->transmit(u8_buffer, sizeof(u8_buffer)) != NDS_DRIVER_OK) {
			error_handler();
		}
#else
		if (Transport->receive(u8_buffer, sizeof(u8_buffer)) != NDS_DRIVER_OK) {
			error_handler();
		}
#endif

#ifdef USE_DIR_TRANSMIT
        segment_write(0, u8_counter++);
#else
        uint8_t u8_ok = 1;

		for (u8_i = 0; u8_i < DATA_SIZE; u8_i++) {
			if (u8_buffer[u8_i] != u8_i) {
				u8_ok = 0;
			}
		}

		for (u8_i = 0; u8_i < DATA_SIZE; u8_i++) {
			u8_buffer[u8_i] = 0;
		}

        if (u8_ok) {
            segment_write(0, u8_counter++);
        }
#endif

#ifdef USE_NODE_MASTER
        delay(100);
#endif
	}

	return 0;
}
