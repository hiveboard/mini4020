/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  uart.c
 *
 *	File Description: uart ��������
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0               2009.04.25       PROCHIP            
 *   		      	
 **************************************************************************************/
#include <stdio.h>

#include "sep4020.h"
#include "uart.h"
#include "intc.h"

/************************************************************************************
* ������:	   UartInit
* ��ڲ���:   U32 UART ��ַ��U8 UART  �жϺ�
* ���ز���:   ��
* ��������:   ��ʼ��ѡ����UART 
*************************************************************************************/
void UartInit(U32 uart_add, U8 uart_int)    
{
    mask_irq(uart_int);           // �ж�����
    UartConfig(uart_add,80000000,115200,8,0,0);     //��ͨģʽ��������9600��8bit �� ����FIFO trigger level 1byte,����FIFO trigger level 0byte
    irq_enable(uart_int);
    unmask_irq(uart_int);
    
}

/************************************************************************************
* ������:	   UartHWFlowcontrolTest
* ��ڲ���:   U32 sysclk,U32 baudrate,U32 data_bit ,U32 tri_level_t,U32 tri_level_r
* ���ز���:   1 (�ɹ�)  0(ʧ��)
* ��������:   �����Զ�Ӳ������ģʽ
*************************************************************************************/
U32 UartHWFlowcontrolTest(U32 sysclk,U32 baudrate,U32 data_bit,U32 tri_level_t,U32 tri_level_r)

{
    S32 i;
    U32 baud,bit,triger_r,triger_t,baudh,baudl;
    
    baud  = sysclk/16/baudrate;
    baudh = baud >> 8;                 // �����ʸ�8λ
    baudl = baud & 0xff;               // �����ʵ�8λ
    
    switch(data_bit)                   // bit�ĵڰ�λ��Ϊ1��������divisor latch�Ĵ���
    {
        case 5: bit = 0x80;
            break;
        case 6: bit = 0x81;
            break;
        case 7: bit = 0x82;
            break;
        case 8: bit = 0x83;
            break;
        default: ;
            break;
    }
    
    *(RP)UART0_LCR = bit ;
    *(RP)UART0_DLBH = baudh ;
    *(RP)UART0_DLBL = baudl ;
    *(RP)UART1_LCR = bit ;
    *(RP)UART1_DLBH = baudh ;
    *(RP)UART1_DLBL = baudl ;    
    
    bit &= (~(0x1<<7));                //���÷�����ͨ�Ĵ���
    
    *(RP)UART0_LCR = bit ;
    *(RP)UART1_LCR = bit ;
       
    //���÷���FIFO������
    switch(tri_level_t)
    {
        case 0: triger_t = 0x0;
            break;
        case 2: triger_t = 0x1;
            break;
        case 4: triger_t = 0x2;
            break;
        case 8: triger_t = 0x3;
            break;
        default: ;
            break;
    }
    
    triger_t = (triger_t<<4);          // 0 bit ����
    
    *(RP)UART0_FCR = triger_t ;
    *(RP)UART1_FCR = triger_t ;
    
    //���ý���FIFO������
    switch(tri_level_r)
    {
        case 1: triger_r = 0x0;
            break;
        case 4: triger_r = 0x1;
            break;
        case 8: triger_r = 0x2;
            break;
        case 14: triger_r = 0x3;
            break;
        default:
            break;      
    }
    
    triger_r = (triger_r<<6);          // 8 bit ����
    
    *(RP)UART1_FCR = triger_r ;
    *(RP)UART0_FCR = triger_r ;
    
    //����Ϊ���ù������Զ�Ӳ������ģʽ
    *(RP)UART1_MCR = 0x20 ;
    *(RP)UART0_MCR = 0x20 ;
    
    //��modem״̬�仯�жϺͽ���FIFO�����ж�
    *(RP)UART0_IER = 0x9 ;
    *(RP)UART1_IER = 0x9 ;
    
    for(i=0;i<300;i++);   //delay
    
    *(RP)UART1_MCR = 0x22 ;
    *(RP)UART0_MCR = 0x22 ;   // ���շ�׼����
    
    return 1; 
    
}


