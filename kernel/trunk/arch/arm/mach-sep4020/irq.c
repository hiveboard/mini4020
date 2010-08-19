#include <linux/init.h>
#include <linux/list.h>

#include "common.h"
#include <asm/mach/irq.h>
#include <asm/hardware.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/system.h>

static void sep4020int_mask(unsigned int irq)
{
	u32 intmr;
	intmr = *(RP)(INTC_IMR_V);
	intmr |= (1 << irq);
	*(RP)(INTC_IMR_V)=intmr;
}

static void sep4020int_ack(unsigned int irq)
{
	u32 intmr;
	intmr = *(RP)(INTC_IMR_V);
	intmr |= (1 << irq);
	*(RP)(INTC_IMR_V)=intmr;
}

static void sep4020int_unmask(unsigned int irq)
{
	u32 intmr;
	intmr = *(RP)(INTC_IMR_V);
	intmr &= ~(1 << irq);
	*(RP)(INTC_IMR_V)=intmr;
} 

static struct irqchip sep4020_chip=
{
	.ack    = sep4020int_ack,
	.mask   = sep4020int_mask,
	.unmask = sep4020int_unmask,
 };
 
 void __init sep4020_init_irq(void)
 {
 	unsigned int i;
	unsigned long flags;
	local_save_flags(flags);
	*(RP)(INTC_IER_V) = 0XFFFFFFFF;
	*(RP)(INTC_IMR_V) = 0XFFFFFFFF;
	*(RP)(INTC_IPLR_V) = 0X0;
	local_irq_restore(flags);	
 	
 	for(i = 0; i < NR_IRQS; i++)
 		{
 		set_irq_handler(i, do_level_IRQ);
	        set_irq_chip(i, &sep4020_chip);
	        set_irq_flags(i, IRQF_VALID | IRQF_PROBE);
		}
		
//	__raw_writel(0, INTC_IMR_V);
 }
