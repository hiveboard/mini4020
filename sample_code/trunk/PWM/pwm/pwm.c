/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  pwm.c
 *
 *	File Description:   PWM文件
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0          2009.04.25           Jack           
 *   		      	
 ***************************************************************************************/
#include	<stdio.h>

#include	"sep4020.h"
#include	"pwm.h"
#include	"intc.h"
    
#define PWM_TEST 	  1    //PWM模式测试
#define TIMER_TEST	  0    //TIMER模式测试
#define GPIO_TEST     0    //高速GPIO模式测试
#define INTRPT_TEST   1    //对中断的测试

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       PWMInit                                             
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       PWM模块初始化
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong              
*                                                                       
* INPUTS                                                                
*                                                                       
*      无
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      无   
*                                                                       
***************************************************************/	

void PWMInit(void)
{
// 测试PWM输出，输出周期、占空比可调的方波

#if PWM_TEST

	PWMModeSet(0,0x00,0x0,0x00);       /*PWM0，输出低电平（未用），GPIO输出，PWM方式*/
	PWMSet(0,0x0FF,100,50);            /*PWM0，0xFF分频，周期100，占空比50％*/

#endif	


// 测试TIMER模式

#if TIMER_TEST
		
	PWMModeSet(0x0,0x03,0x0,0x03);    /*PWM0,计数结束翻转，GPIO输出，循环计数方式*/
	PWMTimerSet(0,0xFF,100);          /*PWM0，0xFF分频，周期100*/

#endif


// 测试高速GPIO模式

#if GPIO_TEST
		
	PWMModeSet(0x0,0x00,0x0,0x01);    /*PWM0,计数结束翻转（未用），GPIO输出，高速GPIO方式*/
	PWMGPIOSet(0,0xFF,0xaaaa);        /*PWM0，0xFF分频，输出数据0xaaaa*/
	
#endif


// 测试中断

#if INTRPT_TEST
	
	unmask_irq(INTSRC_PWM);           
  	irq_enable(INTSRC_PWM);         
	PWMIntEnable(0);                /*PWM0中断使能*/
	
#endif


    PWMEnable(0);                   /*使能PWM0*/

}


/*************************************************************
* FUNCTION                                                              
*                                                                       
*       PWMModeSet                                             
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       配置PWM模块
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong              
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 PwmNum PWM通道号
*
*		U32 MatchOut TIMER模式下，当计数值等于周期寄存器时输出状态
*			00：输出低电平
*			01：输出高电平
*			10：保持输出不变
*			11：翻转输出电平
*
*		U32 GpioDirection GPIO输入/输出选择
*			0：输出
*			1：输入
*			
*		U32 Function 工作模式
*			00：PWM模式
*			01：高速GPIO模式
*			10：TIMER模式，一次计数
*			11：TIMER模式，循环计数
*      
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      无   
*                                                                       
***************************************************************/	

void PWMModeSet(U32 PwmNum,U32 MatchOut,U32 GpioDirection,U32 Function)    
{
   
	U32 temp,offset;
	
	
	offset = 0x20*PwmNum;
	temp = (MatchOut<<4)|(GpioDirection<<3)|Function;
	write_reg((PWM0_CTRL+offset),temp);
	
	return ;

}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       PWMEnable                                             
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       使能PWM模块
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong              
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 PwmNum PWM通道号
*   
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      无   
*
***************************************************************/	                                                                

