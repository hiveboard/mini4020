/* linux/drivers/char/sep4020_char/thermal_printer.c
*
*   Copyright (c) 2009 prochip company
*   http://www.prochip.com.cn
*	
*
*   sep4020 thermal_printer.
*
*   Changelog:
*   22-Jan-2009     fpmystar    Initial version
*   12-May-2009     mecxon      Current Version   	
*   24-May-2009     mecxon      Codereview Version   		
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
#include <linux/spinlock.h>

#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/hardware.h>

#include "thermal_printer.h"

#define THERMAL_MAJOR 255   											//定义热敏打印机主设备号为255
#define MOTOR_TIME		1850												//步进电机脉宽

#define BLACK_AREA_BEGIN	3												//定义从第三行汉字起为密码区（经验值，根据实际情况修改）
#define BLACK_AREA_END	7													//定义到第七行汉字止为密码区（经验值，根据实际情况修改）

#define TIME 3500

PRT_SPACE t_prt;  
PRT_SPACE *t_prt_dev = &t_prt;
//char font_buf[17280];

//GPIO口的SEL、DIR、DATA寄存器定义
static const unsigned long Port_Sel_Regs[8]={GPIO_PORTA_SEL_V,GPIO_PORTB_SEL_V, GPIO_PORTC_SEL_V,GPIO_PORTD_SEL_V,GPIO_PORTE_SEL_V,GPIO_PORTF_SEL_V, GPIO_PORTG_SEL_V,GPIO_PORTH_SEL_V};
static const unsigned long Port_Dir_Regs[8]={GPIO_PORTA_DIR_V,GPIO_PORTB_DIR_V, GPIO_PORTC_DIR_V,GPIO_PORTD_DIR_V,GPIO_PORTE_DIR_V,GPIO_PORTF_DIR_V, GPIO_PORTG_DIR_V,GPIO_PORTH_DIR_V};
static const unsigned long Port_Data_Regs[8]={GPIO_PORTA_DATA_V,GPIO_PORTB_DATA_V, GPIO_PORTC_DATA_V,GPIO_PORTD_DATA_V,GPIO_PORTE_DATA_V,GPIO_PORTF_DATA_V, GPIO_PORTG_DATA_V,GPIO_PORTH_DATA_V};

#define REG32(addr)	(*(volatile unsigned int *)(addr))

//配置相应的GPIO口通用寄存器
static void prt_port_config(unsigned short prt_port, unsigned char bit_data)
{
    unsigned short port = prt_port;
    unsigned long port_grp, port_index, port_dir;

    port_grp =(unsigned long) ((port & PORT_GRP_MASK) >> PORT_BITS_NUM);
    port_index =(unsigned long) (port & PORT_BIT_MASK);
    port_dir = (unsigned long) ((port & PORT_DIR_MASK) >> PORT_DIR_BIT_POS);
    REG32(Port_Sel_Regs[port_grp]) |= (1 << port_index);	
    if (port_dir == 0x01)                   /* Input Port, bit_data is useless */
        REG32(Port_Dir_Regs[port_grp]) |= (1 << port_index);
    else
    { 
        if (bit_data == 0x01) 
            REG32(Port_Data_Regs[port_grp] ) |= (1 << port_index);
        else
            REG32(Port_Data_Regs[port_grp] ) &= (~(1 << port_index));
        REG32(Port_Dir_Regs[port_grp]) &= (~(1 << port_index));
    }
}

//置端口高电平
static void set_port_high(unsigned short prt_port)
{
    unsigned short port = prt_port;
    unsigned long port_grp, port_index;

    port_grp =(unsigned long) ((port & PORT_GRP_MASK) >> PORT_BITS_NUM);
    port_index =(unsigned long) (port & PORT_BIT_MASK);
    REG32(Port_Data_Regs[port_grp] ) |= (1 << port_index);
}

//置端口低电平
static void set_port_low(unsigned short prt_port)
{
    unsigned short port = prt_port;
    unsigned long port_grp, port_index;

    port_grp =(unsigned long) ((port & PORT_GRP_MASK) >> PORT_BITS_NUM);
    port_index =(unsigned long) (port & PORT_BIT_MASK);
    REG32(Port_Data_Regs[port_grp] ) &= (~(1 << port_index));
}

