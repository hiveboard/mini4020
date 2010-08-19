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

#include <common.h>
#include <asm/hardware.h>

#include <asm/proc-armv/ptrace.h>

/* we always count down the max. */
#define TIMER_LOAD_VAL 0xffffffff

void test(void)
{
	unsigned int second,minute,hour;

	RTC_INT_STS=RTC_INT_STS;

	second = RTC_STA_HMS & 0x3F;
	
	minute = (RTC_STA_HMS>>6) & 0x3F;
	
	hour = RTC_STA_HMS>>12;	

	printf("%d:%d:%d\n",hour,minute,second);
}

void enable_interrupts (void)
{
	return;
}
int disable_interrupts (void)
{
	return 0;
}



void bad_mode (void)
{
	panic ("Resetting CPU ...\n");
	reset_cpu (0);
}

void show_regs (struct pt_regs *regs)
{
	unsigned long flags;
	const char *processor_modes[] =
		{ "USER_26", "FIQ_26", "IRQ_26", "SVC_26", "UK4_26", "UK5_26",
				"UK6_26", "UK7_26",
		"UK8_26", "UK9_26", "UK10_26", "UK11_26", "UK12_26", "UK13_26",
				"UK14_26", "UK15_26",
		"USER_32", "FIQ_32", "IRQ_32", "SVC_32", "UK4_32", "UK5_32",
				"UK6_32", "ABT_32",
		"UK8_32", "UK9_32", "UK10_32", "UND_32", "UK12_32", "UK13_32",
				"UK14_32", "SYS_32"
	};

	flags = condition_codes (regs);

	printf ("pc : [<%08lx>]    lr : [<%08lx>]\n"
			"sp : %08lx  ip : %08lx  fp : %08lx\n",
			instruction_pointer (regs),
			regs->ARM_lr, regs->ARM_sp, regs->ARM_ip, regs->ARM_fp);
	printf ("r10: %08lx  r9 : %08lx  r8 : %08lx\n",
			regs->ARM_r10, regs->ARM_r9, regs->ARM_r8);
	printf ("r7 : %08lx  r6 : %08lx  r5 : %08lx  r4 : %08lx\n",
			regs->ARM_r7, regs->ARM_r6, regs->ARM_r5, regs->ARM_r4);
	printf ("r3 : %08lx  r2 : %08lx  r1 : %08lx  r0 : %08lx\n",
			regs->ARM_r3, regs->ARM_r2, regs->ARM_r1, regs->ARM_r0);
	printf ("Flags: %c%c%c%c",
			flags & CC_N_BIT ? 'N' : 'n',
			flags & CC_Z_BIT ? 'Z' : 'z',
			flags & CC_C_BIT ? 'C' : 'c', flags & CC_V_BIT ? 'V' : 'v');
	printf ("  IRQs %s  FIQs %s  Mode %s%s\n",
			interrupts_enabled (regs) ? "on" : "off",
			fast_interrupts_enabled (regs) ? "on" : "off",
			processor_modes[processor_mode (regs)],
			thumb_mode (regs) ? " (T)" : "");
}

void do_undefined_instruction (struct pt_regs *pt_regs)
{
	printf ("undefined instruction\n");
	show_regs (pt_regs);
	bad_mode ();
}

void do_software_interrupt (struct pt_regs *pt_regs)
{
	printf ("software interrupt\n");
	show_regs (pt_regs);
	bad_mode ();
}

void do_prefetch_abort (struct pt_regs *pt_regs)
{
	printf ("prefetch abort\n");
	show_regs (pt_regs);
	bad_mode ();
}

void do_data_abort (struct pt_regs *pt_regs)
{
	printf ("data abort\n");
	show_regs (pt_regs);
	bad_mode ();
}

void do_not_used (struct pt_regs *pt_regs)
{
	printf ("not used\n");
	show_regs (pt_regs);
	bad_mode ();
}

void do_fiq (struct pt_regs *pt_regs)
{
	printf ("fast interrupt request\n");
	show_regs (pt_regs);
	bad_mode ();
}


typedef struct int_vector{
	unsigned char	IntNum;
	void	(*handler)(void);
}INT_VECTOR;

