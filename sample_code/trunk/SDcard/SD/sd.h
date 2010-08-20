/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  sd.h
 *
 *	File Description: sd卡的相关函数声明
 *
 *    Version         Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0           2009.05.08         CH           
 *   		      	
 *************************************************************************************/
#ifndef  __SD_H
#define  __SD_H

#include	<stdio.h>

#include	"sep4020.h"
#include	"intc.h"

#define MEM_BLOCK_BEGIN		0x30200000
#define MEM_BLOCK_END		0x30220000
#define BLOCK_SIZE		    1280
#define END(i)			    (MEM_BLOCK_END+i*(1<<16))

void CompareMem(U32 src, U32 dest,U32 len);
void FifoReset(void);
U32  InitialSd(void);

void SdSingleW1r1(U32 rca);
void SdSingleW1r4(U32 rca);
void SdSingleW4r1(U32 rca);
void SdSingleW4r4(U32 rca);

void SdMultiW1r1(U32 rca);
void SdMultiW1r4(U32 rca);
void SdMultiW4r1(U32 rca);
void SdMultiW4r4(U32 rca);

#endif