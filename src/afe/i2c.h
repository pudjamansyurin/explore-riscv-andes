/*
 * i2c.h
 *
 *  Created on: Jun 13, 2022
 *      Author: pujak
 */

#ifndef AFE_I2C_H_
#define AFE_I2C_H_

#include <stdint.h>

int32_t i2c_master_init();
int32_t i2c_master_tx(const uint8_t* buf, uint32_t len);
int32_t i2c_master_rx(uint8_t* buf, uint32_t len);

#endif /* AFE_I2C_H_ */
