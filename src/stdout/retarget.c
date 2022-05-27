/*
 * retarget.c
 *
 *  Created on: May 25, 2022
 *      Author: pujak
 */

#include "stdout.h"

#undef putchar
inline int putchar(int c)
{
	uart_putc(c);
	return c;
}

__attribute__((used))
void nds_write(const unsigned char *buf, int size)
{
	int i;
	for (i = 0; i < size; i++)
		putchar(buf[i]);
}

struct stat;
int _fstat(int fd, struct stat *buf)
{
	return 0;
}

int _write(int __fd, const void *__buf, int __nbyte)
{
	unsigned char *buf;
	int i;

	buf = (unsigned char*) __buf;
	for (i = 0; i < __nbyte; i++)
		putchar(*buf++);

	return __nbyte;
}

