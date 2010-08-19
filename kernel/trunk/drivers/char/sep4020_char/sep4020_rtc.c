/* linux/drivers/char/sep4020_char/sep4020_rtc.c
*
* Copyright (c) 2009 prochip company
*	http://www.prochip.com.cn
*	leeming1203@gmail.com.cn
*
* sep4020 rtc driver.
*
* Changelog:
*	22-jan-2009 leeming	Initial version
*	6 -may-2009  ztt     fixed	a lot
*
* 
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*/

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/interrupt.h>
#include <linux/rtc.h>
#include <linux/bcd.h>
#include <linux/clk.h>
#include <linux/mc146818rtc.h>   

#include <asm-arm/delay.h>
#include <asm/hardware.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/rtc.h>


static struct resource *sep4020_rtc_mem;

static int sep4020_rtc_gettime(struct rtc_time *rtc_tm)
{ 
    unsigned int have_retried = 0;
    unsigned long ymd;
    unsigned long hms;
   
    retry_get_time:
        ymd = *(volatile unsigned long*)RTC_STA_YMD_V; 
        hms = *(volatile unsigned long*)RTC_STA_HMS_V; 

      

        rtc_tm->tm_sec  = hms & 0x3F;
        rtc_tm->tm_min  = (hms>>6) & 0x3F;
        rtc_tm->tm_hour = hms>>12;
        rtc_tm->tm_mday = ymd & 0x1F;
        rtc_tm->tm_mon  = (ymd>>5) & 0x0F;
        rtc_tm->tm_year = ymd>>9;

    //printk("the year is %x\n",rtc_tm->tm_year);
    //printk("the month is %x\n",rtc_tm->tm_mon);
    //printk("the day is %x\n",rtc_tm->tm_mday);


	/* 
        因为读日期和读时间不是同时获取的，所以可能会在临界态时出现问题
        例如2009年5月5日23点59分59秒时，
        读取了日期是2009年5月5日，但读时间时已经跳到2009年5月6日的0点0分0秒，
        这样读取的结果就是2009年5月5日0点0分0秒了，与实际值相差很大了，所以必须重新读取一次寄存器
        所以为了防止这种情况的发生，我们在秒为0时重新再读一次时间
	 */

    if (rtc_tm->tm_sec == 0 && !have_retried) 
    {
        have_retried = 1;
        goto retry_get_time;
    }

    rtc_tm->tm_year -=1892;
    rtc_tm->tm_mon -= 1;    

    return 0;
}

static int sep4020_rtc_settime(struct rtc_time *tm)
{
    unsigned long ymd;
    unsigned long hms;
	
    if (tm->tm_year > 2048)
    {
		return -EINVAL;
    }	

    /*set the year month day of the register	*/	
    ymd = ((tm->tm_year+1892)<<9) + ((tm->tm_mon+1)<<5) + (tm->tm_mday);
    *(volatile unsigned long*)RTC_CONFIG_CHECK_V = 0xaaaaaaaa;   
    udelay(100);
    *(volatile unsigned long*)RTC_STA_YMD_V = ymd;        
    udelay(100);
    *(volatile unsigned long*)RTC_CONFIG_CHECK_V = 0x00000000;
    udelay(100);
	
    /*set the hour minute second of the register*/
    hms = ((tm->tm_hour)<<12) + ((tm->tm_min)<<6) + (tm->tm_sec);
    *(volatile unsigned long*)RTC_CONFIG_CHECK_V = 0xaaaaaaaa;
    udelay(100);
    *(volatile unsigned long*)RTC_STA_HMS_V = hms;
    udelay(100);
    *(volatile unsigned long*)RTC_CONFIG_CHECK_V = 0x00000000;
    udelay(100);
	
    return 0;
}

static int sep4020_rtc_ioctl(unsigned int cmd, unsigned long arg)
{
    struct rtc_time septime;
	
    switch (cmd) 
    {
  	
        case RTC_SET_TIME:
        {			
			if (copy_from_user(&septime, (struct rtc_time*)arg, sizeof(struct rtc_time)))             
				return -EFAULT;             
               
			sep4020_rtc_settime(&septime);
			break;
        } 	

 	
        case RTC_RD_TIME:	// Read the time/date from RTC	
        {
			sep4020_rtc_gettime(&septime);

			if(copy_to_user((void *)arg, &septime, sizeof septime))
            return -EFAULT;

        break;
        }
  	
        default:
            return -EINVAL;
    }

	return 0;
}



static int sep4020_rtc_open(void)
{
    return 0;
}

static void sep4020_rtc_release(void)
{
  return;
}

static struct rtc_ops sep4020_rtc_ops = 
{
    .owner		= THIS_MODULE,
    .open		= sep4020_rtc_open,
    .release	= sep4020_rtc_release,
    .ioctl		= sep4020_rtc_ioctl,
    .read_time	= sep4020_rtc_gettime,
    .set_time	= sep4020_rtc_settime,
};

static int sep4020_rtc_probe(struct platform_device *pdev)
{
    struct resource *res;
    int ret;

    /* get the memory region */
    res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (res == NULL)
    {
        dev_err(&pdev->dev, "failed to get memory region resource\n");
        return -ENOENT;
    }
  
    sep4020_rtc_mem = request_mem_region(res->start, res->end-res->start+1,
				     pdev->name);

    if (sep4020_rtc_mem == NULL) 
    {
        dev_err(&pdev->dev, "failed to reserve memory region\n");
        return -ENOENT;
    }

    /* register RTC  */
    ret = register_rtc(&sep4020_rtc_ops);
    if (ret)
	{      
        release_mem_region(res->start, res->end-res->start+1);
        dev_err(&pdev->dev, "error %d during initialisation\n", ret);
        return ret;
	}

    return 0;
}

static int sep4020_rtc_remove(struct platform_device *dev)
{
    unregister_rtc(&sep4020_rtc_ops);

    if (sep4020_rtc_mem != NULL) 
    {       
        release_resource(sep4020_rtc_mem);
        kfree(sep4020_rtc_mem);
    }

    return 0;
}

static struct platform_driver sep4020_rtcdrv = 
{
    .probe = sep4020_rtc_probe,
    .remove = sep4020_rtc_remove,
    .driver =  {
                .name	= "sep4020_rtc",
                .owner	= THIS_MODULE,
                     },
};


static int __init sep4020_rtc_init(void)
{
    printk("SEP4020 RTC, (c) 2008 Prochip\n");
    return platform_driver_register(&sep4020_rtcdrv);
}

static void __exit sep4020_rtc_exit(void)
{
    platform_driver_unregister(&sep4020_rtcdrv);
}

module_init(sep4020_rtc_init);
module_exit(sep4020_rtc_exit);

MODULE_DESCRIPTION("SEP4020 RTC Driver");
MODULE_AUTHOR("leeming Zhang leeming1203@gmail.com");
MODULE_LICENSE("GPL");
















