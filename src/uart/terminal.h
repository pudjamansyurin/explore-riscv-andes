/*
 * terminal.h
 *
 *  Created on: May 5, 2022
 *      Author: pujak
 */

#ifndef TERMINAL_H_
#define TERMINAL_H_

#include "nds-serial/serial.h"

/* Public function declarations */
void term_init(NDS_DRIVER_USART *p_usart, uint32_t u32_baud,
		stdin_reader_t reader, stdout_locker_t locker);
void term_in(unsigned char *str, uint16_t size);

#endif /* TERMINAL_H_ */
