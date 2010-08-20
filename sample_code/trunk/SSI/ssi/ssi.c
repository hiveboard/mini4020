/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  ssi.c
 *
 *	File Description: ssi 驱动程序
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0               2009.04.25       PROCHIP            
 *   		      	
 **************************************************************************************/
#include	<stdio.h>
#include	<string.h>

#include	"sep4020.h"
#include	"intc.h"
#include	"ssi.h"

/************************************************************************************
* 函数名:	  SsiDisable
* 入口参数:  无
* 返回参数:  无
* 函数功能:  禁止SSI (不使能时所有传输被停止,所有FIFO都被清空.配置其它寄存器之前
*                 先完成此操作 )
*************************************************************************************/
void SsiDisable(void)
{
    *(RP)SSI_SSIENR = 0x0 ;
}


/************************************************************************************
* 函数名:	  SsiEnable
* 入口参数:  无
* 返回参数:  无
* 函数功能:  使能SSI
*************************************************************************************/
void SsiEnable(void)
{
    *(RP)SSI_SSIENR |= 0x1 ;
}


/************************************************************************************
* 函数名:	  SsiWriteDr
* 入口参数:  U16 data ,要写入的数据
* 返回参数:  无
* 函数功能:  写SSI数据寄存器
*************************************************************************************/
void SsiWriteDr(U16 data)
{
*(RP)SSI_DR = data ;
}



/************************************************************************************
* 函数名:	  SsiReadReg
* 入口参数:  reg 数据寄存器
* 返回参数:  无
* 函数功能:  读数据寄存器
*************************************************************************************/
U32 SsiReadReg(U32 reg)
{
    return *(RP)reg ;
}

/************************************************************************************
* 函数名:	  SsiSpiCs0init
* 入口参数:  reg 数据寄存器
* 返回参数:  无
* 函数功能:  初始化SSI
*************************************************************************************/
  void SsiSpiCs0init(void)   
{
    *(RP)SSI_CONTROL0 = 0xc7 ;    //SCPOL=1,串行时钟停止状态为高,SCPH=1，串行时钟在第一个bit 开始时进入,SPI协议，数据帧长度8 
    *(RP)SSI_BAUDR = 0x8 ;            //设置波特率，Fsclk_out = Fssi_clk / SSIDV   ，10M ，按芯片规格设置
    
    //设置FIFO阈值
    *(RP)SSI_TXFTLR = 0x0 ;      //阈值为0
    *(RP)SSI_SER = 0x1 ;           //使能从设备,通过设置片选实现,在此选定1号设备 
    
    //设置DMA传输参数
    
    *(RP)SSI_DMACR = 0x3 ;       //发送接收都使能
    *(RP)SSI_DMATDLR = 0x3 ;   //发送数量，FIFO中数据数量等于或少于此数量时发DMA发送请求
    *(RP)SSI_DMARDLR = 0x1 ;   //接收数量，FIFO中数据数量等于或大于此数量时发DMA发送请求

}

/************************************************************************************
* 函数名:	  AMDFlashRDSR
* 入口参数:  无
* 返回参数:  Flash的状态
* 函数功能:  通过DMA方式读取Flash的状态寄存器内容 ，AMD Flash模型
*************************************************************************************/
U8 AMDFlashRDSR(void)
{
    U8 source[2];
    source[0]= source[1]=5;    //读状态寄存器指令
    
    *(RP)DMAC_C1SRCADDR = (unsigned int)source ; // 源地址
    *(RP)DMAC_C1DESTADDR = SSI_DR ;                   // 目的地址
    *(RP)DMAC_C1CONTROL = 0x901b ;                   // 设置burst数目，传输宽度，源地址和目的地址自动增加
    
    //配置通道使能。传输类型，屏蔽错误中断和传输中断，控制方，源和目的设备号，通道号
    *(RP)DMAC_C1CONFIGURATION = 0x281b ; 
    *(RP)DMAC_C1DESCRIPTOR = 0 ;                       //配置通道链表寄存器:指向下一个链表描述符起始位置的地址
    
    //等待传输结束，通过状态寄存器判断，若未完成则再等一个周期，60ms，请参照规格书
    while (SsiReadReg(SSI_SR) &0x1)
    {
        Delay10MicroSeconds(6000);
    }
    
    SsiReadReg(SSI_DR);
    
    return  SsiReadReg(SSI_DR); 
    
}


/************************************************************************************
* 函数名:	  AMDFlashIDREAD
* 入口参数:  无
* 返回参数:  Flash的ID号
* 函数功能:  通过DMA方式读取Flash的ID
*************************************************************************************/
U8 AMDFlashIDREAD(void)
{
    U8 source[2];
    source[0]= source[1]=0x15;   //读Flash 指令，请参考芯片手册
    
    *(RP)DMAC_C1SRCADDR = (unsigned int)source ;
    *(RP)DMAC_C1DESTADDR = SSI_DR;
    *(RP)DMAC_C1CONTROL = 0x901b;
    *(RP)DMAC_C1CONFIGURATION = 0x281b;	
    *(RP)DMAC_C1DESCRIPTOR = 0;
    
    while (SsiReadReg(SSI_SR) &0x1)
    {
        Delay10MicroSeconds(6000);
    }
    
    SsiReadReg(SSI_DR);
    
    return  SsiReadReg(SSI_DR);
    
}


