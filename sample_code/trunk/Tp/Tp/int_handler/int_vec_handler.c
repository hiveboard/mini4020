
  /**********************************************************************************
 *	Copyright (c) 2008 by ASIC.
 *	PROPRIETARY RIGHTS of ASIC are involved in the subject matter of this
 *	material.  All manufacturing, reproduction, use, and sales rights
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  int_vec_handler.c
 *
 *	File Description:
 *
 *    Version         Date            Author
 *------------------------------------------------------------------------------------
 *    0.0.1           2008.3.12       QI Haochun 		      	
 *************************************************************************************
*/
#include <stdio.h>
#include "intc.h"
#include "key.h"


INT_VECTOR vector[] =
{
  
     /* interrupt number,   handler */

  {  INTSRC_NULL,             NULL },
  {  INTSRC_EXINT0,           },
  {  INTSRC_EXINT1,          NULL },
  {  INTSRC_EXINT2,          NULL },
  {  INTSRC_EXINT3,         NULL },
  {  INTSRC_EXINT4,          NULL },
  {  INTSRC_EXINT5,          NULL },
  {  INTSRC_EXINT6,          NULL },
  {  INTSRC_EXINT7,          NULL },
  {  INTSRC_EXINT8,          Tp_handler },
  {  INTSRC_EXINT9,          NULL },
  {  INTSRC_EXINT10,        NULL },
  {  INTSRC_SDIO,             NULL },
  {  INTSRC_SMC1,            NULL },
  {  INTSRC_SMC0,            NULL },
  {  INTSRC_USB,               NULL },
  {  INTSRC_NULL,             NULL },
  {  INTSRC_SSI,                NULL },
  {  INTSRC_I2S,                NULL },
  {  INTSRC_LCDC,             NULL },
  {  INTSRC_PWM,             NULL },
  {  INTSRC_UART3,           NULL },
  {  INTSRC_UART2,           NULL },
  {  INTSRC_UART1,           NULL },
  {  INTSRC_UART0,           NULL},
  {  INTSRC_TIMER3,          NULL },
  {  INTSRC_TIMER2,          NULL },
  {  INTSRC_TIMER1,         NULL },
  {  INTSRC_MAC,              NULL },
  {  INTSRC_EMI,               NULL },
  {  INTSRC_DMAC,            NULL },
  {  INTSRC_RTC,               NULL },
};


void int_vector_handler(void)
{
    U32 intnum;
    U8 i = 0;
    
    intnum = *(RP)(INTC_IFSR);  //读取IRQ 中断最终状态寄存器获得中断号
    
    while (intnum != 1)
    {
        intnum = intnum >> 1;
        i++;
    }
    
    (*vector[i].handler)(); //通过调用相应的处理函数
    
    return ;
}