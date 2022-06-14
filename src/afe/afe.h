/*
 * afe.h
 *
 *  Created on: Apr 26, 2022
 *      Author: pujak
 */

#ifndef SRC_MODULES_AFE_H_
#define SRC_MODULES_AFE_H_

#include "afe_i2c.h"

/* Exported macros */
#define I2C_AFE_CMD_LEN                 3
#define I2C_RX_BUF_LEN                  (250 + sizeof(uint32_t))
#define I2C_SLAVE_ADDR                  0x33

#define TX_LEN                          21
#define RX_LEN                          40
#define FREQ_CNT                        4

#define AFE_NOISE_DATA_LEN              (RX_LEN*FREQ_CNT)
#define AFE_SELFTX_DATA_LEN             TX_LEN
#define AFE_SELFRX_DATA_LEN             RX_LEN
#define AFE_MUTUAL_DATA_LEN             (TX_LEN*RX_LEN)

typedef enum {
	AFE_CMD_WAKEUP = 0x10,
	AFE_CMD_OFF = 0x11,

	AFE_CMD_SCAN_NOISE_START = 0x20,
	AFE_CMD_GET_NOISE_1ST_DATA = 0x21,
	AFE_CMD_GET_NOISE_LAST_DATA = 0x22,

	AFE_CMD_SCAN_TX_START = 0x30,
	AFE_CMD_GET_TX_DATA = 0x31,

	AFE_CMD_SCAN_RX_START = 0x40,
	AFE_CMD_GET_RX_DATA = 0x41,

	AFE_CMD_SCAN_MUTUAL_START = 0x50,
	AFE_CMD_GET_MUTUAL_1ST_DATA = 0x51,
	AFE_CMD_GET_MUTUAL_2ND_DATA = 0x52,
	AFE_CMD_GET_MUTUAL_3RD_DATA = 0x53,
	AFE_CMD_GET_MUTUAL_4TH_DATA = 0x54,
	AFE_CMD_GET_MUTUAL_5TH_DATA = 0x55,
	AFE_CMD_GET_MUTUAL_6TH_DATA = 0x56,
	AFE_CMD_GET_MUTUAL_LAST_DATA = 0x57,

	AFE_CMD_HEADER = 0xA5,
	AFE_CMD_STOP = 0xB5,

	AFE_CMD_RESET = 0xFD,
	AFE_CMD_RESTART = 0xFE,
	AFE_CMD_UNKNOWN = 0xFF,
} eAFE_CMD;

/* Private types */
typedef enum {
	DT_NOISE, DT_MUTUAL, DT_SELF_TX, DT_SELF_RX, DT_COUNT,
} eDataType_t;

/* Public function declarations */
uint8_t afe_power(uint8_t on);
uint8_t afe_scan(eDataType_t type);
uint8_t afe_read(eDataType_t type);
void afe_print(eDataType_t type);
void afe_printall(void);

#endif /* SRC_MODULES_AFE_H_ */
