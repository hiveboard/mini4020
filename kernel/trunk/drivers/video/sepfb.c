/* linux/drivers/video/sepfb.c
*
* Copyright (c) 2009 prochip company
*	http://www.prochip.com.cn
*	aokikyon@gmail.com.cn
*
* sep4020 I2S-uda1341 driver.
*
* Changelog:
*
*	2008－10－15 	根据三星fb驱动修改 作者：方攀 修改：程杰 
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
*/

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/errno.h>
#include <linux/string.h>
#include <linux/mm.h>
#include <linux/tty.h>
#include <linux/delay.h>
#include <linux/fb.h>
#include <linux/dma-mapping.h>
#include <linux/platform_device.h>
#ifdef CONFIG_PM
#include <linux/pm.h>
#endif

#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <asm/arch/board.h>
#ifdef CONFIG_FB_SEP4020_320_240
#include <asm/arch/fb-320*240.h>
#else
#include <asm/arch/fb.h>
#endif

#include "sepfb.h"


static struct sep4020fb_mach_info *mach_info;

/* Debugging stuff */
#ifdef CONFIG_FB_SEP4020_DEBUG
static int debug = 1;
#else
static int debug = 0;
#endif

#define dprintk(msg...)	if (debug) { printk(KERN_DEBUG "sep4020fb: " msg); }

/* useful functions */

/* sep4020fb_set_lcdaddr
*
* initialise lcd controller address pointers 设置LCDframebuffer地址
*/

static void sep4020fb_set_lcdaddr(struct sep4020fb_info *fbi)
{
	unsigned long saddr1;
	saddr1  = fbi->fb->fix.smem_start ;
	writel(saddr1, LCDC_SSA_V);
}

/*
*	sep4020fb_check_var():
*	Get the video params out of 'var'. If a value doesn't fit, round it up,
*	if it's too big, return -EINVAL.
*
*/
static int sep4020fb_check_var(struct fb_var_screeninfo *var, struct fb_info *info)
{
	struct sep4020fb_info *fbi = info->par;

	/* validate x/y resolution */
	if (var->yres > fbi->mach_info->yres.max)
		var->yres = fbi->mach_info->yres.max;
	else if (var->yres < fbi->mach_info->yres.min)
		var->yres = fbi->mach_info->yres.min;

	if (var->xres > fbi->mach_info->xres.max)
		var->yres = fbi->mach_info->xres.max;
	else if (var->xres < fbi->mach_info->xres.min)
		var->xres = fbi->mach_info->xres.min;

	/* validate bpp */
	if (var->bits_per_pixel > fbi->mach_info->bpp.max)
		var->bits_per_pixel = fbi->mach_info->bpp.max;
	else if (var->bits_per_pixel < fbi->mach_info->bpp.min)
		var->bits_per_pixel = fbi->mach_info->bpp.min;

	/* set r/g/b positions */
	if (var->bits_per_pixel == 16) 
	{
		var->red.offset	= 11;
		var->green.offset	= 5;
		var->blue.offset	= 0;
		var->red.length	= 5;
		var->green.length	= 6;
		var->blue.length	= 5;
		var->transp.length= 0;
	} else 
	{
		var->red.length	= var->bits_per_pixel;
		var->red.offset	= 0;
		var->green.length	= var->bits_per_pixel;
		var->green.offset	= 0;
		var->blue.length	= var->bits_per_pixel;
		var->blue.offset	= 0;
		var->transp.length= 0;
	}

	return 0;
}

/* sep4020fb_activate_var
*
* activate (set) the controller from the given framebuffer
* information
*/

static void sep4020fb_activate_var(struct sep4020fb_info *fbi, struct fb_var_screeninfo *var)
{
	switch (var->bits_per_pixel) 
	{
	case 16:
	default:
		fbi->regs.sep_pcr |= (TFT|COLOR|PBSIZE);
	}

	/* check to see if we need to update sync/borders */

	fbi->regs.sep_size = var->yres;
	fbi->regs.sep_size |= ((var->xres/16)<<20);
  
	/* write new registers */

	writel(fbi->regs.sep_ssa, LCDC_SSA_V);
	writel(fbi->regs.sep_size, LCDC_SIZE_V);
	writel(fbi->regs.sep_pcr, LCDC_PCR_V );
	writel(fbi->regs.sep_hcr, LCDC_HCR_V );
	writel(fbi->regs.sep_vcr, LCDC_VCR_V );
      writel(fbi->regs.sep_pwmr, LCDC_PWMR_V );
      writel(fbi->regs.sep_lecr, LCDC_LECR_V );
      writel(fbi->regs.sep_dmacr, LCDC_DMACR_V );
      writel(fbi->regs.sep_lcdisren, LCDC_LCDISREN_V );
      writel(fbi->regs.sep_lcdisr, LCDC_LCDISR_V );

	/* set lcd address pointers */
	sep4020fb_set_lcdaddr(fbi);

	writel(0x1, LCDC_LECR_V );
}


