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

#define THERMAL_MAJOR 255   											//����������ӡ�����豸��Ϊ255
#define MOTOR_TIME		1850												//�����������

#define BLACK_AREA_BEGIN	3												//����ӵ����к�����Ϊ������������ֵ������ʵ������޸ģ�
#define BLACK_AREA_END	7													//���嵽�����к���ֹΪ������������ֵ������ʵ������޸ģ�

#define TIME 3500

PRT_SPACE t_prt;  
PRT_SPACE *t_prt_dev = &t_prt;
//char font_buf[17280];

//GPIO�ڵ�SEL��DIR��DATA�Ĵ�������
static const unsigned long Port_Sel_Regs[8]={GPIO_PORTA_SEL_V,GPIO_PORTB_SEL_V, GPIO_PORTC_SEL_V,GPIO_PORTD_SEL_V,GPIO_PORTE_SEL_V,GPIO_PORTF_SEL_V, GPIO_PORTG_SEL_V,GPIO_PORTH_SEL_V};
static const unsigned long Port_Dir_Regs[8]={GPIO_PORTA_DIR_V,GPIO_PORTB_DIR_V, GPIO_PORTC_DIR_V,GPIO_PORTD_DIR_V,GPIO_PORTE_DIR_V,GPIO_PORTF_DIR_V, GPIO_PORTG_DIR_V,GPIO_PORTH_DIR_V};
static const unsigned long Port_Data_Regs[8]={GPIO_PORTA_DATA_V,GPIO_PORTB_DATA_V, GPIO_PORTC_DATA_V,GPIO_PORTD_DATA_V,GPIO_PORTE_DATA_V,GPIO_PORTF_DATA_V, GPIO_PORTG_DATA_V,GPIO_PORTH_DATA_V};

#define REG32(addr)	(*(volatile unsigned int *)(addr))

//������Ӧ��GPIO��ͨ�üĴ���
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

//�ö˿ڸߵ�ƽ
static void set_port_high(unsigned short prt_port)
{
    unsigned short port = prt_port;
    unsigned long port_grp, port_index;

    port_grp =(unsigned long) ((port & PORT_GRP_MASK) >> PORT_BITS_NUM);
    port_index =(unsigned long) (port & PORT_BIT_MASK);
    REG32(Port_Data_Regs[port_grp] ) |= (1 << port_index);
}

//�ö˿ڵ͵�ƽ
static void set_port_low(unsigned short prt_port)
{
    unsigned short port = prt_port;
    unsigned long port_grp, port_index;

    port_grp =(unsigned long) ((port & PORT_GRP_MASK) >> PORT_BITS_NUM);
    port_index =(unsigned long) (port & PORT_BIT_MASK);
    REG32(Port_Data_Regs[port_grp] ) &= (~(1 << port_index));
}

//���˿ڵ�ƽֵ
static int read_port_level(unsigned short prt_port)
{
    unsigned short port = prt_port;
    unsigned long port_grp, port_index, level;

    port_grp =(unsigned long) ((port & PORT_GRP_MASK) >> PORT_BITS_NUM);
    port_index =(unsigned long) (port & PORT_BIT_MASK);
    level = ((REG32(Port_Data_Regs[port_grp]) & (1 << port_index)) >> port_index);
    return level;
}

