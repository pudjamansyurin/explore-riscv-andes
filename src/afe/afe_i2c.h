/*
 * i2c.h
 *
 *  Created on: Jun 13, 2022
 *      Author: pujak
 */

#ifndef AFE_AFE_I2C_H_
#define AFE_AFE_I2C_H_

#include <stdint.h>

int32_t afe_i2c_init();
int32_t afe_i2c_transmit(const uint8_t* buf, uint32_t len);
int32_t afe_i2c_receive(uint8_t* buf, uint32_t len);

#endif /* AFE_AFE_I2C_H_ */