//读端口电平值
static int read_port_level(unsigned short prt_port)
{
    unsigned short port = prt_port;
    unsigned long port_grp, port_index, level;

    port_grp =(unsigned long) ((port & PORT_GRP_MASK) >> PORT_BITS_NUM);
    port_index =(unsigned long) (port & PORT_BIT_MASK);
    level = ((REG32(Port_Data_Regs[port_grp]) & (1 << port_index)) >> port_index);
    return level;
}

//设置定义器定时值
static void set_timer_val(int timer, unsigned long val)			//置定时器初值，1为步进电机定时，2为加热源定时
{
    unsigned long timer_val;
    timer_val = val * TIMER_Uint;		//系统时钟
    switch (timer)
    {
        case 1:
            REG32(TIMER_T4CR_V) &= ~0x06;
            REG32(TIMER_T4LCR_V) = timer_val;
            break;
        case 2:
            REG32(TIMER_T3CR_V) &= ~0x06;
            REG32(TIMER_T3LCR_V) = timer_val;
            break;
        default:
            break;
    }
}

//使能定时器
static void enable_timer(unsigned long timer)							//使能相应定时器，1为步进电机定时，2为加热源定时
{
    switch (timer)
    {
    case 1:
        REG32(TIMER_T4CR_V) |= 0x07;		//通道4控制寄存器
        break;
    case 2:
        REG32(TIMER_T3CR_V) |= 0x07;		//通道3控制寄存器
        break;
    default:
        break;
    }
}

//关闭定时器
static void disable_timer(unsigned long timer)		////关闭相应定时器，1为步进电机定时，2为加热源定时
{	
    switch (timer)
        {
    case 1:
        REG32(TIMER_T4CR_V) = 0x06;		//通道4控制寄存器
        break;
    case 2:
        REG32(TIMER_T3CR_V) = 0x06;		//通道3控制寄存器
        break;
    default:
        break;
    }
}

//硬件定时器的处理函数//spinlock
static irqreturn_t timer_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
    unsigned long int_stat;
    int_stat = REG32(TIMER_TIMSR_V);
    if (int_stat & (1 << MOTOR_TIMER_INDEX))	        //步进电机定时器中断
    {	
        REG32(TIMER_T4ISCR_V);		                    //通道4中断状态寄存器
        motor_timer_isr();
    }
    if (int_stat & (1 << TEM_TIMER_INDEX))						//加热控制器定时器中断
    {	
        REG32(TIMER_T3ISCR_V);												//通道3中断状态寄存器
        tem_timer_isr();
    }
    return IRQ_HANDLED;
}

//设置热敏打印机的硬件定时器
static void config_print_timer(void)
{
    REG32(TIMER_T4CR_V) = 0x2;              //通道4控制寄存器
    REG32(TIMER_T3CR_V) = 0x2;              //通道3控制寄存器
}

//初始化打印机的GPIO口
void init_thermalport(void)
{
    prt_port_config(CR_M_PA,    0);     		//初始化步进电机控制口A
    prt_port_config(CR_M_PB,    0);     		//初始化步进电机控制口B
    prt_port_config(TC1,        0);					//初始化加热源
    prt_port_config(PR_PWR,     0);					//初始化步进电机电源
    prt_port_config(PR_LAT,     0);     		//数据锁存不使能
    prt_port_config(PR_DST1,    1);     		//初始化加热块1
    prt_port_config(PR_DST2,    1);					//初始化加热块2
    prt_port_config(DATA_CS,    0);					//初始化数据信号
    prt_port_config(CLK_CS,     0);					//初始化时针信号
    prt_port_config(PR_D0,      0);					//初始化数据口（D0）
    prt_port_config(PR_D1,      0);					//初始化数据口（D1）
    prt_port_config(PR_D2,      0);					//初始化数据口（D2）
    prt_port_config(PR_D3,      0);					//初始化数据口（D3）
    prt_port_config(PR_D4,      0);					//初始化数据口（D4）
    prt_port_config(PR_D5,      0);					//初始化数据口（D5）
    prt_port_config(PR_D6,      0);					//初始化数据口（D6）
    prt_port_config(PR_D7,      0);					//初始化数据口（D7）
    prt_port_config(NO_PAGE_LED,0);					//初始化缺纸信号灯
    prt_port_config(BL_CHK_IN,  0);					//初始化黑标输入
    prt_port_config(BLK_SEN,    0);					//初始化缺纸信号
    prt_port_config(TE_MPO,     0);					//初始化过热信号
}

