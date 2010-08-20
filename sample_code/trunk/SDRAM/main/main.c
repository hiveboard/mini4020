/*************************************************************************************
 *	Copyright (c) 2007 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  main.c
 *
 *	File Description: ������, ʵ�־��幦��.
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
   
    /*�Կռ��0x30001000�����4K�Ŀռ��ʼ��*/
   for(i=0;i<1024;i++)
    *(RP16)(0x30001000+2*i) = i;
   
  /********** ��ʼSDRAM���ݴ������ *****************************************************************
  * ��AXD�и��ٹ۲�.�������Ҳ��ֱ�Ӵ�AXDдSDRAM,����û��д�Ƿ���ȷ
  **************************************************************************************************/ 
   
    
    printf("Tests begin!\n");                                  /*SDRAM����*/
    
    BasicDataTran(0x30001000, 0x30000000,1024);               //�������ݴ�0x30001000��0x30000000����СΪ0x800
    
    if(COMPARE_MEM(0x30001000,0x30000000,1024) != E_OK)       /*�԰��˵ĳɹ�����ж�*/
     {
        printf("Move data from 0x30001000 to 0x30000000 failure!\n");
     }
     else
     {
        printf("Move data from 0x30001000 to 0x30000000 successfully!\n");
     }  
  
    //��鴫��
    
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
    
    return 0; //Ϊ��ȥ��������Ϣ
}
