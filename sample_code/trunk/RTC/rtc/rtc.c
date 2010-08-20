/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  main.c
 *
 *	File Description:   RTC函数
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
*       初始化RTC模块
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong              
*                                                                       
* INPUTS                                                                
*                                                                       
*      	无       
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      无   
*                                                                       
***************************************************************/	

void InitRTC(void)    
{

  RTCSetDate(2008,8,25);          //设置年 ，月，日
  RTCSetTime(12,50,00);           //设置时，分，秒

}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCSetDate                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       设定RTC的年、月、日
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      U32 year:	年
*	   U32 month:	月
*	   U32 day:		日                
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
	write_reg(RTC_CONFIG_CHECK , 0xaaaaaaaa);             /*确认配置时间*/
	write_reg(RTC_STA_YMD , temp);                        /*写入年月日*/                    
	write_reg(RTC_CONFIG_CHECK , 0x00000000);             /*关闭配置时间确认寄存器*/     

	return 0;	
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCGetDate                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       获得RTC的年、月、日
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      struct RTCDateStruct * date 日期结构体指针                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     无               
*                                                                       
***************************************************************/

void RTCGetDate(DATE * pdate)	
{
	U32 temp;
	
	temp=read_reg(RTC_STA_YMD);                             //获取日期          
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
*       设定RTC的时、分、秒
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      U32 hour:	小时
*	   U32 minute:	分钟
*	   U32 second:	秒钟                
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
	write_reg(RTC_CONFIG_CHECK, 0xaaaaaaaa);              //确认配置时间
	write_reg(RTC_STA_HMS, temp);                         //写入时、分、秒          
	write_reg(RTC_CONFIG_CHECK, 0x00000000);              //关闭配置时间确认寄存器

	return 0;
}	

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCGetTime                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       获取RTC的时、分、秒
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      struct RTCTimeStruct * time 时间结构体指针                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      无               
*                                                                       
***************************************************************/

