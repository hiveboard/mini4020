/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  pwm.c
 *
 *	File Description:   PWM�ļ�
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0          2009.04.25           Jack           
 *   		      	
 ***************************************************************************************/
#include	<stdio.h>

#include	"sep4020.h"
#include	"pwm.h"
#include	"intc.h"
    
#define PWM_TEST 	  1    //PWMģʽ����
#define TIMER_TEST	  0    //TIMERģʽ����
#define GPIO_TEST     0    //����GPIOģʽ����
#define INTRPT_TEST   1    //���жϵĲ���

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       PWMInit                                             
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       PWMģ���ʼ��
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong              
*                                                                       
* INPUTS                                                                
*                                                                       
*      ��
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      ��   
*                                                                       
***************************************************************/	

void PWMInit(void)
{
// ����PWM�����������ڡ�ռ�ձȿɵ��ķ���

#if PWM_TEST

	PWMModeSet(0,0x00,0x0,0x00);       /*PWM0������͵�ƽ��δ�ã���GPIO�����PWM��ʽ*/
	PWMSet(0,0x0FF,100,50);            /*PWM0��0xFF��Ƶ������100��ռ�ձ�50��*/

#endif	


// ����TIMERģʽ

#if TIMER_TEST
		
	PWMModeSet(0x0,0x03,0x0,0x03);    /*PWM0,����������ת��GPIO�����ѭ��������ʽ*/
	PWMTimerSet(0,0xFF,100);          /*PWM0��0xFF��Ƶ������100*/

#endif


// ���Ը���GPIOģʽ

#if GPIO_TEST
		
	PWMModeSet(0x0,0x00,0x0,0x01);    /*PWM0,����������ת��δ�ã���GPIO���������GPIO��ʽ*/
	PWMGPIOSet(0,0xFF,0xaaaa);        /*PWM0��0xFF��Ƶ���������0xaaaa*/
	
#endif


// �����ж�

#if INTRPT_TEST
	
	unmask_irq(INTSRC_PWM);           
  	irq_enable(INTSRC_PWM);         
	PWMIntEnable(0);                /*PWM0�ж�ʹ��*/
	
#endif


    PWMEnable(0);                   /*ʹ��PWM0*/

}


/*************************************************************
* FUNCTION                                                              
*                                                                       
*       PWMModeSet                                             
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ����PWMģ��
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong              
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 PwmNum PWMͨ����
*
*		U32 MatchOut TIMERģʽ�£�������ֵ�������ڼĴ���ʱ���״̬
*			00������͵�ƽ
*			01������ߵ�ƽ
*			10�������������
*			11����ת�����ƽ
*
*		U32 GpioDirection GPIO����/���ѡ��
*			0�����
*			1������
*			
*		U32 Function ����ģʽ
*			00��PWMģʽ
*			01������GPIOģʽ
*			10��TIMERģʽ��һ�μ���
*			11��TIMERģʽ��ѭ������
*      
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      ��   
*                                                                       
***************************************************************/	

void PWMModeSet(U32 PwmNum,U32 MatchOut,U32 GpioDirection,U32 Function)    
{
   
	U32 temp,offset;
	
	
	offset = 0x20*PwmNum;
	temp = (MatchOut<<4)|(GpioDirection<<3)|Function;
	write_reg((PWM0_CTRL+offset),temp);
	
	return ;

}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       PWMEnable                                             
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ʹ��PWMģ��
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong              
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 PwmNum PWMͨ����
*   
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      ��   
*
***************************************************************/	                                                                