//初始化待打印的数据
void init_thermaldata(void)
{
    int i;
    for(i=0; i<MAX_ROW_FONT24x24*PRT_FONT24X24; i++)		//24*20
    {
        t_prt.point_row[i][0] = 0;	
        t_prt.point_row[i][1] = 0;
    }
    t_prt.momtor_step = 0;							//进纸电机的全局计数器清0		
    t_prt.tem_step = 0;									//加热电机的全局计数器清0
    t_prt.total_rownum = 0;							//打印的总行数清0
    t_prt.momtor_speed = 3000;   				//走纸速度
    t_prt.row_interval = 18;						//每行汉字之间的间隔
    t_prt.word_interval = 1;						//每个汉字之间的间隔
    t_prt.black_check = NO_CHECK_BM;		//黑标检测标志
    t_prt.status = INITTION;						//初始化状态
}

//初始化打印机
void init_thermal(void)
{
    init_thermalport();									//初始化热敏打印的GPIO口
    init_thermaldata();									//初始化热敏打印的结构体
    config_print_timer();								//设置热敏打印机的硬件定时器
}

//控制步进电机的转动
void moto_move(int steps, int dir)			//参数DIR表示方向（0：正向,1：反向）,参数表示距离（steps=0.025cm）
{
    int i=0;
    set_port_high(PR_PWR);							//使能电机
    if(dir == 0)
    {		
        for(i=0; i<steps; i++)
        {		                          
        set_port_low(CR_M_PA); 
        set_port_low(CR_M_PB);
        udelay(MOTOR_TIME);
        set_port_high(CR_M_PA);
        set_port_low(CR_M_PB);
        udelay(MOTOR_TIME);
        set_port_high(CR_M_PA);
        set_port_high(CR_M_PB);
        udelay(MOTOR_TIME);
        set_port_low(CR_M_PA);
        set_port_high(CR_M_PB);
        udelay(MOTOR_TIME);
        } 
	}
	else
	{
		for(i=0; i<steps; i++)
		{		                         
        set_port_high(CR_M_PA); 
        set_port_low(CR_M_PB);
        udelay(MOTOR_TIME);
        set_port_low(CR_M_PA);
        set_port_low(CR_M_PB);
        udelay(MOTOR_TIME);
        set_port_low(CR_M_PA);
        set_port_high(CR_M_PB);
        udelay(MOTOR_TIME);
        set_port_high(CR_M_PA);
        set_port_high(CR_M_PB);
        udelay(MOTOR_TIME);
        }
    }
    set_port_low(PR_PWR);									//关闭电机
}

//向打印机的缓冲区中送入1字节的数据
void send_byte(unsigned char bytedata)
{
	*(RP)DATA_PORT_SEL  |= 0x00ff;      //GPIO口置为通用模式	
 	*(RP)DATA_PORT_DATA = bytedata;     //GPIO口送入待打印数据
 	*(RP)DATA_PORT_DIR  &= ~0x00ff;    	//GPIO口用作输出
	set_port_high(DATA_CS);							//以下用GPIO口模拟打印机时序
	set_port_low(CLK_CS);
	set_port_low(DATA_CS);
	set_port_high(CLK_CS);
	set_port_low(CLK_CS);
	set_port_high(DATA_CS);
}

