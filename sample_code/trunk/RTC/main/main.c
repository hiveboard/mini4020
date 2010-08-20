/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  main.c
 *
 *	File Description:   RTC��������
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0          2009.04.25           Jack            
 *   		      	
 ***************************************************************************************/
#include	<stdio.h>

#include	"sep4020.h"
#include	"intc.h"
#include	"rtc.h"

#define INT_ALARM 		1       //���Ӳ���
#define INT_SECOND 		0       //���жϲ���
#define INT_SAMPLE		0       //�������
#define INT_WATCHDOG	0       //���Ź�����


DATE RTCDate; //���������սṹ��
TIME RTCTime; //����ʱ����ṹ��

int main(void)

{  
  InitRTC(); //RTC��ʼ��


//����Alarm�ж�

#if INT_ALARM

  RTCSetAlarm(8,25,12,51);        //�����£��գ�ʱ����
  
  unmask_irq(INTSRC_RTC);         //�������ж�
  irq_enable(INTSRC_RTC);         //�ж�ʹ��
  RTCEnableInt(ALARM);            //��ʱ�ж�ʹ��
  
#endif


// ����Second�ж�

#if INT_SECOND
  
  RTCEnableInt(SECOND);           //���ж�ʹ��
  irq_enable(INTSRC_RTC);
  
#endif


// ����Sample�ж�

 #if INT_SAMPLE
  
  RTCDisableInt(SAMPLE);
   
  RTCSetSample(0x8000);           //����ʱ��T=1��
  
  unmask_irq(INTSRC_RTC);
  irq_enable(INTSRC_RTC);
  RTCEnableInt(SAMPLE);           //�����ж�ʹ��        
  
#endif  

  


//����WatchDog��λ

#if INT_WATCHDOG
 
  RTCDisableInt(WATCHDOG);
 
  RTCSetWatchDog(0x0F);           //WatchDog����ֵ
  RTCWatchDogInit(0,0x00,0);      //ʱ��Դ����Ƶ���ӣ���λ����
  RTCEnableInt(WATCHDOG);         //ʹ��WATCHDOG�ж�
  RTCEnableInt(RESET);            //RESETʹ��
  
  unmask_irq(INTSRC_RTC);
  irq_enable(INTSRC_RTC);    
  RTCEnableWatchDog();            //WatchDogʹ��      
  
#endif
  
  while(1);
 
 return 0; //Ϊ����������
}







	

