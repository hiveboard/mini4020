#include <asm/hardware.h>
#include <asm/io.h>

#include <asm/arch/irqs.h>
#include <asm/mach/arch.h>
#include <asm/mach/map.h>

#include <linux/config.h>
#include <linux/platform_device.h>
#include <asm/arch/board.h>
//typedef unsigned long long u64;
/* --------------------------------------------------------------------
 *  Ethernet
 * -------------------------------------------------------------------- */

#if defined(CONFIG_ARM_SEP4020_ETHER) || defined(CONFIG_ARM_SEP4020_ETHER_MODULE)
static u64 eth_dmamask = 0xffffffffUL;
static struct sep_eth_data eth_data;

static struct resource sep_eth_resources[] = {
	[0] = {
		.start	= MAC_BASE_V,
		.end	= MAC_BASE_V + SZ_4K - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= INTSRC_MAC,
		.end	  = INTSRC_MAC,
		.flags	= IORESOURCE_IRQ,
	},
};

struct platform_device sep4020_eth_device = {
	.name		= "sep4020_ether",
	.id		= -1,
	.dev		= {
				.dma_mask		= &eth_dmamask,
				.coherent_dma_mask	= 0xffffffff,
				.platform_data		= &eth_data,
	},
	.resource	= sep_eth_resources,
	.num_resources	= ARRAY_SIZE(sep_eth_resources),
};

void __init sep_add_device_eth(struct sep_eth_data *data)
{
	 unsigned long temp;
	if (!data)
		return;
		
		 printk("sep_ether: sepether_probe!\n\r");
 /*配置phy dm9161的中断为外部中断7*/
	if (data->phy_irq_pin) {
		//at91_set_gpio_input(data->phy_irq_pin, 0);
		//at91_set_deglitch(data->phy_irq_pin, 1);
		temp = __raw_readl(GPIO_PORTA_DIR_V);
		__raw_writel(0x80|temp,GPIO_PORTA_DIR_V);
		temp = __raw_readl(GPIO_PORTA_SEL_V);
		__raw_writel(0x80|temp,GPIO_PORTA_SEL_V);
		temp = __raw_readl(GPIO_PORTA_INTRCTL_V);
		__raw_writel(0x40|temp,GPIO_PORTA_INTRCTL_V);
		temp = __raw_readl(GPIO_PORTA_INCTL_V);
		__raw_writel(0x80|temp,GPIO_PORTA_INCTL_V);
    temp = __raw_readl(GPIO_PORTA_INTRCLR_V);
		__raw_writel(0x80|temp,GPIO_PORTA_INTRCLR_V);
	}

	eth_data = *data;
	platform_device_register(&sep4020_eth_device);
}
#else
void __init sep_add_device_eth(struct sep_eth_data *data) {}
#endif

/* LCD Controller */

static struct resource sep_lcd_resource[] = {
	[0] = {
		.start = LCDC_BASE_V,
		.end   = LCDC_BASE_V + SZ_4K - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = INTSRC_LCDC,
		.end   = INTSRC_LCDC,
		.flags = IORESOURCE_IRQ,
	}

};

static u64 sep_device_lcd_dmamask = 0xffffffffUL;

struct platform_device sep_device_lcd = {
	.name		  = "sep4020-lcd",
	.id		  = -1,
	.num_resources	  = ARRAY_SIZE(sep_lcd_resource),
	.resource	  = sep_lcd_resource,
	.dev              = {
		.dma_mask		= &sep_device_lcd_dmamask,
		.coherent_dma_mask	= 0xffffffffUL
	}
};

EXPORT_SYMBOL(sep_device_lcd);

void __init sep4020_fb_set_platdata(struct sep4020fb_mach_info *pd)
{
	struct sep4020fb_mach_info *npd;

	npd = kmalloc(sizeof(*npd), GFP_KERNEL);
	if (npd) {
		printk("platform_device_register sep4020_fb_set_platdata\n");
		memcpy(npd, pd, sizeof(*npd));
		sep_device_lcd.dev.platform_data = npd;
		platform_device_register(&sep_device_lcd);
	} else {
		printk(KERN_ERR "no memory for LCD platform data\n");
	}
}

