/*
 * afe_spi.c
 *
 *  Created on: Jun 15, 2022
 *      Author: Viewtrix Technology
 */
#include "afe_spi.h"

extern NDS_DRIVER_SPI Driver_SPI1;
extern void error_handler();

static NDS_DRIVER_SPI* pDrv_SPI = &Driver_SPI1;
static volatile char spi_event_transfer_complete = 0;

static void wait_complete(void) {
	while (!spi_event_transfer_complete);
	spi_event_transfer_complete = 0;
}

void spi_callback(uint32_t event) {
	switch (event) {
		case NDS_SPI_EVENT_TRANSFER_COMPLETE:
			spi_event_transfer_complete = 1;
			break;
		case NDS_SPI_EVENT_DATA_LOST:
			error_handler();
			break;
		case NDS_SPI_EVENT_MODE_FAULT:
			error_handler();
			break;
	}
}

int32_t afe_spi_init(uint32_t mode, uint32_t bitrate)
{
	int32_t status = NDS_DRIVER_OK;

	// initialize SPI
	status = pDrv_SPI->Initialize(spi_callback);
	if(status != NDS_DRIVER_OK)
	{
		return status;
	}

	// power up the SPI peripheral
	status = pDrv_SPI->PowerControl(NDS_POWER_FULL);
	if(status != NDS_DRIVER_OK)
	{
		return status;
	}

	// configure the SPI to slave, 8-bit data length
	status = pDrv_SPI->Control(mode  |
			 NDS_SPI_CPOL0_CPHA0 |
			 NDS_SPI_MSB_LSB     |
			 NDS_SPI_DATA_BITS(8), bitrate);
	if(status != NDS_DRIVER_OK)
	{
		return status;
	}

	return status;
}

int32_t afe_spi_transmit(const uint8_t* buf, uint32_t len)
{
	pDrv_SPI->Send(buf, len);
	wait_complete();

	if(pDrv_SPI->GetDataCount() != len)
	{
		return -1;
	}

	return 0;
}

int32_t afe_spi_receive(uint8_t* buf, uint32_t len)
{
	pDrv_SPI->Receive(buf, len);
	wait_complete();

	if(pDrv_SPI->GetDataCount() != len)
	{
		return -1;
	}

	return 0;
}

int32_t afe_spi_transfer(uint8_t* txbuf, uint8_t* rxbuf, uint32_t len)
{
	pDrv_SPI->Transfer(txbuf, rxbuf, len);
	wait_complete();

	if(pDrv_SPI->GetDataCount() != len)
	{
		return -1;
	}

	return 0;
}
