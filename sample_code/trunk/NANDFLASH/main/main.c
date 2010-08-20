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
		head = head + 4;                                  //从head地址开始处初始化一块32*512bytes内容
	}
	
    
    InitEmi();                                            //EMI初始化
    
   	InitGpio();                                           //PORTG7初始化，打开nandflash的WP#
   	
//***************************Nandflash data field基本操作******************************************
    memset((U32*)0x30180000,0x0,0x200);                   //地址0x30180000的前512字节清零
    
    NandErase(0x0);                                       //擦除第0页所在的第0块
    NandRead(0x30180000,0x0);                             //读取第0页到目标地址0x30180000	            	   
    NandWrite(MEM_BLOCK_BEGIN,0x0);                       //把源地址MEM_BLOCK_BEGIN开始的一页写入nandflash的第0页				
    NandRead(0x30180000,0x0);				              //读取第0页到目标地址0x30180000
    CompareMem(MEM_BLOCK_BEGIN,0x30180000,0x200);         //比较MEM_BLOCK_BEGIN和0x30180000处的512字节数据内容是否一致
    printf("Write page 0 ok\n\n");
    
    NandErase(0x10);                                      //擦除nandflash第16页所在的第0块
    NandRead(0x30180000,0x10);	                          //读取第16页到目标地址0x30180000		   
    NandWrite(MEM_BLOCK_BEGIN,0x10);                      //把源地址MEM_BLOCK_BEGIN开始的一页写入nandflash的第16页				
    NandRead(0x30180000,0x10);			                  //读取第16页到目标地址0x30180000	
    CompareMem(MEM_BLOCK_BEGIN,0x30180000,0x200);         //比较MEM_BLOCK_BEGIN和0x30180000处的512字节数据内容是否一致
    printf("Write page 16 ok\n\n");

     memset((U32*)0x30180000,0x0,0x300);
    NandErase(0x0);	                                       //擦除第0页所在的第0块	
    for(i=0x0; i<0x20; i++)
    {
    	NandRead(0x30180000+(i<<9),i);                     //读取第0块到目标地址	                  
    }
    for(i=0x0; i<0x20; i++)
    {
    	NandWrite(MEM_BLOCK_BEGIN+(i<<9),i);               //写第0块（共32页）
    }
    for(i=0x0; i<0x20; i++)
    {
    	NandRead(0x30180000+(i<<9),i);                     //读第0块（共32页）
    }										                
    CompareMem(MEM_BLOCK_BEGIN,0x30180000,0x4000);         //比较MEM_BLOCK_BEGIN和0x30180000处的0x4000字节（整块数据）内容是否一致
    printf("Write block 0 ok\n\n");
    
    
    NandErase(0x20);	                                   //擦除第32页所在的第1块
    for(i=0x20;i<0x40;i++)
    {
    	NandRead(0x30180000+(i<<9),i);                     //读取第1块到目标地址
    }
    for(i=0x20;i<0x40;i++)
    {
    	NandWrite(MEM_BLOCK_BEGIN+(i<<9),i);               //写第1块（共32页）
    }
    for(i=0x20;i<0x40;i++)
    {
    	NandRead(0x30180000+(i<<9),i);                     //读第1块（共32页）
    }										               
   	CompareMem(MEM_BLOCK_BEGIN,0x30180000,0x4000);         //比较MEM_BLOCK_BEGIN和0x30180000处的0x4000字节（整块数据）内容是否一致
    printf("Write block 1 ok\n\n");
   
  	if(NandReadStatus()==0)                                //读nandflash的Status                                
  	{
  		printf("ReadStatus OK\n");
  	}
  	
  	
    if(NandReadId()==0)                                    //读nandflash的ID
    {
    	printf("ReadId OK\n");
    }


//***************************ECC读写******************************************************************
	
	
	NandErase(0x0);                                        //擦除第0块
	NandWrite(0x30190000,0x0);                             //把源地址0x30190000开始的一页写入nandflash的第0页
	MakeEcc512((U8*)&EccTableWrite,(U8 *)0x30190000);      //从0x30190000处的512 byte数据里产生3 byte ECC	
	
	NandRead(0x30190000,0x0);                              //读取第0页到目标地址0x30190000
	MakeEcc512((U8 *)&EccTableRead,(U8 *)0x30190000);      //从0x30190000的512 byte数据里产生3 byte ECC	
	
	CompareEcc512((U8*)&EccTableWrite,(U8*)&EccTableRead,(U8*)0x30190000,NULL,NULL);
                                                           //比较两个ECC，检查是否存在错误
    while(1);
}