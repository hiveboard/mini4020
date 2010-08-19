#include <asm/hardware.h>
#include <asm/io.h>

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/interrupt.h>
#include <linux/string.h>
#include <linux/tty.h>
#include <linux/config.h>
#include <linux/device.h>
#include <linux/serial_8250.h>

#include <asm/setup.h>
#include <asm/mach-types.h>
#include <asm/mach/arch.h>
#include "common.h"
#include <asm/arch/irqs.h>
#include <asm/arch/board.h>
#ifdef CONFIG_FB_SEP4020_320_240
#include <asm/arch/fb-320*240.h>
#else
#include <asm/arch/fb.h>
#endif
extern struct platform_device sep4020_device_rtc;
extern struct platform_device epson_ohci_device;
extern struct platform_device sep4020_device_usbgadget;

static void __init
fixup_gfd4020(struct machine_desc *desc, struct tag *tag_array,
	    char **cmdline, struct meminfo *mi)
{
	mi->nr_banks=1;
	mi->bank[0].start = 0x30000000;
	mi->bank[0].size = 0x02000000;
	mi->bank[0].node = 0;

/*	tag_array->hdr.tag  = ATAG_CORE;
	tag_array->hdr.size = tag_size(tag_core);
	tag_array->u.core.flags = 1;
	tag_array->u.core.pagesize = 0;
	tag_array->u.core.rootdev = 0;
	tag_array = tag_next(tag_array);
	
//	tag_array->hdr.tag = ATAG_MEM;
//	tag_array->hdr.size = tag_size(tag_mem32);
//	tag_array->u.mem.size = 0x00800000;
//	tag_array->u.mem.start = 0x30000000;
//	tag_array = tag_next(tag_array);

	tag_array->hdr.tag = ATAG_INITRD;
	tag_array->hdr.size = tag_size(tag_initrd);
	tag_array->u.initrd.size = 0x00200000;
	tag_array->u.initrd.start = 0xc0300000;
	tag_array = tag_next(tag_array);
	
	tag_array->hdr.tag = ATAG_NONE;
*/
}

/*static void __init
fixup_gfd4020(struct machine_desc *desc, struct tag *unused,
	    char **cmdline, struct meminfo *mi)
{
	mi->nr_banks=1;
	mi->bank[0].start = 0x30000000;
	mi->bank[0].size = 0x00800000;
	mi->bank[0].node = 0;
}
*/

static struct plat_serial8250_port serial_platform_data[] = {
	{
		.membase	= (void*)UART0_BASE_V,
		.mapbase	= UART0_BASE,
		.irq		= INTSRC_UART0,
		.uartclk	= UARTCLK,
		.regshift	= 2,
		.iotype		= UPIO_MEM,
		.flags		= UPF_BOOT_AUTOCONF | UPF_SKIP_TEST|UPF_LOW_LATENCY,
	},
	{
		.membase	= (void*)UART1_BASE_V,
		.mapbase	= UART1_BASE,
		.irq		= INTSRC_UART1,
		.uartclk	= UARTCLK,
		.regshift	= 2,
		.iotype		= UPIO_MEM,
		.flags		= UPF_BOOT_AUTOCONF | UPF_SKIP_TEST|UPF_LOW_LATENCY,
	},
	{
		.membase        = (void*)UART2_BASE_V,
		.mapbase	= UART2_BASE,
		.irq		= INTSRC_UART2,
		.uartclk	= UARTCLK,
		.regshift	= 2,
		.iotype		= UPIO_MEM,
		.flags		= UPF_BOOT_AUTOCONF | UPF_SKIP_TEST|UPF_LOW_LATENCY,
	},
	{
		.membase        = (void*)UART3_BASE_V,
		.mapbase	= UART3_BASE,
		.irq		= INTSRC_UART3,
		.uartclk	= UARTCLK,
		.regshift	= 2,
		.iotype		= UPIO_MEM,
		.flags		= UPF_BOOT_AUTOCONF | UPF_SKIP_TEST|UPF_LOW_LATENCY,
	},
	{ },
};

static struct sep_eth_data __initdata dk_eth_data = {
	.phy_irq_pin	= INTSRC_EXTINT7,
	.is_rmii	= 1,
};

/*
 * MCI (SD/MMC)
 */
static struct sep_mmc_data __initdata dk_mmc_data = {
	.slot_b		= 0x0,
	.wire4		= 0x1,
	//.det_pin	= ... not connected
	//.wp_pin		= ... not connected
	//.vcc_pin	= ... not connected
};
/* LCD driver info */

static struct sep4020fb_mach_info __initdata ub4020_lcd_cfg = {
	.regs	= {
  .sep_size =  YMAX | XMAX,
  .sep_pcr = TFT|COLOR|PBSIZE|BPIX|PIXPOL|FLMPOL|LPPOL|CLKPOL|OEPOL|END_SEL|ACD_SEL|ACD|PCD,
  .sep_hcr = H_WIDTH|H_WAIT_1|H_WAIT_2,
  .sep_vcr = V_WIDTH|PASS_FRAME_WAIT|V_WAIT_1|V_WAIT_2,
  .sep_pwmr = SCR|CC_EN|PW,
  .sep_dmacr = BL|HM|TM,
},

	.width		= LCDWIDTH,
	.height		= LCDHEIGHT,

	.xres		= {
		.min	= LCDWIDTH,
		.max	= LCDWIDTH,
		.defval	= LCDWIDTH,
	},

	.yres		= {
		.min	= LCDHEIGHT,
		.max	= LCDHEIGHT,
		.defval = LCDHEIGHT,
	},

	.bpp		= {
		.min	= 16,
		.max	= 16,
		.defval = 16,
	},
};

static struct platform_device serial_device = {
	.name			= "serial8250",
	.id			= PLAT8250_DEV_PLATFORM,
	.dev			= {
		.platform_data	= serial_platform_data,
	},
};

static struct platform_device *devices[] __initdata = {
	&serial_device,
	&sep4020_device_rtc,
	&epson_ohci_device,
      &sep4020_device_usbgadget
};

void __init sep4020_init(void)
{	
	INT_ENABLE(INTSRC_UART0);
	INT_ENABLE(INTSRC_UART1);
	INT_ENABLE(INTSRC_UART2);
	INT_ENABLE(INTSRC_UART3);
	INT_ENABLE(INTSRC_EXTINT7);
	 platform_add_devices(devices, ARRAY_SIZE(devices));
	 /* Ethernet */
	sep_add_device_eth(&dk_eth_data);
	//mmc
	sep_add_device_mmc(&dk_mmc_data);
      sep4020_fb_set_platdata(&ub4020_lcd_cfg);

}

MACHINE_START(GFD4020, "4020 board")
	.phys_io	= 0x10000000,
	.io_pg_offst	= ((0xe0000000) >> 18) & 0xfffc,
	.boot_params	= 0x30000100,
	.fixup		= fixup_gfd4020,
	.map_io		= sep4020_map_io,
	.init_irq	=  sep4020_init_irq,
	.init_machine	= sep4020_init,
	.timer		= &sep4020_timer,
MACHINE_END
