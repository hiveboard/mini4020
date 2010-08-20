/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  uart.c
 *
 *	File Description: uart 驱动程序
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0               2009.04.25       PROCHIP            
 *   		      	
 **************************************************************************************/
#include <stdio.h>

#include "sep4020.h"
#include "uart.h"
#include "intc.h"

/************************************************************************************
* 函数名:	   UartInit
* 入口参数:   U32 UART 地址，U8 UART  中断号
* 返回参数:   无
* 函数功能:   初始化选定的UART 
*************************************************************************************/
void UartInit(U32 uart_add, U8 uart_int)    
{
    mask_irq(uart_int);           // 中断屏蔽
    UartConfig(uart_add,80000000,115200,8,0,0);     //普通模式，波特率9600，8bit ， 接收FIFO trigger level 1byte,发送FIFO trigger level 0byte
    irq_enable(uart_int);
    unmask_irq(uart_int);
    
}

/************************************************************************************
* 函数名:	   UartHWFlowcontrolTest
* 入口参数:   U32 sysclk,U32 baudrate,U32 data_bit ,U32 tri_level_t,U32 tri_level_r
* 返回参数:   1 (成功)  0(失败)
* 函数功能:   测试自动硬件流控模式
*************************************************************************************/
U32 UartHWFlowcontrolTest(U32 sysclk,U32 baudrate,U32 data_bit,U32 tri_level_t,U32 tri_level_r)

{
    S32 i;
    U32 baud,bit,triger_r,triger_t,baudh,baudl;
    
    baud  = sysclk/16/baudrate;
    baudh = baud >> 8;                 // 波特率高8位
    baudl = baud & 0xff;               // 波特率低8位
    
    switch(data_bit)                   // bit的第八位设为1，即访问divisor latch寄存器
    {
        case 5: bit = 0x80;
            break;
        case 6: bit = 0x81;
            break;
        case 7: bit = 0x82;
            break;
        case 8: bit = 0x83;
            break;
        default: ;
            break;
    }
    
    *(RP)UART0_LCR = bit ;
    *(RP)UART0_DLBH = baudh ;
    *(RP)UART0_DLBL = baudl ;
    *(RP)UART1_LCR = bit ;
    *(RP)UART1_DLBH = baudh ;
    *(RP)UART1_DLBL = baudl ;    
    
    bit &= (~(0x1<<7));                //设置访问普通寄存器
    
    *(RP)UART0_LCR = bit ;
    *(RP)UART1_LCR = bit ;
       
    //配置发送FIFO触发级
    switch(tri_level_t)
    {
        case 0: triger_t = 0x0;
            break;
        case 2: triger_t = 0x1;
            break;
        case 4: triger_t = 0x2;
            break;
        case 8: triger_t = 0x3;
            break;
        default: ;
            break;
    }
    
    triger_t = (triger_t<<4);          // 0 bit 触发
    
    *(RP)UART0_FCR = triger_t ;
    *(RP)UART1_FCR = triger_t ;
    
    //配置接收FIFO触发级
    switch(tri_level_r)
    {
        case 1: triger_r = 0x0;
            break;
        case 4: triger_r = 0x1;
            break;
        case 8: triger_r = 0x2;
            break;
        case 14: triger_r = 0x3;
            break;
        default:
            break;      
    }
    
    triger_r = (triger_r<<6);          // 8 bit 触发
    
    *(RP)UART1_FCR = triger_r ;
    *(RP)UART0_FCR = triger_r ;
    
    //以下为配置工作于自动硬件流控模式
    *(RP)UART1_MCR = 0x20 ;
    *(RP)UART0_MCR = 0x20 ;
    
    //打开modem状态变化中断和接收FIFO触发中断
    *(RP)UART0_IER = 0x9 ;
    *(RP)UART1_IER = 0x9 ;
    
    for(i=0;i<300;i++);   //delay
    
    *(RP)UART1_MCR = 0x22 ;
    *(RP)UART0_MCR = 0x22 ;   // 接收方准备好
    
    return 1; 
    
}


