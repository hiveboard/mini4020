/*
 * Startup Code for S3C44B0 CPU-core
 *
 * (C) Copyright 2004
 * DAVE Srl
 *
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


#include <config.h>
#include <version.h>


/*
 * Jump vector table
 */


.globl _start
_start: b	reset
	ldr	pc, _undefined_instruction
	ldr	pc, _software_interrupt
	ldr	pc, _prefetch_abort
	ldr	pc, _data_abort
	ldr	pc, _not_used
	ldr	pc, _irq
	ldr	pc, _fiq

_undefined_instruction: .word undefined_instruction
_software_interrupt:	.word software_interrupt
_prefetch_abort:	.word prefetch_abort
_data_abort:		.word data_abort
_not_used:		.word not_used
_irq:			.word irq
_fiq:			.word fiq

	.balignl 16,0xdeadbeef


/*
 *************************************************************************
 *
 * Startup Code (reset vector)
 *
 * do important init only if we don't start from memory!
 * relocate u-boot to ram
 * setup stack
 * jump to second stage
 *
 *************************************************************************
 */

_TEXT_BASE:
	.word	TEXT_BASE

.globl _armboot_start
_armboot_start:
	.word _start

/*
 * These are defined in the board-specific linker script.
 */
.globl _bss_start
_bss_start:
	.word __bss_start

.globl _bss_end
_bss_end:
	.word _end

#ifdef CONFIG_USE_IRQ
/* IRQ stack memory (calculated at run-time) */
.globl IRQ_STACK_START
IRQ_STACK_START:
	.word	0x0badc0de

/* IRQ stack memory (calculated at run-time) */
.globl FIQ_STACK_START
FIQ_STACK_START:
	.word 0x0badc0de
#endif


/*
 * the actual reset code
 */

reset:
	/*stack setup for each mode*/
	
	/* SVC32 mode*/	 
	mrs	r0,cpsr
	bic	r0,r0,#0x1f
	orr	r0,r0,#0x13
	msr	cpsr,r0
	ldr	r0, _TEXT_BASE
	sub	r0, r0, #CFG_MALLOC_LEN
	sub	r0, r0, #CFG_GBL_DATA_SIZE
#ifdef CONFIG_USE_IRQ
	sub	r0, r0, #(CONFIG_STACKSIZE_IRQ+CONFIG_STACKSIZE_FIQ)
#endif
	sub	sp, r0, #12	


#ifdef CONFIG_USE_IRQ
	/* IRQ mode*/
	mov	R4, #0xD2
    	msr	cpsr, R4   
	ldr	r0, _TEXT_BASE
	sub	r0, r0, #CFG_MALLOC_LEN
	sub	r0, r0, #CFG_GBL_DATA_SIZE
	sub	r0, r0, #(CONFIG_STACKSIZE_FIQ)
	sub	sp, r0, #12	
	

	/* FIQ mode*/
	mov	R4, #0xD1
    	msr	cpsr, R4   
	ldr	r0, _TEXT_BASE
	sub	r0, r0, #CFG_MALLOC_LEN
	sub	r0, r0, #CFG_GBL_DATA_SIZE
	sub	sp, r0, #12	
#endif
	
	/* ABORT mode*/
	mov	R4, #0xD7
    	msr	cpsr, R4   
	ldr	r0, _TEXT_BASE
	sub	r0, r0, #CFG_MALLOC_LEN
	sub	r0, r0, #CFG_GBL_DATA_SIZE
	sub	sp, r0, #8
	
	/* UNDEFINE mode*/
	mov	R4, #0xDB
    	msr	cpsr, R4   
	ldr	r0, _TEXT_BASE
	sub	r0, r0, #CFG_MALLOC_LEN
	sub	r0, r0, #CFG_GBL_DATA_SIZE
	sub	sp, r0, #4
	
	/* SYSTEM mode*/
	mov	R4, #0xDF
    	msr	cpsr, R4   
	ldr	r0, _TEXT_BASE
	sub	r0, r0, #CFG_MALLOC_LEN
	sub	sp, r0, #CFG_GBL_DATA_SIZE

	/*Return to SVC mode*/	
	mov	R4, #0xD3
    	msr	cpsr, R4 


	/*
	 * we do sys-critical inits only at reboot,
	 * not when booting from ram!
	 */

#ifndef CONFIG_SKIP_LOWLEVEL_INIT
	bl	cpu_init_crit
	/*
	 * before relocating, we have to setup RAM timing
	 * because memory timing is board-dependend, you will
	 * find a lowlevel_init.S in your board directory.
	 */
	bl	lowlevel_init
