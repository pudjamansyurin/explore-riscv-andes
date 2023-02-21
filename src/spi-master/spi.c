/*
 * spi.c
 *
 *  Created on: May 5, 2022
 *      Author: 
 */

#include "spi.h"
#include <string.h>

/* External symbols --------------------------------------------------------- */
extern void error_handler(void);

/* Private variables -------------------------------------------------------- */
static volatile uint8_t u8_xferDone;
static NDS_DRIVER_SPI* hspi;

/* Private function definitions --------------------------------------------- */
static void spi_callback(uint32_t event) 
{
    switch (event) 
    {
        case NDS_SPI_EVENT_TRANSFER_COMPLETE:
            u8_xferDone = 1;
            break;

        case NDS_SPI_EVENT_MODE_FAULT:
        case NDS_SPI_EVENT_DATA_LOST:
            error_handler();
            break;

        default:
            break;
    }
}

static void wait_complete(void) 
{
    while(!u8_xferDone) {};
    u8_xferDone = 0;
}

/* Public function definitions ---------------------------------------------- */
void spiMstr_init(NDS_DRIVER_SPI* p_spi, uint32_t u32_speed) 
{
    hspi = p_spi;

    // initialize SPI
    hspi->Initialize(spi_callback);

    // power up the SPI peripheral
    hspi->PowerControl(NDS_POWER_FULL);

    // configure the SPI to master, 8-bit data length
    hspi->Control(NDS_SPI_MODE_MASTER 
                    | NDS_SPI_CPOL0_CPHA0
                    | NDS_SPI_MSB_LSB
                    | NDS_SPI_SS_MASTER_HW_OUTPUT
                    | NDS_SPI_DATA_BITS(16), 
                    u32_speed);
}

void spiMstr_send(const void* p_buf, uint32_t u32_cnt)
{
    hspi->Send(p_buf, u32_cnt);
    wait_complete();
}

void spiMstr_receive(void* p_buf, uint32_t u32_cnt)
{
    hspi->Receive(p_buf, u32_cnt);
    wait_complete();
}