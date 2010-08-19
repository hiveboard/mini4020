/* linux/drivers/char/sep4020_char/sep4020_key.c
 *
* Copyright (c) 2009 prochip company
*	http://www.prochip.com.cn
*	
*
* sep4020 key driver.
*
* Changelog:
*	22-Jan-2009 leeming	Initial version
*	3-May-2009 llx  Current Version   	
 *
 * 
 *
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*/

#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>
#include <linux/time.h>
#include <linux/spinlock_types.h>
#include <linux/delay.h>

#include <asm/semaphore.h>
#include <asm/types.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/hardware.h>


#define KEY_MAJOR 254          //主设备号
#define MAX_KEY_BUF 16         //按键缓冲区的大小
#define KEY_XNUM 5 
#define KEY_YNUM 5

#define KEY_UP 0
#define KEY_DOWN 1
#define KEY_UNSURE 2

#define KEY_TIMER_DELAY_JUDGE 4     //判断是否有按键按下   jiffes
#define KEY_TIMER_DELAY_LONGTOUCH 10    //判断是否长时间按下  jiffes


static int key_map[] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19,20,21,22,23,24};
								
struct keydev
{    
	unsigned int keystatus;          //按键状态    
	unsigned int buf[MAX_KEY_BUF];   //按键缓冲区  
	unsigned int write,read;         //按键缓冲区头和尾
	wait_queue_head_t wq;            //等待队列
	struct cdev cdev;
} ;
	
struct keydev *key_dev;     //键盘结构体


struct timer_list key_timer;        //定时器



//开启键盘中断
static void unmaskkey(void)
{
	enable_irq(INTSRC_EXTINT0);
	enable_irq(INTSRC_EXTINT1);
	enable_irq(INTSRC_EXTINT2);
	enable_irq(INTSRC_EXTINT3);
	enable_irq(INTSRC_EXTINT4);
}

//关闭键盘中断
static void maskkey(void)
{
	disable_irq(INTSRC_EXTINT0);
	disable_irq(INTSRC_EXTINT1);
	disable_irq(INTSRC_EXTINT2);
	disable_irq(INTSRC_EXTINT3);
	disable_irq(INTSRC_EXTINT4);
}

static void write_row(int index, int HighLow)
{
	switch (index)
	{
		case 0:
			if (HighLow) 
			{
				*(volatile unsigned long*)GPIO_PORTD_DATA_V |= 0x01;
			}
			else 
			{
				*(volatile unsigned long*)GPIO_PORTD_DATA_V &= ~0x01;
			}
			break;
		case 1: 
			if (HighLow) 
			{
				*(volatile unsigned long*)GPIO_PORTD_DATA_V |= 0x02;
			}
			else 
			{
				*(volatile unsigned long*)GPIO_PORTD_DATA_V &= ~0x02;
			}
			break;
		case 2: 
			if (HighLow) 
			{
				*(volatile unsigned long*)GPIO_PORTD_DATA_V |= 0x04;
			}
			else 
			{
				*(volatile unsigned long*)GPIO_PORTD_DATA_V &= ~0x04;
			}
			break;
		case 3:
			if (HighLow) 
			{
				*(volatile unsigned long*)GPIO_PORTD_DATA_V |= 0x08;
			}
			else 
			{
				*(volatile unsigned long*)GPIO_PORTD_DATA_V &= ~0x08;
			}
			break;
		case 4: 
			if (HighLow) 
			{
				*(volatile unsigned long*)GPIO_PORTD_DATA_V |= 0x10;
			}
			else 
			{
				*(volatile unsigned long*)GPIO_PORTD_DATA_V &= ~0x10;
			}
			break;
		default:
			break;
	}
}

static int read_col(int index)
{
	return (*(volatile unsigned long*)GPIO_PORTA_DATA_V >> index) & 0x1;
}

static int keyevent(void)
{
	int i = 0;
	int col = 0, row = 0;
	int init_col = 0;
	int col_count = 0, row_count = 0;
	int keynum = 0;

	init_col = (*(volatile unsigned long*)(INTC_ISR_V)>>1) & 0x001F;
	
 	for (i=0; i<KEY_XNUM; i++)         //获取列数 
	{
		if ((init_col >> i) & 0x01 )  
		{
			col = i ;
			col_count++;
		}
	}

	if ((!read_col(col)))
	{
		for (i=0; i<KEY_YNUM; i++)   	 //通过对轮流PD0~PD4输出高电平；获取行数
		{ 
			write_row(i,1);
			//for (delay=0; delay<100; delay++); //delay
			if (read_col(col))
			{ 
				row = i ;
				row_count++;
			}
			write_row(i, 0);
		} 
	} 
        
	if (col_count==1 && row_count==1)
	{
		keynum = col * KEY_YNUM + row;
		return keynum;
	}
	else
	{
	printk("key errors\n");
	return -1;
	}


	
}