/*
*      sep4020fb_set_par - Optional function. Alters the hardware state.
*      @info: frame buffer structure that represents a single frame buffer
*
*/
static int sep4020fb_set_par(struct fb_info *info)
{
	struct sep4020fb_info *fbi = info->par;
	struct fb_var_screeninfo *var = &info->var;

	if (var->bits_per_pixel == 16)
		fbi->fb->fix.visual = FB_VISUAL_TRUECOLOR;
	else
		fbi->fb->fix.visual = FB_VISUAL_PSEUDOCOLOR;

	fbi->fb->fix.line_length     = (var->width*var->bits_per_pixel)/8;

	/* activate this new configuration */
	sep4020fb_activate_var(fbi, var);
	return 0;
}

/* from pxafb.c */
static inline unsigned int chan_to_field(unsigned int chan, struct fb_bitfield *bf)
{
	chan &= 0xffff;
	chan >>= 16 - bf->length;
	return chan << bf->offset;
}

static int sep4020fb_setcolreg(unsigned regno,
			       unsigned red, unsigned green, unsigned blue,
			       unsigned transp, struct fb_info *info)
{
	struct sep4020fb_info *fbi = info->par;
	unsigned int val;

	switch (fbi->fb->fix.visual) 
	{
	case FB_VISUAL_TRUECOLOR:
		/* true-colour, use pseuo-palette */

		if (regno < 16) 
		{
			u32 *pal = fbi->fb->pseudo_palette;

			val  = chan_to_field(red,   &fbi->fb->var.red);
			val |= chan_to_field(green, &fbi->fb->var.green);
			val |= chan_to_field(blue,  &fbi->fb->var.blue);

			pal[regno] = val;
		}
		break;

	case FB_VISUAL_PSEUDOCOLOR:
		if (regno < 256) {
			/* currently assume RGB 5-6-5 mode */

			val  = ((red   >>  0) & 0xf800);
			val |= ((green >>  5) & 0x07e0);
			val |= ((blue  >> 11) & 0x001f);
		}
		break;

	default:
		return 1;   /* unknown type */
	}

	return 0;
}


/**
 *    sep4020fb_blank
 *	@blank_mode: the blank mode we want.
 *	@info: frame buffer structure that represents a single frame buffer
 *
 *	Blank the screen if blank_mode != 0, else unblank. Return 0 if
 *	blanking succeeded, != 0 if un-/blanking failed due to e.g. a
 *	video mode which doesn't support it. Implements VESA suspend
 *	and powerdown modes on hardware that supports disabling hsync/vsync:
 *	blank_mode == 2: suspend vsync
 *	blank_mode == 3: suspend hsync
 *	blank_mode == 4: powerdown
 *
 *	Returns negative errno on error, or zero on success.
 *
 */
static int sep4020fb_blank(int blank_mode, struct fb_info *info)
{

	if (mach_info == NULL)
		return -EINVAL;

	return 0;
}

static int sep4020fb_debug_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	return snprintf(buf, PAGE_SIZE, "%s\n", debug ? "on" : "off");
}

static int sep4020fb_debug_store(struct device *dev, struct device_attribute *attr,
					   const char *buf, size_t len)
{
	if (mach_info == NULL)
		return -EINVAL;

	if (len < 1)
		return -EINVAL;

	if (strnicmp(buf, "on", 2) == 0 || strnicmp(buf, "1", 1) == 0) 
	{
		debug = 1;
	}
	else if (strnicmp(buf, "off", 3) == 0 || strnicmp(buf, "0", 1) == 0) 
	{
		debug = 0;
	} 
	else 
	{
		return -EINVAL;
	}

	return len;
}


static DEVICE_ATTR(debug, 0666, sep4020fb_debug_show, sep4020fb_debug_store);

static struct fb_ops sep4020fb_ops = {
	.owner		= THIS_MODULE,
	.fb_check_var	= sep4020fb_check_var,
	.fb_set_par		= sep4020fb_set_par,
	.fb_blank		= sep4020fb_blank,
	.fb_setcolreg	= sep4020fb_setcolreg,
	.fb_fillrect	= cfb_fillrect,
	.fb_copyarea	= cfb_copyarea,
	.fb_imageblit	= cfb_imageblit,
};


/*
 * sep4020fb_map_video_memory():
 *
 */
