/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  main.c
 *
 *	File Description:   IIS��Ƶ���ŵ�������
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *     1.0           2009.04.25           lbn            
 *   		      	
 **************************************************************************************/ 
#include	<stdio.h>

#include	"iis.h"
#include	"intc.h"
#include	"sep4020.h"


int main(void)
{ 

    mask_irq(INTSRC_DMAC);       //����DMA��������IRQ�ж�

    InitPWM();                   //��ʼ��PWM3:����PWM3��Ƶ��ռ�ձȺ�ʹ��PWM3
	
    InitDatapath();              //��ʼ�������������ź�:ʹ��GPIO_PORTG11�˿ڿ���nCSC�˿�
	
    InitCodec();                 //��ʼ������оƬ:����L3���߽ӿ��ź��Լ�L3���ߵĵ�ַ�����ݴ���ģʽ

    InitDMA();                   //��ʼ��DMA������:����DMA��ԴĿ�ĵ�ַ�Լ����������üĴ���
    
    InitIIS();                   //��ʼ��IIS������:����IIS���������жϼĴ���
   
    irq_enable(INTSRC_DMAC);     //ʹ��DMA��������IRQ�ж�
    
    unmask_irq(INTSRC_DMAC);     //�����ε�DMA��������IRQ�ж�
	 
    while(1);
    
    return 0;		   

}




