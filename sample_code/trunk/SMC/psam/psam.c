/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  psam.c
 *
 *	File Description: ���ܿ���������
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
#include	"psam.h"

extern psamcard psam1;
extern S8 back_respone[80];


/************************************************************************************
* ������:      Psam1StrInit
* ��ڲ���:  ��
* ���ز���:  ��
* ��������:  �Կ�Ƭ�Ľṹ����г�ʼ��,�ýṹ�����ڱ��淢��
                  �ͽ��յ�����������
*************************************************************************************/
void Psam1StrInit(void)
{
    memset(psam1.send_data,0,5);
    psam1.command_len = 0;
    memset(psam1.command_data,0,MAXLEN);
    memset(psam1.receive_data,0,MAXLEN);
    psam1.receive_len = 0;
}

/************************************************************************************
* ������:     Psam1Init
* ��ڲ���:  ��
* ���ز���:  ��
* ��������:  ��SMCģ��Ĵ������г�ʼ������,��������ʹ��SMC1ģ��
*************************************************************************************/
void Psam1Init(void)
{
    *(RP)GPIO_PORTE_SEL &= (~0x0038);     //PE3:5����Ϊ������;
    *(RP)SMC1_CTRL  &= (~0x00000001);    //SM_CTRL���ƼĴ���,��ʹ��PSAM��
    *(RP)SMC1_INT_MASK = 0x00000000;   //SM_INT�ж����μĴ���
    *(RP)SMC1_CTRL = 0x00060000;           //SM_CTRL���ƼĴ���,b23:16ʱ�ӷ�Ƶ����,b3�첽��˫���ַ���,b2�������,b0ʹ���ź�
    *(RP)SMC1_INT_MASK |= 0x0000001f;  //SM_INT�ж����μĴ���
    *(RP)SMC1_FD |= 0x01740001;             //SM_FD������Ԫʱ��Ĵ���,B31:16ʱ��ת������372,b3:0�����ʵ�������1

}


/************************************************************************************
* ������:     Psam1ResetCold
* ��ڲ���:  ��
* ���ز���:  ��
* ��������:  ��Ƭ���临λ����,��ʹ�ܿ�Ƭ,�����Ž�������
*************************************************************************************/
void Psam1ResetCold(void)
{
    S32 reg_data;
    *(RP)SMC1_CTRL |= 0x00000001; //SM_CTRL���ƼĴ���,ʹ��PSAM��
    
    //���չ���
    psam1.receive_len = 0;
    reg_data = *(RP)SMC1_STATUS;
    
    while((reg_data & 0x48) != 0x48)    //��Ϊ��,�Ҳ���ʱ
    {
        if ((reg_data & 0x40) != 0x40) 	 //FIFO ��Ϊ��
        {
            psam1.receive_data[psam1.receive_len] = *(RP)SMC1_RX;  //��ȡ����FIFO�е�����
            psam1.receive_len ++;
        }
        
        reg_data = *(RP)SMC1_STATUS;
    }
}


/************************************************************************************
* ������:     Psam1CommandSend
* ��ڲ���:  *command		�����������ݵ���ָ��
*		   len				�����������ݵĳ���
* ���ز���:   ��
* ��������:   ��ɷ����������,����ɷ������ݵĽ��չ���.�ú���ֻ��Psam1Pro()��������
*		   ���ϲ�͸��.
*************************************************************************************/
void Psam1CommandSend(S8 *command,S32 len)
{
    S32 reg_data;
    S32 i=0;
    
    // �����������
    for(i=0; i<len; i++)
    {
        *(RP)SMC1_TX = *command;
        command++;
    }
    
    *(RP)SMC1_TX_TRIG = len;
    
    // �����������
    reg_data = *(RP)(SMC1_STATUS);
    
    while ((reg_data & 0x40) == 0x40)
    {
        for (i=0;i<10;i++) ;
        reg_data = *(RP)SMC1_STATUS;
    }
    
    psam1.receive_len = 0;
    
    while((reg_data & 0x48) != 0x48)    //��Ϊ��,�Ҳ���ʱ
    {
        if ((reg_data & 0x40) != 0x40) 	  //��Ϊ��
        {
            psam1.receive_data[psam1.receive_len] = *(RP)SMC1_RX;   //��ȡ����FIFO�е�����
            psam1.receive_len ++;
        }
        
        reg_data = *(RP)SMC1_STATUS;
    }
}


/************************************************************************************
* ������:     Psam1Pro
* ��ڲ���:  *buffer				�����������ݵ���ָ��
* ���ز���:  ��
* ��������:  ��ɿ�Ƭ�����շ���ȫ����
*************************************************************************************/
S32 Psam1Pro(S8 *buffer)
{
    S32 i=0,temp;
    Psam1StrInit();
    
    for(i=0;i<5;i++)
    {
        psam1.send_data[i]=*(buffer++);
    }
    
    if(psam1.send_data[1]==0x84||psam1.send_data[1]==0x07||psam1.send_data[1]==0x00)
    {
        psam1.command_len = 0;
    }
    else if (psam1.send_data[1]==0xA4)
    {
        psam1.command_len = 2;
    }
    else if(psam1.send_data[1]==0XFA)
    {
        psam1.command_len = 8;
    }
    
    for(i=0;i<psam1.command_len;i++)
    {
        psam1.command_data[i]=*(buffer++);
    }
    
    Psam1CommandSend(psam1.send_data,5);
    
    if (psam1.send_data[1] == psam1.receive_data[0])
    {
        if(psam1.command_len!=0)
        Psam1CommandSend(psam1.command_data,psam1.command_len);
    }
    
    if (psam1.receive_data[0] == 0x61)
    {
        memset(back_respone,0,80);
        back_respone[1] = 0xc0;
        back_respone[4] = psam1.receive_data[1];
        Psam1CommandSend(back_respone,0x05);
        
    }
    else if(psam1.receive_data[0] == 0x90)
    {
        temp = 0x01;
    }
    else if(psam1.receive_data[psam1.receive_len-2] == 0x90)
    {
        temp =  0x01;
    }
    else
    {
        temp = 0x00;
    }
    return temp;
}