void PWMEnable(U32 PwmNum)
{
	
	U32 temp;
	
	temp = read_reg(PWM_ENABLE);
	temp |= 1<<PwmNum;
	write_reg(PWM_ENABLE,temp);     /*��Ӧͨ��ʹ��*/
	
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       PWMDisable                                             
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ��ʹ��PWMģ��
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong              
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 PwmNum PWMͨ����
*   
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      ��   
*
***************************************************************/	                                                                

void PWMDisable(U32 PwmNum)
{
	
	U32 temp;
	
	temp = read_reg(PWM_ENABLE);
	temp &= ~(1<<PwmNum);
	write_reg(PWM_ENABLE,temp);      /*��Ӧͨ����ʹ��*/
	
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       PWMSet                                             
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ����PWMģ�飬���ռ�ձȿɵ��ķ���
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong              
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 PwmNum PWMͨ����
*		U32 PwmDiv PWM��Ƶ����
*		U32 PwmPeriod PWM����
*		U32 PwmDuty PWMռ�ձ�
*   
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      ��   
*
***************************************************************/	                                                                

void PWMSet(U32 PwmNum,U32 PwmDiv,U32 PwmPeriod,U32 PwmDuty)
{
	
	U32 temp,offset;
	
	offset = 0x20*PwmNum;
	write_reg((PWM0_DIV+offset),PwmDiv);        
	write_reg((PWM0_PERIOD+offset),PwmPeriod);
	temp = PwmPeriod*PwmDuty/100;
	write_reg((PWM0_DATA+offset),temp);
	
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       PWMTimerSet                                             
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ����PWM Timer
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong              
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 PwmNum PWMͨ����
*		U32 PwmDiv PWM��Ƶ����
*		U32 PwmPeriod PWM����
*   
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      ��   
*
***************************************************************/	                                                                

void PWMTimerSet(U32 PwmNum,U32 TimerDiv,U32 TimerPeriod)
{
	
	U32 offset;
	
	offset = 0x20*PwmNum;
	write_reg((PWM0_DIV+offset),TimerDiv);            
	write_reg((PWM0_PERIOD+offset),TimerPeriod);
	
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       PWMGPIOSet                                             
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ����PWM Timer
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong              
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 PwmNum PWMͨ����
*		U32 PwmDiv PWM��Ƶ����
*		U32 PwmPeriod PWM����
*   
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      ��   
*
***************************************************************/	                                                                

void PWMGPIOSet(U32 PwmNum,U32 PwmDiv,U32 PwmDuty)
{
	
	U32 offset;
	
	offset = 0x20*PwmNum;
	write_reg((PWM0_DIV+offset),PwmDiv);        
	write_reg((PWM0_DATA+offset),PwmDuty);
	
}
/*************************************************************
* FUNCTION                                                              
*                                                                       
*       PWMIntEnable                                             
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ʹ�ܸ�·PWMģ���ж�
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong              
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 PwmNum PWMͨ����
*   
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      ��   
*
***************************************************************/	                                                                

void PWMIntEnable(U32 PwmNum)
{
	
	U32 temp;
	
	temp = read_reg(PWM_INTMASK);
	temp &= ~(1<<PwmNum);
	write_reg(PWM_INTMASK,temp);       /*���ζ�Ӧͨ��*/
	
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       PWMIntDisable                                             
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       �رո�·PWMģ���ж�
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong              
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 PwmNum PWMͨ����
*   
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      ��   
*
***************************************************************/	                                                                

void PWMIntDisable(U32 PwmNum)
{
	
	U32 temp;
	
	temp = read_reg(PWM_INTMASK);
	temp |= 1<<PwmNum;
	write_reg(PWM_INTMASK,temp);        /*�رն�Ӧ�ж�*/
	
}


/*************************************************************
* FUNCTION                                                              
*                                                                       
*       PWMIntHandler                                             
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       PWM�жϴ�����
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong              
*                                                                       
* INPUTS                                                                
*                                                                       
*      ��
*   
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      ��   
*
***************************************************************/	                                                                

void PWMIntHandler(void)
{
	int temp1,temp2,offset;
	int i;
	
	irq_disable(INTSRC_PWM);             /*�ر��ж�*/      
	
	temp1 = read_reg(PWM_INT);
	write_reg(PWM_INT,temp1);           //����жϱ�־
	
	for(i=3;i>=0;i--)
	{
		offset = 0x20*i;
		temp2=read_reg(PWM0_STATUS+offset);
		printf("PWM%d interrupted,the STATUS is %x \n",i,temp2);	
		
	}
	
	irq_enable(INTSRC_PWM);               /*ʹ���ж�*/
 
}