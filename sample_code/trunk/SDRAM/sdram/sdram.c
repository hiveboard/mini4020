/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  sdram.c
 *
 *	File Description:   SDRAM�ĺ����ļ�
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0          2009.04.25            Jack           
 *   		      	
 ***************************************************************************************/
#include "sep4020.h"
#include "sdram.h"


/***************************************************
* FUNCTION :
*             BasicDataTran
*
*  DESCRIPTION:
*             �������ݴ������
*             ��Դ��ַ�����ݰ��˵�Ŀ�ĵ�ַ,
*             �Ѱ������ݿ�Ĵ�С�ɶ���(SINGLE_TIMES)
*             
* INPUTS:
*             src_addr     Դ��ַ
*             dest_addr    Ŀ�ĵ�ַ
* OUTPUTS:
*             ��
***************************************************/



void BasicDataTran(U32 src_addr, U32 dest_addr,U32 length)
{
    U32 i;
    
   for (i=0; i<length; i++)
    {
  	    *(RP16)(dest_addr+2*i) = *(RP16)(src_addr+2*i);
    }
    
    
}

/***************************************************
* FUNCTION :
*             COMPARE_MEM
*
* DESCRIPTION:
*             �Ƚϰ��˺�,Ŀ�ĵ�ַ�ϵ�����
*             ��Դ��ַ�ϵ������Ƿ�һ��
* INPUTS:
*             src_addr     Դ��ַ
*             dest_addr    Ŀ�ĵ�ַ
*             length       ���������ݵĳ���
* OUTPUTS:
*             ����flag=E_OK��ʾ�ɹ�
*             ����flag=E_HA��ʾʧ�� 
***************************************************/

U32 COMPARE_MEM(U32 src_addr, U32 dest_addr, U32 length)
{
    U32 i;
    U32 flag = E_OK;

    for(i=0; i<length; i++)
    {
        if(*(RP16)(src_addr+i*2) != *(RP16)(dest_addr+i*2))
        {
    	    flag = E_HA;
    	    
    	   // break;
        }
    }
    return flag;  
}








