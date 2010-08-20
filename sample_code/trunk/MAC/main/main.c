/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  main.c
 *
 *	File Description: ��������ʵ�־��幦��.
 *
 *    Version         Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0          2009.05.08          CH           
 *   		      	
 *************************************************************************************/
#include	<stdio.h>
#include    <stdlib.h>    
#include    <string.h>

#include	"intc.h"
#include	"sep4020.h"
#include	"mac.h"

// ��������Ҫ�����MAC�ڲ��Ļػ�������������MAC֡��Ȼ��Է����������ͽ������������㣬 
// �������÷����������ͽ�����������
// �������MAC���ƼĴ������ûػ�ģʽ��

int main(void)
{  
   
    //============= MAC LOOPBACK AT INTERAL =========//	
    phy_hard_reset(1);
    delay (10000);
    
    phy_hard_reset(0);
    delay (10000);
    
    phy_hard_reset(1);          //��λ
	delay (10000);
	
    mii_reg_read();             //��ȡphy�Ĵ���

    phy_selfloop_back();        //�ػ�����
	
    while(1);
}