//黑标检测，确定打印的起始位置
int bl_check(void)
{
    int i;    
    int f_sta = 0;													//记录黑标检测的前一状态
    int c_sta = 0;													//记录黑标检测的当前状态
    set_port_low(NO_PAGE_LED);              //关闭缺纸信号灯
    c_sta = read_port_level(BL_CHK_IN);     //记录当前黑标信号
    for(i=0;i<400;i++)	                    //定义相邻的黑标距离为步进电机400步、步进电机正转寻找黑标
    { 
				moto_move(0,1);
    		c_sta = read_port_level(BL_CHK_IN);
    		if((f_sta==1)&&(c_sta==0))          //找到黑标提前退出循环
						return 1;		
    		else
        		f_sta=c_sta;
    }
    
    if(i>=400)                              //未检测到黑标，无纸
    {	
        return PRintNOPAPER;                //返回缺纸信号
    }
    
	return 1;
}

//步进电机定时器中断，通道4
void motor_timer_isr(void)
{
	set_port_high(PR_PWR);
	switch(t_prt.momtor_step % 4)
	{
		case 0:
			set_port_high(CR_M_PA);		
			set_port_low(CR_M_PB);
			break;
		case 1:
			set_port_high(CR_M_PA);  
			set_port_high(CR_M_PB);
			break;
		case 2:
			set_port_low(CR_M_PA);		
			set_port_high(CR_M_PB);
			break;
		case 3:
			set_port_low(CR_M_PA);
		   	set_port_low(CR_M_PB);
		   	break;
	}
	t_prt.momtor_step ++;
    disable_timer(1);
		set_timer_val(1,TIME);//2200
		enable_timer(1);
		return;	
}

//加热时序处理函数
void tem_timer_isr(void)
{
    t_prt.tem_step ++;
    if(t_prt.tem_step % 2)
    {
        set_dst1_high;		//加热块1有效
        set_dst2_low;		//加热块2失效
        disable_timer(2);
        set_timer_val(2,TIME*2); 
        enable_timer(2);	//定时3500
    }
    else
    {
        set_dst1_low;		//加热块1失效
        set_dst2_low;		//加热块2失效
        disable_timer(2);	//关闭定时器通道3
        complete(&t_prt_dev->complete_request); 								//唤醒等待进程
    }		     
    return;
}

//计算一个打印行的点数
int point_count(char *buf,int t)
{
		int i,j,k;
		int ln=0;
		int mask;
		for(i=((t%24)*3+(t/24)*864);i<(t/24+1)*864;i=i+72)
		{
			for(j=i;j<i+3;j++)
			{
					mask = 0x80;
					for(k=0;k<8;k++)
					{
							if(t_prt_dev->font_buf[j]&&mask)
							ln++;
							mask>>= 1;
					}
			}
		}
return ln;
}

void prt_font_24(int row)
{
    int i,j,k,m;
    set_port_high(TC1);
    udelay(1000);
    init_completion(&t_prt_dev->complete_request);
//
    for(i = 0; i <= row * 24; i++)
    {
        if( i == row* 24) 	//打印完成
        {
            set_port_low(PR_PWR); 
            disable_timer(1);
            disable_timer(2);
            break;
        }
                
        if((i % 24)  ||  ( !i ))                				//一行汉字没有打印完成
        {
          	k=i%24*3+i/24*72*MAX_NUM_FONT24x24;
            for(j = 0;  j < MAX_NUM_FONT24x24; j++)
            {
            	m=j*72;
                send_byte(t_prt_dev->font_buf[k + m]);
                send_byte(t_prt_dev->font_buf[k + m + 1]);
                send_byte(t_prt_dev->font_buf[k + m + 2]);
                send_byte(0x00);
            }
            set_lat_low;	
            set_lat_high;
            set_dst1_low;
            set_dst2_high;

           // time=point_count(t_prt_dev->font_buf,i)*5+3500;								//加热时间（经验值）
           // if(i > 3*BLACK_AREA_BEGIN && i< 3*BLACK_AREA_END)													//密码区（经验值）     
            disable_timer(1);
            disable_timer(2);
            set_timer_val(1,TIME);	
            enable_timer(1);
            set_timer_val(2,TIME*2);
            enable_timer(2);		//使能定时器

            wait_for_completion(&t_prt_dev->complete_request);				//等待完成应答
        }
        else
        {
          for(j=0; j<46;j++)        //赋值有无必要？
                send_byte(0x00);
                
            set_lat_low;
            set_lat_high;
            udelay(2000 * t_prt.row_interval);                                                          
        }
    }
    set_port_low(TC1);
    return;
}