//���ö�������ʱֵ
static void set_timer_val(int timer, unsigned long val)			//�ö�ʱ����ֵ��1Ϊ���������ʱ��2Ϊ����Դ��ʱ
{
    unsigned long timer_val;
    timer_val = val * TIMER_Uint;		//ϵͳʱ��
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

//ʹ�ܶ�ʱ��
static void enable_timer(unsigned long timer)							//ʹ����Ӧ��ʱ����1Ϊ���������ʱ��2Ϊ����Դ��ʱ
{
    switch (timer)
    {
    case 1:
        REG32(TIMER_T4CR_V) |= 0x07;		//ͨ��4���ƼĴ���
        break;
    case 2:
        REG32(TIMER_T3CR_V) |= 0x07;		//ͨ��3���ƼĴ���
        break;
    default:
        break;
    }
}

//�رն�ʱ��
static void disable_timer(unsigned long timer)		////�ر���Ӧ��ʱ����1Ϊ���������ʱ��2Ϊ����Դ��ʱ
{	
    switch (timer)
        {
    case 1:
        REG32(TIMER_T4CR_V) = 0x06;		//ͨ��4���ƼĴ���
        break;
    case 2:
        REG32(TIMER_T3CR_V) = 0x06;		//ͨ��3���ƼĴ���
        break;
    default:
        break;
    }
}

//Ӳ����ʱ���Ĵ�����//spinlock
static irqreturn_t timer_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
    unsigned long int_stat;
    int_stat = REG32(TIMER_TIMSR_V);
    if (int_stat & (1 << MOTOR_TIMER_INDEX))	        //���������ʱ���ж�
    {	
        REG32(TIMER_T4ISCR_V);		                    //ͨ��4�ж�״̬�Ĵ���
        motor_timer_isr();
    }
    if (int_stat & (1 << TEM_TIMER_INDEX))						//���ȿ�������ʱ���ж�
    {	
        REG32(TIMER_T3ISCR_V);												//ͨ��3�ж�״̬�Ĵ���
        tem_timer_isr();
    }
    return IRQ_HANDLED;
}

//����������ӡ����Ӳ����ʱ��
static void config_print_timer(void)
{
    REG32(TIMER_T4CR_V) = 0x2;              //ͨ��4���ƼĴ���
    REG32(TIMER_T3CR_V) = 0x2;              //ͨ��3���ƼĴ���
}

//��ʼ����ӡ����GPIO��
void init_thermalport(void)
{
    prt_port_config(CR_M_PA,    0);     		//��ʼ������������ƿ�A
    prt_port_config(CR_M_PB,    0);     		//��ʼ������������ƿ�B
    prt_port_config(TC1,        0);					//��ʼ������Դ
    prt_port_config(PR_PWR,     0);					//��ʼ�����������Դ
    prt_port_config(PR_LAT,     0);     		//�������治ʹ��
    prt_port_config(PR_DST1,    1);     		//��ʼ�����ȿ�1
    prt_port_config(PR_DST2,    1);					//��ʼ�����ȿ�2
    prt_port_config(DATA_CS,    0);					//��ʼ�������ź�
    prt_port_config(CLK_CS,     0);					//��ʼ��ʱ���ź�
    prt_port_config(PR_D0,      0);					//��ʼ�����ݿڣ�D0��
    prt_port_config(PR_D1,      0);					//��ʼ�����ݿڣ�D1��
    prt_port_config(PR_D2,      0);					//��ʼ�����ݿڣ�D2��
    prt_port_config(PR_D3,      0);					//��ʼ�����ݿڣ�D3��
    prt_port_config(PR_D4,      0);					//��ʼ�����ݿڣ�D4��
    prt_port_config(PR_D5,      0);					//��ʼ�����ݿڣ�D5��
    prt_port_config(PR_D6,      0);					//��ʼ�����ݿڣ�D6��
    prt_port_config(PR_D7,      0);					//��ʼ�����ݿڣ�D7��
    prt_port_config(NO_PAGE_LED,0);					//��ʼ��ȱֽ�źŵ�
    prt_port_config(BL_CHK_IN,  0);					//��ʼ���ڱ�����
    prt_port_config(BLK_SEN,    0);					//��ʼ��ȱֽ�ź�
    prt_port_config(TE_MPO,     0);					//��ʼ�������ź�
}

