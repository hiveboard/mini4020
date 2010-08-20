/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  key.h
 *
 *	File Description: Key 驱动函数头文件
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0               2009.04.25       PROCHIP            
 *   		      	
 **************************************************************************************/
#ifndef __KEY_H
#define __KEY_H

#include "sep4020.h"
#include "intc.h"

void EnableKey(void);	
void EntIntKey(void);
void DisableKey(void);
void KeyInit(void);
void WriteRow(S32 index,S32 HighLow);
S32 ReadCol(S32 index);
void EntIntKey(void);
void Delay10MicroSeconds(S32 n);

#endif