//打印机打印函数
int thermal_printer(unsigned int size)             
{
 //   if(bl_check()!=1)
 //   		return t_prt.status=PRintNOPAPER;
    t_prt.total_rownum = (size/864)+1;														//待打印的点行数(24*24/8*12=864)
    if(t_prt.total_rownum == 0)
        return t_prt.status = PRintNOCONTENT;
    prt_font_24(t_prt.total_rownum);
    return t_prt.status = SUCCESS;			 
}

//文件操作结构体中的打开函数
int sep4020_thermal_open(struct inode *inode, struct file *filp)
{	
	spin_lock(&(t_prt_dev->my_lock));								//为驱动加锁
    init_thermal();
    filp->private_data = t_prt_dev;  //将设备结构体指针赋值给文件私有数据
    return 0;
}

//文件操作结构体中向文件写函数
static ssize_t sep4020_thermal_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
    init_thermal();   
     memset(t_prt_dev->font_buf,0, 17280);
    if(copy_from_user(t_prt_dev->font_buf,buf,size))
    {
        printk("these words can't print!!!\n");
        return -ENOTTY;
    }
printk("%d\n",size);
    thermal_printer(size); 
    return 0;
}

//文件操作结构体中控制函数
int sep4020_thermal_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    return 0;
}

//在应用程序执行完成后执行
static int sep4020_thermal_release(struct inode *inode, struct file *filp)
{
		spin_unlock(&(t_prt_dev->my_lock));					//为驱动解锁
		return 0;
}

//文件操作结构体
static struct file_operations sep4020_thermal_fops = 
{
    .owner = THIS_MODULE,
    .read  = NULL,
    .write = sep4020_thermal_write,
    .ioctl = sep4020_thermal_ioctl,
    .open  = sep4020_thermal_open,
    .release = sep4020_thermal_release,
};

//初始化设备模块
static int __init sep4020_thermal_init(void)
{
    int err,result;
    dev_t devno;

    devno=MKDEV(THERMAL_MAJOR,0);
    result = register_chrdev_region(devno, 1, "sep4020_thermal");		//向系统静态申请设备号

    if(result < 0)
        return result;

    t_prt_dev = kmalloc(sizeof(struct printer_space),GFP_KERNEL);
    if (t_prt_dev==NULL)
    {
        result = -ENOMEM;
        unregister_chrdev_region(devno, 1);
        return result;
    }
    memset(t_prt_dev,0,sizeof(struct printer_space));

    if(request_irq(INTSRC_TIMER2,timer_interrupt,0,"sep4020_thermal", t_prt_dev)) 	//注册中断函数
    {
    		kfree(t_prt_dev);
    		unregister_chrdev_region(devno,1);
    		return -EBUSY;
    }
    cdev_init(&(t_prt_dev->cdev),&(sep4020_thermal_fops));
    t_prt_dev->cdev.owner = THIS_MODULE;
    err = cdev_add(&t_prt_dev->cdev, devno, 1);		//向系统注册该设备
    if(err)
    {
    		unregister_chrdev_region(devno,1);
    		kfree(t_prt_dev);
        printk("adding thermal device err!!\r\n");	
				return err;
		}
    return 0;
}

//移除设备模块
static void __exit sep4020_thermal_exit(void)
{
    cdev_del(&t_prt_dev->cdev);
    kfree(t_prt_dev);
    unregister_chrdev_region(MKDEV(THERMAL_MAJOR, 0),1);
}

module_init(sep4020_thermal_init);
module_exit(sep4020_thermal_exit);

MODULE_AUTHOR("fpmystar@gmail.com");
MODULE_LICENSE("GPL");
