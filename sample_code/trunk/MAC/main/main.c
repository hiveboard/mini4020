/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  main.c
 *
 *	File Description: 主函数，实现具体功能.
 *
 *    Version         Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0          2009.05.08          CH           
 *   		      	
 *************************************************************************************/
#include	<stdio.h>
#include    <stdlib.h>    
#include    <string.h>

#include	"intc.h"
#include	"sep4020.h"
#include	"mac.h"

// 本程序主要完成了MAC内部的回环操作，首先造MAC帧，然后对发送描述符和接收描述符清零， 
// 接着配置发送描述符和接收描述符，
// 最后配置MAC控制寄存器设置回环模式。

int main(void)
{  
   
    //============= MAC LOOPBACK AT INTERAL =========//	
    phy_hard_reset(1);
    delay (10000);
    
    phy_hard_reset(0);
    delay (10000);
    
    phy_hard_reset(1);          //复位
	delay (10000);
	
    mii_reg_read();             //读取phy寄存器

    phy_selfloop_back();        //回环测试
	
    while(1);
}

