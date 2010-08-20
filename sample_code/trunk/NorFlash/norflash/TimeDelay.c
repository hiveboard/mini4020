/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  TimeDelay.c
 *
 *	File Description: NorFlash演示用到的延时函数
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0               2009.04.25       PROCHIP            
 *   		      	
 **************************************************************************************/
#include "sep4020.h"

extern void Delay10MicroSeconds(S32 n);  
extern void Delay30NanoSeconds(S32 n); 

/************************************************************************************
* 函数名:	   Delay10MicroSeconds 
* 入口参数:   无
* 返回参数:   无 
* 函数功能:   延时函数，最小单位10微秒 
*************************************************************************************/
extern void Delay10MicroSeconds(S32 n)
{
    S32 i;
    for(i=0;i<(n*100);i++);	
}


/************************************************************************************
* 函数名:	   Delay30MicroSeconds 
* 入口参数:   无
* 返回参数:   无 
* 函数功能:   延时函数，最小单位30微秒 
*************************************************************************************/
extern void Delay30NanoSeconds(S32 n)
{
    S32 i;
    for(i=0;i<n;i++);
}