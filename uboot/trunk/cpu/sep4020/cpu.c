/*
 * (C) Copyright 2004
 * DAVE Srl
 * http://www.dave-tech.it
 * http://www.wawnet.biz
 * mailto:info@wawnet.biz
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

/*
 * SEP4020 CPU specific code
 */

#include <common.h>
#include <command.h>
#include <asm/hardware.h>

static void sep4020_flush_cache(void)
{
	volatile int i;
	/* flush cycle */
	for(i=0x10002000;i<0x10004800;i+=16)
	{
		*((int *)i)=0x0;
	}
}


int cpu_init (void)
{

	return 0;
}


int cleanup_before_linux (void)
{
	/*
		cache memory should be enabled before calling
		Linux to make the kernel uncompression faster
	*/

	TIMER_T1CR = 0x0;
	TIMER_T1LCR = 0x0;
	

	return 0;
}



void reset_cpu (ulong addr)
{
	/*
		reset the cpu using watchdog
	*/

	/* Disable the watchdog.*/
	printf("Reset the CPU ...\n");
	
	RTC_INT_EN = 0x00;
	INTC_IER = 0x80000000;
	INTC_IMR = 0x7FFFFFFF;
	
	RTC_WD_CNT = 0x01000001;
	RTC_CTR = 0x1000002;
	RTC_INT_EN = 0x22;

	while(1) {
		/*NOP*/
	}
}

int do_reset (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	disable_interrupts ();
	reset_cpu (0);

	/*NOTREACHED*/
	return (0);
}

void icache_enable (void)
{
	ulong reg;

	sep4020_flush_cache();
}

void icache_disable (void)
{

}

int icache_status (void)
{
	return 0;
}

void dcache_enable (void)
{
	icache_enable();
}

void dcache_disable (void)
{
	icache_disable();
}

int dcache_status (void)
{
	return dcache_status();
}

/*
	RTC stuff
*/
#include <rtc.h>
#ifndef BCD2HEX
	#define BCD2HEX(n)  ((n>>4)*10+(n&0x0f))
#endif
#ifndef HEX2BCD
	#define HEX2BCD(x) ((((x) / 10) << 4) + (x) % 10)
#endif


void rtc_get (struct rtc_time* tm)
{
	tm->tm_year  = RTC_STA_YMD>>9;
	tm->tm_mon   = (RTC_STA_YMD>>5) & 0x0F;
	tm->tm_wday   = RTC_STA_YMD & 0x1F;
	tm->tm_mday   = RTC_STA_YMD & 0x1F;
	tm->tm_hour  = RTC_STA_HMS>>12;
	tm->tm_min  = (RTC_STA_HMS>>6) & 0x3F;
	tm->tm_sec  = RTC_STA_HMS & 0x3F;

	if (tm->tm_sec==0) {
		/* we have to re-read the rtc data because of the "one second deviation" problem */
		/* see RTC datasheet for more info about it */
		tm->tm_year  = RTC_STA_YMD>>9;
		tm->tm_mon   = (RTC_STA_YMD>>5) & 0x0F;
		tm->tm_wday   = RTC_STA_YMD & 0x1F;
		tm->tm_mday   = RTC_STA_YMD & 0x1F;
		tm->tm_hour  = RTC_STA_HMS>>12;
		tm->tm_min  = (RTC_STA_HMS>>6) & 0x3F;
		tm->tm_sec  = RTC_STA_HMS & 0x3F;
	}
}

void rtc_set (struct rtc_time* tm)
{
	
	RTC_CONFIG_CHECK = 0xAAAAAAAA;
	udelay(200);
	RTC_STA_YMD = (tm->tm_year << 9)|(tm->tm_mon << 5)|tm->tm_mday;
	udelay(200);
	RTC_STA_HMS = (tm->tm_hour << 12)|(tm->tm_min << 6)|tm->tm_sec;
	udelay(200);
	RTC_CONFIG_CHECK = 0x00000000;
	
}

void rtc_reset (void)
{
	RTC_CONFIG_CHECK = 0xAAAAAAAA;
	udelay(200);
	RTC_STA_YMD = 0x0;
	udelay(200);
	RTC_STA_HMS = 0x0;
	udelay(200);
	RTC_CONFIG_CHECK = 0x00000000;
}
