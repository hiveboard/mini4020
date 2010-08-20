/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  ssi.c
 *
 *	File Description: ssi ��������
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0               2009.04.25       PROCHIP            
 *   		      	
 **************************************************************************************/
#include	<stdio.h>
#include	<string.h>

#include	"sep4020.h"
#include	"intc.h"
#include	"ssi.h"

/************************************************************************************
* ������:	  SsiDisable
* ��ڲ���:  ��
* ���ز���:  ��
* ��������:  ��ֹSSI (��ʹ��ʱ���д��䱻ֹͣ,����FIFO�������.���������Ĵ���֮ǰ
*                 ����ɴ˲��� )
*************************************************************************************/
void SsiDisable(void)
{
    *(RP)SSI_SSIENR = 0x0 ;
}


/************************************************************************************
* ������:	  SsiEnable
* ��ڲ���:  ��
* ���ز���:  ��
* ��������:  ʹ��SSI
*************************************************************************************/
void SsiEnable(void)
{
    *(RP)SSI_SSIENR |= 0x1 ;
}


/************************************************************************************
* ������:	  SsiWriteDr
* ��ڲ���:  U16 data ,Ҫд�������
* ���ز���:  ��
* ��������:  дSSI���ݼĴ���
*************************************************************************************/
void SsiWriteDr(U16 data)
{
*(RP)SSI_DR = data ;
}



/************************************************************************************
* ������:	  SsiReadReg
* ��ڲ���:  reg ���ݼĴ���
* ���ز���:  ��
* ��������:  �����ݼĴ���
*************************************************************************************/
U32 SsiReadReg(U32 reg)
{
    return *(RP)reg ;
}

/************************************************************************************
* ������:	  SsiSpiCs0init
* ��ڲ���:  reg ���ݼĴ���
* ���ز���:  ��
* ��������:  ��ʼ��SSI
*************************************************************************************/
  void SsiSpiCs0init(void)   
{
    *(RP)SSI_CONTROL0 = 0xc7 ;    //SCPOL=1,����ʱ��ֹͣ״̬Ϊ��,SCPH=1������ʱ���ڵ�һ��bit ��ʼʱ����,SPIЭ�飬����֡����8 
    *(RP)SSI_BAUDR = 0x8 ;            //���ò����ʣ�Fsclk_out = Fssi_clk / SSIDV   ��10M ����оƬ�������
    
    //����FIFO��ֵ
    *(RP)SSI_TXFTLR = 0x0 ;      //��ֵΪ0
    *(RP)SSI_SER = 0x1 ;           //ʹ�ܴ��豸,ͨ������Ƭѡʵ��,�ڴ�ѡ��1���豸 
    
    //����DMA�������
    
    *(RP)SSI_DMACR = 0x3 ;       //���ͽ��ն�ʹ��
    *(RP)SSI_DMATDLR = 0x3 ;   //����������FIFO�������������ڻ����ڴ�����ʱ��DMA��������
    *(RP)SSI_DMARDLR = 0x1 ;   //����������FIFO�������������ڻ���ڴ�����ʱ��DMA��������

}

/************************************************************************************
* ������:	  AMDFlashRDSR
* ��ڲ���:  ��
* ���ز���:  Flash��״̬
* ��������:  ͨ��DMA��ʽ��ȡFlash��״̬�Ĵ������� ��AMD Flashģ��
*************************************************************************************/
U8 AMDFlashRDSR(void)
{
    U8 source[2];
    source[0]= source[1]=5;    //��״̬�Ĵ���ָ��
    
    *(RP)DMAC_C1SRCADDR = (unsigned int)source ; // Դ��ַ
    *(RP)DMAC_C1DESTADDR = SSI_DR ;                   // Ŀ�ĵ�ַ
    *(RP)DMAC_C1CONTROL = 0x901b ;                   // ����burst��Ŀ�������ȣ�Դ��ַ��Ŀ�ĵ�ַ�Զ�����
    
    //����ͨ��ʹ�ܡ��������ͣ����δ����жϺʹ����жϣ����Ʒ���Դ��Ŀ���豸�ţ�ͨ����
    *(RP)DMAC_C1CONFIGURATION = 0x281b ; 
    *(RP)DMAC_C1DESCRIPTOR = 0 ;                       //����ͨ������Ĵ���:ָ����һ��������������ʼλ�õĵ�ַ
    
    //�ȴ����������ͨ��״̬�Ĵ����жϣ���δ������ٵ�һ�����ڣ�60ms������չ����
    while (SsiReadReg(SSI_SR) &0x1)
    {
        Delay10MicroSeconds(6000);
    }
    
    SsiReadReg(SSI_DR);
    
    return  SsiReadReg(SSI_DR); 
    
}


/************************************************************************************
* ������:	  AMDFlashIDREAD
* ��ڲ���:  ��
* ���ز���:  Flash��ID��
* ��������:  ͨ��DMA��ʽ��ȡFlash��ID
*************************************************************************************/
U8 AMDFlashIDREAD(void)
{
    U8 source[2];
    source[0]= source[1]=0x15;   //��Flash ָ���ο�оƬ�ֲ�
    
    *(RP)DMAC_C1SRCADDR = (unsigned int)source ;
    *(RP)DMAC_C1DESTADDR = SSI_DR;
    *(RP)DMAC_C1CONTROL = 0x901b;
    *(RP)DMAC_C1CONFIGURATION = 0x281b;	
    *(RP)DMAC_C1DESCRIPTOR = 0;
    
    while (SsiReadReg(SSI_SR) &0x1)
    {
        Delay10MicroSeconds(6000);
    }
    
    SsiReadReg(SSI_DR);
    
    return  SsiReadReg(SSI_DR);
    
}