static void sep4020_key_setup(void)
{
	//关闭键盘中断
	maskkey();

	*(volatile unsigned long*)GPIO_PORTD_SEL_V  |= 0x1F ;      //通用用途
	*(volatile unsigned long*)GPIO_PORTD_DIR_V  &= (~0x1F);    //输出
	*(volatile unsigned long*)GPIO_PORTD_DATA_V &= (~0x1F);    

	*(volatile unsigned long*)GPIO_PORTA_SEL_V |= 0x001F ;     //通用用途
	*(volatile unsigned long*)GPIO_PORTA_DIR_V |= 0x001F ;     //输入
	*(volatile unsigned long*)GPIO_PORTA_INTRCTL_V |= 0x03ff;  //低电平触发 
	*(volatile unsigned long*)GPIO_PORTA_INCTL_V |= 0x001F;    //外部中断源输入
    
	*(volatile unsigned long*)GPIO_PORTA_INTRCLR_V |= 0x001F;    
	*(volatile unsigned long*)GPIO_PORTA_INTRCLR_V = 0x0000;    //清除中断    

	//开启键盘中断
	unmaskkey();      
}

static irqreturn_t sep4020_key_irqhandler(int irq, void *dev_id, struct pt_regs *reg)
{
	 //关闭键盘中断
	maskkey();  

	*(volatile unsigned long*)GPIO_PORTA_INTRCLR_V |= 0x001F;
	*(volatile unsigned long*)GPIO_PORTA_INTRCLR_V = 0x0000;    //清除中断  

	key_dev->keystatus = KEY_UNSURE;
	key_timer.expires = jiffies + KEY_TIMER_DELAY_JUDGE;
	add_timer(&key_timer);     //启动定时器

	//we will turn on the irq in the timer_handler
	return IRQ_HANDLED;
}

static void key_timer_handler(unsigned long arg)
{
	int irq_value = 0;
	int key_value = 0;

	irq_value = *(volatile unsigned long*)GPIO_PORTA_DATA_V;  //读取中断口数值
	
	if ((irq_value&0x1f) != 0x1f)   //如果有低电平，表示键盘仍然有键被按着
	{
		if (key_dev->keystatus == KEY_UNSURE)
		{
			key_dev->keystatus = KEY_DOWN;
			
			key_value = keyevent();      //读取键盘的位置

			if (key_value > 0)
			{key_dev->buf[key_dev->write] = key_map[key_value];  

			if (++(key_dev->write) == MAX_KEY_BUF)               //按键缓冲区循环存取
			{
				key_dev->write = 0;
			}
}
		
			wake_up_interruptible(&(key_dev->wq));
		

			//检测是否持续按键
			key_timer.expires = jiffies + KEY_TIMER_DELAY_LONGTOUCH;
			add_timer(&key_timer);							
		}
		else    //一定是键按下 
		{
			key_timer.expires = jiffies + KEY_TIMER_DELAY_LONGTOUCH;    //延迟
			add_timer(&key_timer);
		}
	}
	else      //键已抬起
	{
		key_dev->keystatus = KEY_UP;

		//打开键盘中断
		unmaskkey();    
	}	
}

static ssize_t sep4020_key_read(struct file *filp, char __user *buff, size_t size, loff_t *ppos)
{
	int total_num = 0;
	unsigned long err;
	int i;
	int buffer[17] = {0};
  
	retry:
   	if ((key_dev->write) != (key_dev->read))    //当前缓冲队列中有数据
		{
			if ((key_dev->write) > (key_dev->read)) 
			{
				total_num = (key_dev->write) - (key_dev->read);
			}
			else 
			{
				total_num = (16 - key_dev->read) + key_dev->write;
			}
			
			if (size > total_num)
			{
				size = total_num;
			}

			buffer[0] = size;
			for (i=1; i<(size+1); i++)
			{
				buffer[i] = key_dev->buf[key_dev->read];
				if (++(key_dev->read) == MAX_KEY_BUF)
				{
					key_dev->read = 0;
				}
			}
			err = copy_to_user(buff, (char *)buffer, (size+1)*4);
			return err ? -EFAULT : 0;
		}
		else      //当前缓冲队列中没有数据
		{
			if(filp->f_flags & O_NONBLOCK)    //假如用户采用的是非堵塞方式读取

             return -EAGAIN;

			//用户采用阻塞方式读取，调用该函数使进程睡眠
			interruptible_sleep_on(&(key_dev->wq)); 
 
			goto retry;
		}
}

