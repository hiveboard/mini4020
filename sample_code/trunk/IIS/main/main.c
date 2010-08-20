/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  main.c
 *
 *	File Description:   IIS音频播放的主函数
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *     1.0           2009.04.25           lbn            
 *   		      	
 **************************************************************************************/ 
#include	<stdio.h>

#include	"iis.h"
#include	"intc.h"
#include	"sep4020.h"


int main(void)
{ 

    mask_irq(INTSRC_DMAC);       //屏蔽DMA控制器的IRQ中断

    InitPWM();                   //初始化PWM3:设置PWM3分频、占空比和使能PWM3
	
    InitDatapath();              //初始化数据流控制信号:使用GPIO_PORTG11端口控制nCSC端口
	
    InitCodec();                 //初始化解码芯片:设置L3总线接口信号以及L3总线的地址与数据传输模式

    InitDMA();                   //初始化DMA控制器:设置DMA的源目的地址以及控制与配置寄存器
    
    InitIIS();                   //初始化IIS控制器:设置IIS控制器及中断寄存器
   
    irq_enable(INTSRC_DMAC);     //使能DMA控制器的IRQ中断
    
    unmask_irq(INTSRC_DMAC);     //打开屏蔽的DMA控制器的IRQ中断
	 
    while(1);
    
    return 0;		   

}