#endif

remap:
#if 0
	mov	r0, 	pc
	add	r0,	r0,	#0x20000000
	add	r0,	r0,	#0x08
	mov	pc,	r0

	mov	r0,	r0
	mov	r0,	r0
	mov	r0,	r0
	mov	r0,	r0

    	ldr	r4,	=0x11000020
      	ldr	r5,	=0xb
	str	r5,	[ r4 ]
#endif
/*
	init BSS section
*/
	ldr r0, = 0
	ldr r1, _bss_start
	ldr r2, _bss_end
bss_init:
	str r0, [r1]
	add r1,r1,#4
	cmp r1,r2
	blt bss_init

#ifndef CONFIG_SKIP_RELOCATE_UBOOT
relocate:				/* relocate U-Boot to RAM	    */
	adr	r0, _start		/* r0 <- current position of code   */
	ldr	r1, _TEXT_BASE		/* test if we run from flash or RAM */
	cmp     r0, r1                  /* don't reloc during debug         */
	beq    vector_copy

	ldr	r2, _armboot_start
	ldr	r3, _bss_start
	sub	r2, r3, r2		/* r2 <- size of armboot            */
	add	r2, r0, r2		/* r2 <- source end address         */

copy_loop:
	ldmia	r0!, {r3-r10}		/* copy from source address [r0]    */
	stmia	r1!, {r3-r10}		/* copy to   target address [r1]    */
	cmp	r0, r2			/* until source end addreee [r2]    */
	ble	copy_loop


/*
	now copy to sram the interrupt vector
*/
vector_copy:
	ldr	r0, _TEXT_BASE
	add	r2, r0, #128
	ldr	r1, =0x30000000 /*modified by shixq from 0x0c000000 to 0x30000000*/
/*	add	r1, r1, #0x08 *//*deleted by shixq*/
vector_copy_loop:
	ldmia	r0!, {r3-r10}
	stmia	r1!, {r3-r10}
	cmp	r0, r2
	ble	vector_copy_loop
#endif	/* CONFIG_SKIP_RELOCATE_UBOOT */

	/*enable the irq*/
	mrs	R4, cpsr
    	bic	R4, R4, #0x80
    	msr	cpsr, R4	

	ldr	pc, _start_armboot

_start_armboot:	.word start_armboot


/*
 *************************************************************************
 *
 * CPU_init_critical registers
 *
 * setup important registers
 * setup memory timing
 *
 *************************************************************************
 */


cpu_init_crit:
	/* PLLCON */
	ldr 	r0, =0x10001004	/*96M*/
	ldr	r1, =0x400B		
	str	r1, [r0]

	ldr 	r0, =0x10001014	/*Normal*/
	ldr	r1, =0x1
	str	r1, [r0]
	
	ldr 	r0, =0x10001004	/*96M*/
	ldr	r1, =0xC00B		
	str	r1, [r0]

	ldr 	r0, =0x1000100C	/*open all*/
	ldr	r1, =0xFFFFFFFF
	str	r1, [r0]

	/*UARTCON*/
#if 1
	ldr 	r0, =0x1000500C	/*databit:8*/
	ldr	r1, =0x83
	str	r1, [r0]

	ldr 	r0, =0x10005004	/*baud=9600*/
	ldr	r1, =0x0
	str	r1, [r0]

	ldr 	r0, =0x10005000
	ldr	r1, =0x2F
	str	r1, [r0]

	ldr 	r0, =0x1000500C
	ldr	r1, =0x3
	str	r1, [r0]
#endif
	mov	pc, lr


/*************************************************/
/*	interrupt vectors	*/
/*************************************************/
@
@ IRQ stack frame.
@
#define S_FRAME_SIZE	72

#define S_OLD_R0	68
#define S_PSR		64
#define S_PC		60
#define S_LR		56
#define S_SP		52

#define S_IP		48
#define S_FP		44
#define S_R10		40
#define S_R9		36
#define S_R8		32
#define S_R7		28
#define S_R6		24
#define S_R5		20
#define S_R4		16
#define S_R3		12
#define S_R2		8
#define S_R1		4
#define S_R0		0

#define MODE_SVC 0x13
#define I_BIT	 0x80