/************************************************************************************
* 函数名:	  AMDFlashSE
* 入口参数:  U32 address （需要擦除的Flash的地址）
* 返回参数:  无
* 函数功能:  通过DMA方式擦除Flash
*************************************************************************************/
void AMDFlashSE(U32 address)
{
    U8 source[4],i;
    
    //写使能，任何写操作之前都要进行此操作
    SsiWriteDr(0x6);
    source[0] = 0x6A;     //擦除指令
    source[1] = (U8)(address>>16);
    source[2] = (U8)(address>>8);
    source[3] = (U8)(address);
    
    *(RP)DMAC_C1SRCADDR = (unsigned int)source ;
    *(RP)DMAC_C1DESTADDR = SSI_DR;
    *(RP)DMAC_C1CONTROL = 0x1102d;  
    
    //Channel enable:umask error interrupt and accomplishment
    *(RP)DMAC_C1CONFIGURATION = 0x281b ;
    *(RP)DMAC_C1DESCRIPTOR = 0 ;
    
    while (SsiReadReg(SSI_SR) &0x1)
    {
        Delay10MicroSeconds(6000);
    }
    
    for(i=5; i>0; i--)
    {
        SsiReadReg(SSI_DR); //clear unuseful receive data
    }

}


/************************************************************************************
* 函数名:	  AMDFlashPP
* 入口参数:  U32 address （需要擦除的Flash的地址），S8  data （数据缓冲），len（写长度）
* 返回参数:  无
* 函数功能:  通过DMA方式写Flash
*************************************************************************************/
void AMDFlashPP(U32 address,S8 *buff,U32 len)
{
    U32 i;
    S8 temp[255];
    
    for(i=0; i<255; i++) temp[i] = 0;	//清空发送缓冲区
    memcpy(temp+4,buff,len);	       //把数据拷贝到发送缓冲区，留四个字节放命令，地址
    SsiWriteDr(0x6);
    
    while (SsiReadReg(SSI_SR) &0x1)
    {
        Delay10MicroSeconds(6000);
    }
    
    temp[0] = 2;
    temp[1] = (U8)(address>>16);
    temp[2] = (U8)(address>>8);
    temp[3] = (U8)(address);
    
    *(RP)DMAC_C1SRCADDR = (unsigned int)temp ;
    *(RP)DMAC_C1DESTADDR = SSI_DR ;
    *(RP)DMAC_C1CONTROL = ((len+4)<<14)+0x0101b ;  
    
    *(RP)DMAC_C1CONFIGURATION = 0x281b ;
    *(RP)DMAC_C1DESCRIPTOR = 0 ;
    
    while (SsiReadReg(SSI_SR) &0x1)
    {
        Delay10MicroSeconds(6000);
    }
    
    for(i=8;i>0;i--)
    {
        SsiReadReg(SSI_DR);  //清除无用数据
    }   
      
}


/************************************************************************************
* 函数名:	  AMDFlashREAD
* 入口参数:  U32 address （Flash地址）,len(读长度)
* 返回参数:  S8 *buff 返回所需要读的Flash地址上的数据 
* 函数功能:  U32 address：Flash地址
*************************************************************************************/
void AMDFlashREAD(U32 address,S8 *buff,U32 len)
{
    S8 temp[255];
    S8 command[5];  
    
    command[0] = 0x03;               //写命令
    command[1] = (U8)(address>>16);
    command[2] = (U8)(address>>8);
    command[3] = (U8)(address);
    command[4] = 0;                 //为了给SPI读数据提供时钟 需要加冗余数据 
    
    //设置发送DMA
    *(RP)DMAC_C1SRCADDR = (unsigned int)command ;
    *(RP)DMAC_C1DESTADDR = SSI_DR;
    *(RP)DMAC_C1CONTROL = ((len+4)<<14)+0x0101b;
    *(RP)DMAC_C1CONFIGURATION = 0x281b;	
    *(RP)DMAC_C1DESCRIPTOR = 0;
    
    //设置接受DMA
    *(RP)DMAC_C0SRCADDR = SSI_DR;
    *(RP)DMAC_C0DESTADD = (unsigned int)temp;
    *(RP)DMAC_C0CONTROL = ((len+4)<<14) + 0x2000;
    *(RP)DMAC_C0CONFIGURATION = 0x029d;
    
    while (SsiReadReg(SSI_SR) &0x1) 
    {
        Delay10MicroSeconds(6000);
    }
    
    memcpy(buff,temp+4,len);
      
}

/************************************************************************************
* 函数名:	  Delay10MicroSecond
* 入口参数:  S32 延时参数
* 返回参数:  无 
* 函数功能:  延时，最小单位10 微秒
*************************************************************************************/  
void Delay10MicroSeconds(S32 n)
{
    S32 i;
    
    for(i=0;i<(n*100);i++);
	
}


