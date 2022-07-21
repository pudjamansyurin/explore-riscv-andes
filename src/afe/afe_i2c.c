/*
 * i2c.c
 *
 *  Created on: Jun 13, 2022
 *      Author: pujak
 */

#include "afe_i2c.h"
#include "Driver_I2C.h"

#define I2C_SLAVE_ADDR          0x33
#define I2C_BUS_SPEED			NDS_I2C_BUS_SPEED_FAST_PLUS

extern NDS_DRIVER_I2C Driver_I2C0;
static NDS_DRIVER_I2C* pDrv_I2C = &Driver_I2C0;
static volatile char i2c_event_transfer_complete = 0;

static void wait_complete(void) {
	while (!i2c_event_transfer_complete);
	i2c_event_transfer_complete = 0;
}

static void i2c_callback(uint32_t event)
{
	// callback function body
	if(event & NDS_I2C_EVENT_TRANSFER_INCOMPLETE)
	{
		if(event & NDS_I2C_EVENT_SLAVE_RECEIVE)
		{
			// Slave address hit is the entry for slave mode driver
			// to detect slave RX/TX action depend on master TX/RX action

		}
		else if(event & NDS_I2C_EVENT_SLAVE_TRANSMIT)
		{
			// Slave address hit is the entry for slave mode driver
			// to detect slave RX/TX action depend on master TX/RX action

		}
	}
	else if(event & NDS_I2C_EVENT_TRANSFER_DONE)
	{
		i2c_event_transfer_complete = 1;
	}
	else if(event & NDS_I2C_EVENT_ADDRESS_NACK)
	{
		i2c_event_transfer_complete = 1;
	}
}

int32_t afe_i2c_init()
{
	int32_t status = NDS_DRIVER_OK;

	status = pDrv_I2C->Initialize(i2c_callback);
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

	status = pDrv_I2C->Control(NDS_I2C_BUS_SPEED, I2C_BUS_SPEED);
	if(status != NDS_DRIVER_OK)
	{
		return status;
	}

#ifndef USE_NODE_MASTER
	// status = pDrv_I2C->Control(NDS_I2C_OWN_ADDRESS, (I2C_MASTER_ADDR | NDS_I2C_ADDRESS_10BIT));
	status = pDrv_I2C->Control(NDS_I2C_OWN_ADDRESS, (I2C_SLAVE_ADDR));
	if(status != NDS_DRIVER_OK)
	{
		return status;
	}
#endif

	return status;
}

int32_t afe_i2c_transmit(const uint8_t* buf, uint32_t len)
{
#ifdef USE_NODE_MASTER
	pDrv_I2C->MasterTransmit(I2C_SLAVE_ADDR, buf, len, false);
#else
	pDrv_I2C->SlaveTransmit(buf, len);
#endif
	wait_complete();

	if(pDrv_I2C->GetDataCount() != len)
	{
		return -1;
	}
	return 0;
}

int32_t afe_i2c_receive(uint8_t* buf, uint32_t len)
{
#ifdef USE_NODE_MASTER
	pDrv_I2C->MasterReceive(I2C_SLAVE_ADDR, buf, len, false);
#else
	pDrv_I2C->SlaveReceive(buf, len);
#endif
	wait_complete();

	if(pDrv_I2C->GetDataCount() != len)
	{
		return -1;
	}
	return 0;
}

const sAfeTransport_t I2C_Transport =  {
		.init = afe_i2c_init,
		.transmit = afe_i2c_transmit,
		.receive = afe_i2c_receive
};


