/*
 * segment.c
 *
 *  Created on: May 24, 2022
 *      Author: pujak
 */

#include "segment.h"
#include "Driver_GPIO.h"
#include "gpio_ae210p.h"

extern NDS_DRIVER_GPIO Driver_GPIO;
#define GPIO_Dri       Driver_GPIO

#define GPIO_SW_USED_MASK     0xff
#define GPIO_7SEG_USED_MASK   0xffff0000

#define GPIO_7SEG1_OFFSET     16
#define GPIO_7SEG2_OFFSET     24

static uint8_t seven_segment_value[10] = {
		0x3f, 0x06, 0xdb, 0x4f, 0xe6, 0x6d, 0xfc, 0x07, 0x7f, 0x67
}; // seven-segment value 0 ~ 9

static void gpio_callback(uint32_t event) {
	switch (event) {
		case NDS_GPIO_EVENT_PIN0:
			// Set 7-segments value to 1
			segment_write(0, 1);
			break;
		case NDS_GPIO_EVENT_PIN1:
			// Set 7-segments value to 2
			segment_write(0, 2);
			break;
		case NDS_GPIO_EVENT_PIN2:
			// Set 7-segments value to 3
			segment_write(0, 3);
			break;
		case NDS_GPIO_EVENT_PIN3:
			// Set 7-segments value to 4
			segment_write(0, 4);
			break;
		case NDS_GPIO_EVENT_PIN4:
			// Set 7-segments value to 5
			segment_write(0, 5);
			break;
		case NDS_GPIO_EVENT_PIN5:
			// Set 7-segments value to 6
			segment_write(0, 6);
			break;
		case NDS_GPIO_EVENT_PIN6:
			// Set 7-segments value to 7
			segment_write(0, 7);
			break;
		case NDS_GPIO_EVENT_PIN7:
			// Set 7-segments value to 8
			segment_write(0, 8);
			break;
		default:
			break;
	}
}

void segment_init(void) {
	// initialize GPIO
	GPIO_Dri.Initialize(gpio_callback);

	// set GPIO direction (7-segments: output, switchs: input)
	GPIO_Dri.SetDir(GPIO_7SEG_USED_MASK, NDS_GPIO_DIR_OUTPUT);
	GPIO_Dri.SetDir(GPIO_SW_USED_MASK, NDS_GPIO_DIR_INPUT);

	// Set switchs interrupt mode to negative edge and enable it
	GPIO_Dri.Control(NDS_GPIO_SET_INTR_NEGATIVE_EDGE | NDS_GPIO_INTR_ENABLE, GPIO_SW_USED_MASK);
}

void segment_write(int channel, int num){
	int offset;

	offset = channel ? GPIO_7SEG2_OFFSET : GPIO_7SEG1_OFFSET;
	GPIO_Dri.Write(0xFF << offset, 1);
	GPIO_Dri.Write(seven_segment_value[num%10] << offset, 0);
}


