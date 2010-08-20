/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  main.c
 *
 *	File Description: UART演示主函数
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


int main(void)
{
    UartPort Uart[4] = {
                                    {UART0_BASE,24},
                                    {UART1_BASE,23},
                                    {UART2_BASE,22},
                                    {UART3_BASE,21},
                                  };
                                  
    UartInit(Uart[0].UartAddr, Uart[0].UartIntNo); //串口0
    Prints("Prochip",Uart[0].UartAddr); 
    
    while(1);
    
    return 0 ; //防止编译器可能的警告，程序不会运行到这里
}