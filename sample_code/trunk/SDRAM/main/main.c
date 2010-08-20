/*************************************************************************************
 *	Copyright (c) 2007 by PROCHIP Limited.
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
 *    0.0.1           2009.4.25         Jack           
 *   		      	
 *************************************************************************************/
#include	<stdio.h>
#include    <string.h>

#include	"sep4020.h"
#include	"intc.h"
#include    "sdram.h"

int main(void)
{
   U16 i;
   
    /*对空间从0x30001000起，向后4K的空间初始化*/
   for(i=0;i<1024;i++)
    *(RP16)(0x30001000+2*i) = i;
   
  /********** 开始SDRAM数据传输测试 *****************************************************************
  * 在AXD中跟踪观察.此项测试也可直接打开AXD写SDRAM,看有没有写是否都正确
  **************************************************************************************************/ 
   
    
    printf("Tests begin!\n");                                  /*SDRAM测试*/
    
    BasicDataTran(0x30001000, 0x30000000,1024);               //搬运数据从0x30001000到0x30000000，大小为0x800
    
    if(COMPARE_MEM(0x30001000,0x30000000,1024) != E_OK)       /*对搬运的成功与否判断*/
     {
        printf("Move data from 0x30001000 to 0x30000000 failure!\n");
     }
     else
     {
        printf("Move data from 0x30001000 to 0x30000000 successfully!\n");
     }  
  
    //多块传输
    
    for(i=0;i<=0x400;i++) 
    {
        BasicDataTran(0x30001000, 0x30003000+i*(0x1000),1024);
        
        if (COMPARE_MEM(0x30001000, 0x30003000+i*(0x1000),1024) != E_OK)
        {
          printf("Move data from 0x30001000 to 0x30003000 + %d *0x1000  failure!\n",i);
        }  
        else
        {
           printf("Move data from 0x30001000 to 0x30003000 + %d *0x1000  successfully!\n",i);
        }
    }
    
    printf("sdram test over!");         

    while(1);
    
    return 0; //为了去除警告信息
}