void RTCGetTime( TIME * ptime )
{
	U32 temp;
	
	temp=read_reg(RTC_STA_HMS);                            //获取时间
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
*      	设定RTC的定时器
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
*     	无                
*                                                                       
***************************************************************/

void RTCSetAlarm(U32 month,U32 day,U32 hour, U32 minute)
{
	U32 temp;
	
	temp = (month<<28)+(day<<23)+(hour<<6) + minute;
	write_reg(RTC_ALARM_ALL, temp);                   //定时
	
}


/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCSetSample                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	设定RTC的Sample计数值
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 frequency采样计数值。
*			计数时间 T = frequency/32768秒。
*			frequency = 0x0080， T = 1/256秒
*			frequency = 0x8000， T = 1秒
*			frequency = 0x0001， T = 0秒
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	无                
*                                                                       
***************************************************************/

void RTCSetSample(U32 frequency)
{
	U32 temp;
	
	temp = (frequency<<16)+frequency;
	write_reg(RTC_SAMP, temp);              //采样计数    
	RTCEnableSample();                      //使能采样
               
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCEnableInt                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	使能RTC的中断
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	int_num:中断号
*				0:Sample
*				1:WatchDog
*               2:Second
*				3:Minute
*				4:Alarm
*				5:Reset
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	无                
*                                                                       
***************************************************************/

void RTCEnableInt(U32 int_num)
{
	U32 temp;
	
	temp =read_reg(RTC_INT_EN);
	temp |=1<<int_num;
	write_reg(RTC_INT_EN,temp);             //中断使能
	
}


/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCDisableInt                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	不使能RTC的中断
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	int_num:中断号
*				0:Sample
*				1:WatchDog
*               2:Second
*				3:Minute
*				4:Alarm
*				5:Reset
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	无                
*                                                                       
***************************************************************/

void RTCDisableInt(U32 int_num)
{
	U32 temp;
	
	temp = read_reg(RTC_INT_EN);
	temp &= ~(1<<int_num);
	write_reg(RTC_INT_EN,temp);              //中断不使能

}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCEnableSample                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	使能RTC的Sample功能
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	无
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	无                
*                                                                       
***************************************************************/

void RTCEnableSample(void)
{
	U32 temp;
	
	temp = read_reg(RTC_CTR);
	temp |= 0x1;
	write_reg(RTC_CTR,temp);                     //采样使能

}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCDisableSample                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	不使能RTC的Sample功能
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	无
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	无                
*                                                                       
***************************************************************/

void RTCDisableSample(void)
{
	U32 temp;
	
	temp = read_reg(RTC_CTR);
	temp &= ~0x1;
	write_reg(RTC_CTR,temp);                      //采样不使能  

}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCWatchDogInit                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	初始化WatchDog
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 ClockSource: WatchDog时钟源
*			0：RTC时钟（32.768k)
*			1：系统分频时钟
*		U32 ClockDivider：系统时钟分频因子
*
*		U32 ResetType：复位类型
*			0：WatchDog中断一次后复位
*			1：WatchDog中断两次后复位
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	无                
*                                                                       
***************************************************************/

void RTCWatchDogInit(U32 ClockSource,U32 ClockDivider,U32 ResetType)
{
	U32 temp;
	
	temp = read_reg(RTC_CTR);
	temp &= 0x7;
	temp |=((ClockSource<<24)|(ClockDivider<<8)|(ResetType<<3));
	write_reg(RTC_CTR,temp);               //初始化WatchDog

}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCWatchDogPause                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	暂停WatchDog
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 pause 
*			1：暂停
*			0：继续
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	无                
*                                                                       
***************************************************************/

void RTCWatchDogPause(U32 pause)
{
	U32 temp;
	
	temp = read_reg(RTC_CTR);
	pause?(temp |= 0x4) : (temp &= ~0x4);
	write_reg(RTC_CTR,temp);               //暂停WatchDog

}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCEnableWatchDog                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	使能RTC的WatchDog功能
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	无
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	无                
*                                                                       
***************************************************************/

void RTCEnableWatchDog(void)
{
	U32 temp;
	
	temp = read_reg(RTC_CTR);
	temp |= 0x2;
	write_reg(RTC_CTR,temp);              //使能WatchDog

}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCDisableWatchDog                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	不使能RTC的WatchDog功能
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	无
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	无                
*                                                                       
***************************************************************/

void RTCDisableWatchDog(void)
{
	U32 temp;
	
	temp = read_reg(RTC_CTR);
	temp &= ~0x2;
	write_reg(RTC_CTR,temp);                //停止WatchDog

}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCSetWatchDog                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	设定RTC的WatchDog计数值
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 watchdog WatchDog计数值。
*			
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	无                
*                                                                       
***************************************************************/

void RTCSetWatchDog(U32 watchdog)
{
	U32 temp;
	
	temp = (watchdog<<24)+watchdog;
	write_reg(RTC_WD_CNT, temp);            //WatchDog计数值

}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCWatchDogService                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	设定RTC的WatchDog,喂狗
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	无
*			
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	无                
*                                                                       
***************************************************************/

void RTCWatchDogService(void)
{
		
	U32 temp;
	
	temp = read_reg(RTC_CTR);
	temp |= (1<<4);
	write_reg(RTC_CTR,temp);               //WatchDog服务
	
}

/**************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCSetKey                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	设定RTC的密钥
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 key：密钥值
*			
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	无                
*                                                                       
***************************************************************/

void RTCSetKey(U32 key)
{
		
	write_reg(RTC_KEY0, key);               //密钥设定
	
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       RTCIntHandler                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	RTC中断处理函数
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	无
*			
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	无                
*                                                                       
***************************************************************/

void RTCIntHandler(void)
{
	U32 temp;
	int i;
	temp=read_reg(RTC_INT_STS);
	write_reg(RTC_INT_STS,temp);       //中断状态
	
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
			RTCWatchDogService();     //喂狗
		}
		
		 
	}
	
	RTCGetTime(&RTCTime);             //获取时间
  
  	printf("%d:%d:%d\n",(int)RTCTime.hour,(int)RTCTime.minute,(int)RTCTime.second);

}