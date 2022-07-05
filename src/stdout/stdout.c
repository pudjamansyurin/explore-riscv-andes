/*
 * stdout.c
 *
 *  Created on: May 24, 2022
 *      Author: pujak
 */

#include "stdout.h"
#include "Driver_USART.h"
#include "usart_ae210p.h"

extern NDS_DRIVER_USART Driver_USART1;
#define USART_Dri       Driver_USART1

static volatile int usart_event_complete = 0;

static void wait_usart_complete(void) {
	while (!usart_event_complete);
	usart_event_complete = 0;
}

static void usart_callback(uint32_t event) {
	switch (event) {
		case NDS_USART_EVENT_RECEIVE_COMPLETE:
		case NDS_USART_EVENT_TRANSFER_COMPLETE:
		case NDS_USART_EVENT_SEND_COMPLETE:
		case NDS_USART_EVENT_TX_COMPLETE:
			usart_event_complete = 1;
			break;

	    case NDS_USART_EVENT_RX_TIMEOUT:
	        while (1);
	        break;

	    case NDS_USART_EVENT_RX_OVERFLOW:
	    case NDS_USART_EVENT_TX_UNDERFLOW:
	        while (1);
	        break;
	}
}

void stdout_init(uint32_t baudrate) {
	// initialize the USART driver
	USART_Dri.Initialize(usart_callback);

	// power up the USART peripheral
	USART_Dri.PowerControl(NDS_POWER_FULL);

	// configure the USART control
	USART_Dri.Control(NDS_USART_MODE_ASYNCHRONOUS |
			   NDS_USART_DATA_BITS_8 |
			   NDS_USART_PARITY_NONE |
			   NDS_USART_STOP_BITS_1 |
			   NDS_USART_FLOW_CONTROL_NONE, baudrate);

	// enable transmitter lines
	USART_Dri.Control(NDS_USART_CONTROL_TX, 1);
}

void uart_putc(int c) {
	USART_Dri.Send(&c, 1);
	wait_usart_complete();
}
