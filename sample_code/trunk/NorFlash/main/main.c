/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  main.c
 *
 *	File Description: NorFlash演示程序主函数
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0               2009.04.25       PROCHIP            
 *   		      	
 **************************************************************************************/

#include <stdio.h>
#include <string.h> 

#include "norflash.h"
#include "config.h"

int main(void)
{
    U32 i  ;
    S32 status  = 0 ;
    WORD cfi_data[11] ;
    
    memset((RP*)DataHead, 0xaa, SIZE1);	  // 初始化一块数据块，用于测试用途 
    
     
     
     CheckToggleReady(0x20100000); 
     CheckToggleReady(0x20200000);
     CheckToggleReady(0x20300000);
     CheckToggleReady(0x20400000);
     CheckToggleReady(0x20500000);
     CheckToggleReady(0x20600000);
     CheckToggleReady(0x20700000);
    
    // 测试1，整片擦除
    status = EraseEntireChip();
    
    if (!status)
    {
        printf("Erase Entire Chip OK!!\n");
    }
    else
    {
        printf("Erase Entire Chip Fail!!\n");
        printf("Error Number %ld \n ",status);
   }
   
   Delay10MicroSeconds(1000);
   
   // 测试2 读取Flash的ID
   status = CheckSST39VF160X();
   
   if (status == 0x234B)
   {
        printf("The type is SST_39VF1601!!!\n");
   }
   else if(status == 0x234A)
   {
        printf("The type is SST_39VF1602!!!\n");
    }
    else
    {
        printf("Check SST Fail!!!\n");
    }
    
    Delay10MicroSeconds(1000);
	
// 测试3 读取CFI数据
	
    CfiQuery(cfi_data);
    printf("CFI read ok\n");
    for (i=0; i< 11; i++)
    {
        printf("%d  ",cfi_data[i]);
    }
    printf("\n");

// 测试3 以块为单位向NORFLASH中写入数据，块大小 为 64K个字节 ，2M Flash 共32块
 	
    for (i=0; i<520; i++)
    {
        ProgramOneBlock((WORD *)(DataHead), 0x10000*i); 
        if (Check((DataHead), (U32)(NorHead+(0x10000*i)), BLOCK_SIZE) == TRUE) 
        {
            printf(" %ld Block Program successfully !\n" , (i+1) );

 	    }
 	else
 	{ 
 	    printf("The %u block write or check fail! \n",(unsigned)i);
 	    break ;
 	}
 
        Delay10MicroSeconds(1000);
    }
 
    printf(" 0xAAAA is writed to 2Mflash!\nNorFlash test ok!\n");
    
    

    
    while(1) ;
    
    return 0 ;  //去掉警告，程序不会运行到这里
    
}


