/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
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
 *    1.0           2009.05.06        ch            
 *   		      	
 **************************************************************************************/

 
#include	<stdio.h>

#include	"sep4020.h"
#include	"intc.h"
#include	"nand.h"
#include	"ecc.h"

int main(void)
{
	U32 i;
	U8 EccTableRead[3] = {0};
    U8 EccTableWrite[3] = {0};
    U32 head = MEM_BLOCK_BEGIN;
   
   	
    for(i=0; i<0x4000; i++)
	{
		*(RP)head = i;
		head = head + 4;                                  //��head��ַ��ʼ����ʼ��һ��32*512bytes����
	}
	
    
    InitEmi();                                            //EMI��ʼ��
    
   	InitGpio();                                           //PORTG7��ʼ������nandflash��WP#
   	
//***************************Nandflash data field��������******************************************
    memset((U32*)0x30180000,0x0,0x200);                   //��ַ0x30180000��ǰ512�ֽ�����
    
    NandErase(0x0);                                       //������0ҳ���ڵĵ�0��
    NandRead(0x30180000,0x0);                             //��ȡ��0ҳ��Ŀ���ַ0x30180000	            	   
    NandWrite(MEM_BLOCK_BEGIN,0x0);                       //��Դ��ַMEM_BLOCK_BEGIN��ʼ��һҳд��nandflash�ĵ�0ҳ				
    NandRead(0x30180000,0x0);				              //��ȡ��0ҳ��Ŀ���ַ0x30180000
    CompareMem(MEM_BLOCK_BEGIN,0x30180000,0x200);         //�Ƚ�MEM_BLOCK_BEGIN��0x30180000����512�ֽ����������Ƿ�һ��
    printf("Write page 0 ok\n\n");
    
    NandErase(0x10);                                      //����nandflash��16ҳ���ڵĵ�0��
    NandRead(0x30180000,0x10);	                          //��ȡ��16ҳ��Ŀ���ַ0x30180000		   
    NandWrite(MEM_BLOCK_BEGIN,0x10);                      //��Դ��ַMEM_BLOCK_BEGIN��ʼ��һҳд��nandflash�ĵ�16ҳ				
    NandRead(0x30180000,0x10);			                  //��ȡ��16ҳ��Ŀ���ַ0x30180000	
    CompareMem(MEM_BLOCK_BEGIN,0x30180000,0x200);         //�Ƚ�MEM_BLOCK_BEGIN��0x30180000����512�ֽ����������Ƿ�һ��
    printf("Write page 16 ok\n\n");

     memset((U32*)0x30180000,0x0,0x300);
    NandErase(0x0);	                                       //������0ҳ���ڵĵ�0��	
    for(i=0x0; i<0x20; i++)
    {
    	NandRead(0x30180000+(i<<9),i);                     //��ȡ��0�鵽Ŀ���ַ	                  
    }
    for(i=0x0; i<0x20; i++)
    {
    	NandWrite(MEM_BLOCK_BEGIN+(i<<9),i);               //д��0�飨��32ҳ��
    }
    for(i=0x0; i<0x20; i++)
    {
    	NandRead(0x30180000+(i<<9),i);                     //����0�飨��32ҳ��
    }										                
    CompareMem(MEM_BLOCK_BEGIN,0x30180000,0x4000);         //�Ƚ�MEM_BLOCK_BEGIN��0x30180000����0x4000�ֽڣ��������ݣ������Ƿ�һ��
    printf("Write block 0 ok\n\n");
    
    
    NandErase(0x20);	                                   //������32ҳ���ڵĵ�1��
    for(i=0x20;i<0x40;i++)
    {
    	NandRead(0x30180000+(i<<9),i);                     //��ȡ��1�鵽Ŀ���ַ
    }
    for(i=0x20;i<0x40;i++)
    {
    	NandWrite(MEM_BLOCK_BEGIN+(i<<9),i);               //д��1�飨��32ҳ��
    }
    for(i=0x20;i<0x40;i++)
    {
    	NandRead(0x30180000+(i<<9),i);                     //����1�飨��32ҳ��
    }										               
   	CompareMem(MEM_BLOCK_BEGIN,0x30180000,0x4000);         //�Ƚ�MEM_BLOCK_BEGIN��0x30180000����0x4000�ֽڣ��������ݣ������Ƿ�һ��
    printf("Write block 1 ok\n\n");
   
  	if(NandReadStatus()==0)                                //��nandflash��Status                                
  	{
  		printf("ReadStatus OK\n");
  	}
  	
  	
    if(NandReadId()==0)                                    //��nandflash��ID
    {
    	printf("ReadId OK\n");
    }


//***************************ECC��д******************************************************************
	
	
	NandErase(0x0);                                        //������0��
	NandWrite(0x30190000,0x0);                             //��Դ��ַ0x30190000��ʼ��һҳд��nandflash�ĵ�0ҳ
	MakeEcc512((U8*)&EccTableWrite,(U8 *)0x30190000);      //��0x30190000����512 byte���������3 byte ECC	
	
	NandRead(0x30190000,0x0);                              //��ȡ��0ҳ��Ŀ���ַ0x30190000
	MakeEcc512((U8 *)&EccTableRead,(U8 *)0x30190000);      //��0x30190000��512 byte���������3 byte ECC	
	
	CompareEcc512((U8*)&EccTableWrite,(U8*)&EccTableRead,(U8*)0x30190000,NULL,NULL);
                                                           //�Ƚ�����ECC������Ƿ���ڴ���
    while(1);
}