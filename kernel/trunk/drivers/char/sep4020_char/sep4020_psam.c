/* linux/drivers/char/sep4020_char/sep4020_psam.c
 *
* Copyright (c) 2009 prochip company
*	http://www.prochip.com.cn
*	
*
* sep4020 psam card driver.
*
* Changelog:
*	22-Jan-2009 leeming	Initial version
*	21-May-2009 llx  Current Version 
*	7-  	
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
#include <linux/delay.h>
#include <linux/time.h>


#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/hardware.h>




#define PSAM_MAJOR 252   //主设备号

#define RESPONE     0x61

#define TIME_OUT    100000
#define TIME_DELAY    20

#define COLDRESET   0x11
#define RECEIVE_LEN 0x12

#define MAXLEN 50

struct psam_dev
{
	int begin_time;
	int final_time;
	char receive_user[MAXLEN +5];
	int user_len;
	char send_data[5];			//命令头
	int command_len;			   //命令长度
	char command_data[MAXLEN];	//命令字节
	char receive_data[MAXLEN];	//接收字节
	int receive_len;			   //接收长度
	struct cdev cdev;
   struct timer_list PSAM_timer;
	int present;
};

struct psam_dev *psamdev;

struct timeval *tv;

void Psam_DATA_Dir(int Data_dir)

{
  
	if(!Data_dir)
    	{
    	*(volatile unsigned long*)GPIO_PORTG_SEL_V |= 0x01000;
		*(volatile unsigned long*)GPIO_PORTG_DIR_V &= (~0x01000);
		*(volatile unsigned long*)GPIO_PORTG_DATA_V &= (~0x01000);
	}
	
	else 
	{
		*(volatile unsigned long*)GPIO_PORTG_SEL_V |= 0x01000;
		*(volatile unsigned long*)GPIO_PORTG_DIR_V &= (~0x01000);
		*(volatile unsigned long*)GPIO_PORTG_DATA_V |= 0x01000;
	}
}

static void UseTimer(void)
{
	psamdev->PSAM_timer.expires = jiffies + TIME_DELAY;
	add_timer(&psamdev->PSAM_timer);
	psamdev->present = 1;
}

static int Psam1CommandSend(char *command, int len, int re_len)
{
	int reg_data,result = 0;

	int i, real_len = 0;

	// 发送命令过程
	
	
	Psam_DATA_Dir(1);

	//printk("len = %d\n",len);
	*(volatile unsigned long*)SMC0_TX_TRIG_V = len;	
	
	UseTimer();
	for(i=0;i<len;i++)
	{
		*(volatile unsigned char*)SMC0_TX_V = *(command+i);
	
		while((*(volatile unsigned long*)SMC0_STATUS_V & 0x20) && (psamdev->present));//TX_FIFO Full?

	}			
	

	reg_data = *(volatile unsigned char*)SMC0_STATUS_V;

	//printk("/////reg_data = %x\n",reg_data);

    //接收命令过程
	UseTimer();	
   while(psamdev->present)
	{
      if((reg_data & 0x012) != 0x12)
		{
        	reg_data = *(volatile unsigned char*)SMC0_STATUS_V; 

      		}
	  	else
	 	{
	     	Psam_DATA_Dir(0);
	     	break;
	  	} 
	} 

	// 接收命令过程
	psamdev->receive_len = 0;
	
	i=0;
	//printk("afrer receive message\n");

	UseTimer();	
	while(((*(volatile unsigned long*)SMC0_STATUS_V & 0x40)==0x40) && (psamdev->present));//waitting for the first ACK data.
	//printk("while again \n");

	reg_data = *(volatile unsigned long*)SMC0_STATUS_V;


	UseTimer();	
	while(psamdev->present) 									
	{ 
		if((reg_data & 0x40)!=0x40) //There is some data in the RX_FIFO,Get one data!
		{

			psamdev->receive_data[psamdev->receive_len] = *(volatile unsigned long*)SMC0_RX_V;		//读取接收FIFO中的数据

			real_len++;
			
			psamdev->receive_len++;			
		}		
		
		if(real_len >= re_len) //enough?
		{
			result = 0;
			break;	
		}

		reg_data = *(volatile unsigned long*)SMC0_STATUS_V;

		if ((reg_data & 0x08) == 0x08)
		{
			result = -EPERM;
			break;
		}
	}
	//printk("finish all while\n");

	reg_data = *(volatile unsigned long*)SMC0_STATUS_V;

	

	if ((reg_data & 0x08) == 0x08)
	{
		printk("Action Timeout:0x%x\n",reg_data);
	}
 
	return result;	
}

static int Psam1Pro(char *buffer, int total_len,int receive1_len,int receive2_len)
{
	int i=0;
	char back_respone[5];
	int result = 0;
	int send_value;  

	//每次发送前初始化全局结构体

	memset(psamdev->send_data, 0, 5);
	psamdev->command_len = 0;
	
	memset(psamdev->command_data, 0, MAXLEN);
	memset(psamdev->receive_data, 0, MAXLEN);
	psamdev->receive_len = 0;
	
	for (i = 0; i < 5; i++)
	{
		psamdev->send_data[i] = *(buffer++);
	}

	psamdev->command_len = total_len - 5;
	
	for (i = 0; i < psamdev->command_len; i++)
	{
		psamdev->command_data[i] = *(buffer++);
	}
	
	//printk("228\n");
	send_value = Psam1CommandSend(psamdev->send_data, 5, receive1_len);
	//printk("230\n");
	if (send_value < 0)
	{
		result = -EPERM;
		return result;
	}
	

	//printk("before psam1command send\n");

	if (psamdev->send_data[1] == psamdev->receive_data[0])
	{
		if (psamdev->command_len != 0)
		{
			send_value = Psam1CommandSend(psamdev->command_data, psamdev->command_len, receive2_len);

			if (send_value < 0)
			{
				result = -EPERM;
				return result;
			}
		}
	}
	
	
	
	if (psamdev->receive_data[0] == RESPONE)   
	{
		memset(back_respone, 0, 5);
		back_respone[1] = 0xc0;
	
		back_respone[4] = psamdev->receive_data[1];
		
		send_value = Psam1CommandSend(back_respone, 0x05, psamdev->receive_data[1] + 3);
		
		if (send_value < 0)
		{
			result = -EPERM;
			return result;
		}
	}
	
	return 0;
}

static void PSAM_timer_handler(unsigned long arg)
{
	psamdev->present = 0;
}

static ssize_t sep4020_psam_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{	
	if (size > psamdev->receive_len)
		size = psamdev->receive_len;
	
	return copy_to_user(buf, psamdev->receive_data, size);
}


static ssize_t sep4020_psam_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
	int len,result;
	int pro_value;
	char send[2] = {0};
	
	if(size!= 2)
	{
		len = copy_from_user(psamdev->receive_user, buf, size);
		psamdev->user_len = size;
	
	}
	else
	{
		len = copy_from_user(send, buf, size);	
		//printk("copy from user finished\n");
		pro_value = Psam1Pro(psamdev->receive_user, psamdev->user_len, send[0], send[1]);
		
		if (pro_value < 0)
		{
			result = -EPERM;
			return result;
		}

	}
	
	//printk("len = %d\n",len);
	return len;		
}

static int sep4020_psam_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	int reg_status,i = 0;
	int result;

	switch(cmd)
	{
		case COLDRESET:
			{

				 Psam_DATA_Dir(0);

				*(volatile unsigned long*)SMC0_CTRL_V &= (~0x00000001);
				udelay(20);
				*(volatile unsigned long*)SMC0_CTRL_V |= 0x00000001;										//使能PSAM卡

				psamdev->receive_len = 0;
				reg_status = *(volatile unsigned long*)SMC0_STATUS_V;

				while ((reg_status & 0x08) != 0x08) 					//不为空,且不超时
				{ 
					if((reg_status & 0x40) != 0x40)
					{
						psamdev->receive_data[psamdev->receive_len] = *(volatile unsigned long*)SMC0_RX_V;		//读取接收FIFO中的数据
						(psamdev->receive_len)++;	
						i++;
					}
					
					i++;

					if(i >= 640000)
					{	
						i = 0;
						result = -EPERM;
						return result;	
						//break;		
					} 
		
					reg_status = *(volatile unsigned long*)SMC0_STATUS_V;
				}

				reg_status = *(volatile unsigned long*)SMC0_STATUS_V;
				
				if ((reg_status & 0x08) == 0x08)
				{
					printk("Reset Timeout,real_len= %d\n",psamdev->receive_len);

					result = -EPERM;
					return result;
				}
	

				break;
			}
		case RECEIVE_LEN:
			{
				put_user(psamdev->receive_len,(char*)arg);
				
				break;
			}
		default:
			return -EINVAL;
	}
	return 0;
}
	
int sep4020_psam_open(struct inode *inode, struct file *filp)
{
#if 0
	int reg_status;
	udelay(40);
	*(volatile unsigned long*)SMC0_CTRL_V |= 0x00000001;							//使能PSAM卡
	
	reg_status = *(volatile unsigned long*)SMC0_STATUS_V;                   //读卡的状态：无卡，SMC0_STATUS【31】=0；有卡，SMC0_STATUS【31】=1
	
	return 0;
#endif

	int reg_status,result;
	int delay = 0;
	
	*(volatile unsigned long*)SMC0_CTRL_V |= 0x00000001;										//使能PSAM卡
	
	reg_status = *(volatile unsigned long*)SMC0_STATUS_V;                   //读卡的状态：无卡，SMC0_STATUS【31】=0；有卡，SMC0_STATUS【31】=1
#if 0	
	while ((reg_status & 0x80000000) != 0x80000000)
	{
		udelay(10);
		delay++;
		reg_status = *(volatile unsigned long*)SMC0_STATUS_V;

		if(delay > TIME_OUT)
		{
			result = -EPERM;
			return result;
		}
 	}        
#endif
	setup_timer(&psamdev->PSAM_timer,PSAM_timer_handler,0); 			//添加定时器
	return 0;
}

int sep4020_psam_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static struct file_operations sep4020_psam_fops = 
{
	.owner = THIS_MODULE,
	.read  = sep4020_psam_read,
	.write = sep4020_psam_write,
	.ioctl = sep4020_psam_ioctl,
	.open  = sep4020_psam_open,
	.release = sep4020_psam_release,
};

static void sep4020_psam_setup(void)   //初始化smc模块
{	
	 *(volatile unsigned long*)GPIO_PORTE_SEL_V &= (~0x0038);     	//PE3:5配置为特殊用途
    *(volatile unsigned long*)SMC0_CTRL_V  &= (~0x00000001);    	//SM_CTRL控制寄存器,不使能PSAM卡
    *(volatile unsigned long*)SMC0_INT_MASK_V = 0x00000000;   		//SM_INT中断屏蔽寄存器
    *(volatile unsigned long*)SMC0_CTRL_V = 0x000b0000;           //SM_CTRL控制寄存器,b23:16时钟分频参数,b3异步半双工字符传,b2正相编码,b0使能信号
    *(volatile unsigned long*)SMC0_INT_MASK_V |= 0x0000001f;  		//SM_INT中断屏蔽寄存器
    *(volatile unsigned long*)SMC0_FD_V |= 0x01740001;            //SM_FD基本单元时间寄存器,B31:16时钟转换因子372,b3:0波特率调整因子1
    *(volatile unsigned long*)SMC0_CT_V = 0x0100;
}

static int __init sep4020_psam_init(void)
{
	int err,result;
	
	dev_t devno = MKDEV(PSAM_MAJOR, 0);
	
	result = register_chrdev_region(devno, 1, "sep4020_psam");
	
	if (result < 0)
	{
		return result;
	}

	sep4020_psam_setup();

	//动态申请设备结构体的内存
	psamdev = kmalloc(sizeof(struct psam_dev), GFP_KERNEL);
	if (psamdev == NULL)
	{
		result = -ENOMEM;
		unregister_chrdev_region(devno, 1);
		return result;
	}
	
	memset(psamdev,0,sizeof(struct psam_dev));  //初始化
		
	//将fops注册到结构体中
	cdev_init(&(psamdev->cdev), &sep4020_psam_fops);
	psamdev->cdev.owner = THIS_MODULE;

	//向系统注册该字符设备
	err = cdev_add(&psamdev->cdev, devno, 1);
	if (err)
	{
		printk("psam card adding err\r\n");
		unregister_chrdev_region(devno,1);
		kfree(psamdev);
		return err;
	}	
		
	return 0;
}

static void __exit sep4020_psam_exit(void)
{
	cdev_del(&psamdev->cdev);
	kfree(psamdev);
	unregister_chrdev_region(MKDEV(PSAM_MAJOR, 0), 1);
}

module_init(sep4020_psam_init);
module_exit(sep4020_psam_exit);

MODULE_AUTHOR("Leeming Zhang,leeming1203@gmail.com");
MODULE_LICENSE("GPL");

