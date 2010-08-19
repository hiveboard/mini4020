/* linux/drivers/char/sep4020_char/sep4020_mgcard.c
 *
* Copyright (c) 2009 prochip company
*	http://www.prochip.com.cn
*	wuweiwei3596253@163.com
*
* sep4020 magnetic card driver.
*
* Changelog:
*	10-Apr-2009 www	Initial version
*	25-May-2009 www   Current Version   	
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
#include <linux/delay.h>
#include <linux/time.h>

#include <asm/semaphore.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/hardware.h>



#define MC_MAJOR 255 /* 主设备号*/

#define  CP_LOW       	0x80		//PF7(0x0080)

#define  FORWARD			0x01
#define	BACK		   	0x02
#define	NO_TRACK2		   0x04
#define   NO_TRACK3		0x08

#define  OK			   0x05
#define  STATE		   0x00

#define  TRACK2	   	0x20
#define  TRACK3	   	0x40
#define  KEY_TIMER_DELAY  100
#define  SIZE1_DATA_TRK2       	300
#define  SIZE2_DATA_TRK3       	1000
#define  SIZE3_CARD_NUM2       	50
#define  SIZE4_CARD_NUM3       	120


	struct mgcd_dev    //磁卡数据
{
	unsigned char	data_trk2[SIZE1_DATA_TRK2];   	//轨2 原始数据    第2磁道数据编码最大记录长度为40个字符40*5=200
	unsigned char	data_trk3[SIZE2_DATA_TRK3];    	//轨3 原始数据    第3磁道数据编码最大记录长度为107个字符107*5=535
   char card_num2[SIZE3_CARD_NUM2];              	//经过处理的轨2数据存储到card_num2[50]
   char card_num3[SIZE4_CARD_NUM3];              	//经过处理的轨3数据存储到card_num3[50]  
   int            track2_num;       		       //记录的是轨2数据记录占用的空间长度  (处理之后的长度)
	int            track3_num;	      		    	//记录的是轨3数据记录占用的空间长度（处理之后的长度)
	unsigned char  result_value;    			      //FORWARD BACK NO_TRACK2 NO_TRACK3
   unsigned char  state;           			      //THE STATE OF BRUSHCARD
	struct cdev cdev;
   struct semaphore sem;
   struct timer_list MGCD_timer;
};

static struct mgcd_dev  *g_BrushCard;
static char direction_check(void);
static unsigned char convert_track(void);
static int convert_forward(char *data, int len, int track);
static int convert_back(char *data,int len, int track);
static char ChangeData(char data);
static void ShowTrack2(char *back_track);
static void ShowTrack3(char *back_track);
static char check(char data);
static int sep4020_request_irqs(void);
int TIMING1 = 0;   
int TIMING2 = 0;   
int mg_openflag = 0;

static unsigned char convert_track(void)       //对原始数据进行处理
{
	char direction;
	
	direction = direction_check(); //前面加一段磁道长度判断

	if(direction == FORWARD)
	{
		if(!(g_BrushCard->track2_num = convert_forward((char *)g_BrushCard->data_trk2, g_BrushCard->track2_num,TRACK2)))
			g_BrushCard->result_value |= NO_TRACK2;
		if(!(g_BrushCard->track3_num = convert_forward((char *)g_BrushCard->data_trk3, g_BrushCard->track3_num,TRACK3)))
			g_BrushCard->result_value |= NO_TRACK3;
	}
	else if(direction == BACK)
	{
		if(!(g_BrushCard->track2_num = convert_back((char *)g_BrushCard->data_trk2, g_BrushCard->track2_num,TRACK2)))
			g_BrushCard->result_value |= NO_TRACK2;
		if(!(g_BrushCard->track3_num = convert_back((char *)g_BrushCard->data_trk3, g_BrushCard->track3_num,TRACK3)))
			g_BrushCard->result_value |= NO_TRACK3;
	}
	if((g_BrushCard->track2_num == 0 && g_BrushCard->track3_num == 0)
	|| g_BrushCard->track2_num > 0x28 || g_BrushCard->track2_num < 0x10
	|| g_BrushCard->track3_num > 0x6B || g_BrushCard->track2_num < 0x12)	//轨2数据16－40 ,轨3数据18－107 
	{
		g_BrushCard->state = STATE;

		return 0;
	}
	else
	{
		g_BrushCard->state = OK;
		ShowTrack2(g_BrushCard->card_num2);
      ShowTrack3(g_BrushCard->card_num3);
	}

	g_BrushCard->result_value |= direction;
	
	return g_BrushCard->result_value;
}