static ssize_t sep4020_key_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
	return 0;
}

static int sep4020_key_open(struct inode *inode, struct file *filp)
{
	sep4020_key_setup();
	init_waitqueue_head(&(key_dev->wq));
	return 0;
}

static int sep4020_key_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static struct file_operations sep4020_key_fops = 
{
	.owner = THIS_MODULE,
	.read  = sep4020_key_read,
	.write = sep4020_key_write,
	.open  = sep4020_key_open,
	.release = sep4020_key_release,
};

static int sep4020_request_irqs(void)
{
	if (request_irq(INTSRC_EXTINT0,sep4020_key_irqhandler,SA_INTERRUPT,"4020KEY",NULL))	//申请中断
		goto 	irq0_fail;	
	if (request_irq(INTSRC_EXTINT1,sep4020_key_irqhandler,SA_INTERRUPT,"4020KEY",NULL))	
		goto 	irq1_fail;	
	if (request_irq(INTSRC_EXTINT2,sep4020_key_irqhandler,SA_INTERRUPT,"4020KEY",NULL))	
		goto 	irq2_fail;	
	if (request_irq(INTSRC_EXTINT3,sep4020_key_irqhandler,SA_INTERRUPT,"4020KEY",NULL))	
		goto 	irq3_fail;	
	if (request_irq(INTSRC_EXTINT4,sep4020_key_irqhandler,SA_INTERRUPT,"4020KEY",NULL))	
		goto 	irq4_fail;	
			
	return 0;
		
	//出错处理
	irq4_fail:
		free_irq(INTSRC_EXTINT4,NULL);
	irq3_fail:
		free_irq(INTSRC_EXTINT3,NULL);
	irq2_fail:
		free_irq(INTSRC_EXTINT2,NULL);
	irq1_fail:
		free_irq(INTSRC_EXTINT1,NULL);
	irq0_fail:
		free_irq(INTSRC_EXTINT0,NULL);
	
	return -1;
}

static void sep4020_free_irqs(void)
{
	free_irq(INTSRC_EXTINT0,NULL);
	free_irq(INTSRC_EXTINT1,NULL);
	free_irq(INTSRC_EXTINT2,NULL);
	free_irq(INTSRC_EXTINT3,NULL);
	free_irq(INTSRC_EXTINT4,NULL);
}

static int __init sep4020_key_init(void)
{
	int err,result;
	dev_t devno;


	devno = MKDEV(KEY_MAJOR, 0);
	result = register_chrdev_region(devno, 1, "sep4020_key");   //向系统静态申请设备号

	if (result < 0)
	{
		return result;
	}
	
	key_dev = kmalloc(sizeof(struct keydev), GFP_KERNEL);
	if (key_dev == NULL)
	{
		result = -ENOMEM;
		unregister_chrdev_region(devno, 1);
		return result;
	}
	memset(key_dev,0,sizeof(struct keydev));  //初始化
	
	if(sep4020_request_irqs())  //注册中断函数
	{ 
		unregister_chrdev_region(devno,1);
		kfree(key_dev); 
		printk("request key irq failed!\n");
		return -1;
	}
	
	cdev_init(&key_dev->cdev, &sep4020_key_fops);
	key_dev->cdev.owner = THIS_MODULE;
	key_dev->keystatus = KEY_UP;
	setup_timer(&key_timer,key_timer_handler,0); //初始化定时器  

	//向系统注册该字符设备
	err = cdev_add(&key_dev->cdev, devno, 1);
	if (err)
	{
		printk("key adding err\r\n");
		unregister_chrdev_region(devno,1);
		kfree(key_dev);
		sep4020_free_irqs();
		return err;
	}	
	return 0;
}

static void __exit sep4020_key_exit(void)
{
	sep4020_free_irqs();
	cdev_del(&key_dev->cdev);
	kfree(key_dev);
	unregister_chrdev_region(MKDEV(KEY_MAJOR, 0), 1);
}

module_init(sep4020_key_init);
module_exit(sep4020_key_exit);

MODULE_AUTHOR("Leeming Zhang");
MODULE_LICENSE("GPL");

