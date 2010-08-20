/*************************************************************************************
 *	Copyright (c) 2007 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  intc.h
 *
 *	File Description: The file define some macro definition used in some assembler file.
 *
 *    Version         Date            Author           
 *------------------------------------------------------------------------------------    
 *    0.0.1           2005.3.22       Michael <yuyu_zh@seu.edu.cn>
 *    0.0.2           2008.1.10       zhangyang <zyang@prochip.com.cn>            
 *   		      	
 *************************************************************************************
 */
 
#ifndef  __INTC_H
#define  __INTC_H

#include "sep4020.h"

/*
 **************************************
 * INTC ģ����ж�Դ
 **************************************
 */

#define  INTSRC_RTC        31		        
#define  INTSRC_DMAC       30	                
#define  INTSRC_EMI        29
#define  INTSRC_MAC        28
#define  INTSRC_TIMER1     27
#define  INTSRC_TIMER2     26
#define  INTSRC_TIMER3     25
#define  INTSRC_UART0      24
#define  INTSRC_UART1      23
#define  INTSRC_UART2      22
#define  INTSRC_UART3      21
#define  INTSRC_PWM        20
#define  INTSRC_LCDC       19
#define  INTSRC_I2S        18
#define  INTSRC_SSI        17

#define  INTSRC_USB        15
#define  INTSRC_SMC0       14
#define  INTSRC_SMC1       13
#define  INTSRC_SDIO       12  
#define  INTSRC_EXINT10    11              
#define  INTSRC_EXINT9     10              
#define  INTSRC_EXINT8     9               
#define  INTSRC_EXINT7     8               
#define  INTSRC_EXINT6     7               
#define  INTSRC_EXINT5     6               
#define  INTSRC_EXINT4     5               
#define  INTSRC_EXINT3     4               
#define  INTSRC_EXINT2     3               
#define  INTSRC_EXINT1     2               
#define  INTSRC_EXINT0     1
#define  INTSRC_NULL       0



/*
 **************************************
 * �жϴ���
 **************************************
 */
 
#define set_plevel(plevel)          *(RP)INTC_IPLR = plevel                      //������ͨ�жϵ����ȼ����ޣ�ֻ�����ȼ����ڴ�ֵ���жϲ���ͨ��
#define set_int_force(intnum)       *(RP)INTC_IFR = (1 << intnum)                //��1�����ǿ�Ƹ�λ��Ӧ���ж�Դ�����ж��ź�
#define irq_enable(intnum)          *(RP)INTC_IER |= ((U32)1 << intnum)               //��1�������ж�Դ��IRQ �ж��ź�
#define irq_disable( intnum)        *(RP)INTC_IER &= ~(1<< intnum)               //��0�󣬲������ж�Դ��IRQ �ж��ź�
#define mask_irq(intnum)            *(RP)INTC_IMR |= (1 << intnum)               //��1�����ζ�Ӧ��IRQ �ж��ź�               
#define unmask_irq(intnum)          *(RP)INTC_IMR &= ~((U32)1 << intnum)              //��0��ͨ����Ӧ��IRQ �ж��ź�
#define mask_all_irq()              *(RP)INTC_IMR = 0xFFFFFFFF                   //���ζ�Ӧ��IRQ �ж��ź�
#define unmask_all_irq()            *(RP)INTC_IMR = 0x00000000                   //ͨ����Ӧ��IRQ �ж��ź�
#define enable_all_irq()            *(RP)INTC_IER = 0XFFFFFFFF                   //�����ж�Դ��IRQ �ж��ź�
#define disable_all_irq()           *(RP)INTC_IER = 0X00000000                   //�������ж�Դ��IRQ �ж��ź�


/* definition of vector struct for interrrupt */
typedef struct int_vector{
	U8		IntNum;
	void	(*handler)(void);
}INT_VECTOR;

/* declaration of function */
extern void int_vector_handler(void);

#endif