/************************************************************************************
* ������:	   UartLoopbackTest
* ��ڲ���:   U32 sysclk,U32 baudrate,U32 data_bit,U32 tri_level_t
* ���ز���:   1 (�ɹ�)  0(ʧ��)
* ��������:   ����loopbackģʽ
*************************************************************************************/
U32 UartLoopbackTest(U32 sysclk,U32 baudrate,U32 data_bit,U32 tri_level_t)

{
    S32 i ;
    U32 baud , bit,triger_t , baudh , baudl ;
    
    baud = sysclk/16/baudrate;
    baudh = baud >> 8;               //�����ʸ�8λ
    baudl =baud & 0xff;              //�����ʵ�8λ
    
    switch(data_bit)                 // bit�ĵڰ�λ��Ϊ1��������divisor latch�Ĵ���
    {
        case 5: bit = 0x80;
            break;
        case 6: bit = 0x81;
            break;
        case 7: bit = 0x82;
            break;
        case 8: bit = 0x83;
            break;
        default: ;
            break;
    }
    
    bit = bit|0x1c;                  //���������żУ�飬2��ֹͣλ
    
    *(RP)UART0_LCR = bit ;
    *(RP)UART0_DLBH = baudh ;
    *(RP)UART0_DLBL = baudl ;
    
    bit &= (~(0x1<<7));              //���÷�����ͨ�Ĵ���
    
    *(RP)UART0_LCR = bit ;
    
    //����UART����FIFO������
    switch(tri_level_t)
    {
        case 0: triger_t = 0x0;
            break;
        case 2: triger_t = 0x1;
            break;
        case 4: triger_t = 0x2;
            break;
        case 8: triger_t = 0x3;
            break;
        default: ;
            break;
    }
    
    triger_t = (triger_t<<4);        //UART���մ�������Ϊ1(0x00)
    
    *(RP)UART0_FCR = triger_t ;
    
    //����UART2������loopbackģʽ
    *(RP)UART0_MCR = 0x10 ;
    
    //��modem״̬�仯�ж�
    *(RP)UART0_IER = 0x1 ;
    
    //д��FIFO
    
    *(RP)UART0_TXFIFO = 'L' ;
    *(RP)UART0_TXFIFO = 'O' ;
    *(RP)UART0_TXFIFO = 'O' ;
    *(RP)UART0_TXFIFO = 'P' ;
    *(RP)UART0_TXFIFO = 'B' ;
    *(RP)UART0_TXFIFO = 'A' ;
    *(RP)UART0_TXFIFO = 'C' ;
    *(RP)UART0_TXFIFO = 'K' ;
    *(RP)UART0_TXFIFO = ' ' ;
    *(RP)UART0_TXFIFO = 'M' ;
    *(RP)UART0_TXFIFO = 'O' ;
    *(RP)UART0_TXFIFO = 'D' ;
    *(RP)UART0_TXFIFO = 'E' ;
    *(RP)UART0_TXFIFO = ' ' ;
    *(RP)UART0_TXFIFO = 'O' ;
    *(RP)UART0_TXFIFO = 'K' ;
    *(RP)UART0_TXFIFO = '!' ;
    *(RP)UART0_TXFIFO = '\n' ;
    
    for(i=0; i<100; i++);
    {
        *(RP)UART0_MCR = 0x16 ;
    }
    
    return 0; 

}
 
   
/************************************************************************************
* ������:	   Prints
* ��ڲ���:   S8 *s,U32 uart_add
* ���ز���:   1 (�ɹ�)  0(ʧ��)
* ��������:   �����ݴ�ӡ����λ�������ն�
*************************************************************************************/
void Prints(S8 *s,U32 uart_add)     

{
    S32 i;
    
    while(*s != '\0')
    {
        *(RP)(uart_add) = *s++;   //����ָ�봫�����ݣ������Ӳ������ģ�����ֻҪ��ָ��ָ�������ݵ�ַ�����ҽ���ѭ��
        
        for(i=0; i<5000 ; i++) ;  //delay
    }
    
    *(RP)uart_add = '\n' ;
    
}
    

/************************************************************************************
* ������:	   UartConfig
* ��ڲ���:   U32 uart_add,U32 sysclk,U32 baudrate,U32 data_bit ��U32 tri_level_t,U32 tri_level_r 
* ���ز���:   ��
* ��������:   ����UART��PC  ͨ��֮ǰ������
*************************************************************************************/
void UartConfig(U32 uart_add,U32 sysclk,U32 baudrate,U32 data_bit,U32 tri_level_t,U32 tri_level_r)

