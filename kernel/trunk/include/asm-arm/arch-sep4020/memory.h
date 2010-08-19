#ifndef __ASM_ARCH_MMU_H
#define __ASM_ARCH_MMU_H

#include <linux/config.h>

/*
 * Task size: 3GB
 */


/*
 * This decides where the kernel will search for a free chunk of vm
 * space during mmap's.
 */

/*
 * Page offset: 3GB
 */
#define PHYS_OFFSET	UL(0x30000000)

/*
 * On integrator, the dram is contiguous
 */
#define __virt_to_bus(x) __virt_to_phys(x)

#define __bus_to_virt(x) __phys_to_virt(x)

#endif

