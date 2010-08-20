/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  main.c
 *
 *	File Description: Key 演示程序主函数
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0               2009.04.25       PROCHIP            
 *   		      	
 **************************************************************************************/

#include <stdio.h>

#include "sep4020.h"
#include "intc.h"
#include "key.h"

int main(void)
{
    KeyInit();
    
    while(1);
    
    return 0 ;  //避免编译器warning ，永远不会到达这里
}