static char direction_check(void)  //judge the brushcard direction
{    
   
	int i = 0,num = 0;
	char ascii_data = 0x00; 
	while(1)
	{
		if(g_BrushCard->data_trk2[num] == 0x00)					//去除前面的引导串
		{
			for(i = 0; i < 5;i++)
			{
				if(g_BrushCard->data_trk2[num++] & TRACK2)  	//the track2 originality data is (0x20 or 0x00) & track2(0x20) 
					ascii_data |= (0x00<<i); 						//if track2 originality data is ox20 ,reverse to 0x00
				else
					ascii_data |= (0x01<<i);					 	//if track2 originality data is ox00 ,reverse to 0x01
			}
			if(ascii_data == 0x0b)  				//if the first ascii_data is 0x0b(ox3b :the beginning sign),return the result is forward
				return FORWARD;                                    
			else
			{
				break;
			}
		}
		num++;															//judge the originality data until apear ox00
	}
	
	num = g_BrushCard->track2_num - 1;  						//back direction to judge
	ascii_data=0x00;
	
	while(1)
	{
		if(g_BrushCard->data_trk2[num] == 0x00)				//去除前面的引导串
		{
			for(i = 0;i < 5;i++)
			{
				if(g_BrushCard->data_trk2[num--] & TRACK2)
					ascii_data |= (0x00<<i);
				else
					ascii_data |= (0x01<<i);
			}
			if(ascii_data == 0x0b)
				return BACK;            
			else
               		{ 
				return 0;
			}
		}
		num--;
	}
}



static int convert_forward(char *data, int len, int track)
{ 
	int i = 0,now_postion = 0,real_num = 0;
	char *temp;															//记录当前指向的字符
	char ascii_data = 0x00, tmp_data[107], track_data;
	
	track_data = track;
	temp = data;
	
	while(1)
	{
		if(*temp&track_data)											//去除前面的引导串
		{
			temp ++;
			now_postion ++;
			if(now_postion > len)
				return 0;
		}
		else
		{
			while(ascii_data != 0x1f)							//前一次数据不为结束符就继续读取
			{				
				ascii_data = 0x00;

				for(i = 0;i < 5;i++)
				{
					if(*temp & track_data)
						ascii_data |= (0x00<<i);
					else
						ascii_data |= (0x01<<i);

					temp ++;
				}
				if(ascii_data == 0x00)
					break;

				if(check(ascii_data))
					tmp_data[real_num] = ascii_data;
				else
					return 0;

				real_num ++;
			}
			memset(data,0x00,len);
			if(real_num < 0x10)
				return 0;
			i = 0;
			do	{
					*(data + i) = tmp_data[i];
				}while((tmp_data[i++] != 0x1f)&&(i<120));
          
			return i;
		}
	}
}


static int convert_back(char *data,int len, int track)
{
	int i = 0,now_postion = 0,real_num = 0;
	char *temp;											//记录当前指向的字符
	char ascii_data = 0x00, tmp_data[107], track_data;

	track_data = track;
	temp = data + len - 1;
	
	while(1)
	{
		if(*temp&track_data)							//去除前面的引导串
		{
			temp --;
			now_postion ++;
			if(now_postion > len)
				return 0;
		}
		else
		{
			while(ascii_data != 0x1f)
			{				
				ascii_data = 0x00;
				for(i = 0; i < 5; i++)
				{
					if(*temp & track_data)
						ascii_data |= (0x00<<i);
					else
						ascii_data |= (0x01<<i);

					temp --;
				}

				if(ascii_data == 0x00)
					break;

				if(check(ascii_data))
					tmp_data[real_num] = ascii_data;
				else
					return 0;

				real_num ++;
			}
			memset(data,0x00,len);

			if(real_num < 0x10)
				return 0;

			i = 0;
			do{
				*(data + i) = tmp_data[i];
			}while((tmp_data[i++] != 0x1f)&&(i<120));

			return i;

		}
	}
}



static char ChangeData(char data)
{  
	data &= (~0x10);
	data = data + 0x30;
	return data;
}


static void ShowTrack2(char *back_track)
{
	int i = 0;
	
	if(g_BrushCard->data_trk2[0] != 0x0b)
		return;

	for(i = 0; i<g_BrushCard->track2_num; i++)	
	{
		*(back_track + i ) = ChangeData(g_BrushCard->data_trk2[i]);
	}

	*(back_track + i) = '\0';
   
}		

	
static void ShowTrack3(char *back_track)
{
 	int i;
	
	if(g_BrushCard->data_trk3[0] != 0x0b)
		return;

	for(i = 0; i<g_BrushCard->track3_num; i++)
	{
		*(back_track + i ) = ChangeData(g_BrushCard->data_trk3[i]);
	}

	*(back_track + i) = '\0';
}
										