static int __init sep4020fb_map_video_memory(struct sep4020fb_info *fbi)
{
	fbi->map_size = PAGE_ALIGN(fbi->fb->fix.smem_len + PAGE_SIZE);
	fbi->map_cpu  = dma_alloc_writecombine(fbi->dev, fbi->map_size, &fbi->map_dma, GFP_KERNEL);

	fbi->map_size = fbi->fb->fix.smem_len;

	if (fbi->map_cpu) 
	{
		/* prevent initial garbage on screen */
   
		fbi->screen_dma		= fbi->map_dma;
		fbi->fb->screen_base	= fbi->map_cpu;
		fbi->fb->fix.smem_start  = fbi->screen_dma;

		printk("map_video_memory: dma=%08x cpu=%p size=%08x\n",fbi->map_dma, fbi->map_cpu, fbi->fb->fix.smem_len);
		writel(fbi->map_dma, LCDC_SSA_V);
	}

	return fbi->map_cpu ? 0 : -ENOMEM;
}

static inline void sep4020fb_unmap_video_memory(struct sep4020fb_info *fbi)
{
	dma_free_writecombine(fbi->dev,fbi->map_size,fbi->map_cpu, fbi->map_dma);
}

/*
 * sep4020fb_init_registers - Initialise all LCD-related registers
 */

static int sep4020fb_init_registers(struct sep4020fb_info *fbi)
{
	/* Initialise LCD with values from haret */

	writel(0, LCDC_LECR_V);  //禁用LCDC     
	writel(fbi->regs.sep_size, LCDC_SIZE_V);
	writel(fbi->regs.sep_pcr, LCDC_PCR_V );
	writel(fbi->regs.sep_hcr, LCDC_HCR_V );
	writel(fbi->regs.sep_vcr, LCDC_VCR_V );
	writel(fbi->regs.sep_pwmr, LCDC_PWMR_V );
	writel(fbi->regs.sep_lecr, LCDC_LECR_V );
	writel(fbi->regs.sep_dmacr, LCDC_DMACR_V );
	writel(fbi->regs.sep_lcdisren, LCDC_LCDISREN_V );
	writel(fbi->regs.sep_lcdisr, LCDC_LCDISR_V );
	
	return 0;
}

static char driver_name[]="sep4020fb";

