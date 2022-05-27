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
			segment_write(1);
			break;
		case NDS_GPIO_EVENT_PIN1:
			// Set 7-segments value to 2
			segment_write(2);
			break;
		case NDS_GPIO_EVENT_PIN2:
			// Set 7-segments value to 3
			segment_write(3);
			break;
		case NDS_GPIO_EVENT_PIN3:
			// Set 7-segments value to 4
			segment_write(4);
			break;
		case NDS_GPIO_EVENT_PIN4:
			// Set 7-segments value to 5
			segment_write(5);
			break;
		case NDS_GPIO_EVENT_PIN5:
			// Set 7-segments value to 6
			segment_write(6);
			break;
		case NDS_GPIO_EVENT_PIN6:
			// Set 7-segments value to 7
			segment_write(7);
			break;
		case NDS_GPIO_EVENT_PIN7:
			// Set 7-segments value to 8
			segment_write(8);
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

void segment_write(int num){
	uint8_t seg1, seg2, segmax;

	seg1 = num%10;
	seg2 = num/10;

	segmax = sizeof(seven_segment_value);
	if (seg2 >= segmax) {
		seg2 = seven_segment_value[segmax-1];
	}

	GPIO_Dri.Write(GPIO_7SEG_USED_MASK, 1);
	GPIO_Dri.Write(seven_segment_value[seg2] << GPIO_7SEG2_OFFSET, 0);
	GPIO_Dri.Write(seven_segment_value[seg1] << GPIO_7SEG1_OFFSET, 0);
}


