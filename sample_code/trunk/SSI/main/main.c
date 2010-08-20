/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  main.c
 *
 *	File Description: SSI ��ʾ������
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
    
    //��ʼ��
    //ȡ������Flash��д������Hold �ź�
    *(RP)GPIO_PORTG_SEL |= 0x80 ; //оƬA22 ���ӵ�Flash ��nWP, �ߵ�ƽ���,ͨ����;
    *(RP)GPIO_PORTG_DIR &= 0x0 ;
    *(RP)GPIO_PORTG_DATA |=0x80 ;
    
    *(RP)GPIO_PORTF_SEL |=0x40 ;  //GPF���߽�Ϊͨ����;�����øߵ�ƽ��������ӵ�Flash��hold�ܽ�
    *(RP)GPIO_PORTF_DIR &= 0x0 ;
    *(RP)GPIO_PORTF_DATA |=0x40 ; 
    
    SsiDisable();
    SsiSpiCs0init();            //SSI ��ʼ��
    SsiEnable();
    
    //��ʼ����
    //ȡ��Flash ��ID
    if (AMDFlashIDREAD() == 0x1f)
    {
        printf("It is atmel's flash\n");
    }
    else
    {
        printf(" The SSI bus is busy!please reload the program\n");
        return E_OBJ ;
    }
    
    //����Flash
    AMDFlashSE(0);        //ͨ��DMA��ʽ����Flash
    printf("Erasing the Flash.");
    
    while (AMDFlashRDSR() & 1)printf(".");
    printf("\nDone\n\n\n"); 
    
    //Flash д����
    printf("Write String to the Flash:");
    
    tx_buff[len]=0;
    printf("\n%s\n",tx_buff);
    AMDFlashPP(0x0,tx_buff,len);     //ͨ��DMA��ʽдFlash
    
    printf("Writing.");
    while (AMDFlashRDSR() & 1)printf(".");
    printf("\nDone\n\n\n");
    
    //������
    for(i=0;i<255;i++)
    {
        rx_buff[i]=0;
    }
    
    printf("Read the String writed Previously:");
    AMDFlashREAD(0x0,rx_buff,len); 	  //������FLASH��ַ0��ʼ��LEN�����ݶ������ջ�����rx_buff��
    printf("\n%s\n",rx_buff);
    
    printf("Test done\n");
    
    while(1);
    
    return 0 ; //Ϊ��ȥ�����������ܵľ��棬���򲻻����е���λ��

}


	

