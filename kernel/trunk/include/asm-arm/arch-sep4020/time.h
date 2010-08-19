#include <asm/leds.h>
#include <asm/hardware.h>

static irqreturn_t
sep4020_timer_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
//	volatile int clear_int;
//	*(int*)0xc0200060 = 0x05;
//	if(*(volatile unsigned int*)(TIMER_T1IMSR) & 0x01)
//		clear_int = *(volatile unsigned int*)(TIMER_T1ISCR);
	do_leds();
	do_timer(regs);
	#ifndef CONFIG_SMP
	update_process_times(user_mode(regs));
#endif
	do_profile(regs);
	return IRQ_HANDLED;	
}

void __init time_init(void)
{
	sep4020_timer_init();
//	*(int*)0xc0200064 = 0x05;
	timer_irq.handler = sep4020_timer_interrupt;
	setup_irq(INTSRC_TIMER1, &timer_irq);
}
