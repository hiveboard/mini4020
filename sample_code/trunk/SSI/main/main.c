/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  main.c
 *
 *	File Description: SSI 演示主函数
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0               2009.04.25       PROCHIP            
 *   		      	
 **************************************************************************************/
#include	<stdio.h>

#include	"sep4020.h"
#include	"intc.h"
#include	"ssi.h"

int main(void)
{
    char tx_buff[255]="0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
    char rx_buff[255];
    U32 i ;
    U32 len = 62 ;
    
    //初始化
    //取消串行Flash的写保护和Hold 信号
    *(RP)GPIO_PORTG_SEL |= 0x80 ; //芯片A22 连接到Flash 的nWP, 高电平输出,通用用途
    *(RP)GPIO_PORTG_DIR &= 0x0 ;
    *(RP)GPIO_PORTG_DATA |=0x80 ;
    
    *(RP)GPIO_PORTF_SEL |=0x40 ;  //GPF第七脚为通用用途，配置高电平输出，连接到Flash的hold管脚
    *(RP)GPIO_PORTF_DIR &= 0x0 ;
    *(RP)GPIO_PORTF_DATA |=0x40 ; 
    
    SsiDisable();
    SsiSpiCs0init();            //SSI 初始化
    SsiEnable();
    
    //开始测试
    //取得Flash 的ID
    if (AMDFlashIDREAD() == 0x1f)
    {
        printf("It is atmel's flash\n");
    }
    else
    {
        printf(" The SSI bus is busy!please reload the program\n");
        return E_OBJ ;
    }
    
    //擦除Flash
    AMDFlashSE(0);        //通过DMA方式擦除Flash
    printf("Erasing the Flash.");
    
    while (AMDFlashRDSR() & 1)printf(".");
    printf("\nDone\n\n\n"); 
    
    //Flash 写测试
    printf("Write String to the Flash:");
    
    tx_buff[len]=0;
    printf("\n%s\n",tx_buff);
    AMDFlashPP(0x0,tx_buff,len);     //通过DMA方式写Flash
    
    printf("Writing.");
    while (AMDFlashRDSR() & 1)printf(".");
    printf("\nDone\n\n\n");
    
    //读测试
    for(i=0;i<255;i++)
    {
        rx_buff[i]=0;
    }
    
    printf("Read the String writed Previously:");
    AMDFlashREAD(0x0,rx_buff,len); 	  //将串行FLASH地址0开始的LEN个数据读到接收缓冲区rx_buff中
    printf("\n%s\n",rx_buff);
    
    printf("Test done\n");
    
    while(1);
    
    return 0 ; //为了去掉编译器可能的警告，程序不会运行到此位置

}


	