static char check(char data)
{
	int num = 0,i = 0;
	
	for(i= 0; i < 8; i++)
	{
		if(data&(0x01 << i))
		num ++;
	}

	if(num%2)
		return OK;
	else
		return 0;
}	


static void sep4020_mgcd_setup(void)
{
   *(volatile unsigned long*)GPIO_PORTF_SEL_V |= 0x00E0;				//CLS =PF7,轨3DATA=PF6,轨2DATA=PF5作为输入信号
	*(volatile unsigned long*)GPIO_PORTF_DIR_V |= 0x00E0;				//1110 0000


	*(volatile unsigned long*)GPIO_PORTA_DIR_V |= 0x0100 ;	     	//PA8(INT8)轨2CLK
	*(volatile unsigned long*)GPIO_PORTA_SEL_V |= 0x0100 ; 			//0001 0000 0000      					
	*(volatile unsigned long*)GPIO_PORTA_INTRCTL_V |= 0x00010000; 	//中断类型为低电平解发 	0011 0000 0000 0000 0000
	*(volatile unsigned long*)GPIO_PORTA_INCTL_V |= 0x0100;      	//中断输入选择          0001 0000 0000 
	*(volatile unsigned long*)GPIO_PORTA_INTRCLR_V |= 0x0100;		//中断清除              0001 0000 0000 
	*(volatile unsigned long*)GPIO_PORTA_INTRCLR_V = 0x0000;

	*(volatile unsigned long*)GPIO_PORTA_DIR_V |= 0x0020 ;	     	//PA5(INT5)轨3CLK
	*(volatile unsigned long*)GPIO_PORTA_SEL_V |= 0x0020 ; 			//0010 0000      					
	*(volatile unsigned long*)GPIO_PORTA_INTRCTL_V |= 0x0400;   	//中断类型为低电平解发 	0000 1100 0000 0000
	*(volatile unsigned long*)GPIO_PORTA_INCTL_V |= 0x0200;      	//中断输入选择          0010 0000
	*(volatile unsigned long*)GPIO_PORTA_INTRCLR_V |= 0x0020;		//中断清除              0010 0000
	*(volatile unsigned long*)GPIO_PORTA_INTRCLR_V = 0x0000;
	
	memset(g_BrushCard->data_trk2,0x00,SIZE1_DATA_TRK2);
	memset(g_BrushCard->data_trk3,0x00,SIZE2_DATA_TRK3);
	memset(g_BrushCard->card_num2, 0x00, SIZE3_CARD_NUM2);
   memset(g_BrushCard->card_num3, 0x00, SIZE4_CARD_NUM3);
	g_BrushCard->track2_num = 0;
	g_BrushCard->track3_num = 0;
	g_BrushCard->result_value = 0;
	g_BrushCard->state = 0;

	enable_irq(INTSRC_EXTINT8);
	enable_irq(INTSRC_EXTINT5);

	return;
}

static irqreturn_t sep4020_mgcd_irqhandler3(int irq, void *dev_id, struct pt_regs *reg)
{      
   disable_irq(INTSRC_EXTINT5);
	*(volatile unsigned long*)GPIO_PORTA_INTRCLR_V |= 0x0020;				//中断清除PA5
   *(volatile unsigned long*)GPIO_PORTA_INTRCLR_V = 0x0000; 

	if(!( *(volatile unsigned long*)GPIO_PORTF_DATA_V & CP_LOW))
	{
		g_BrushCard->data_trk3[g_BrushCard->track3_num++] = (char)(*(volatile unsigned long*)GPIO_PORTF_DATA_V & TRACK3);
		
	}
 
   enable_irq(INTSRC_EXTINT5);
   

   if(TIMING1==0)
   {  g_BrushCard->MGCD_timer.expires = jiffies + KEY_TIMER_DELAY;
		add_timer(&g_BrushCard->MGCD_timer);
		TIMING1=1;
	}
	return IRQ_HANDLED;
}

static irqreturn_t sep4020_mgcd_irqhandler2(int irq, void *dev_id, struct pt_regs *reg)
{
   disable_irq(INTSRC_EXTINT8);
	*(volatile unsigned long*)GPIO_PORTA_INTRCLR_V |= 0x0100;
   *(volatile unsigned long*)GPIO_PORTA_INTRCLR_V = 0x0000;		//中断清除PA8   

	if(!( *(volatile unsigned long*)GPIO_PORTF_DATA_V & CP_LOW))
	{ 
		g_BrushCard->data_trk2[g_BrushCard->track2_num++] = (char)(*(volatile unsigned long*)GPIO_PORTF_DATA_V & TRACK2);
		
	}
   enable_irq(INTSRC_EXTINT8);

   g_BrushCard->MGCD_timer.expires = jiffies + KEY_TIMER_DELAY;
      
   if(TIMING2==0)
       {
		add_timer(&g_BrushCard->MGCD_timer);
	   TIMING2=1;
	}
	return IRQ_HANDLED;
}

