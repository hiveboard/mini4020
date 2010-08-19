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

#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/hardware.h>

#define PSAM_MAJOR 252   //���豸��

#define RESPONE     0x61

#define TIME_OUT    10000
#define TIME_DELAY    15

#define COLDRESET   0x11
#define RECEIVE_LEN 0x12

#define MAXLEN 50

struct psam_dev
{
	char send_data[5];			//����ͷ
	int command_len;			   //�����
	char command_data[MAXLEN];	//�����ֽ�
	char receive_data[MAXLEN];	//�����ֽ�
	int receive_len;			   //���ճ���
	struct cdev cdev;
};

struct psam_dev *psamdev;

static int Psam1CommandSend(char *command, int len)
{
	int reg_data,result;
	int delay = 0;
	int i, integ, sur_value, m;

	// �����������
	if(len > 8)
	{
		integ = len/8;
		sur_value = len%8;
		for (m=0; m<integ; m++)
		{
			for (i=0; i<8; i++)
			{
				*(volatile unsigned long*)SMC1_TX_V = *command;
				command++;
			}	
			*(volatile unsigned long*)SMC1_TX_TRIG_V = 8;
			mdelay(TIME_DELAY);
		}

		for (i=0; i<sur_value; i++)
		{
			*(volatile unsigned long*)SMC1_TX_V = *command;
			command++;
		}
		*(volatile unsigned long*)SMC1_TX_TRIG_V = sur_value;
	}

	else
	{
		for (i=0; i<len; i++)
		{
			*(volatile unsigned long*)SMC1_TX_V = *command;
			command++;
		}
		*(volatile unsigned long*)SMC1_TX_TRIG_V = len;
	}
	

	// �����������
	reg_data = *(volatile unsigned long*)SMC1_STATUS_V;
	while ((reg_data & 0x40) == 0x40)           //��Ϊ��
	{
		udelay(10);
		delay++;
		reg_data = *(volatile unsigned long*)SMC1_STATUS_V;
		
		if(delay > TIME_OUT)
		{
			result = -EPERM;
			return result;
		}
   	        
	}
	  
	psamdev->receive_len = 0;
	while((reg_data & 0x48) != 0x48) 									//empty and rx timeout��Ϊ��,�Ҳ���ʱ
	{ 
		if ((reg_data & 0x40) != 0x40) 									//not empty
		{
			psamdev->receive_data[psamdev->receive_len] = *(volatile unsigned long*)SMC1_RX_V;		//��ȡ����FIFO�е�����
			psamdev->receive_len ++;			
		}
		reg_data = *(volatile unsigned long*)SMC1_STATUS_V;
	}

	if (reg_data && 0x08 == 0)
	{
		result = -EPERM;
		return result;
	}
 
	return 0;	
}

static int Psam1Pro(char *buffer, int data_len)
{
	int i=0;
	char back_respone[5];
	int result = 0;
	int send_value;  

	//ÿ�η���ǰ��ʼ��ȫ�ֽṹ��
	memset(psamdev->send_data, 0, 5);
	psamdev->command_len = 0;
	memset(psamdev->command_data, 0, MAXLEN);
	memset(psamdev->receive_data, 0, MAXLEN);
	psamdev->receive_len = 0;
	
	for (i = 0; i < 5; i++)
	{
		psamdev->send_data[i] = *(buffer++);
	}

	psamdev->command_len = data_len;
	
	for (i = 0; i < psamdev->command_len; i++)
	{
		psamdev->command_data[i] = *(buffer++);
	}

	send_value = Psam1CommandSend(psamdev->send_data, 5);
	if (send_value < 0)
	{
		result = -EPERM;
		return result;
	}
	
	if (psamdev->send_data[1] == psamdev->receive_data[0])
	{
		if (psamdev->command_len != 0)
		{
			send_value = Psam1CommandSend(psamdev->command_data, psamdev->command_len);
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
		send_value = Psam1CommandSend(back_respone, 0x05);
		if (send_value < 0)
		{
			result = -EPERM;
			return result;
		}
	}
	
	return 0;
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
	char *send;
	
	send = (char *)kmalloc(size, GFP_KERNEL);
	if(send == NULL)
	{
		result = -ENOMEM;
		return result;
	}

	len = copy_from_user(send, buf, size);
	
	pro_value = Psam1Pro(send, size-5);
	if (pro_value < 0)
	{
		kfree(send);
		result = -EPERM;
		return result;
	}

	kfree(send);
	return len;		
}

static int sep4020_psam_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	int reg_status;
	int result;

	switch(cmd)
	{
		case COLDRESET:
			{
				*(volatile unsigned long*)SMC1_CTRL_V &= (~0x00000001);
				*(volatile unsigned long*)SMC1_CTRL_V |= 0x00000001;										//ʹ��PSAM��

				psamdev->receive_len = 0;
				reg_status = *(volatile unsigned long*)SMC1_STATUS_V;

				while ((reg_status & 0x08) != 0x08) 					//��Ϊ��,�Ҳ���ʱ
				{ 
					if((reg_status & 0x40) != 0x40)
					{
						psamdev->receive_data[psamdev->receive_len] = *(volatile unsigned long*)SMC1_RX_V;		//��ȡ����FIFO�е�����
						(psamdev->receive_len)++;	
					}
		
					reg_status = *(volatile unsigned long*)SMC1_STATUS_V;
				}

				if (reg_status && 0x08 == 0)
				{
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
	int reg_status,result;
	int delay = 0;

	*(volatile unsigned long*)SMC1_CTRL_V |= 0x00000001;										//ʹ��PSAM��
	
	reg_status = *(volatile unsigned long*)SMC1_STATUS_V;                   //������״̬���޿���SMC1_STATUS��31��=0���п���SMC1_STATUS��31��=1
	
	while ((reg_status & 0x80000000) != 0x80000000)
	{
		udelay(10);
		delay++;
		reg_status = *(volatile unsigned long*)SMC1_STATUS_V;

		if(delay > TIME_OUT)
		{
			result = -EPERM;
			return result;
		}
 	}        
	
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

static void sep4020_psam_setup(void)   //��ʼ��smcģ��
{	
	//PE3:5����Ϊ������;,����ʹ�õ���smc1
	*(volatile unsigned long*)GPIO_PORTE_SEL_V &= (~0x00000038);       //0000 0000 0011 1000 PE3:5=000 ����Ϊ������;

	//SM_CTRL���ƼĴ���
	*(volatile unsigned long*)SMC1_CTRL_V  &= (~0x00000001);		      //��ʹ��PSAM��

	//SM_CTRL���ƼĴ���
	*(volatile unsigned long*)SMC1_CTRL_V = 0x00100000;			      //b23:16ʱ�ӷ�Ƶ����,b3�첽��˫���ַ���,b2�������,b0ʹ���ź�
	
	//SM_FD������Ԫʱ��Ĵ���
	*(volatile unsigned long*)SMC1_FD_V  |= 0x01740001;				  //B31:16ʱ��ת������372,b3:0�����ʵ�������1
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

	//��̬�����豸�ṹ����ڴ�
	psamdev = kmalloc(sizeof(struct psam_dev), GFP_KERNEL);
	if (psamdev == NULL)
	{
		result = -ENOMEM;
		unregister_chrdev_region(devno, 1);
		return result;
	}
	
	memset(psamdev,0,sizeof(struct psam_dev));  //��ʼ��
		
	//��fopsע�ᵽ�ṹ����
	cdev_init(&(psamdev->cdev), &sep4020_psam_fops);
	psamdev->cdev.owner = THIS_MODULE;

	//��ϵͳע����ַ��豸
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
