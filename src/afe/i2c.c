/*
 * i2c.c
 *
 *  Created on: Jun 13, 2022
 *      Author: pujak
 */
#include "../afe/i2c.h"

#include "Driver_I2C.h"

#define I2C_MASTER_ADDR          0x30
#define I2C_SLAVE_ADDR           0x33

extern NDS_DRIVER_I2C Driver_I2C0;

static NDS_DRIVER_I2C* pDrv_I2C = &Driver_I2C0;

int32_t i2c_master_init()
{
	int32_t status = NDS_DRIVER_OK;

	// default slave mode
	status = pDrv_I2C->Initialize(NULL);
	if(status != NDS_DRIVER_OK)
	{
		return status;
	}

	status = pDrv_I2C->Control(NDS_I2C_BUS_CLEAR, 0);
	if(status != NDS_DRIVER_OK)
	{
		return status;
	}

	status = pDrv_I2C->PowerControl(NDS_POWER_FULL);
	if(status != NDS_DRIVER_OK)
	{
		return status;
	}

	status = pDrv_I2C->Control(NDS_I2C_BUS_SPEED, NDS_I2C_BUS_SPEED_FAST);
	if(status != NDS_DRIVER_OK)
	{
		return status;
	}

	// status = pDrv_I2C->Control(NDS_I2C_OWN_ADDRESS, (I2C_MASTER_ADDR | NDS_I2C_ADDRESS_10BIT));
	status = pDrv_I2C->Control(NDS_I2C_OWN_ADDRESS, (I2C_MASTER_ADDR));
	if(status != NDS_DRIVER_OK)
	{
		return status;
	}

	return status;
}

int32_t i2c_master_tx(const uint8_t* buf, uint32_t len)
{
	pDrv_I2C->MasterTransmit(I2C_SLAVE_ADDR << 0, buf, len, false);
	while(pDrv_I2C->GetStatus().busy);

	if(pDrv_I2C->GetDataCount() != len)
	{
		return -1;
	}

	return 0;
}

int32_t i2c_master_rx(uint8_t* buf, uint32_t len)
{
	// read-request to read the data from the ROM address
	pDrv_I2C->MasterReceive(I2C_SLAVE_ADDR << 0, buf, len, false);
	while(pDrv_I2C->GetStatus().busy);

	if(pDrv_I2C->GetDataCount() != len)
	{
		return -1;
	}

	return 0;
}


