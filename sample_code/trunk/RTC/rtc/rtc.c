/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  main.c
 *
 *	File Description:   RTC����
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0          2009.04.25            Jack            
 *   		      	
 ***************************************************************************************/
#include	<stdio.h>
#include	<string.h>

#include	"sep4020.h"
#include	"intc.h"
#include    "rtc.h"


extern DATE RTCDate;
extern TIME RTCTime;

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       InitRTC                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ��ʼ��RTCģ��
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

void InitRTC(void)    
{

  RTCSetDate(2008,8,25);          //������ ���£���
  RTCSetTime(12,50,00);           //����ʱ���֣���

}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCSetDate                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       �趨RTC���ꡢ�¡���
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      U32 year:	��
*	   U32 month:	��
*	   U32 day:		��                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      -1:error
*       0:ok                
*                                                                       
***************************************************************/

int RTCSetDate(U32 year, U32 month, U32 day)		
{
	U32 temp;
	
	if(year >2047 || month >12|| day > 31)
	{
		return (-1);
	}
	
	temp = (year << 9) + (month << 5) +(day);
	write_reg(RTC_CONFIG_CHECK , 0xaaaaaaaa);             /*ȷ������ʱ��*/
	write_reg(RTC_STA_YMD , temp);                        /*д��������*/                    
	write_reg(RTC_CONFIG_CHECK , 0x00000000);             /*�ر�����ʱ��ȷ�ϼĴ���*/     

	return 0;	
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCGetDate                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ���RTC���ꡢ�¡���
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      struct RTCDateStruct * date ���ڽṹ��ָ��                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     ��               
*                                                                       
***************************************************************/

void RTCGetDate(DATE * pdate)	
{
	U32 temp;
	
	temp=read_reg(RTC_STA_YMD);                             //��ȡ����          
	pdate->day = temp & 0x1F;
	pdate->month = (temp>>5) & 0x0F;
	pdate->year = temp>>9;		
	
}
/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCSetTime                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       �趨RTC��ʱ���֡���
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      U32 hour:	Сʱ
*	   U32 minute:	����
*	   U32 second:	����                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      -1:error
*       0:ok                
*                                                                       
***************************************************************/

int RTCSetTime( U32 hour,U32 minute,U32 second)
{
	U32 temp;
	
	if(hour > 24 || minute > 60 || second > 60)
	{
		return (-1);
	}
	
	temp = (hour << 12) + (minute << 6) + second;
	write_reg(RTC_CONFIG_CHECK, 0xaaaaaaaa);              //ȷ������ʱ��
	write_reg(RTC_STA_HMS, temp);                         //д��ʱ���֡���          
	write_reg(RTC_CONFIG_CHECK, 0x00000000);              //�ر�����ʱ��ȷ�ϼĴ���

	return 0;
}	

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCGetTime                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ��ȡRTC��ʱ���֡���
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      struct RTCTimeStruct * time ʱ��ṹ��ָ��                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      ��               
*                                                                       
***************************************************************/

void RTCGetTime( TIME * ptime )
{
	U32 temp;
	
	temp=read_reg(RTC_STA_HMS);                            //��ȡʱ��
	ptime->second = temp & 0x3F;
	ptime->minute = (temp>>6) & 0x3F;
	ptime->hour = temp>>12;
}	
/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCSetAlarm                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	�趨RTC�Ķ�ʱ��
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 month
*		U32 day
*		U32 hour
*		U32 minute                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	��                
*                                                                       
***************************************************************/

void RTCSetAlarm(U32 month,U32 day,U32 hour, U32 minute)
{
	U32 temp;
	
	temp = (month<<28)+(day<<23)+(hour<<6) + minute;
	write_reg(RTC_ALARM_ALL, temp);                   //��ʱ
	
}


/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCSetSample                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	�趨RTC��Sample����ֵ
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 frequency��������ֵ��
*			����ʱ�� T = frequency/32768�롣
*			frequency = 0x0080�� T = 1/256��
*			frequency = 0x8000�� T = 1��
*			frequency = 0x0001�� T = 0��
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	��                
*                                                                       
***************************************************************/

void RTCSetSample(U32 frequency)
{
	U32 temp;
	
	temp = (frequency<<16)+frequency;
	write_reg(RTC_SAMP, temp);              //��������    
	RTCEnableSample();                      //ʹ�ܲ���
               
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCEnableInt                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	ʹ��RTC���ж�
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	int_num:�жϺ�
*				0:Sample
*				1:WatchDog
*               2:Second
*				3:Minute
*				4:Alarm
*				5:Reset
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	��                
*                                                                       
***************************************************************/

void RTCEnableInt(U32 int_num)
{
	U32 temp;
	
	temp =read_reg(RTC_INT_EN);
	temp |=1<<int_num;
	write_reg(RTC_INT_EN,temp);             //�ж�ʹ��
	
}


/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCDisableInt                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	��ʹ��RTC���ж�
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	int_num:�жϺ�
*				0:Sample
*				1:WatchDog
*               2:Second
*				3:Minute
*				4:Alarm
*				5:Reset
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	��                
*                                                                       
***************************************************************/

void RTCDisableInt(U32 int_num)
{
	U32 temp;
	
	temp = read_reg(RTC_INT_EN);
	temp &= ~(1<<int_num);
	write_reg(RTC_INT_EN,temp);              //�жϲ�ʹ��

}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCEnableSample                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	ʹ��RTC��Sample����
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
* OUTPUTS                                                               
*                                                                       
*     	��                
*                                                                       
***************************************************************/

void RTCEnableSample(void)
{
	U32 temp;
	
	temp = read_reg(RTC_CTR);
	temp |= 0x1;
	write_reg(RTC_CTR,temp);                     //����ʹ��

}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCDisableSample                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	��ʹ��RTC��Sample����
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
* OUTPUTS                                                               
*                                                                       
*     	��                
*                                                                       
***************************************************************/

void RTCDisableSample(void)
{
	U32 temp;
	
	temp = read_reg(RTC_CTR);
	temp &= ~0x1;
	write_reg(RTC_CTR,temp);                      //������ʹ��  

}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCWatchDogInit                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	��ʼ��WatchDog
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 ClockSource: WatchDogʱ��Դ
*			0��RTCʱ�ӣ�32.768k)
*			1��ϵͳ��Ƶʱ��
*		U32 ClockDivider��ϵͳʱ�ӷ�Ƶ����
*
*		U32 ResetType����λ����
*			0��WatchDog�ж�һ�κ�λ
*			1��WatchDog�ж����κ�λ
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	��                
*                                                                       
***************************************************************/

void RTCWatchDogInit(U32 ClockSource,U32 ClockDivider,U32 ResetType)
{
	U32 temp;
	
	temp = read_reg(RTC_CTR);
	temp &= 0x7;
	temp |=((ClockSource<<24)|(ClockDivider<<8)|(ResetType<<3));
	write_reg(RTC_CTR,temp);               //��ʼ��WatchDog

}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCWatchDogPause                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	��ͣWatchDog
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 pause 
*			1����ͣ
*			0������
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	��                
*                                                                       
***************************************************************/

void RTCWatchDogPause(U32 pause)
{
	U32 temp;
	
	temp = read_reg(RTC_CTR);
	pause?(temp |= 0x4) : (temp &= ~0x4);
	write_reg(RTC_CTR,temp);               //��ͣWatchDog

}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCEnableWatchDog                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	ʹ��RTC��WatchDog����
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
* OUTPUTS                                                               
*                                                                       
*     	��                
*                                                                       
***************************************************************/

void RTCEnableWatchDog(void)
{
	U32 temp;
	
	temp = read_reg(RTC_CTR);
	temp |= 0x2;
	write_reg(RTC_CTR,temp);              //ʹ��WatchDog

}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCDisableWatchDog                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	��ʹ��RTC��WatchDog����
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
* OUTPUTS                                                               
*                                                                       
*     	��                
*                                                                       
***************************************************************/

void RTCDisableWatchDog(void)
{
	U32 temp;
	
	temp = read_reg(RTC_CTR);
	temp &= ~0x2;
	write_reg(RTC_CTR,temp);                //ֹͣWatchDog

}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCSetWatchDog                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	�趨RTC��WatchDog����ֵ
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 watchdog WatchDog����ֵ��
*			
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	��                
*                                                                       
***************************************************************/

void RTCSetWatchDog(U32 watchdog)
{
	U32 temp;
	
	temp = (watchdog<<24)+watchdog;
	write_reg(RTC_WD_CNT, temp);            //WatchDog����ֵ

}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCWatchDogService                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	�趨RTC��WatchDog,ι��
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

void RTCWatchDogService(void)
{
		
	U32 temp;
	
	temp = read_reg(RTC_CTR);
	temp |= (1<<4);
	write_reg(RTC_CTR,temp);               //WatchDog����
	
}

/**************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCSetKey                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	�趨RTC����Կ
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 key����Կֵ
*			
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	��                
*                                                                       
***************************************************************/

void RTCSetKey(U32 key)
{
		
	write_reg(RTC_KEY0, key);               //��Կ�趨
	
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCIntHandler                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	RTC�жϴ�����
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

void RTCIntHandler(void)
{
	U32 temp;
	int i;
	temp=read_reg(RTC_INT_STS);
	write_reg(RTC_INT_STS,temp);       //�ж�״̬
	
	printf("there is one INT or more!\n");
	
	for(i=5;i>=0;i--)
	{
		if(temp>>i)
		{
			printf("the INT number is %d(0:SAMPLE,1:WATCHDOG,2:SECOND,3:MINUTE,4:ALARM,5:RESET)\n",i);
			temp &=~(1<<i);
		}
		
		if(i==1)
		{
			RTCWatchDogService();     //ι��
		}
		
		 
	}
	
	RTCGetTime(&RTCTime);             //��ȡʱ��
  
  	printf("%d:%d:%d\n",(int)RTCTime.hour,(int)RTCTime.minute,(int)RTCTime.second);

}