/* RTC controller */
static struct resource sep4020_rtc_resource[] = {
	[0] = {
		.start = RTC_BASE_V,
		.end   = RTC_BASE_V+ 0x2f,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = INTSRC_RTC,
		.end   = INTSRC_RTC,
		.flags = IORESOURCE_IRQ,
	}
};

struct platform_device sep4020_device_rtc = {
	.name		  = "sep4020_rtc",
	.id		  = -1,
	.num_resources	  = ARRAY_SIZE(sep4020_rtc_resource),
	.resource	  = sep4020_rtc_resource,
};

EXPORT_SYMBOL(sep4020_device_rtc);
/* --------------------------------------------------------------------
 *  MMC / SD
 * -------------------------------------------------------------------- */

#if defined(CONFIG_MMC_SEP4020) 
static u64 mmc_dmamask = 0xffffffffUL;
static struct sep_mmc_data mmc_data;

static struct resource mmc_resources[] = {
	[0] = {
		.start	= MMC_BASE_V,
		.end	= MMC_BASE_V + SZ_4K - 1,
		.flags	= IORESOURCE_MEM,
	},
	[1] = {
		.start	= INTSRC_SDIO,
		.end	= INTSRC_SDIO,
		.flags	= IORESOURCE_IRQ,
	},
};

static struct platform_device sep4020_mmc_device = {
	.name		= "sep4020_mci",
	.id		= -1,
	.dev		= {
				.dma_mask		= &mmc_dmamask,
				.coherent_dma_mask	= 0xffffffff,
				.platform_data		= &mmc_data,
	},
	.resource	= mmc_resources,
	.num_resources	= ARRAY_SIZE(mmc_resources),
};

EXPORT_SYMBOL(sep4020_mmc_device);

void __init sep_add_device_mmc(struct sep_mmc_data *data)
{
	//printk("platform_device_register..........................\n");
	mmc_data = *data;
	
	platform_device_register(&sep4020_mmc_device);
}
#else
void __init sep_add_device_mmc(struct sep_mmc_data *data) {}
#endif


/* --------------------------------------------------------------------
 *  EPSON S1R72V17
 * -------------------------------------------------------------------- */

static struct resource s1r72xxx_ohci_resources[] = {

       [0] = {

              .start  = 0x2c000000,		//nCSD
              .end    = 0x2d000000,
              .flags  = IORESOURCE_MEM,

       },

       [1] = {
              .start  = INTSRC_EXTINT5,
              .end    = INTSRC_EXTINT5,
              .flags  = IORESOURCE_IRQ,

       },

};

static u64 v17_dma_mask = 0xffffffffUL ;

struct platform_device epson_ohci_device = {

       .name	= "s1r72v17",
       .id        = -1,
       .dev       = {
              .dma_mask = &v17_dma_mask,
              .coherent_dma_mask = 0xffffffffUL,
       },
       .num_resources  = ARRAY_SIZE(s1r72xxx_ohci_resources),
       .resource       = s1r72xxx_ohci_resources,
};

//for EPSON End

/* sep4020 USB Device (Gadget)*/

static struct resource sep4020_usbgadget_resource[] = {
	[0] = {
		.start = USBD_BASE_V,
		.end   = USBD_BASE_V + SZ_4K - 1,
		.flags = IORESOURCE_MEM,
	},
	[1] = {
		.start = INTSRC_USB,
		.end   = INTSRC_USB,
		.flags = IORESOURCE_IRQ,
	}

};

struct platform_device sep4020_device_usbgadget = {
	.name		  = "sep4020-usbgadget",
	.id		  = -1,
	.num_resources	  = ARRAY_SIZE(sep4020_usbgadget_resource),
	.resource	  = sep4020_usbgadget_resource,
};

EXPORT_SYMBOL(sep4020_device_usbgadget);

