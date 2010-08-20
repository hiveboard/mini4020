/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  ssi.h
 *
 *	File Description: ssi 驱动头文件
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0               2009.04.25       PROCHIP            
 *   		      	
 **************************************************************************************/
#ifndef  __SSI_H
#define  __SSI_H

void SsiDisable(void);
void SsiEnable(void);
void SsiSpiCs0init(void);
U8 AMDFlashRDSR(void);
U8 AMDFlashIDREAD(void);
void AMDFlashSE(U32 address);
void AMDFlashPP(U32 address,char *buff,U32 len);
void AMDFlashREAD(U32 address,char *buff,U32 le);
void Delay10MicroSeconds(S32 n);

#endif