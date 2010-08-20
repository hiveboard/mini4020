/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  main.c
 *
 *	File Description:   lcd的主函数
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0          2009.04.25            Jack           
 *   		      	
 ***************************************************************************************/
#include "lcdc.h"
#include "sep4020.h"

extern const unsigned char pic[320*240*2];

int main(void)
{

    InitLcdc();                //初始化LCDC的控制寄存器
    ShowPics(pic);             //图片显示

    while(1);
    
    return 0;                 //为了消初警告
      
}