static void MGCD_timer_handler(unsigned long arg)
{
	disable_irq(INTSRC_EXTINT8);
	disable_irq(INTSRC_EXTINT5);
	
	convert_track();
	up(&g_BrushCard->sem);

	enable_irq(INTSRC_EXTINT8);
	enable_irq(INTSRC_EXTINT5);
}


static ssize_t sep4020_mgcd_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{      
	
	down_interruptible(&g_BrushCard->sem);//使调用进程进入可以被信号打断的睡眠状态
	if(g_BrushCard->state == STATE) 
   	return -1;

	if (size > g_BrushCard->track2_num)
		 size = g_BrushCard->track2_num;
	
	return copy_to_user(buf, g_BrushCard->card_num2, size);

}

static ssize_t sep4020_mgcd_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
	return 0;
}

int sep4020_mgcd_open(struct inode *inode, struct file *filp)
{
 	if ( mg_openflag )
   	return -EBUSY;

  	mg_openflag = 1;
   TIMING1 = 0;
	TIMING2 = 0;

	sep4020_request_irqs();                        //注册中断函数

	sep4020_mgcd_setup();                          //对磁条卡进行初始化
       
   setup_timer(&g_BrushCard->MGCD_timer,MGCD_timer_handler,0); //添加定时器

	return 0;
}

static void sep4020_free_irqs(void)
{
	free_irq(INTSRC_EXTINT5,NULL);
   free_irq(INTSRC_EXTINT8,NULL);
}

int sep4020_mgcd_release(struct inode *inode, struct file *filp)
{
   mg_openflag = 0;
	sep4020_free_irqs();
	del_timer_sync(&g_BrushCard->MGCD_timer);
  	return 0;	
}

static struct file_operations sep4020_mgcd_fops = 
{
	.owner = THIS_MODULE,
	.read  = sep4020_mgcd_read,
	.write = sep4020_mgcd_write,
	.open  = sep4020_mgcd_open,
	.release = sep4020_mgcd_release,
};

static int sep4020_request_irqs(void)
{

	if(request_irq(INTSRC_EXTINT5,sep4020_mgcd_irqhandler3,SA_INTERRUPT,"sep4020mgcd",NULL))	
	{
		printk("request MGCD irq5 failed!\n");
		return -1;			
	}

	if(request_irq(INTSRC_EXTINT8,sep4020_mgcd_irqhandler2,SA_INTERRUPT,"sep4020mgcd",NULL))	
	{//free
		printk("request MGCD irq8 failed!\n");
		return -1;			
	}

	return 0;
}



static int __init sep4020_mgcd_init(void)
{
	int err,result;
	dev_t devno = MKDEV(MC_MAJOR, 0);
    
	
		result = register_chrdev_region(devno, 1, "sep4020_mgcd"); //创建设备节点
	
	
	if(result < 0)
		return result;
	
	g_BrushCard = kmalloc(sizeof(struct mgcd_dev),GFP_KERNEL); //动态申请设备结构体的内存

	if (!g_BrushCard)
	{
		result = -ENOMEM;
		goto fail_malloc;
  
	}
	memset(g_BrushCard,0,sizeof(struct mgcd_dev));

   printk("%s\n","memset");

	cdev_init(&(g_BrushCard->cdev), &sep4020_mgcd_fops);  //将fops注册到结构体中
	g_BrushCard->cdev.owner = THIS_MODULE;
	err = cdev_add(&g_BrushCard->cdev, devno, 1);
	if(err)
		{
		printk("adding err\r\n");
		unregister_chrdev_region(devno,1);
		kfree(g_BrushCard);
		sep4020_free_irqs();
		return err;
	}	
init_MUTEX_LOCKED(&g_BrushCard->sem);
	return 0;
	
	fail_malloc: unregister_chrdev_region(devno,1);
	return result;
}

static void __exit sep4020_mgcd_exit(void)
{
	cdev_del(&g_BrushCard->cdev);
	kfree(g_BrushCard);
	unregister_chrdev_region(MKDEV(MC_MAJOR, 0),1);
}

module_init(sep4020_mgcd_init);
module_exit(sep4020_mgcd_exit);

MODULE_AUTHOR("Wu WeiWei");
MODULE_LICENSE("GPL");


