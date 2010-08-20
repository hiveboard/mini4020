/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  main.c
 *
 *	File Description: 主函数, 实现具体功能.
 *
 *    Version         Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0           2009.05.08          CH           
 *   		      	
 *************************************************************************************/

#include	<stdio.h>

#include	"sep4020.h"
#include	"intc.h"
#include	"sd.h"


int main(void)
{
    int i;
    U32 head;
    U32 rca;
    
    //**************************初始化一段数据****************************************
    head = MEM_BLOCK_BEGIN;      
    for(i=0; i<BLOCK_SIZE; i++)
    {
	*(RP)head = i;
	head = head + 4;
    }
	
    //********************************初始化SD卡**************************************
    //得到SD卡的rca地址号，进行下面操作的时候都要给卡发送地址号
    rca = InitialSd();
     
     
    //********************************初始化FIFO**************************************
    FifoReset();
    

    //********************************读写测试开始************************************    
    SdSingleW1r1(rca);			//单块 一位写一位读
  
    SdSingleW1r4(rca);			//单块 一位写四位读
   
    SdSingleW4r1(rca);			//单块 四位写一位读
   
    SdSingleW4r4(rca);			//单块 四位写四位读
  
    SdMultiW1r1(rca); 			//多块 一位写一位读
  
    SdMultiW1r4(rca); 			//多块 一位写四位读
   
    SdMultiW4r1(rca); 			//多块 四位写一位读

    SdMultiW4r4(rca); 			//多块 四位写四位读

    while(1);
    
    return 0;
}