/*
 * use bad_save_user_regs for abort/prefetch/undef/swi ...
 * use irq_save_user_regs / irq_restore_user_regs for IRQ/FIQ handling
 */

	.macro	bad_save_user_regs
	sub	sp, sp, #S_FRAME_SIZE
	stmia	sp, {r0 - r12}			@ Calling r0-r12
	add	r8, sp, #S_PC

	ldr	r2, _armboot_start
	sub	r2, r2, #(CONFIG_STACKSIZE+CFG_MALLOC_LEN)
	sub	r2, r2, #(CFG_GBL_DATA_SIZE+8)	@ set base 2 words into abort stack
	ldmia	r2, {r2 - r4}			@ get pc, cpsr, old_r0
	add	r0, sp, #S_FRAME_SIZE		@ restore sp_SVC

	add	r5, sp, #S_SP
	mov	r1, lr
	stmia	r5, {r0 - r4}			@ save sp_SVC, lr_SVC, pc, cpsr, old_r
	mov	r0, sp
	.endm

	.macro	irq_save_user_regs
	sub	sp, sp, #S_FRAME_SIZE
	stmia	sp, {r0 - r12}			@ Calling r0-r12
	add	r8, sp, #S_PC
	stmdb	r8, {sp, lr}^			@ Calling SP, LR
	str	lr, [r8, #0]			@ Save calling PC
	mrs	r6, spsr
	str	r6, [r8, #4]			@ Save CPSR
	str	r0, [r8, #8]			@ Save OLD_R0
	mov	r0, sp
	.endm

	.macro	irq_restore_user_regs
	ldmia	sp, {r0 - lr}^			@ Calling r0 - lr
	mov	r0, r0
	ldr	lr, [sp, #S_PC]			@ Get PC
	add	sp, sp, #S_FRAME_SIZE
	subs	pc, lr, #4			@ return & move spsr_svc into cpsr
	.endm

	.macro get_bad_stack
	ldr	r13, _armboot_start		@ setup our mode stack
	sub	r13, r13, #(CONFIG_STACKSIZE+CFG_MALLOC_LEN)
	sub	r13, r13, #(CFG_GBL_DATA_SIZE+8) @ reserved a couple spots in abort stack

	str	lr, [r13]			@ save caller lr / spsr
	mrs	lr, spsr
	str	lr, [r13, #4]

	mov	r13, #MODE_SVC			@ prepare SVC-Mode
	msr	spsr_c, r13
	mov	lr, pc
	movs	pc, lr
	.endm

	.macro get_irq_stack			@ setup IRQ stack
	ldr	sp, IRQ_STACK_START
	.endm

	.macro get_fiq_stack			@ setup FIQ stack
	ldr	sp, FIQ_STACK_START
	.endm
/*
 * exception handlers
 */
	.align	5
undefined_instruction:
	//get_bad_stack
	//bad_save_user_regs
	//bl	do_undefined_instruction
	ldr	pc, _undefined_instruction
	

	.align	5
software_interrupt:
	//get_bad_stack
	//bad_save_user_regs
	//bl	do_software_interrupt
	ldr	pc, _software_interrupt
	
	.align	5
prefetch_abort:
	//get_bad_stack
	//bad_save_user_regs
	//bl	do_prefetch_abort
	ldr	pc, _prefetch_abort
	

	.align	5
data_abort:
	//get_bad_stack
	//bad_save_user_regs
	//bl	do_data_abort
	ldr	pc, _data_abort
	.align	5
not_used:
	//get_bad_stack
	//bad_save_user_regs
	//bl	do_not_used
	ldr	pc, _not_used
	
      .align	5
irq:
	stmfd	sp!, {r0,r1}

	ldr		r0, =0x0
	str		r1, [r0]
	
	ldmfd	sp!, {r0}
	ldr		r1, =0x0
	str		r0, [r1]
	
	add		r13, r13, #4
	
	sub		r14, r14, #4
	mov		r0, r14
	
	mrs		r1, spsr
	orr		r1, r1, #0x80
	msr		cpsr, r1

	bic		r1, r1, #0x80
	
	stmfd	sp!, {r0}
	stmfd	sp!, {r14}
	stmfd	sp!, {r1}
	
	ldr		r0, =0x0
	ldr		r1, [r0]
	stmfd	sp!, {r1}
		
	ldr		r1, =0x0
	ldr		r0, [r1]
	stmfd	sp!, {r0}
	
	ldmfd	sp!, {r0,r1}
	stmfd	sp!, {r0-r12}


	bl		do_irq


	ldmfd	sp!, {r0-r12}
	ldmfd	sp!, {r14}
	msr		cpsr, r14
	ldmfd	sp!, {r14}
	ldmfd	sp!, {pc}

	.align	5
fiq:
	//get_fiq_stack
	/* someone ought to write a more effiction fiq_save_user_regs */
	//irq_save_user_regs
	//bl	do_fiq
	//irq_restore_user_regs
	ldr	pc, _fiq