static int __init sep4020fb_probe(struct platform_device *pdev)
{
	struct sep4020fb_info *info;
	struct fb_info	   *fbinfo;
	struct sep4020fb_hw *mregs;
	int ret;
	//int irq;
#ifdef CONFIG_FB_SEP4020_320_240
	printk("The 320*240 LCD is initialized------------------------------------------>\n");
#endif
#ifdef CONFIG_FB_SEP4020_640_480
	printk("The 640*480 LCD is initialized------------------------------------------>\n");
#endif
	mach_info = pdev->dev.platform_data;
	
	if (mach_info == NULL) 
	{
		dev_err(&pdev->dev,"no platform data for lcd, cannot attach\n");
		return -EINVAL;
	}

	mregs = &mach_info->regs;

	fbinfo = framebuffer_alloc(sizeof(struct sep4020fb_info), &pdev->dev);
	if (!fbinfo) 
	{
		return -ENOMEM;
	}

	info = fbinfo->par;
	info->fb = fbinfo;
	info->regs = mach_info->regs;
	platform_set_drvdata(pdev, fbinfo);
	strcpy(fbinfo->fix.id, driver_name);
	memcpy(&info->regs, &mach_info->regs, sizeof(info->regs));

	info->mach_info		    	= pdev->dev.platform_data;

	fbinfo->fix.type			= FB_TYPE_PACKED_PIXELS;
	fbinfo->fix.type_aux	    	= 0;
	fbinfo->fix.xpanstep	    	= 0;
	fbinfo->fix.ypanstep	    	= 0;
	fbinfo->fix.ywrapstep	    	= 0;
	fbinfo->fix.accel	    		= FB_ACCEL_NONE;

	fbinfo->var.nonstd	    	= 0;
	fbinfo->var.activate	    	= FB_ACTIVATE_NOW;
	fbinfo->var.height	    	= mach_info->height;
	fbinfo->var.width	    		= mach_info->width;
	fbinfo->var.accel_flags     	= 0;
	fbinfo->var.vmode	    		= FB_VMODE_NONINTERLACED;

	fbinfo->fbops		    	= &sep4020fb_ops;
	fbinfo->flags		    	= FBINFO_FLAG_DEFAULT;
	fbinfo->pseudo_palette      	= &info->pseudo_pal;

	fbinfo->var.xres	    		= mach_info->xres.defval;
	fbinfo->var.xres_virtual    	= mach_info->xres.defval;
	fbinfo->var.yres	    		= mach_info->yres.defval;
	fbinfo->var.yres_virtual    	= mach_info->yres.defval;
	fbinfo->var.bits_per_pixel  	= mach_info->bpp.defval;

	fbinfo->var.upper_margin    	= 0;	//3
	fbinfo->var.lower_margin    	= 0;	//3
	fbinfo->var.vsync_len	   	= 0;	//10

	fbinfo->var.left_margin	    	= 0;	//20
	fbinfo->var.right_margin    	= 0;	//20
	fbinfo->var.hsync_len	    	= 0;	//46

	fbinfo->var.red.offset      	= 11;
	fbinfo->var.green.offset    	= 5;
	fbinfo->var.blue.offset     	= 0;
	fbinfo->var.transp.offset   	= 0;
	fbinfo->var.red.length      	= 5;
	fbinfo->var.green.length    	= 6;
	fbinfo->var.blue.length     	= 5;
	fbinfo->var.transp.length   	= 0;

	fbinfo->fix.smem_len        	=	mach_info->xres.max * mach_info->yres.max * mach_info->bpp.max / 8;

	if (!request_mem_region((unsigned long)LCDC_BASE_V, SZ_4K, "sep4020-lcd")) 
	{
		ret = -EBUSY;
		goto dealloc_fb;
	}

	msleep(1);

	/* Initialize video memory */
	ret = sep4020fb_map_video_memory(info);
	if (ret) 
	{
		printk( KERN_ERR "Failed to allocate video RAM: %d\n", ret);
		ret = -ENOMEM;
		goto release_mem;
	}
	dprintk("got video memory\n");

	ret = sep4020fb_init_registers(info);
	ret = sep4020fb_check_var(&fbinfo->var, fbinfo);
	
   	writel(1, LCDC_LECR_V);  //使能LCDC    
   
	ret = register_framebuffer(fbinfo);	
	if (ret < 0) 
	{
		printk(KERN_ERR "Failed to register framebuffer device: %d\n", ret);
		goto free_video_memory;
	}

	/* create device files */
	device_create_file(&pdev->dev, &dev_attr_debug);
	printk(KERN_INFO "fb%d: %s frame buffer device\n",fbinfo->node, fbinfo->fix.id);

	return 0;

free_video_memory:
	sep4020fb_unmap_video_memory(info);
release_mem:
 	release_mem_region((unsigned long)LCDC_BASE_V, SZ_4K );
dealloc_fb:
	framebuffer_release(fbinfo);
	return ret;
}


/*
 *  Cleanup
 */
static int sep4020fb_remove(struct platform_device *pdev)
{
	struct fb_info	   *fbinfo = platform_get_drvdata(pdev);
	struct sep4020fb_info *info = fbinfo->par;

	msleep(1);

	sep4020fb_unmap_video_memory(info);

	release_mem_region((unsigned long)LCDC_BASE_V, SZ_4K );
	unregister_framebuffer(fbinfo);

	return 0;
}

#ifdef CONFIG_PM

/* suspend and resume support for the lcd controller */

static int sep4020fb_suspend(struct platform_device *dev, pm_message_t state)
{
	struct fb_info	   *fbinfo = platform_get_drvdata(dev);
	struct sep4020fb_info *info = fbinfo->par;

	/* sleep before disabling the clock, we need to ensure
	 * the LCD DMA engine is not going to get back on the bus
	 * before the clock goes off again (bjd) */
	msleep(1);
	return 0;
}

static int sep4020fb_resume(struct platform_device *dev)
{
	struct fb_info	   *fbinfo = platform_get_drvdata(dev);
	struct sep4020fb_info *info = fbinfo->par;

	msleep(1);
	sep4020fb_init_registers(info);
	return 0;
}

#else
#define sep4020fb_suspend NULL
#define sep4020fb_resume  NULL
#endif

static struct platform_driver sep4020fb_driver = {
	.probe		= sep4020fb_probe,
	.remove		= sep4020fb_remove,
	.suspend		= sep4020fb_suspend,
	.resume		= sep4020fb_resume,
	.driver		= {
		.name		= "sep4020-lcd",
		.owner	= THIS_MODULE,
	},
};

int __devinit sep4020fb_init(void)
{
	return platform_driver_register(&sep4020fb_driver);
}

static void __exit sep4020fb_cleanup(void)
{
	platform_driver_unregister(&sep4020fb_driver);
}


module_init(sep4020fb_init);
module_exit(sep4020fb_cleanup);

MODULE_AUTHOR("fpmystar@gmail.com,aokikyon@gmial.com");
MODULE_DESCRIPTION("Framebuffer driver for the sep4020");
MODULE_LICENSE("GPL");
