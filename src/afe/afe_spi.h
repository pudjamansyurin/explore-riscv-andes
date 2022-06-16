/*
 * afe_spi.h
 *
 *  Created on: Jun 15, 2022
 *      Author: Viewtrix Technology
 */

#ifndef AFE_AFE_SPI_H_
#define AFE_AFE_SPI_H_

#include "Driver_SPI.h"

int32_t afe_spi_init(uint32_t mode, uint32_t bitrate);
int32_t afe_spi_transmit(const uint8_t* buf, uint32_t len);
int32_t afe_spi_receive(uint8_t* buf, uint32_t len);
int32_t afe_spi_transfer(uint8_t* txbuf, uint8_t* rxbuf, uint32_t len);

#endif /* AFE_AFE_SPI_H_ */
