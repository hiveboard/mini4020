/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  sdram.c
 *
 *	File Description:   SDRAM的函数文件
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
*             基本数据传输测试
*             把源地址的内容搬运到目的地址,
*             把搬运数据块的大小可定义(SINGLE_TIMES)
*             
* INPUTS:
*             src_addr     源地址
*             dest_addr    目的地址
* OUTPUTS:
*             无
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
*             比较搬运后,目的地址上的内容
*             与源地址上的内容是否一致
* INPUTS:
*             src_addr     源地址
*             dest_addr    目的地址
*             length       所搬运数据的长度
* OUTPUTS:
*             返回flag=E_OK表示成功
*             返回flag=E_HA表示失败 
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








