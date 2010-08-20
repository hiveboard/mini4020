/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  uart.h
 *
 *	File Description: uart 驱动头文件
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0               2009.04.25       PROCHIP            
 *   		      	
 **************************************************************************************/
  #ifndef _UART_H
  #define _UART_H
  
  #include <stdio.h> 
  #include "sep4020.h"
  
  typedef struct _UartPort
  {
    U32 UartAddr ; 
    U32 UartIntNo;
   } UartPort;

void UartInit(U32, U8);
void UartConfig(U32 uart_add,U32 sysclk,U32 baudrate,U32 data_bit,U32 tri_level_t,U32 tri_level_r);
void Pmu(void);
U32  UartHWFlowcontrolTest(U32 sysclk, U32 baudrate, U32 data_bit, U32 tri_level_t, U32 tri_level_r);
void Prints(char *s,U32 uart_add);
void UartIntEn(U32 uart_add, S8 recie, S8 thrie);
U32 RlsIsrUart0(void);
void RdaIsrUart0(void);
void TmoIsrUart0(void);
void ThrIsrUart0(void);
U32 MdsIsrUart0(void);

#endif