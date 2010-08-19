#include <linux/sched.h>
#include <linux/kernel.h>
#include <linux/mm.h>
#include <linux/init.h>
#include <linux/bootmem.h>

#include <asm/sizes.h>
#include <asm/hardware.h>
#include <asm/pgtable.h>
#include <asm/page.h>
#include <asm/mach/map.h>
#include <asm/io.h>
/*
 * This maps the generic sep4020 registers
 */
static struct map_desc sep4020_io_desc[] __initdata = {
 {		.virtual	=  MM_BASE_V,
		.pfn		=__phys_to_pfn(MM_BASE),
		.length		= SZ_32M,
		.type		= MT_MEMORY
 },
 {		.virtual	= INTC_BASE_V,
		.pfn		=__phys_to_pfn(INTC_BASE) ,
		.length		= SZ_4K ,
		.type		= MT_DEVICE
 },
 {		.virtual	=PMU_BASE_V ,
		.pfn		=__phys_to_pfn(PMU_BASE) ,
		.length		= SZ_4K,
		.type		= MT_DEVICE
 },
 {		.virtual	=RTC_BASE_V ,
		.pfn		=__phys_to_pfn(RTC_BASE) ,
		.length		= SZ_4K,
		.type		= MT_DEVICE
 },
 {		.virtual	=TIMER_BASE_V ,
		.pfn		=__phys_to_pfn(TIMER_BASE) ,
		.length		= SZ_4K,
		.type		= MT_DEVICE
 },
 {		.virtual	=PWM_BASE_V ,
		.pfn		=__phys_to_pfn(PWM_BASE) ,
		.length		= SZ_4K,
		.type		= MT_DEVICE
 },
 {		.virtual	=UART0_BASE_V ,
		.pfn		=__phys_to_pfn(UART0_BASE) ,
		.length		= SZ_4K,
		.type		= MT_DEVICE
 },
 {		.virtual	=UART1_BASE_V ,
		.pfn		=__phys_to_pfn(UART1_BASE) ,
		.length		= SZ_4K,
		.type		= MT_DEVICE
 },
 {		.virtual	=UART2_BASE_V ,
		.pfn		=__phys_to_pfn(UART2_BASE) ,
		.length		= SZ_4K,
		.type		= MT_DEVICE
 },
 {		.virtual	=UART3_BASE_V ,
		.pfn		=__phys_to_pfn(UART3_BASE) ,
		.length		= SZ_4K,
		.type		= MT_DEVICE
 },
 {		.virtual	= SSI_BASE_V,
		.pfn		=__phys_to_pfn(SSI_BASE) ,
		.length		= SZ_4K,
		.type		= MT_DEVICE
 },
 {		.virtual	=I2S_BASE_V ,
		.pfn		=__phys_to_pfn(I2S_BASE) ,
		.length		= SZ_4K,
		.type		= MT_DEVICE
 },
 {		.virtual	= MMC_BASE_V,
		.pfn		=__phys_to_pfn( MMC_BASE) ,
		.length		= SZ_4K,
		.type		= MT_DEVICE
 },
  {		.virtual	=GPIO_BASE_V ,
		.pfn		=__phys_to_pfn(GPIO_BASE) ,
		.length		= SZ_4K,
		.type		= MT_DEVICE
 },
  
 {		.virtual	=SMC0_BASE_V ,
		.pfn		=__phys_to_pfn(SMC0_BASE) ,
		.length		= SZ_4K,
		.type		= MT_DEVICE
 },
 {		.virtual	=SMC1_BASE_V ,
		.pfn		=__phys_to_pfn(SMC1_BASE) ,
		.length		= SZ_4K,
		.type		= MT_DEVICE
 },
 {		.virtual	=USBD_BASE_V ,
		.pfn		=__phys_to_pfn(USBD_BASE) ,
		.length		= SZ_4K,
		.type		= MT_DEVICE
 },
 {		.virtual	= EMI_BASE_V,
		.pfn		=__phys_to_pfn(EMI_BASE) ,
		.length		= SZ_4K,
		.type		= MT_DEVICE
 },
  {		.virtual	=DMAC_BASE_V ,
		.pfn		=__phys_to_pfn(DMAC_BASE) ,
		.length		= SZ_4K,
		.type		= MT_DEVICE
 },
  {		.virtual	=LCDC_BASE_V ,
		.pfn		=__phys_to_pfn(LCDC_BASE) ,
		.length		= SZ_4K,
		.type		= MT_DEVICE
 },
  {		.virtual	=MAC_BASE_V ,
		.pfn		=__phys_to_pfn(MAC_BASE) ,
		.length		= SZ_4K,
		.type		= MT_DEVICE
 },
   {		.virtual	=AMBA_BASE_V ,
		.pfn		=__phys_to_pfn(AMBA_BASE) ,
		.length		= SZ_4K,
		.type		= MT_DEVICE
 },
   {		.virtual	=ESRAM_BASE_V ,
		.pfn		=__phys_to_pfn(ESRAM_BASE) ,
		.length		= SZ_64K,
		.type		= MT_DEVICE
 }
 
};

void __init sep4020_map_io(void)
{
	iotable_init(sep4020_io_desc, ARRAY_SIZE(sep4020_io_desc));
}