//��ʼ������ӡ������
void init_thermaldata(void)
{
    int i;
    for(i=0; i<MAX_ROW_FONT24x24*PRT_FONT24X24; i++)		//24*20
    {
        t_prt.point_row[i][0] = 0;	
        t_prt.point_row[i][1] = 0;
    }
    t_prt.momtor_step = 0;							//��ֽ�����ȫ�ּ�������0		
    t_prt.tem_step = 0;									//���ȵ����ȫ�ּ�������0
    t_prt.total_rownum = 0;							//��ӡ����������0
    t_prt.momtor_speed = 3000;   				//��ֽ�ٶ�
    t_prt.row_interval = 18;						//ÿ�к���֮��ļ��
    t_prt.word_interval = 1;						//ÿ������֮��ļ��
    t_prt.black_check = NO_CHECK_BM;		//�ڱ����־
    t_prt.status = INITTION;						//��ʼ��״̬
}

//��ʼ����ӡ��
void init_thermal(void)
{
    init_thermalport();									//��ʼ��������ӡ��GPIO��
    init_thermaldata();									//��ʼ��������ӡ�Ľṹ��
    config_print_timer();								//����������ӡ����Ӳ����ʱ��
}

//���Ʋ��������ת��
void moto_move(int steps, int dir)			//����DIR��ʾ����0������,1������,������ʾ���루steps=0.025cm��
{
    int i=0;
    set_port_high(PR_PWR);							//ʹ�ܵ��
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
    set_port_low(PR_PWR);									//�رյ��
}

//���ӡ���Ļ�����������1�ֽڵ�����
void send_byte(unsigned char bytedata)
{
	*(RP)DATA_PORT_SEL  |= 0x00ff;      //GPIO����Ϊͨ��ģʽ	
 	*(RP)DATA_PORT_DATA = bytedata;     //GPIO���������ӡ����
 	*(RP)DATA_PORT_DIR  &= ~0x00ff;    	//GPIO���������
	set_port_high(DATA_CS);							//������GPIO��ģ���ӡ��ʱ��
	set_port_low(CLK_CS);
	set_port_low(DATA_CS);
	set_port_high(CLK_CS);
	set_port_low(CLK_CS);
	set_port_high(DATA_CS);
}

//�ڱ��⣬ȷ����ӡ����ʼλ��
int bl_check(void)
{
    int i;    
    int f_sta = 0;													//��¼�ڱ����ǰһ״̬
    int c_sta = 0;													//��¼�ڱ���ĵ�ǰ״̬
    set_port_low(NO_PAGE_LED);              //�ر�ȱֽ�źŵ�
    c_sta = read_port_level(BL_CHK_IN);     //��¼��ǰ�ڱ��ź�
    for(i=0;i<400;i++)	                    //�������ڵĺڱ����Ϊ�������400�������������תѰ�Һڱ�
    { 
				moto_move(0,1);
    		c_sta = read_port_level(BL_CHK_IN);
    		if((f_sta==1)&&(c_sta==0))          //�ҵ��ڱ���ǰ�˳�ѭ��
						return 1;		
    		else
        		f_sta=c_sta;
    }
    
    if(i>=400)                              //δ��⵽�ڱ꣬��ֽ
    {	
        return PRintNOPAPER;                //����ȱֽ�ź�
    }
    
	return 1;
}

//���������ʱ���жϣ�ͨ��4
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

//����ʱ������
void tem_timer_isr(void)
{
    t_prt.tem_step ++;
    if(t_prt.tem_step % 2)
    {
        set_dst1_high;		//���ȿ�1��Ч
        set_dst2_low;		//���ȿ�2ʧЧ
        disable_timer(2);
        set_timer_val(2,TIME*2); 
        enable_timer(2);	//��ʱ3500
    }
    else
    {
        set_dst1_low;		//���ȿ�1ʧЧ
        set_dst2_low;		//���ȿ�2ʧЧ
        disable_timer(2);	//�رն�ʱ��ͨ��3
        complete(&t_prt_dev->complete_request); 								//���ѵȴ�����
    }		     
    return;
}

