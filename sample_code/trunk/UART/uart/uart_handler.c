/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  uart_handler.c
 *
 *	File Description: uart �жϴ���
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0               2009.04.25       PROCHIP            
 *   		      	
 **************************************************************************************/
 
#include "sep4020.h"
#include "intc.h"
#include "uart.h"

//********************UART0�жϴ�����******************//

void Uart0IntHandler(void)
{
    S32 a = *(RP)UART0_IIR ;
    
    switch ((a &0X0E) >> 1) //�ж�bit��3��1���ж�Դ���
    {
        case 3:
            RlsIsrUart0(); //��������״̬�ж�
            break;
        case 2:
            RdaIsrUart0(); //����FIFO�е����ݴﵽ�������ж�
            break;
        case 6:
            TmoIsrUart0(); //��ʱ�ж�
            break;
        case 1:
            ThrIsrUart0(); //����FIFO�ж�
            break;
        case 0:
            MdsIsrUart0(); //״̬�����仯�ж�
            break;
        default:
            break ;
    }
    
}


U32 RlsIsrUart0(void) //���ڷ��ؽ�������״̬�жϵ�Ӳ������
{
    return *(RP)UART0_LSR ;  // ��״̬�Ĵ���
}

void RdaIsrUart0(void) //����ʵ�ֶ�ȡ����FIFO�е����ݣ����ѽ��յ����ݴ�ӡ����
{
    U32 i;
    U8 a[9];
    
    for(i=0; i<8; i++)
    {
        a[i] = (U8)*(RP)(UART0_RXFIFO);  //��ȡ����FIFO�������
        a[8] = '\0';
        Prints((S8 *)a,0x10005000);
        return;
    }

}


void TmoIsrUart0(void) //����ʵ�ֳ�ʱ�ж�
{
    U32 i;
    volatile U32 a;
    
    for(i=0; i<8; i++)
    {
        a = *(RP)UART0_RXFIFO ;
    }
    
    Prints("timer out now!!\n",0x10005000);
       
    return;
}



void ThrIsrUart0(void) //������Ҫ���͵����ݷ��봫��FIFO
{
    U32 i , b ;
    U8 str[] = "Prochip" ; 
    
    b = *(RP)UART0_IIR ; //��ո��ж�
    
    for(i=0; str[i]!='\n'; i++)
    {
        *(RP)UART0_RXFIFO = str[i] ;   //��ȡ����FIFO�������
    }
	
}


U32 MdsIsrUart0(void) //��������modem״̬�����仯�жϵ�Ӳ������
{
    return *(RP)UART0_MSR ;
}