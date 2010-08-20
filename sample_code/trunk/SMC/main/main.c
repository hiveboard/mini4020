/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  main.c
 *
 *	File Description: 智能卡演示程序主函数
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0               2009.04.25       PROCHIP            
 *   		      	
 **************************************************************************************/
#include <stdio.h>

#include "psam.h"
#include "sep4020.h"
#include "intc.h"

psamcard psam1;
S8 back_respone[80];

int main(void)
{
    S32	re_value,i;
    S8 com1[]={0x00,0x84,0x00,0x00,0x04};				// 取随机数命令
    S8 com2[]={0x00,0xA4,0x00,0x00,0x02,0x3F,0x00};	        // 选择文件命令
    
    Psam1StrInit();									// 结构体的初始化
    Psam1Init();										// SMC模块寄存器初始化
    
    // 冷复位过程
    printf("Ready to reset card!\n" );
    Psam1ResetCold();
    printf("recevie data:\n");
    
    for (i = 0;i<psam1.receive_len;i++)
    {
        printf(" 0x%x", psam1.receive_data[i]);
    }
    
    printf("\n");
    printf("Cold reset card complete!!\n\n" );
    
    // 发送随机数命令
    printf("Send get challenge command:\n" );
    re_value = Psam1Pro(com1);
    printf("recevie data:\n");
    
    for (i = 0;i<psam1.receive_len;i++)
    {
        printf(" 0x%x", psam1.receive_data[i]);
    }
    
    printf("\n");
    printf("Test get challenge command OK!!\n\n" );
    
    // 发送选择主文件命令
    printf("Send select main file command:\n" );
    re_value = Psam1Pro(com2);
    printf("recevie data:\n");
    
    for (i = 0;i<psam1.receive_len;i++)
    {
        printf(" 0x%x", psam1.receive_data[i]);
    }
    
    printf("\n");
    printf("Select main file command OK!!\n\n" );
    
    // 测试结束
    printf("Test done!\n");	
    
    while(1);
    return 0;    //避免编译器可能的警告，程序不会运行到这里
    
}