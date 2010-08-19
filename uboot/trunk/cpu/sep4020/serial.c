/*
 * (C) Copyright 2004
 * DAVE Srl
 * http://www.dave-tech.it
 * http://www.wawnet.biz
 * mailto:info@wawnet.biz
 *
 * (C) Copyright 2002-2004
 * Wolfgang Denk, DENX Software Engineering, <wd@denx.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Marius Groeger <mgroeger@sysgo.de>
 *
 * (C) Copyright 2002
 * Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Alex Zuepke <azu@sysgo.de>
 *
 * Copyright (C) 1999 2000 2001 Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <common.h>
#include <asm/hardware.h>

/* flush serial input queue. returns 0 on success or negative error
 * number otherwise
 */
static int serial_flush_input(void)
{
	volatile u32 tmp;

	/* keep on reading as long as the receiver is not empty */
	while(UART0_LSR&0x01) {
		tmp = UART0_RXFIFO;
	}

	return 0;
}


/* flush output queue. returns 0 on success or negative error number
 * otherwise
 */
static int serial_flush_output(void)
{
	/* wait until the transmitter is no longer busy */
	while(!(UART0_LSR & 0x40)) {
	}

	return 0;
}


void serial_setbrg (void)
{
	DECLARE_GLOBAL_DATA_PTR;

	unsigned char divisor0 = 0, divisor1 = 0;

	/* get correct divisor */
	switch(gd->baudrate) {

	case 1200:

		divisor0 = 0xA6;
		divisor1 = 0x0E;

		break;

	case 9600:

		divisor0 = 0xD4;
		divisor1 = 0x01;

		break;

	case 19200:

		divisor0 = 0xEA;
		divisor1 = 0x00;

		break;

	case 38400:

		divisor0 = 0x75;
		divisor1 = 0x00;

		break;

	case 57600:

		divisor0 = 0x4E;
		divisor1 = 0x00;

		break;

	case 115200:

		divisor0 = 0x2F;
		divisor1 = 0x00;

		break;
	}

	serial_flush_output();
	serial_flush_input();
	UART0_LCR = 0x83;
	UART0_DLBH = divisor1;
	UART0_DLBL = divisor0;
	UART0_LCR = 0x03;

	for(divisor0=0; divisor0<100; divisor0++) {
		/* NOP */
	}
}


/*
 * Initialise the serial port with the given baudrate. The settings
 * are always 8 data bits, no parity, 1 stop bit, no start bits.
 *
 */
int serial_init (void)
{
	serial_setbrg ();

	return (0);
}


/*
 * Output a single byte to the serial port.
 */
void serial_putc (const char c)
{
	/* wait for room in the transmit FIFO */
	while(!((*(volatile unsigned char *)(0x10005014))& 0x40)); /*deleted by shixq*/

	if (c=='\n'){
		UART0_TXFIFO=(0x0a);
		UART0_TXFIFO=(0x0d);
	}
	else
	UART0_TXFIFO = (unsigned char)c;

	/*
		to be polite with serial console add a line feed
		to the carriage return character
	*/
	
}

/*
 * Read a single byte from the serial port. Returns 1 on success, 0
 * otherwise. When the function is succesfull, the character read is
 * written into its argument c.
 */
int serial_tstc (void)
{
	return (UART0_LSR & 0x01);
}

/*
 * Read a single byte from the serial port. Returns 1 on success, 0
 * otherwise. When the function is succesfull, the character read is
 * written into its argument c.
 */
int serial_getc (void)
{
	int rv;

	for(;;) {
		rv = serial_tstc();

		if(rv > 0)
			return UART0_RXFIFO;
	}
}

void
serial_puts (const char *s)
{
	while (*s) {
		serial_putc (*s++);
	}
}
