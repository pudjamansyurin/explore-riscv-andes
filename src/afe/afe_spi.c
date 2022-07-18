/*
 * afe_spi.c
 *
 *  Created on: Jun 15, 2022
 *      Author: Viewtrix Technology
 */
#include "afe.h"
#include "afe_spi.h"
#include "Driver_SPI.h"

#ifdef  USE_NODE_MASTER
#define SPI_MODE		NDS_SPI_MODE_MASTER
#define SPI_BITRATE		(1250*1000)
#else
#define SPI_MODE		NDS_SPI_MODE_SLAVE
#define SPI_BITRATE		0
#endif
#define SPI_DUMMY_SIZE	2
#define SPI_DUMMY_BYTE  0x55

extern NDS_DRIVER_SPI Driver_SPI1;
extern void error_handler();

static NDS_DRIVER_SPI* pDrv_SPI = &Driver_SPI1;
static volatile char spi_event_transfer_complete = 0;

static void wait_complete(void) {
	while (!spi_event_transfer_complete);
	spi_event_transfer_complete = 0;
}

static void spi_callback(uint32_t event) {
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

static void transform_data(uint8_t *u8_dst, const uint8_t *u8_src, uint16_t u16_len) {
	uint16_t u16_i;

	u8_dst[0] = SPI_DUMMY_BYTE;
	u8_dst[1] = SPI_DUMMY_BYTE;

	for(u16_i=0; u16_i<u16_len; u16_i++) {
		u8_dst[u16_i+SPI_DUMMY_SIZE] = u8_src[u16_i];
	}
}

int32_t afe_spi_init(void)
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
	status = pDrv_SPI->Control(
			 SPI_MODE  			 |
			 NDS_SPI_CPOL0_CPHA0 |
			 NDS_SPI_MSB_LSB     |
			 NDS_SPI_DATA_BITS(8),
			 SPI_BITRATE);
	if(status != NDS_DRIVER_OK)
	{
		return status;
	}

	return status;
}

int32_t afe_spi_transmit(const uint8_t* buf, uint32_t len)
{
	uint8_t u8_out[len+SPI_DUMMY_SIZE];

	transform_data(u8_out, buf, len);

	pDrv_SPI->Send(u8_out, len);
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

const sAfeTransport_t SPI_Transport =  {
		.init = afe_spi_init,
		.transmit = afe_spi_transmit,
		.receive = afe_spi_receive
};