INT_VECTOR vector[]={
					/* interrupt number,            handler */
					
			  {	INTSRC_NULL,                NULL        },
			  {	INTSRC_EXINT0,              NULL        },
                    {	INTSRC_EXINT1,              NULL        },
                    {	INTSRC_EXINT2,              NULL        },
                    {	INTSRC_EXINT3,              NULL        },
                    {	INTSRC_EXINT4,              NULL        },
                    {	INTSRC_EXINT5,              NULL        },
                    {	INTSRC_EXINT6,              NULL        },
                    {	INTSRC_EXINT7,              NULL        },
                    {	INTSRC_EXINT8,              NULL        },
                    {	INTSRC_EXINT9,              NULL        },
                    {	INTSRC_EXINT10,             NULL        },
                    {	INTSRC_SDIO,                NULL        },
                    {	INTSRC_SMC1,                NULL        },
                    {	INTSRC_SMC0,                NULL        },                    
                    {	INTSRC_USB,                 NULL        },
                    {	INTSRC_NULL,                NULL        },
                    {	INTSRC_SSI,                 NULL        },
                    {	INTSRC_I2S,                 NULL        },
                    {	INTSRC_LCDC,                NULL        },
                    {	INTSRC_PWM,                 NULL	    },
                    {	INTSRC_UART3,               NULL        },
                    {	INTSRC_UART2,               NULL        },
                    {	INTSRC_UART1,               NULL        },
                    {	INTSRC_UART0,               NULL        },
                    {	INTSRC_TIMER3,              NULL        },
                    {	INTSRC_TIMER2,              NULL        },
			  {	INTSRC_TIMER1,              NULL        },
			  {	INTSRC_MAC,                 NULL        },
			  {	INTSRC_EMI,                 NULL        },
			  {	INTSRC_DMAC,                NULL        },
			  {	INTSRC_RTC,                 test        },
			};
void do_irq (void)
{
	unsigned int	intnum;	
	unsigned char	i = 0;
		
	intnum = *(volatile unsigned int *)(0x10000030);
	while(intnum != 1)
	{
		intnum = intnum>>1;
		i++;
	}
	(*vector[i].handler)();

	return;
}



static ulong timestamp;
static ulong lastdec;


int interrupt_init (void)
{
	
	/*Just For Display,the ACTURE Stack Address is build in start.s*/
	FIQ_STACK_START = _armboot_start-CFG_MALLOC_LEN-CFG_GBL_DATA_SIZE-12;
	IRQ_STACK_START = _armboot_start-CFG_MALLOC_LEN-CFG_GBL_DATA_SIZE-CONFIG_STACKSIZE_IRQ-12;

	/*For Test*/
//	INTC_IER |= 0x80000000;
//	INTC_IMR &= 0x7FFFFFFF;
//	RTC_INT_EN |= 0x04;
	
	/*End of Test*/

	TIMER_T1LCR = TIMER_LOAD_VAL;
	TIMER_T1CR = 0x17;
	
	lastdec = TIMER_T1LCR = TIMER_LOAD_VAL;
	timestamp = 0;
	return 0;
}

/*
 * timer without interrupts
 */

void reset_timer (void)
{
	reset_timer_masked ();
}

ulong get_timer (ulong base)
{
	return get_timer_masked () - base;
}

void set_timer (ulong t)
{
	timestamp = t;
}

void udelay (unsigned long usec)
{
	ulong tmo;

	tmo = CFG_HZ / 1000000;
	tmo *= usec;

	tmo += get_timer (0);

	while (get_timer_masked () < tmo)
		/*NOP*/;
}

void reset_timer_masked (void)
{
	/* reset time */
	lastdec = TIMER_T1CCR;
	timestamp = 0;
}

ulong get_timer_masked (void)
{
	ulong now = TIMER_T1CCR;

	if (lastdec >= now) {
		/* normal mode */
		timestamp += lastdec - now;
	} else {
		/* we have an overflow ... */
		timestamp += lastdec + TIMER_LOAD_VAL - now;
	}
	lastdec = now;

	return timestamp;
}

void udelay_masked (unsigned long usec)
{
	ulong tmo;
	ulong endtime;
	signed long diff;

	if (usec >= 1000) {
		tmo = usec / 1000;
		tmo *= CFG_HZ;
		tmo /= 8;
	} else {
		tmo = usec * CFG_HZ;
		tmo /= (1000*8);
	}

	endtime = get_timer(0) + tmo;

	do {
		ulong now = get_timer_masked ();
		diff = endtime - now;
	} while (diff >= 0);
}
