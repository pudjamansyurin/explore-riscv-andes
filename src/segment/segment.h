/*
 * segment.h
 *
 *  Created on: May 24, 2022
 *      Author: pujak
 */

#ifndef SEGMENT_SEGMENT_H_
#define SEGMENT_SEGMENT_H_

#include "Driver_GPIO.h"

void segment_init(NDS_GPIO_SignalEvent_t cb_event);
void segment_write(int channel, int num);

#endif /* SEGMENT_SEGMENT_H_ */
