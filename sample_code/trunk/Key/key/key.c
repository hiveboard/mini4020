/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  key.c
 *
 *	File Description: Key 驱动函数
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0               2009.04.25       PROCHIP            
 *   		      	
 **************************************************************************************/
#include <stdio.h>

#include "Key.h"
#include"intc.h"


//按键映射，和按键号一一对应

S8 *KeyMap[]={
                        "*", "Enter",        "Exit",     "Trade",   "F4",
                        "#", "GetPaper", "Delect", "Right",   "F3",
                        "9", "6",             "3",         "Down",  "Up",
                        "8", "5",             "2",         "Left",     "F2",
                        "7", "4",             "1",         "0",         "F1"
                        };


/************************************************************************************
* 函数名:	   EnableKey
* 入口参数:   无
* 返回参数:   无
* 函数功能:   使能按键中断 
*************************************************************************************/
void EnableKey(void)
{
    unmask_irq(INTSRC_EXINT0);
    unmask_irq(INTSRC_EXINT1);
    unmask_irq(INTSRC_EXINT2);
    unmask_irq(INTSRC_EXINT3);	
    unmask_irq(INTSRC_EXINT4);
	
    irq_enable(INTSRC_EXINT0);
    irq_enable(INTSRC_EXINT1);
    irq_enable(INTSRC_EXINT2);
    irq_enable(INTSRC_EXINT3);
    irq_enable(INTSRC_EXINT4);
}	

/************************************************************************************
* 函数名:	   DisableKey
* 入口参数:   无
* 返回参数:   无
* 函数功能:   屏蔽按键中断 
*************************************************************************************/
void DisableKey(void)
{
    mask_irq(INTSRC_EXINT0);
    mask_irq(INTSRC_EXINT1);
    mask_irq(INTSRC_EXINT2);
    mask_irq(INTSRC_EXINT3);
    mask_irq(INTSRC_EXINT4);
}	

/************************************************************************************
* 函数名:	   KeyInit
* 入口参数:   无
* 返回参数:   无
* 函数功能:   按键初始化函数， 更改按键接口请在此函数修改
*************************************************************************************/
void KeyInit(void)                 //统一风格   寄存器读写               
{
    DisableKey();
        
    *(RP)GPIO_PORTD_SEL  |= 0x1F ;                  //配置GPIO D 组，76、77、78、79、80 引脚为通用用途
    *(RP)GPIO_PORTD_DIR  &= (~0x1F) ;           //配置76、77、78、79、80 为输出引脚	
    *(RP)GPIO_PORTD_DATA &= (~0x1F) ;        //输出低电平
    
    *(RP)GPIO_PORTA_SEL |= 0x001F ;             //配置GPIO A 组33，34，35，36，37引脚为通用用途
    *(RP)GPIO_PORTA_DIR |= 0x001F ;            // 配置为输入
    *(RP)GPIO_PORTA_INTRCTL |= 0x03FF ;       //中断类型是低电平触发
    *(RP)GPIO_PORTA_INCTL |= 0x001F ;       //输入方式为中断输入
    *(RP)GPIO_PORTA_INTRCLR |= 0x001F ;      //清除中断
    *(RP)GPIO_PORTA_INTRCLR = 0x0000 ;     //清除中断	
    
    EnableKey();    
    return ;	
      
}

/************************************************************************************
* 函数名:	   WriteRow
* 入口参数:   index 行号，HighLow 电平 
* 返回参数:   无
* 函数功能:   设置按键输出脚电平
*************************************************************************************/
void WriteRow(S32 index,S32 HighLow)
{ 
    switch(index)
    {
        case 0:
            if(HighLow) *(RP)GPIO_PORTD_DATA |= 0x01;else *(RP)GPIO_PORTD_DATA &= ~0x01;
            break;
        case 1: 
            if(HighLow) *(RP)GPIO_PORTD_DATA |= 0x02;else *(RP)GPIO_PORTD_DATA &= ~0x02;
            break;
        case 2:
            if(HighLow) *(RP)GPIO_PORTD_DATA |= 0x04;else *(RP)GPIO_PORTD_DATA &= ~0x04;
            break;
        case 3:
            if(HighLow) *(RP)GPIO_PORTD_DATA |= 0x08;else *(RP)GPIO_PORTD_DATA &= ~0x08;
            break;
        case 4:
            if(HighLow) *(RP)GPIO_PORTD_DATA |= 0x10;else *(RP)GPIO_PORTD_DATA &= ~0x10;
            break;
        default:
            break;
    }
        
    return ;
}

/************************************************************************************
* 函数名:	   ReadCol
* 入口参数:   index 列号 
* 返回参数:   列输出电平只
* 函数功能:   读取列输出电平
*************************************************************************************/
S32 ReadCol(S32 index)										
{
    return (( *(RP)GPIO_PORTA_DATA >>  index   ) & 0x1);
}

/************************************************************************************
* 函数名:	   EntIntKey
* 入口参数:   无
* 返回参数:   无
* 函数功能:   按键中断处理
*************************************************************************************/
void EntIntKey(void)  //加防抖 0506 

{
    S32 Keynum = 0;  //键值
    S32 i = 0;
    S32 j = 0;
    S32 col = 0, row = 0;
    S32 InitCol=0;
    S32 ColCount=0,RowCount=0;
    
    DisableKey();	//先屏蔽按键再做按键识别
    
    //清除中断
    *(RP)GPIO_PORTA_INTRCLR |= 0x001F;            //清除中断
    *(RP)GPIO_PORTA_INTRCLR  = 0x0000;           //清除中断
    
    //通过读取中断状态寄存器判断哪一列产生中断
    InitCol = (*(RP)(INTC_ISR)>>1) & 0x001F ; 
    
    for( i = 0 ; i < 5 ; i ++)
    {
        if( (InitCol >> i) & 0x01 )
        {
            col = i ;
            ColCount ++;
        }
    }


	//扫描，通过对轮流PD0~PD4 输出高电平；获取行数
    if ((!ReadCol(col)))
    {
        for(i=0;i<5;i++) 
        {
            WriteRow(i,1);
            Delay10MicroSeconds(10);
            
            if( ReadCol( col ) )
            {
                row = i ;
                RowCount++;
            }
            
        WriteRow(i,0);
        }
    } 

     if(ColCount==1 && RowCount==1)   //ColCount==1 && RowCount==1 表示判断结束
     {
        Keynum = col*5 + row;	                 // 得到列地址,然后计算出键值
        
        // 等到按键抬起后表示一次按键完成，打印出相应的按键功能
        
        j = *(RP)GPIO_PORTA_DATA ; 
        
        while(j != 0x1F)
        {
            Delay10MicroSeconds(10) ;
            j =  *(RP)GPIO_PORTA_DATA ;
        }
        
        printf("0x%ld \n",Keynum );
        printf("Key_value =%s\n",KeyMap[Keynum]);
        
    }
    
    EnableKey(); //处理完毕，打开按键
    
    return ;

}

/************************************************************************************
* 函数名:	   Delay10MicroSeconds
* 入口参数:   n 延时单位数
* 返回参数:   无
* 函数功能:   延时函数，单位时间为10微秒，由于在此没有操作系统，再使用操作系统时，
                   建议使用系统定时器进行相关处理，不要再中断处理中进行延时
*************************************************************************************/
void Delay10MicroSeconds(S32 n)
{
    S32 i;
    
    for(i=0;i<(n*100);i++);
    
    return ;
}