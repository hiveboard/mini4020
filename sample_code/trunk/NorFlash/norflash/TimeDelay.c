/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  TimeDelay.c
 *
 *	File Description: NorFlash��ʾ�õ�����ʱ����
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0               2009.04.25       PROCHIP            
 *   		      	
 **************************************************************************************/
#include "sep4020.h"

extern void Delay10MicroSeconds(S32 n);  
extern void Delay30NanoSeconds(S32 n); 

/************************************************************************************
* ������:	   Delay10MicroSeconds 
* ��ڲ���:   ��
* ���ز���:   �� 
* ��������:   ��ʱ��������С��λ10΢�� 
*************************************************************************************/
extern void Delay10MicroSeconds(S32 n)
{
    S32 i;
    for(i=0;i<(n*100);i++);	
}


/************************************************************************************
* ������:	   Delay30MicroSeconds 
* ��ڲ���:   ��
* ���ز���:   �� 
* ��������:   ��ʱ��������С��λ30΢�� 
*************************************************************************************/
extern void Delay30NanoSeconds(S32 n)
{
    S32 i;
    for(i=0;i<n;i++);
}