{
    U32 baud , bit1 , triger_r , triger_t , baudh , baudl ;
    
    baud  = sysclk/16/baudrate;
    baudh = baud >> 8;                                  //�����ʸ�8λ
    baudl = baud & 0xff;                                //�����ʵ�8λ
    
    switch(data_bit)                                    // ���������Ӧ��ͬ�ı�����00(5bits),01(6bits),10(7bits),11(8bits)
    {
        case 5: bit1 = 0x80;
            break;
        case 6: bit1 = 0x81;
            break;
        case 7: bit1 = 0x82;
            break;
        case 8: bit1 = 0x83;
            break;
        default: ;
            break;
    }
    
    //	bit1 = bit1|0x0b;                                 //��У�飬1��ֹͣλ
    *(RP)(uart_add+0x0c) = bit1 ;                //�п��ƼĴ���������Divisor Latch �Ĵ��������ʣ�ÿ���������ݵı�������8
    *(RP)(uart_add+0x04) = baudh ;           // дDivisor Latch �Ĵ����������ʸ�8λ
    *(RP)(uart_add+0x00) = baudl ;           // дDivisor Latch �Ĵ����������ʵ�8λ
    
    bit1 &= (~(0x1<<7));                                 //���÷�����ͨ�Ĵ���
    
    *(RP)(uart_add+0x0c) = bit1 ; 
    
    //���÷���FIFO������
    switch(tri_level_t)
    {
        case 0: triger_t = 0x0;
            break;
        case 2: triger_t = 0x1;
            break;
        case 4: triger_t = 0x2;
            break;
        case 8: triger_t = 0x3;
            break;
        default: ;
            break;
    }
    
    triger_t = (triger_t<<4);                            // 0 bit ����
    
    *(RP)(uart_add+0x08) = triger_t ;              //  ����1 byte ������0 bit����               
    
    //����
    
    switch(tri_level_r)
    {
        case 1: triger_r = 0x0;
            break;
        case 4: triger_r = 0x1;
            break;
        case 8: triger_r = 0x2;
            break;
        case 14:triger_r = 0x3;
            break;
        default: ;
            break;
    }
    
    triger_r = (triger_r<<6);
    
    *(RP)(uart_add+0x08) |= triger_r ;
    *(RP)(uart_add+0x04) = 0x0 ;       // ��disable �жϼ�ʹ�ܼĴ��� 
                   
    UartIntEn(uart_add,1,0);             // �����жϴ���
	
}


/************************************************************************************
* ������:	   UartIntEn
* ��ڲ���:  S8 recie, S8 thrie
* ���ز���:   ��
* ��������:   �ж�uart�Ƿ�׼�����պͷ�������,  �Ӷ�����FIFO��ʹ�ܿ���
*************************************************************************************/
void UartIntEn(U32 uart_add, S8 recie, S8 thrie)	 //�ж�uart0�Ƿ�׼�����պͷ������ݣ��Ӷ�����FIFO��ʹ�ܿ���

{
    U32 UART_IER;
    
    switch (uart_add)
    {
        case 0x10005000:
            UART_IER = UART0_IER;
            break;
        case 0x10006000:
            UART_IER = UART1_IER;
            break;
        case 0x10007000:
            UART_IER = UART2_IER;
            break;
        case 0x10008000:
            UART_IER = UART3_IER;
            break;
        default: ;
            UART_IER = UART0_IER;
            break;
    }
    
    if(recie == 1)
    {
        *(RP)UART_IER |= 0x1 ;   //������FIFO�������ж�ʹ��
    }
    else
        if(recie == 0)
        {
            *(RP)UART_IER &= ~0x1 ; //�رս���FIFO�������ж�ʹ��
        }
        
    if(thrie == 1)
    {
        *(RP)UART_IER |= 0x1<<1 ;   //������FIFO���������ж�ʹ��
    }
    else
        if(thrie == 0)
        {
             *(RP)UART_IER &= ~(0x1<<1) ;  //�رշ���FIFO���������ж�ʹ��
        }

}