/************************************************************************************
* 函数名:	   UartLoopbackTest
* 入口参数:   U32 sysclk,U32 baudrate,U32 data_bit,U32 tri_level_t
* 返回参数:   1 (成功)  0(失败)
* 函数功能:   测试loopback模式
*************************************************************************************/
U32 UartLoopbackTest(U32 sysclk,U32 baudrate,U32 data_bit,U32 tri_level_t)

{
    S32 i ;
    U32 baud , bit,triger_t , baudh , baudl ;
    
    baud = sysclk/16/baudrate;
    baudh = baud >> 8;               //波特率高8位
    baudl =baud & 0xff;              //波特率低8位
    
    switch(data_bit)                 // bit的第八位设为1，即访问divisor latch寄存器
    {
        case 5: bit = 0x80;
            break;
        case 6: bit = 0x81;
            break;
        case 7: bit = 0x82;
            break;
        case 8: bit = 0x83;
            break;
        default: ;
            break;
    }
    
    bit = bit|0x1c;                  //正常输出，偶校验，2个停止位
    
    *(RP)UART0_LCR = bit ;
    *(RP)UART0_DLBH = baudh ;
    *(RP)UART0_DLBL = baudl ;
    
    bit &= (~(0x1<<7));              //设置访问普通寄存器
    
    *(RP)UART0_LCR = bit ;
    
    //配置UART发送FIFO触发级
    switch(tri_level_t)
    {
        case 0: triger_t = 0x0;
            break;
        case 2: triger_t = 0x1;
            break;
        case 4: triger_t = 0x2;
            break;
        case 8: triger_t = 0x3;
            break;
        default: ;
            break;
    }
    
    triger_t = (triger_t<<4);        //UART接收触发级即为1(0x00)
    
    *(RP)UART0_FCR = triger_t ;
    
    //设置UART2工作于loopback模式
    *(RP)UART0_MCR = 0x10 ;
    
    //打开modem状态变化中断
    *(RP)UART0_IER = 0x1 ;
    
    //写入FIFO
    
    *(RP)UART0_TXFIFO = 'L' ;
    *(RP)UART0_TXFIFO = 'O' ;
    *(RP)UART0_TXFIFO = 'O' ;
    *(RP)UART0_TXFIFO = 'P' ;
    *(RP)UART0_TXFIFO = 'B' ;
    *(RP)UART0_TXFIFO = 'A' ;
    *(RP)UART0_TXFIFO = 'C' ;
    *(RP)UART0_TXFIFO = 'K' ;
    *(RP)UART0_TXFIFO = ' ' ;
    *(RP)UART0_TXFIFO = 'M' ;
    *(RP)UART0_TXFIFO = 'O' ;
    *(RP)UART0_TXFIFO = 'D' ;
    *(RP)UART0_TXFIFO = 'E' ;
    *(RP)UART0_TXFIFO = ' ' ;
    *(RP)UART0_TXFIFO = 'O' ;
    *(RP)UART0_TXFIFO = 'K' ;
    *(RP)UART0_TXFIFO = '!' ;
    *(RP)UART0_TXFIFO = '\n' ;
    
    for(i=0; i<100; i++);
    {
        *(RP)UART0_MCR = 0x16 ;
    }
    
    return 0; 

}
 
   
/************************************************************************************
* 函数名:	   Prints
* 入口参数:   S8 *s,U32 uart_add
* 返回参数:   1 (成功)  0(失败)
* 函数功能:   把数据打印到上位机超级终端
*************************************************************************************/
void Prints(S8 *s,U32 uart_add)     

{
    S32 i;
    
    while(*s != '\0')
    {
        *(RP)(uart_add) = *s++;   //利用指针传输数据，这个是硬件管理的，我们只要把指针指向首数据地址，并且建立循环
        
        for(i=0; i<5000 ; i++) ;  //delay
    }
    
    *(RP)uart_add = '\n' ;
    
}
    

