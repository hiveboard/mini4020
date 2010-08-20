/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  timer.c
 *
 *	File Description:   Timer函数
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0          2009.04.25           PROCHIP            
 *   		      	
 ***************************************************************************************/
#include	<stdio.h>
//#include	<string.h>

#include	"sep4020.h"
#include	"intc.h"
#include    "timer.h"


#define TIMER_RESTART 1   //重启计数模式
#define TIMER_FREEDOM 0   //自由计数模式
#define TIMER_ONESHOT 0   //ONE-SHOT计数模式
#define TIMER_CAPTURE 0   //外部捕获(注：通道3~6支持capture)

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       InitTimer                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       初始化Timer模块
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong              
*                                                                       
* INPUTS                                                                
*                                                                       
*      	无       
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      无   
*                                                                       
***************************************************************/	

void InitTmier(void)    
{
   mask_irq(INTSRC_TIMER2); 

// 设置timer1通道为重启计数模式

  #if TIMER_RESTART
    
   *(RP)TIMER_T3LCR = 508;    //根据产生中断的时间间隔设置加载的计数值
   *(RP)TIMER_T3CR = 0x6;     //设置通道1工作于重启计数模式
   *(RP)TIMER_T3CR |= 0x1;
   
  #endif
  
  
// 设置timer1通道为自由计数模式
 
  #if TIMER_FREEDOM

   *(RP)TIMER_T1LCR = 508;     //根据产生中断的时间间隔设置加载的计数值
   *(RP)TIMER_T1CR = 0x02;     //设置通道1工作于自由计数模式
   *(RP)TIMER_T1CR |= 0x1;
   
  #endif


// 设置timer1通道为ONE-SHOT计数模式

  #if TIMER_ONESHOT
    
   *(RP)TIMER_T1LCR = 508;    //根据产生中断的时间间隔设置加载的计数值
   *(RP)TIMER_T1CR = 0xa;     //设置通道1工作于ONE-SHOT计数模式
   *(RP)TIMER_T1CR |= 0x1;
   
  #endif


// 设置timer3通道为捕获模式

  #if TIMER_CAPTURE
    
   *(RP)TIMER_T3LCR = 0x0;    //根据产生中断的时间间隔设置加载的计数值
   *(RP)TIMER_T3CR = 0x42;     //设置通道3工作于自由计数模式
   *(RP)TIMER_T3CR |= 0x1;
   
  #endif

   irq_enable(INTSRC_TIMER2);         /*中断使能*/
   unmask_irq(INTSRC_TIMER2);         /*打开屏蔽中断*/

}



/*************************************************************
* FUNCTION                                                              
*                                                                       
*       Timer_IRQ_Service1                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	TIMER中断处理函数
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	无
*			
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	无                
*                                                                       
***************************************************************/

void Timer_IRQ_Service1(void)
{
	volatile U32 clear_int;
    clear_int = *(RP)TIMER_T1ISCR; //读通道1中断状态清除寄存器清除中断
    *(RP)TIMER_T1ISCR=clear_int;
    printf("Timer1 restar mode test OK\n");
}


void Timer_IRQ_Service2(void)
{
    volatile U32 clear_int;
    clear_int = *(RP)TIMER_T3ISCR; //读通道1中断状态清除寄存器清除中断
    *(RP)TIMER_T3ISCR=clear_int;
    printf("In the catpure mode, the current value of count register is %ld \n",*(RP)TIMER_T3CAPR);
}


