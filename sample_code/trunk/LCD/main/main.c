/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  main.c
 *
 *	File Description:   lcd��������
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0          2009.04.25            Jack           
 *   		      	
 ***************************************************************************************/
#include "lcdc.h"
#include "sep4020.h"

extern const unsigned char pic[320*240*2];

int main(void)
{

    InitLcdc();                //��ʼ��LCDC�Ŀ��ƼĴ���
    ShowPics(pic);             //ͼƬ��ʾ

    while(1);
    
    return 0;                 //Ϊ����������
      
}