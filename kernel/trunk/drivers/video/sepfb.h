/*linux/drivers/sepfb.h
* Copyright (c) 2009 prochip company
*	http://www.prochip.com.cn
*	aokikyon@gmail.com.cn
*
* sep4020 I2S-uda1341 driver.
*
* Changelog:
*
*	2008－10－15 	根据三星fb驱动修改 作者：方攀 修改：程杰 
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
*/
#ifndef __SEP4020FB_H
#define __SEP4020FB_H

struct sep4020fb_info 
{
	struct fb_info	*fb;
	struct device	*dev;
	struct clk		*clk;

	struct sep4020fb_mach_info *mach_info;

	/* raw memory addresses */
	dma_addr_t		map_dma;	/* physical */
	u_char *		map_cpu;	/* virtual */
	u_int			map_size;

	struct sep4020fb_hw	regs;

	/* addresses of pieces placed in raw buffer */
	u_char *		screen_cpu;	/* virtual address of buffer */
	dma_addr_t		screen_dma;	/* physical address of buffer */
	unsigned int	palette_ready;

	/* keep these registers in case we need to re-write palette */
	u32			pseudo_pal[16];
};

#define PALETTE_BUFF_CLEAR (0x80000000)	/* entry is clear/invalid */

int sep4020fb_init(void);

#endif