/************************************************************************************
* 函数名:	   UartConfig
* 入口参数:   U32 uart_add,U32 sysclk,U32 baudrate,U32 data_bit ，U32 tri_level_t,U32 tri_level_r 
* 返回参数:   无
* 函数功能:   进行UART和PC  通信之前的配置
*************************************************************************************/
void UartConfig(U32 uart_add,U32 sysclk,U32 baudrate,U32 data_bit,U32 tri_level_t,U32 tri_level_r)

{
    U32 baud , bit1 , triger_r , triger_t , baudh , baudl ;
    
    baud  = sysclk/16/baudrate;
    baudh = baud >> 8;                                  //波特率高8位
    baudl = baud & 0xff;                                //波特率低8位
    
    switch(data_bit)                                    // 四种情况对应不同的比特数00(5bits),01(6bits),10(7bits),11(8bits)
    {
        case 5: bit1 = 0x80;
            break;
        case 6: bit1 = 0x81;
            break;
        case 7: bit1 = 0x82;
            break;
        case 8: bit1 = 0x83;
            break;
        default: ;
            break;
    }
    
    //	bit1 = bit1|0x0b;                                 //奇校验，1个停止位
    *(RP)(uart_add+0x0c) = bit1 ;                //行控制寄存器，设置Divisor Latch 寄存器被访问，每个传输数据的比特数是8
    *(RP)(uart_add+0x04) = baudh ;           // 写Divisor Latch 寄存器，波特率高8位
    *(RP)(uart_add+0x00) = baudl ;           // 写Divisor Latch 寄存器，波特率低8位
    
    bit1 &= (~(0x1<<7));                                 //设置访问普通寄存器
    
    *(RP)(uart_add+0x0c) = bit1 ; 
    
    //配置发送FIFO触发级
    switch(tri_level_t)
    {
        case 0: triger_t = 0x0;
            break;
        case 2: triger_t = 0x1;
            break;
        case 4: triger_t = 0x2;
            break;
        case 8: triger_t = 0x3;
            break;
        default: ;
            break;
    }
    
    triger_t = (triger_t<<4);                            // 0 bit 触发
    
    *(RP)(uart_add+0x08) = triger_t ;              //  接收1 byte ，发送0 bit触发               
    
    //接收
    
    switch(tri_level_r)
    {
        case 1: triger_r = 0x0;
            break;
        case 4: triger_r = 0x1;
            break;
        case 8: triger_r = 0x2;
            break;
        case 14:triger_r = 0x3;
            break;
        default: ;
            break;
    }
    
    triger_r = (triger_r<<6);
    
    *(RP)(uart_add+0x08) |= triger_r ;
    *(RP)(uart_add+0x04) = 0x0 ;       // 先disable 中断即使能寄存器 
                   
    UartIntEn(uart_add,1,0);             // 配置中断触发
	
}


/************************************************************************************
* 函数名:	   UartIntEn
* 入口参数:  S8 recie, S8 thrie
* 返回参数:   无
* 函数功能:   判断uart是否准备接收和发送数据,  从而决定FIFO的使能控制
*************************************************************************************/
void UartIntEn(U32 uart_add, S8 recie, S8 thrie)	 //判断uart0是否准备接收和发送数据，从而决定FIFO的使能控制

{
    U32 UART_IER;
    
    switch (uart_add)
    {
        case 0x10005000:
            UART_IER = UART0_IER;
            break;
        case 0x10006000:
            UART_IER = UART1_IER;
            break;
        case 0x10007000:
            UART_IER = UART2_IER;
            break;
        case 0x10008000:
            UART_IER = UART3_IER;
            break;
        default: ;
            UART_IER = UART0_IER;
            break;
    }
    
    if(recie == 1)
    {
        *(RP)UART_IER |= 0x1 ;   //开接收FIFO触发级中断使能
    }
    else
        if(recie == 0)
        {
            *(RP)UART_IER &= ~0x1 ; //关闭接收FIFO触发级中断使能
        }
        
    if(thrie == 1)
    {
        *(RP)UART_IER |= 0x1<<1 ;   //开发送FIFO触发级空中断使能
    }
    else
        if(thrie == 0)
        {
             *(RP)UART_IER &= ~(0x1<<1) ;  //关闭发送FIFO触发级空中断使能
        }

}