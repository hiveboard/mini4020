/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  timer.c
 *
 *	File Description:   Timer����
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0          2009.04.25           PROCHIP            
 *   		      	
 ***************************************************************************************/
#include	<stdio.h>
//#include	<string.h>

#include	"sep4020.h"
#include	"intc.h"
#include    "timer.h"


#define TIMER_RESTART 1   //��������ģʽ
#define TIMER_FREEDOM 0   //���ɼ���ģʽ
#define TIMER_ONESHOT 0   //ONE-SHOT����ģʽ
#define TIMER_CAPTURE 0   //�ⲿ����(ע��ͨ��3~6֧��capture)

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       InitTimer                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ��ʼ��Timerģ��
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong              
*                                                                       
* INPUTS                                                                
*                                                                       
*      	��       
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      ��   
*                                                                       
***************************************************************/	

void InitTmier(void)    
{
   mask_irq(INTSRC_TIMER2); 

// ����timer1ͨ��Ϊ��������ģʽ

  #if TIMER_RESTART
    
   *(RP)TIMER_T3LCR = 508;    //���ݲ����жϵ�ʱ�������ü��صļ���ֵ
   *(RP)TIMER_T3CR = 0x6;     //����ͨ��1��������������ģʽ
   *(RP)TIMER_T3CR |= 0x1;
   
  #endif
  
  
// ����timer1ͨ��Ϊ���ɼ���ģʽ
 
  #if TIMER_FREEDOM

   *(RP)TIMER_T1LCR = 508;     //���ݲ����жϵ�ʱ�������ü��صļ���ֵ
   *(RP)TIMER_T1CR = 0x02;     //����ͨ��1���������ɼ���ģʽ
   *(RP)TIMER_T1CR |= 0x1;
   
  #endif


// ����timer1ͨ��ΪONE-SHOT����ģʽ

  #if TIMER_ONESHOT
    
   *(RP)TIMER_T1LCR = 508;    //���ݲ����жϵ�ʱ�������ü��صļ���ֵ
   *(RP)TIMER_T1CR = 0xa;     //����ͨ��1������ONE-SHOT����ģʽ
   *(RP)TIMER_T1CR |= 0x1;
   
  #endif


// ����timer3ͨ��Ϊ����ģʽ

  #if TIMER_CAPTURE
    
   *(RP)TIMER_T3LCR = 0x0;    //���ݲ����жϵ�ʱ�������ü��صļ���ֵ
   *(RP)TIMER_T3CR = 0x42;     //����ͨ��3���������ɼ���ģʽ
   *(RP)TIMER_T3CR |= 0x1;
   
  #endif

   irq_enable(INTSRC_TIMER2);         /*�ж�ʹ��*/
   unmask_irq(INTSRC_TIMER2);         /*�������ж�*/

}



/*************************************************************
* FUNCTION                                                              
*                                                                       
*       Timer_IRQ_Service1                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	TIMER�жϴ�����
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	��
*			
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	��                
*                                                                       
***************************************************************/

void Timer_IRQ_Service1(void)
{
	volatile U32 clear_int;
    clear_int = *(RP)TIMER_T1ISCR; //��ͨ��1�ж�״̬����Ĵ�������ж�
    *(RP)TIMER_T1ISCR=clear_int;
    printf("Timer1 restar mode test OK\n");
}


void Timer_IRQ_Service2(void)
{
    volatile U32 clear_int;
    clear_int = *(RP)TIMER_T3ISCR; //��ͨ��1�ж�״̬����Ĵ�������ж�
    *(RP)TIMER_T3ISCR=clear_int;
    printf("In the catpure mode, the current value of count register is %ld \n",*(RP)TIMER_T3CAPR);
}


