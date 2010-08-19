/*
 * include/asm-arm/arch-CLPS711x/entry-macro.S
 *
 * Low-level IRQ helper macros for CLPS711X-based platforms
 *
 * This file is licensed under  the terms of the GNU General Public
 * License version 2. This program is licensed "as is" without any
 * warranty of any kind, whether express or implied.
 */
#include <asm/hardware.h>
 
	.macro disable_fiq	
		.endm
		.macro get_irqnr_and_base, irqnr, irqstat, base, tmp		
		//gfd		
		ldr	\base, =INTC_IFSR_V		
		ldr	\base, [\base]		
		mov	\irqnr,#0
1001:			
		cmp	\base, #0		@ no flags set?		
		beq	1002f			@ keep "eq" status when finishing!		
		tst	\base, #1		@ lsb set?		
		addeq	\irqnr, \irqnr, #1	@ if not, incr IRQ#		
		moveq	\base, \base, LSR #1	@ r = r >> 1 		
		beq	1001b				
1002:		
		.endm		
		.macro irq_prio_table		
		.endm


