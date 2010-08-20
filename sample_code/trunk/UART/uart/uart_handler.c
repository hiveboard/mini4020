/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  uart_handler.c
 *
 *	File Description: uart 中断处理
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0               2009.04.25       PROCHIP            
 *   		      	
 **************************************************************************************/
 
#include "sep4020.h"
#include "intc.h"
#include "uart.h"

//********************UART0中断处理函数******************//

void Uart0IntHandler(void)
{
    S32 a = *(RP)UART0_IIR ;
    
    switch ((a &0X0E) >> 1) //判断bit【3：1】中断源标号
    {
        case 3:
            RlsIsrUart0(); //接收数据状态中断
            break;
        case 2:
            RdaIsrUart0(); //接收FIFO中的数据达到触发级中断
            break;
        case 6:
            TmoIsrUart0(); //超时中断
            break;
        case 1:
            ThrIsrUart0(); //传输FIFO中断
            break;
        case 0:
            MdsIsrUart0(); //状态发生变化中断
            break;
        default:
            break ;
    }
    
}


U32 RlsIsrUart0(void) //用于返回接收数据状态中断的硬件错误
{
    return *(RP)UART0_LSR ;  // 行状态寄存器
}

void RdaIsrUart0(void) //用来实现读取接收FIFO中的数据，并把接收的数据打印出来
{
    U32 i;
    U8 a[9];
    
    for(i=0; i<8; i++)
    {
        a[i] = (U8)*(RP)(UART0_RXFIFO);  //读取接收FIFO里的数据
        a[8] = '\0';
        Prints((S8 *)a,0x10005000);
        return;
    }

}


void TmoIsrUart0(void) //用来实现超时中断
{
    U32 i;
    volatile U32 a;
    
    for(i=0; i<8; i++)
    {
        a = *(RP)UART0_RXFIFO ;
    }
    
    Prints("timer out now!!\n",0x10005000);
       
    return;
}



void ThrIsrUart0(void) //用来把要发送的数据放入传输FIFO
{
    U32 i , b ;
    U8 str[] = "Prochip" ; 
    
    b = *(RP)UART0_IIR ; //清空该中断
    
    for(i=0; str[i]!='\n'; i++)
    {
        *(RP)UART0_RXFIFO = str[i] ;   //读取接收FIFO里的数据
    }
	
}


U32 MdsIsrUart0(void) //用来返回modem状态发生变化中断的硬件错误
{
    return *(RP)UART0_MSR ;
}