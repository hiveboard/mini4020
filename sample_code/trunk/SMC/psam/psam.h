/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  psam.h
 *
 *	File Description: ���ܿ�����ͷ�ļ�
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0               2009.04.25       PROCHIP            
 *   		      	
 **************************************************************************************/
#ifndef  __PSAM_H
#define  __PSAM_H

#include	"sep4020.h"
#include	"intc.h"

#define     MAXLEN          0x64

void Psam1Init(void);
void Psam1ResetCold(void);
void Psam1CommandSend(S8 *command,S32 len);
S32  Psam1Pro(S8 *buffer);
void Psam1StrInit(void);



typedef struct psam
{
	S8 send_data[5];			       //����ͷ
	S32 command_len;			       //�����
	S8 command_data[MAXLEN];         //�����ֽ�
	S8 receive_data[MAXLEN];	       //�����ֽ�
	S32 receive_len;			       //���ճ���
	
}psamcard;


#endif 
