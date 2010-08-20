/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  psam.c
 *
 *	File Description: 智能卡驱动程序
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
#include	"psam.h"

extern psamcard psam1;
extern S8 back_respone[80];


/************************************************************************************
* 函数名:      Psam1StrInit
* 入口参数:  无
* 返回参数:  无
* 函数功能:  对卡片的结构体进行初始化,该结构体用于保存发送
                  和接收到的命令内容
*************************************************************************************/
void Psam1StrInit(void)
{
    memset(psam1.send_data,0,5);
    psam1.command_len = 0;
    memset(psam1.command_data,0,MAXLEN);
    memset(psam1.receive_data,0,MAXLEN);
    psam1.receive_len = 0;
}

/************************************************************************************
* 函数名:     Psam1Init
* 入口参数:  无
* 返回参数:  无
* 函数功能:  对SMC模块寄存器进行初始化配置,本测试中使用SMC1模块
*************************************************************************************/
void Psam1Init(void)
{
    *(RP)GPIO_PORTE_SEL &= (~0x0038);     //PE3:5配置为特殊用途
    *(RP)SMC1_CTRL  &= (~0x00000001);    //SM_CTRL控制寄存器,不使能PSAM卡
    *(RP)SMC1_INT_MASK = 0x00000000;   //SM_INT中断屏蔽寄存器
    *(RP)SMC1_CTRL = 0x00060000;           //SM_CTRL控制寄存器,b23:16时钟分频参数,b3异步半双工字符传,b2正相编码,b0使能信号
    *(RP)SMC1_INT_MASK |= 0x0000001f;  //SM_INT中断屏蔽寄存器
    *(RP)SMC1_FD |= 0x01740001;             //SM_FD基本单元时间寄存器,B31:16时钟转换因子372,b3:0波特率调整因子1

}


/************************************************************************************
* 函数名:     Psam1ResetCold
* 入口参数:  无
* 返回参数:  无
* 函数功能:  卡片的冷复位过程,先使能卡片,紧接着接收数据
*************************************************************************************/
void Psam1ResetCold(void)
{
    S32 reg_data;
    *(RP)SMC1_CTRL |= 0x00000001; //SM_CTRL控制寄存器,使能PSAM卡
    
    //接收过程
    psam1.receive_len = 0;
    reg_data = *(RP)SMC1_STATUS;
    
    while((reg_data & 0x48) != 0x48)    //不为空,且不超时
    {
        if ((reg_data & 0x40) != 0x40) 	 //FIFO 不为空
        {
            psam1.receive_data[psam1.receive_len] = *(RP)SMC1_RX;  //读取接收FIFO中的数据
            psam1.receive_len ++;
        }
        
        reg_data = *(RP)SMC1_STATUS;
    }
}


/************************************************************************************
* 函数名:     Psam1CommandSend
* 入口参数:  *command		发送命令内容的首指针
*		   len				发送命令内容的长度
* 返回参数:   无
* 函数功能:   完成发送命令过程,并完成返回数据的接收过程.该函数只供Psam1Pro()函数调用
*		   对上层透明.
*************************************************************************************/
void Psam1CommandSend(S8 *command,S32 len)
{
    S32 reg_data;
    S32 i=0;
    
    // 发送命令过程
    for(i=0; i<len; i++)
    {
        *(RP)SMC1_TX = *command;
        command++;
    }
    
    *(RP)SMC1_TX_TRIG = len;
    
    // 接收命令过程
    reg_data = *(RP)(SMC1_STATUS);
    
    while ((reg_data & 0x40) == 0x40)
    {
        for (i=0;i<10;i++) ;
        reg_data = *(RP)SMC1_STATUS;
    }
    
    psam1.receive_len = 0;
    
    while((reg_data & 0x48) != 0x48)    //不为空,且不超时
    {
        if ((reg_data & 0x40) != 0x40) 	  //不为空
        {
            psam1.receive_data[psam1.receive_len] = *(RP)SMC1_RX;   //读取接收FIFO中的数据
            psam1.receive_len ++;
        }
        
        reg_data = *(RP)SMC1_STATUS;
    }
}


/************************************************************************************
* 函数名:     Psam1Pro
* 入口参数:  *buffer				发送命令内容的首指针
* 返回参数:  无
* 函数功能:  完成卡片命令收发的全过程
*************************************************************************************/
S32 Psam1Pro(S8 *buffer)
{
    S32 i=0,temp;
    Psam1StrInit();
    
    for(i=0;i<5;i++)
    {
        psam1.send_data[i]=*(buffer++);
    }
    
    if(psam1.send_data[1]==0x84||psam1.send_data[1]==0x07||psam1.send_data[1]==0x00)
    {
        psam1.command_len = 0;
    }
    else if (psam1.send_data[1]==0xA4)
    {
        psam1.command_len = 2;
    }
    else if(psam1.send_data[1]==0XFA)
    {
        psam1.command_len = 8;
    }
    
    for(i=0;i<psam1.command_len;i++)
    {
        psam1.command_data[i]=*(buffer++);
    }
    
    Psam1CommandSend(psam1.send_data,5);
    
    if (psam1.send_data[1] == psam1.receive_data[0])
    {
        if(psam1.command_len!=0)
        Psam1CommandSend(psam1.command_data,psam1.command_len);
    }
    
    if (psam1.receive_data[0] == 0x61)
    {
        memset(back_respone,0,80);
        back_respone[1] = 0xc0;
        back_respone[4] = psam1.receive_data[1];
        Psam1CommandSend(back_respone,0x05);
        
    }
    else if(psam1.receive_data[0] == 0x90)
    {
        temp = 0x01;
    }
    else if(psam1.receive_data[psam1.receive_len-2] == 0x90)
    {
        temp =  0x01;
    }
    else
    {
        temp = 0x00;
    }
    return temp;
}
