#include <linux/timex.h>
#include <linux/init.h>
#include <linux/interrupt.h>
#include <linux/sched.h>
#include <linux/time.h>
#include <linux/types.h>

#include <asm/hardware.h>
#include <asm/irq.h>
#include <asm/io.h>

#include <asm/mach/time.h>
#include "common.h"

static unsigned long sep4020_gettimeoffset (void)
{
	unsigned long hwticks;
	hwticks = LATCH - ((*(RP)TIMER_T1CCR_V )& 0xffffffff);	/* since last underflow */
	return (hwticks * (tick_nsec / 1000)) / LATCH;
	return 0;
}

static irqreturn_t sep4020_timer_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{	
	volatile int clear_int;
	write_seqlock(&xtime_lock);
	if(*(RP)(TIMER_T1IMSR_V) & 0X01)
		clear_int = *(RP)(TIMER_T1ISCR_V);
	timer_tick(regs);

	write_sequnlock(&xtime_lock);
	return IRQ_HANDLED;
	
}

static struct irqaction sep4020_timer_irq = {
	.name	= "sep4020 Timer Tick",
	.flags	= SA_INTERRUPT | SA_TIMER,
	.handler = sep4020_timer_interrupt,
	};
	
static void __init sep4020_timer_init(void)
{
	*(RP)TIMER_T1LCR_V = LATCH;
	*(RP)TIMER_T1CR_V = 0x07;
	INT_ENABLE(INTSRC_TIMER1);
	setup_irq(INTSRC_TIMER1,&sep4020_timer_irq);
}

struct sys_timer sep4020_timer = 
{
	.init	= sep4020_timer_init,
	.offset = sep4020_gettimeoffset,
};
