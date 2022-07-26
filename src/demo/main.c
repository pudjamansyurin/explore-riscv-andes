/*
 * Copyright (c) 2012-2018 Andes Technology Corporation
 * All rights reserved.
 *
 */
#include "ae210p.h"
#include "timer.h"
#include "../nds-stdout/uart.h"

//#define TICK_HZ         1
//#ifdef CFG_SIMU
// #define SIMU_FACTOR    (4)
//#else
// #define SIMU_FACTOR    (0)
//#endif

#define TIMER1          0
#define TIMER2          1
#define TIMER3          2

extern void delay(uint64_t ms);

void error_handler()
{
	printf("error: %s, %d\r\n", __FILE__, __LINE__);
    while(1){};
}

void timer_irq_handler(void)
{
	if (timer_irq_status(TIMER1)) {
		timer_irq_clear(TIMER1);
		printf("TIM1");
	} else if (timer_irq_status(TIMER2)) {
		timer_irq_clear(TIMER2);
		printf("TIM2");
	} else if (timer_irq_status(TIMER3)) {
		timer_irq_clear(TIMER3);
		printf("TIM3");
	}
}

void timer_configure(unsigned int tmr, unsigned int ms) {
	unsigned int cycles;

	cycles = msec_to_tick(ms);
	timer_set_period(tmr, cycles);
	timer_irq_enable(tmr);
}

int main(void)
{
	uart_init(38400);
	timer_init();

	timer_configure(TIMER1, 1000);
	timer_start(TIMER1);

	timer_configure(TIMER2, 500);
	timer_start(TIMER2);

	timer_configure(TIMER3, 250);
	timer_start(TIMER3);

	printf("hello world\r\n");

	while(1) {
		printf(".");
		delay(10);
	}

	return 0;
}
