/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  iis.h
 *
 *	File Description: 声明主调函数
 *
 *    Version         Date            Author           
 *------------------------------------------------------------------------------------    
 *     1.0           2009.04.25        lbn            
 *   		      	
 *************************************************************************************/
 
#ifndef _IIS_H
#define _IIS_H


/*IISC control reg*/
#define   MUTE(X)       (X << 25)
#define   RESET_1(X)    (X << 24)
#define   ALIGN(X)      (X << 23)
#define   WS(X)         (X << 22)
#define   WID(X)        (X << 20)
#define   DIR(X)        (X << 19)
#define   MODE(X)       (X << 18)
#define   MONO(X)       (X << 17)
#define   STOP(X)       (X << 16)
#define   DIV(X)         X

/*DMAC control reg*/
#define   TRANSFERSIZE(X)     (X << 14)             //transfer data size
#define   DI(X)               (X << 13)
#define   SI(X)               (X << 12)
#define   DESTSIZE(X)         (X << 9 )
#define   SOURCESIZE(X)       (X << 6 )
#define   DESTBURST(X)        (X << 3)
#define   SOURCEBURST(X)       X 

/****************************************
	macros  for write and read registers
****************************************/
#define write_reg(reg, data) \
	*(RP)reg = data

#define read_reg(reg) \
	*(RP)reg


void delay(int n);
void InitIIS(void);
void InitCodec(void);
void InitDatapath(void);
void InitPWM(void);
void InitDMA(void);

#endif
