/*
 * afe.c
 *
 *  Created on: Jun 14, 2022
 *      Author: Viewtrix Technology
 */

#include "afe.h"
#include "afe_i2c.h"
#include <stdio.h>
#include <string.h>
//#include "stm32f4xx-hal-crc/crc.h"

/* Private variables */
///Buffer used for transactions
static uint8_t u8_arrI2CM_TxBuffer[I2C_AFE_CMD_LEN];
static uint8_t u8_arrI2CM_RxBuffer[I2C_RX_BUF_LEN];

///Buffer used for AFE scan
static int16_t s16_arrAFE_noiseData[AFE_NOISE_DATA_LEN];
static int16_t s16_arrAFE_selfTxData[AFE_SELFTX_DATA_LEN];
static int16_t s16_arrAFE_selfRxData[AFE_SELFRX_DATA_LEN];
static int16_t s16_arrAFE_mutualData[AFE_MUTUAL_DATA_LEN];

/* Private function declarations */
static uint8_t send(eAFE_CMD cmd);
static uint8_t get(eAFE_CMD cmd, int16_t *buffer, uint16_t length);
static void print_array(int16_t *buffer, uint16_t length);
static uint8_t afe_read_noise(void);
static uint8_t afe_read_self_tx(void);
static uint8_t afe_read_self_rx(void);
static uint8_t afe_read_mutual(void);

/* Public function definitions */
uint8_t afe_power(uint8_t on) {
	eAFE_CMD cmd;

	cmd = on ? AFE_CMD_WAKEUP : AFE_CMD_OFF;
	return (send(cmd));
}

uint8_t afe_scan(eDataType_t type) {
	uint8_t cmd;

	switch (type) {
	case DT_NOISE:
		cmd = AFE_CMD_SCAN_NOISE_START;
		break;
	case DT_SELF_TX:
		cmd = AFE_CMD_SCAN_TX_START;
		break;
	case DT_SELF_RX:
		cmd = AFE_CMD_SCAN_RX_START;
		break;
	default:
		cmd = AFE_CMD_SCAN_MUTUAL_START;
		break;
	}

	return send(cmd);
}

uint8_t afe_read(eDataType_t type) {
	uint8_t u8_ok;

	switch (type) {
	case DT_NOISE:
		u8_ok = afe_read_noise();
		break;
	case DT_SELF_TX:
		u8_ok = afe_read_self_tx();
		break;
	case DT_SELF_RX:
		u8_ok = afe_read_self_rx();
		break;
	default:
		u8_ok = afe_read_mutual();
		break;
	}

	return (u8_ok);
}

void afe_print(eDataType_t type) {
	switch (type) {
	case DT_NOISE:
		print_array(s16_arrAFE_noiseData, AFE_NOISE_DATA_LEN);
		break;
	case DT_SELF_TX:
		print_array(s16_arrAFE_selfTxData, AFE_SELFTX_DATA_LEN);
		break;
	case DT_SELF_RX:
		print_array(s16_arrAFE_selfRxData, AFE_SELFRX_DATA_LEN);
		break;
	default:
		print_array(s16_arrAFE_mutualData, AFE_MUTUAL_DATA_LEN);
		break;
	}
}

void afe_printall(void) {
	uint8_t u8_i;

	for (u8_i = 0; u8_i < DT_COUNT; u8_i++)
		afe_print(u8_i);
}

/* Private function definitions */
static uint8_t send(eAFE_CMD cmd) {
	u8_arrI2CM_TxBuffer[0] = AFE_CMD_HEADER;
	u8_arrI2CM_TxBuffer[1] = cmd;
	u8_arrI2CM_TxBuffer[2] = AFE_CMD_STOP;

	return afe_i2c_transmit(u8_arrI2CM_TxBuffer, I2C_AFE_CMD_LEN);
}

static uint8_t get(eAFE_CMD cmd, int16_t *buffer, uint16_t length) {
	uint32_t u32_crc;
	uint16_t u16_size;

	if (!send(cmd))
		return 0;

	u16_size = length * 2;
	memset(u8_arrI2CM_RxBuffer, 0, u16_size + sizeof(uint32_t));

	if (!afe_i2c_receive(u8_arrI2CM_RxBuffer, u16_size + sizeof(uint32_t)))
		return 0;

//	u32_crc = *(uint32_t*) &u8_arrI2CM_RxBuffer[u16_size];
//	if (u32_crc != crc_calculate(u8_arrI2CM_RxBuffer, u16_size))
//		return 0;

	memcpy(buffer, u8_arrI2CM_RxBuffer, u16_size);
	return 1;
}

static void print_array(int16_t *buffer, uint16_t length) {
	while (length--)
		printf("%d ", *buffer++);
	printf("\n");
}

static uint8_t afe_read_noise(void) {
	memset(s16_arrAFE_noiseData, 0, AFE_NOISE_DATA_LEN * sizeof(int16_t));

	do {
		if (!get(AFE_CMD_GET_NOISE_1ST_DATA, &s16_arrAFE_noiseData[0], 125))
			break;

		if (!get(AFE_CMD_GET_NOISE_LAST_DATA, &s16_arrAFE_noiseData[125], 35))
			break;

		return 1;
	} while (0);

	return 0;
}

static uint8_t afe_read_self_tx(void) {
	memset(s16_arrAFE_selfTxData, 0, AFE_SELFTX_DATA_LEN * sizeof(int16_t));

	return get(AFE_CMD_GET_TX_DATA, &s16_arrAFE_selfTxData[0], AFE_SELFTX_DATA_LEN);
}

static uint8_t afe_read_self_rx(void) {
	memset(s16_arrAFE_selfRxData, 0, AFE_SELFRX_DATA_LEN * sizeof(int16_t));

	return get(AFE_CMD_GET_RX_DATA, &s16_arrAFE_selfRxData[0], AFE_SELFRX_DATA_LEN);
}

static uint8_t afe_read_mutual(void) {
	memset(s16_arrAFE_mutualData, 0, AFE_MUTUAL_DATA_LEN * sizeof(int16_t));

	do {
		if (!get(AFE_CMD_GET_MUTUAL_1ST_DATA, &s16_arrAFE_mutualData[0], 125))
			break;

		if (!get(AFE_CMD_GET_MUTUAL_2ND_DATA, &s16_arrAFE_mutualData[125], 125))
			break;

		if (!get(AFE_CMD_GET_MUTUAL_3RD_DATA, &s16_arrAFE_mutualData[125 * 2], 125))
			break;

		if (!get(AFE_CMD_GET_MUTUAL_4TH_DATA, &s16_arrAFE_mutualData[125 * 3], 125))
			break;

		if (!get(AFE_CMD_GET_MUTUAL_5TH_DATA, &s16_arrAFE_mutualData[125 * 4], 125))
			break;

		if (!get(AFE_CMD_GET_MUTUAL_6TH_DATA, &s16_arrAFE_mutualData[125 * 5], 125))
			break;

		if (!get(AFE_CMD_GET_MUTUAL_LAST_DATA, &s16_arrAFE_mutualData[125 * 6], 90))
			break;

		return 1;
	} while (0);

	return 0;
}

