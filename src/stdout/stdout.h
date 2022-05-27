/*
 * stdout.h
 *
 *  Created on: May 24, 2022
 *      Author: pujak
 */

#ifndef STDOUT_STDOUT_H_
#define STDOUT_STDOUT_H_

#include <stdint.h>
#include <stdio.h>

void stdout_init(uint32_t baudrate);
void uart_putc(int c);

#endif /* STDOUT_STDOUT_H_ */
