/*
 *  linux/include/asm-arm/arch-clps711x/io.h
 *
 *  Copyright (C) 1999 ARM Limited
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
 */
#ifndef __ASM_ARM_ARCH_IO_H
#define __ASM_ARM_ARCH_IO_H

#include <asm/hardware.h>

#ifdef __CHECKER__
# define __user		__attribute__((noderef, address_space(1)))
# define __kernel	/* default address space */
# define __safe		__attribute__((safe))
# define __force	__attribute__((force))
# define __nocast	__attribute__((nocast))
# define __iomem	__attribute__((noderef, address_space(2)))
# define __acquires(x)	__attribute__((context(0,1)))
# define __releases(x)	__attribute__((context(1,0)))
# define __acquire(x)	__context__(1)
# define __release(x)	__context__(-1)
# define __cond_lock(x)	((x) ? ({ __context__(1); 1; }) : 0)
extern void __chk_user_ptr(void __user *);
extern void __chk_io_ptr(void __iomem *);
#else
# define __user
# define __kernel
# define __safe
# define __force
# define __nocast
# define __iomem
# define __chk_user_ptr(x) (void)0
# define __chk_io_ptr(x) (void)0
# define __builtin_warning(x, y...) (1)
# define __acquires(x)
# define __releases(x)
# define __acquire(x) (void)0
# define __release(x) (void)0
# define __cond_lock(x) (x)
#endif


#define IO_SPACE_LIMIT 0xffffffff

#define __io(a)			((void __iomem *)(a))
#define __mem_pci(a)		(a)
#define __mem_isa(a)		(a)

/*
 * We don't support ins[lb]/outs[lb].  Make them fault.
 */
#define __raw_readsb(p,d,l)	do { *(int *)0 = 0; } while (0)
#define __raw_readsl(p,d,l)	do { *(int *)0 = 0; } while (0)
#define __raw_writesb(p,d,l)	do { *(int *)0 = 0; } while (0)
#define __raw_writesl(p,d,l)	do { *(int *)0 = 0; } while (0)

#endif
