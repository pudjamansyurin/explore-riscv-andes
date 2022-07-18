/*
 * afe.h
 *
 *  Created on: Jul 18, 2022
 *      Author: pujak
 */

#ifndef AFE_AFE_H_
#define AFE_AFE_H_

#include <stdint.h>

#define USE_NODE_MASTER
#define USE_TRANSPORT_SPI

typedef struct {
	int32_t (*init)(void);
	int32_t (*transmit)(const uint8_t* buf, uint32_t len);
	int32_t (*receive)(uint8_t* buf, uint32_t len);
} sAfeTransport_t;

#endif /* AFE_AFE_H_ */