/************************************************************************************
* ������:	  AMDFlashSE
* ��ڲ���:  U32 address ����Ҫ������Flash�ĵ�ַ��
* ���ز���:  ��
* ��������:  ͨ��DMA��ʽ����Flash
*************************************************************************************/
void AMDFlashSE(U32 address)
{
    U8 source[4],i;
    
    //дʹ�ܣ��κ�д����֮ǰ��Ҫ���д˲���
    SsiWriteDr(0x6);
    source[0] = 0x6A;     //����ָ��
    source[1] = (U8)(address>>16);
    source[2] = (U8)(address>>8);
    source[3] = (U8)(address);
    
    *(RP)DMAC_C1SRCADDR = (unsigned int)source ;
    *(RP)DMAC_C1DESTADDR = SSI_DR;
    *(RP)DMAC_C1CONTROL = 0x1102d;  
    
    //Channel enable:umask error interrupt and accomplishment
    *(RP)DMAC_C1CONFIGURATION = 0x281b ;
    *(RP)DMAC_C1DESCRIPTOR = 0 ;
    
    while (SsiReadReg(SSI_SR) &0x1)
    {
        Delay10MicroSeconds(6000);
    }
    
    for(i=5; i>0; i--)
    {
        SsiReadReg(SSI_DR); //clear unuseful receive data
    }

}


/************************************************************************************
* ������:	  AMDFlashPP
* ��ڲ���:  U32 address ����Ҫ������Flash�ĵ�ַ����S8  data �����ݻ��壩��len��д���ȣ�
* ���ز���:  ��
* ��������:  ͨ��DMA��ʽдFlash
*************************************************************************************/
void AMDFlashPP(U32 address,S8 *buff,U32 len)
{
    U32 i;
    S8 temp[255];
    
    for(i=0; i<255; i++) temp[i] = 0;	//��շ��ͻ�����
    memcpy(temp+4,buff,len);	       //�����ݿ��������ͻ����������ĸ��ֽڷ������ַ
    SsiWriteDr(0x6);
    
    while (SsiReadReg(SSI_SR) &0x1)
    {
        Delay10MicroSeconds(6000);
    }
    
    temp[0] = 2;
    temp[1] = (U8)(address>>16);
    temp[2] = (U8)(address>>8);
    temp[3] = (U8)(address);
    
    *(RP)DMAC_C1SRCADDR = (unsigned int)temp ;
    *(RP)DMAC_C1DESTADDR = SSI_DR ;
    *(RP)DMAC_C1CONTROL = ((len+4)<<14)+0x0101b ;  
    
    *(RP)DMAC_C1CONFIGURATION = 0x281b ;
    *(RP)DMAC_C1DESCRIPTOR = 0 ;
    
    while (SsiReadReg(SSI_SR) &0x1)
    {
        Delay10MicroSeconds(6000);
    }
    
    for(i=8;i>0;i--)
    {
        SsiReadReg(SSI_DR);  //�����������
    }   
      
}


/************************************************************************************
* ������:	  AMDFlashREAD
* ��ڲ���:  U32 address ��Flash��ַ��,len(������)
* ���ز���:  S8 *buff ��������Ҫ����Flash��ַ�ϵ����� 
* ��������:  U32 address��Flash��ַ
*************************************************************************************/
void AMDFlashREAD(U32 address,S8 *buff,U32 len)
{
    S8 temp[255];
    S8 command[5];  
    
    command[0] = 0x03;               //д����
    command[1] = (U8)(address>>16);
    command[2] = (U8)(address>>8);
    command[3] = (U8)(address);
    command[4] = 0;                 //Ϊ�˸�SPI�������ṩʱ�� ��Ҫ���������� 
    
    //���÷���DMA
    *(RP)DMAC_C1SRCADDR = (unsigned int)command ;
    *(RP)DMAC_C1DESTADDR = SSI_DR;
    *(RP)DMAC_C1CONTROL = ((len+4)<<14)+0x0101b;
    *(RP)DMAC_C1CONFIGURATION = 0x281b;	
    *(RP)DMAC_C1DESCRIPTOR = 0;
    
    //���ý���DMA
    *(RP)DMAC_C0SRCADDR = SSI_DR;
    *(RP)DMAC_C0DESTADD = (unsigned int)temp;
    *(RP)DMAC_C0CONTROL = ((len+4)<<14) + 0x2000;
    *(RP)DMAC_C0CONFIGURATION = 0x029d;
    
    while (SsiReadReg(SSI_SR) &0x1) 
    {
        Delay10MicroSeconds(6000);
    }
    
    memcpy(buff,temp+4,len);
      
}

/************************************************************************************
* ������:	  Delay10MicroSecond
* ��ڲ���:  S32 ��ʱ����
* ���ز���:  �� 
* ��������:  ��ʱ����С��λ10 ΢��
*************************************************************************************/  
void Delay10MicroSeconds(S32 n)
{
    S32 i;
    
    for(i=0;i<(n*100);i++);
	
}


