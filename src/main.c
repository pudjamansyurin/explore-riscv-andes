/*
 * Copyright (c) 2012-2018 Andes Technology Corporation
 * All rights reserved.
 *
 */
#include "main.h"
#include "nds-terminal/terminal.h"
#include "nds-adp-gpio/adp_gpio.h"
#include "spi-master/spi.h"
#include <stdio.h>
#include <string.h>

/* Private macros ----------------------------------------------------------- */

/* Private typedef ---------------------------------------------------------- */
typedef enum
{
	SPI_STATE_NONE,
	SPI_STATE_RX,
	SPI_STATE_RX_DONE,
	SPI_STATE_TX,
	SPI_STATE_TX_DONE,
} eSPI_STATE;

/* External functions ------------------------------------------------------- */
extern void delay(uint64_t ms);

/* External variables ------------------------------------------------------- */
extern NDS_DRIVER_USART Driver_USART1;
extern NDS_DRIVER_SPI Driver_SPI1;

/* Private variables -------------------------------------------------------- */
static volatile uint8_t u8_cntr;
static volatile eSPI_STATE SPI_State;
static int16_t s16_txBuf[TX_LEN * RX_LEN];
static int16_t s16_rxBuf[TX_LEN * RX_LEN];

/* Private function declaration --------------------------------------------- */
static void gpio_callback(uint32_t u32_pin);
static void fill_buffer(int16_t* s16p_buf, int16_t s16_val, uint32_t u32_cnt);
void error_handler(void);

/* Public function definitions ---------------------------------------------= */
int main(void)
{
    // Initialize GPIO
    adp_init(gpio_callback);
    adp_pinInit(NDS_GPIO_EVENT_PIN8, NDS_GPIO_DIR_INPUT);

    // Initialize SPI
    spiMstr_init(&Driver_SPI1, 500*KHZ);

    // Initialize terminal
    term_init(&Driver_USART1, 38400, NULL, NULL);

    // Infinite loop
    while(1)
    {
        if (SPI_STATE_RX_DONE == SPI_State)
        {
            SPI_State = SPI_STATE_TX;

            u8_cntr++;
            fill_buffer(s16_txBuf, u8_cntr, ARR_SZ(s16_txBuf));
            spiMstr_send(s16_txBuf, ARR_SZ(s16_txBuf));
        }
        else if (SPI_STATE_TX_DONE == SPI_State) 
        {
            SPI_State = SPI_STATE_NONE;
            spiMstr_receive(s16_rxBuf, ARR_SZ(s16_rxBuf));

            // compare result
            for (uint32_t u32_i = 0; u32_i < ARR_SZ(s16_rxBuf); u32_i++)
            {
                if (s16_txBuf[u32_i] != s16_rxBuf[u32_i])
                {
                    error_handler();
                }
            }

            printf("u8_cntr=%d\r\n", u8_cntr);
            adp_7segWrite(-1, u8_cntr);
        }

        delay(1);
    }

    return 0;
}

/* Private function definitions --------------------------------------------- */
static void gpio_callback(uint32_t u32_pin)
{
    switch (u32_pin)
    {
        case NDS_GPIO_EVENT_PIN0:
            u8_cntr = 0;
            break;

        case NDS_GPIO_EVENT_PIN1:
            if (SPI_STATE_NONE == SPI_State)
            {
                SPI_State = SPI_STATE_RX_DONE;
            }
            break;

        case NDS_GPIO_EVENT_PIN8:
            if (SPI_STATE_TX == SPI_State)
            {
                SPI_State = SPI_STATE_TX_DONE;
            }
            break;

        default:
            break;
    }
}

static void fill_buffer(int16_t* s16p_buf, int16_t s16_val, uint32_t u32_cnt)
{
    for (uint32_t u32_i = 0; u32_i < u32_cnt; u32_i++)
    {
        s16p_buf[u32_i] = s16_val + u32_i;
    }
}

void error_handler(void)
{
    adp_7segWrite(-1, 99);
    while(1) {};
}