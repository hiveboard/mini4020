/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  main.c
 *
 *	File Description:   RTC的主函数
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

#define INT_ALARM 		1       //闹钟测试
#define INT_SECOND 		0       //秒中断测试
#define INT_SAMPLE		0       //捕获测试
#define INT_WATCHDOG	0       //看门狗测试


DATE RTCDate; //定义年月日结构体
TIME RTCTime; //定义时分秒结构体

int main(void)

{  
  InitRTC(); //RTC初始化


//测试Alarm中断

#if INT_ALARM

  RTCSetAlarm(8,25,12,51);        //设置月，日，时，分
  
  unmask_irq(INTSRC_RTC);         //打开屏蔽中断
  irq_enable(INTSRC_RTC);         //中断使能
  RTCEnableInt(ALARM);            //定时中断使能
  
#endif


// 测试Second中断

#if INT_SECOND
  
  RTCEnableInt(SECOND);           //秒中断使能
  irq_enable(INTSRC_RTC);
  
#endif


// 测试Sample中断

 #if INT_SAMPLE
  
  RTCDisableInt(SAMPLE);
   
  RTCSetSample(0x8000);           //计数时间T=1秒
  
  unmask_irq(INTSRC_RTC);
  irq_enable(INTSRC_RTC);
  RTCEnableInt(SAMPLE);           //采样中断使能        
  
#endif  

  


//测试WatchDog复位

#if INT_WATCHDOG
 
  RTCDisableInt(WATCHDOG);
 
  RTCSetWatchDog(0x0F);           //WatchDog计数值
  RTCWatchDogInit(0,0x00,0);      //时钟源，分频因子，复位类型
  RTCEnableInt(WATCHDOG);         //使能WATCHDOG中断
  RTCEnableInt(RESET);            //RESET使能
  
  unmask_irq(INTSRC_RTC);
  irq_enable(INTSRC_RTC);    
  RTCEnableWatchDog();            //WatchDog使能      
  
#endif
  
  while(1);
 
 return 0; //为了消除警告
}







	