void PWMEnable(U32 PwmNum)
{
	
	U32 temp;
	
	temp = read_reg(PWM_ENABLE);
	temp |= 1<<PwmNum;
	write_reg(PWM_ENABLE,temp);     /*对应通道使能*/
	
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       PWMDisable                                             
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       不使能PWM模块
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong              
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 PwmNum PWM通道号
*   
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      无   
*
***************************************************************/	                                                                

void PWMDisable(U32 PwmNum)
{
	
	U32 temp;
	
	temp = read_reg(PWM_ENABLE);
	temp &= ~(1<<PwmNum);
	write_reg(PWM_ENABLE,temp);      /*对应通道不使能*/
	
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       PWMSet                                             
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       配置PWM模块，输出占空比可调的方波
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong              
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 PwmNum PWM通道号
*		U32 PwmDiv PWM分频因子
*		U32 PwmPeriod PWM周期
*		U32 PwmDuty PWM占空比
*   
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      无   
*
***************************************************************/	                                                                

void PWMSet(U32 PwmNum,U32 PwmDiv,U32 PwmPeriod,U32 PwmDuty)
{
	
	U32 temp,offset;
	
	offset = 0x20*PwmNum;
	write_reg((PWM0_DIV+offset),PwmDiv);        
	write_reg((PWM0_PERIOD+offset),PwmPeriod);
	temp = PwmPeriod*PwmDuty/100;
	write_reg((PWM0_DATA+offset),temp);
	
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       PWMTimerSet                                             
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       配置PWM Timer
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong              
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 PwmNum PWM通道号
*		U32 PwmDiv PWM分频因子
*		U32 PwmPeriod PWM周期
*   
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      无   
*
***************************************************************/	                                                                

void PWMTimerSet(U32 PwmNum,U32 TimerDiv,U32 TimerPeriod)
{
	
	U32 offset;
	
	offset = 0x20*PwmNum;
	write_reg((PWM0_DIV+offset),TimerDiv);            
	write_reg((PWM0_PERIOD+offset),TimerPeriod);
	
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       PWMGPIOSet                                             
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       配置PWM Timer
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong              
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 PwmNum PWM通道号
*		U32 PwmDiv PWM分频因子
*		U32 PwmPeriod PWM周期
*   
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      无   
*
***************************************************************/	                                                                

void PWMGPIOSet(U32 PwmNum,U32 PwmDiv,U32 PwmDuty)
{
	
	U32 offset;
	
	offset = 0x20*PwmNum;
	write_reg((PWM0_DIV+offset),PwmDiv);        
	write_reg((PWM0_DATA+offset),PwmDuty);
	
}
/*************************************************************
* FUNCTION                                                              
*                                                                       
*       PWMIntEnable                                             
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       使能各路PWM模块中断
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong              
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 PwmNum PWM通道号
*   
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      无   
*
***************************************************************/	                                                                

void PWMIntEnable(U32 PwmNum)
{
	
	U32 temp;
	
	temp = read_reg(PWM_INTMASK);
	temp &= ~(1<<PwmNum);
	write_reg(PWM_INTMASK,temp);       /*屏蔽对应通道*/
	
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       PWMIntDisable                                             
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       关闭各路PWM模块中断
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong              
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 PwmNum PWM通道号
*   
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      无   
*
***************************************************************/	                                                                

void PWMIntDisable(U32 PwmNum)
{
	
	U32 temp;
	
	temp = read_reg(PWM_INTMASK);
	temp |= 1<<PwmNum;
	write_reg(PWM_INTMASK,temp);        /*关闭对应中断*/
	
}


/*************************************************************
* FUNCTION                                                              
*                                                                       
*       PWMIntHandler                                             
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       PWM中断处理函数
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong              
*                                                                       
* INPUTS                                                                
*                                                                       
*      无
*   
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      无   
*
***************************************************************/	                                                                

void PWMIntHandler(void)
{
	int temp1,temp2,offset;
	int i;
	
	irq_disable(INTSRC_PWM);             /*关闭中断*/      
	
	temp1 = read_reg(PWM_INT);
	write_reg(PWM_INT,temp1);           //清除中断标志
	
	for(i=3;i>=0;i--)
	{
		offset = 0x20*i;
		temp2=read_reg(PWM0_STATUS+offset);
		printf("PWM%d interrupted,the STATUS is %x \n",i,temp2);	
		
	}
	
	irq_enable(INTSRC_PWM);               /*使能中断*/
 
}