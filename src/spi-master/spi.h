/*
 * spi.h
 *
 *  Created on: Nov 3, 2022
 *      Author:
 */

#ifndef SPI_MASTER_H
#define SPI_MASTER_H

#include "Driver_SPI.h"

/* Public macros ------------------------------------------------------------ */
#define KHZ     1000
#define MHZ     1000000

/* Public function declarations --------------------------------------------- */
void spiMstr_init(NDS_DRIVER_SPI* p_spi, uint32_t u32_speed);
void spiMstr_transmit(const void* p_buf, uint32_t u32_cnt);
void spiMstr_receive(void* p_buf, uint32_t u32_cnt);

#endif /* SPI_MASTER_H */