//����һ����ӡ�еĵ���
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
        if( i == row* 24) 	//��ӡ���
        {
            set_port_low(PR_PWR); 
            disable_timer(1);
            disable_timer(2);
            break;
        }
                
        if((i % 24)  ||  ( !i ))                				//һ�к���û�д�ӡ���
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

           // time=point_count(t_prt_dev->font_buf,i)*5+3500;								//����ʱ�䣨����ֵ��
           // if(i > 3*BLACK_AREA_BEGIN && i< 3*BLACK_AREA_END)													//������������ֵ��     
            disable_timer(1);
            disable_timer(2);
            set_timer_val(1,TIME);	
            enable_timer(1);
            set_timer_val(2,TIME*2);
            enable_timer(2);		//ʹ�ܶ�ʱ��

            wait_for_completion(&t_prt_dev->complete_request);				//�ȴ����Ӧ��
        }
        else
        {
          for(j=0; j<46;j++)        //��ֵ���ޱ�Ҫ��
                send_byte(0x00);
                
            set_lat_low;
            set_lat_high;
            udelay(2000 * t_prt.row_interval);                                                          
        }
    }
    set_port_low(TC1);
    return;
}


//��ӡ����ӡ����
int thermal_printer(unsigned int size)             
{
 //   if(bl_check()!=1)
 //   		return t_prt.status=PRintNOPAPER;
    t_prt.total_rownum = (size/864)+1;														//����ӡ�ĵ�����(24*24/8*12=864)
    if(t_prt.total_rownum == 0)
        return t_prt.status = PRintNOCONTENT;
    prt_font_24(t_prt.total_rownum);
    return t_prt.status = SUCCESS;			 
}

//�ļ������ṹ���еĴ򿪺���
int sep4020_thermal_open(struct inode *inode, struct file *filp)
{	
	spin_lock(&(t_prt_dev->my_lock));								//Ϊ��������
    init_thermal();
    filp->private_data = t_prt_dev;  //���豸�ṹ��ָ�븳ֵ���ļ�˽������
    return 0;
}

//�ļ������ṹ�������ļ�д����
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

//�ļ������ṹ���п��ƺ���
int sep4020_thermal_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
    return 0;
}

//��Ӧ�ó���ִ����ɺ�ִ��
static int sep4020_thermal_release(struct inode *inode, struct file *filp)
{
		spin_unlock(&(t_prt_dev->my_lock));					//Ϊ��������
		return 0;
}

//�ļ������ṹ��
static struct file_operations sep4020_thermal_fops = 
{
    .owner = THIS_MODULE,
    .read  = NULL,
    .write = sep4020_thermal_write,
    .ioctl = sep4020_thermal_ioctl,
    .open  = sep4020_thermal_open,
    .release = sep4020_thermal_release,
};

//��ʼ���豸ģ��
static int __init sep4020_thermal_init(void)
{
    int err,result;
    dev_t devno;

    devno=MKDEV(THERMAL_MAJOR,0);
    result = register_chrdev_region(devno, 1, "sep4020_thermal");		//��ϵͳ��̬�����豸��

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

    if(request_irq(INTSRC_TIMER2,timer_interrupt,0,"sep4020_thermal", t_prt_dev)) 	//ע���жϺ���
    {
    		kfree(t_prt_dev);
    		unregister_chrdev_region(devno,1);
    		return -EBUSY;
    }
    cdev_init(&(t_prt_dev->cdev),&(sep4020_thermal_fops));
    t_prt_dev->cdev.owner = THIS_MODULE;
    err = cdev_add(&t_prt_dev->cdev, devno, 1);		//��ϵͳע����豸
    if(err)
    {
    		unregister_chrdev_region(devno,1);
    		kfree(t_prt_dev);
        printk("adding thermal device err!!\r\n");	
				return err;
		}
    return 0;
}

//�Ƴ��豸ģ��
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
