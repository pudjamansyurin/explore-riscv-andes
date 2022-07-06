/*
 * Copyright (c) 2012-2018 Andes Technology Corporation
 * All rights reserved.
 *
 */
#include "ae210p.h"
#include "timer.h"
#include "Driver_WDT.h"
#include "../nds-7segment/segment.h"
#include "../nds-stdout/uart.h"
#include "../nds-pfm/pfm.h"

extern void delay(uint64_t ms);
extern NDS_DRIVER_WDT Driver_WDT;

#define WDT_Dri		  Driver_WDT
#define TIMER1 		  0

volatile unsigned int counter = 0;

/* Private functions */
static void set_timer_irq_period(unsigned int timer, int tick) {
	timer_set_period(timer, tick);
    timer_irq_enable(timer);
    timer_start(timer);
}

/* Interrupt handler */
void timer_irq_handler(void) {
    timer_irq_clear(TIMER1);
    WDT_Dri.RestartTimer();
    counter++;
}

void wdt_handler (uint32_t event) {
	__asm__ ( "j _start" );
}

/* Main program */
int main(void)
{
	NDS_WDT_CAPABILITIES wdt_capabilities;
    const int limit = 10;
    int print_times = 0;
    int disable = 0;
    int factor;
	int tim_tick;
    int wdt_clksrc;

	// Initialize standard output
	uart_init(38400);

	// Initialize seven segment
	segment_init();

	// Initialize timer
	timer_init();

	// Welcome
	delay(1);
	printf("\r\n\r\n======================\r\n");
	printf("Welcome, main() called\r\n");

	// Initialize Watch-dog
	wdt_capabilities = WDT_Dri.GetCapabilities();
    if (wdt_capabilities.irq_stage) {
    	WDT_Dri.Initialize(wdt_handler);
    } else {
    	WDT_Dri.Initialize(NULL);
    }

	if (0 == wdt_capabilities.external_timer) {
        factor = 1;
        tim_tick = msec_to_tick(100);
        wdt_clksrc = NDS_WDT_CLKSRC_EXTERNAL;
    } else {
        factor = 1;
        tim_tick = 10000-1;
        wdt_clksrc = NDS_WDT_CLKSRC_APB;
    }

	printf("Timer restart WDT every %d tick.\r\n", tim_tick);
    set_timer_irq_period(TIMER1, tim_tick);
	WDT_Dri.Control(wdt_clksrc, NDS_WDT_TIME_POW_2_15);

    /* Start Watch-dog */
    WDT_Dri.Enable();

	// Infinite loop
	while(1) {
        if ((counter/factor) > print_times) {
            printf("Timer restart WDT (%d times), system still alive.\r\n", counter);
            print_times++;
        }

        if (print_times >= limit && !disable) {
        	printf("Then, We disable Timer, so the whole system will be reset by WDT.\r\n");
            timer_irq_disable(TIMER1);
            disable = 1;
        }
	}

	return 0;
}
