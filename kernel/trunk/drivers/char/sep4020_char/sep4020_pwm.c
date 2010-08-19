/* linux/drivers/char/sep4020_char/sep4020_pwm.c
 *
* Copyright (c) 2009 prochip company
*	http://www.prochip.com.cn
*	wugangseu@gmail.com
*
* sep4020 pwm driver.
*
* Changelog:
*	1-March-2009 WuGang	Initial version
*	4-June-2009 WuGang  	fixed	a lot
 *
 * 
 *
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*/
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <linux/poll.h>
#include <linux/types.h>
#include <linux/config.h>
#include <linux/devfs_fs_kernel.h>
#include <linux/miscdevice.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/cdev.h>#include <linux/interrupt.h>

#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/hardware.h>


#define DEVICE_NAME  "sep4020_pwm"   
#define PWM_MAJOR     245         

#define PWM1_IOCTL_SET	    1
#define PWM1_IOCTL_STOP	    2

struct pwm_dev
{
	struct cdev cdev;
};

struct pwm_dev *sep4020_pwm_dev;

struct pwm_config
{
	unsigned long  pwm_ctrl;
	unsigned long  pwm_div;
	unsigned long  pwm_period;
	unsigned long  pwm_data;
} ;



static void PWM1_Set_Freq( unsigned long freq )
{
	unsigned long temp;
	struct pwm_config    *pwm_ptr;
	temp = freq;
	pwm_ptr = (struct pwm_config * )freq;
		
	*(volatile unsigned long*)PWM1_CTRL_V = pwm_ptr->pwm_ctrl; 
	*(volatile unsigned long*)PWM1_DIV_V = pwm_ptr->pwm_div;	
	*(volatile unsigned long*)PWM1_PERIOD_V = pwm_ptr->pwm_period;
	*(volatile unsigned long*)PWM1_DATA_V = pwm_ptr->pwm_data;
	
//	printk("PWM1_CTRL_V is 0x%x\n",*(volatile unsigned long*)PWM1_CTRL_V);
//	printk("PWM1_DIV_V is 0x%x\n",*(volatile unsigned long*)PWM1_DIV_V);
//	printk("PWM1_PERIOD_V is 0x%x\n",*(volatile unsigned long*)PWM1_PERIOD_V);
//	printk("PWM1_DATA_V is 0x%x\n",*(volatile unsigned long*)PWM1_DATA_V);
	
	*(volatile unsigned long*)PWM_ENABLE_V |= 1 ;
//	printk("PWM_ENABLE_V is 0x%x\n",*(volatile unsigned long*)PWM_ENABLE_V);	
}


void PWM1_Stop( void )
{
	*(volatile unsigned long*)PWM_ENABLE_V &= ~1 ;
//	  	printk("stop\n");
}


static void pwm_set_data(unsigned char data)
{
	unsigned long status,temp;
	if(data == 0)
	{
		temp = 1;
	}
	else	
	{
		temp = (unsigned long)data;
	}
	status = *(volatile unsigned long*)PWM1_STATUS_V;
	while((status & 0x00000004) == 0x00000004)
	{
		status = *(volatile unsigned long*)PWM1_STATUS_V;	
	}
	*(volatile unsigned long*)PWM1_DATA_V = temp;	
//	printk("PWM1_DATA_V is 0x%x\n",*(volatile unsigned long*)PWM1_DATA_V);		
}



static ssize_t sep4020_pwm_write(struct file *file, const char  *buf, size_t count, loff_t *f_ops)
{
//	unsigned char temp;
//	printk("write   ok!  \n");

	unsigned char wwrite_buf[1024];
	int i,j;
	copy_from_user(wwrite_buf,buf,1024);
	for(i=0;i<1024;i++)
	{
		for(j=0;j<4;j++)
		{
			pwm_set_data(wwrite_buf[i]);
		}
	}
	return count;
}

static int sep4020_pwm_open(struct inode *inode, struct file *file)
{
	*(volatile unsigned long*)GPIO_PORTA_SEL_V &= ~(1 << 6); //specific use
	*(volatile unsigned long*)GPIO_PORTA_DIR_V &= ~(1 << 6);//输出
	*(volatile unsigned long*)GPIO_PORTA_DATA_V &= ~(1 << 6);//初始输出拉低
	return 0;
}

static int sep4020_pwm_release(struct inode *inode, struct file *file)
{
	*(volatile unsigned long*)GPIO_PORTA_SEL_V &= ~(1 << 6); //specific use
	*(volatile unsigned long*)GPIO_PORTA_DIR_V &= ~(1 << 6);//输出
	*(volatile unsigned long*)GPIO_PORTA_DATA_V &= ~(1 << 6);//初始输出拉低
   return 0;
}

static int sep4020_pwm_ioctl(struct inode *inode, struct file *file, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
		case PWM1_IOCTL_SET:
			PWM1_Set_Freq(arg);
			break;

		case PWM1_IOCTL_STOP:
			PWM1_Stop();
			break;
			
		default:
			return -EINVAL;
	}

	return 0;
}

static struct file_operations sep4020_pwm_fops = {
    .owner   =   THIS_MODULE,    /* 这是一个宏，指向编译模块时自动创建的__this_module变量 */
    .open    =   sep4020_pwm_open,
    .release =   sep4020_pwm_release, 
    .write   =   sep4020_pwm_write,
    .ioctl   =   sep4020_pwm_ioctl,
};

static int __init sep4020_pwm_init(void)
{
	int error;
	int result;
	dev_t devno;
	devno = MKDEV(PWM_MAJOR,0);
	
	result = register_chrdev_region(devno,1,DEVICE_NAME);
	
	if(result<0)
	{
		return result;
	}

	//动态申请设备结构体的内存
	sep4020_pwm_dev = kmalloc(sizeof(struct pwm_dev), GFP_KERNEL);
	if (sep4020_pwm_dev == NULL)
	{
		result = -ENOMEM;
		unregister_chrdev_region(devno, 1);
		return result;
	}
	
	memset(sep4020_pwm_dev,0,sizeof(struct pwm_dev));  //初始化

	cdev_init(&sep4020_pwm_dev->cdev,&sep4020_pwm_fops);
	sep4020_pwm_dev->cdev.owner = THIS_MODULE;

	error = cdev_add(&sep4020_pwm_dev->cdev, devno, 1);
	if(error)
	{
		printk("error adding pwm!\n");
		unregister_chrdev_region(devno,1);
		return error;
	}

	printk("registered!\n");
	return 0;
}



static void __exit sep4020_pwm_exit(void)
{	
	cdev_del(&sep4020_pwm_dev->cdev);
	unregister_chrdev_region(MKDEV(PWM_MAJOR,0),1);
	printk("unregistered!\n");
}

module_init(sep4020_pwm_init);
module_exit(sep4020_pwm_exit);

MODULE_AUTHOR("WuGang");                         
MODULE_DESCRIPTION("sep4020 PWM Driver");         
MODULE_LICENSE("GPL");                              
