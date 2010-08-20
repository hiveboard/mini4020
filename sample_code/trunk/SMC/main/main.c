/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  main.c
 *
 *	File Description: ���ܿ���ʾ����������
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0               2009.04.25       PROCHIP            
 *   		      	
 **************************************************************************************/
#include <stdio.h>

#include "psam.h"
#include "sep4020.h"
#include "intc.h"

psamcard psam1;
S8 back_respone[80];

int main(void)
{
    S32	re_value,i;
    S8 com1[]={0x00,0x84,0x00,0x00,0x04};				// ȡ���������
    S8 com2[]={0x00,0xA4,0x00,0x00,0x02,0x3F,0x00};	        // ѡ���ļ�����
    
    Psam1StrInit();									// �ṹ��ĳ�ʼ��
    Psam1Init();										// SMCģ��Ĵ�����ʼ��
    
    // �临λ����
    printf("Ready to reset card!\n" );
    Psam1ResetCold();
    printf("recevie data:\n");
    
    for (i = 0;i<psam1.receive_len;i++)
    {
        printf(" 0x%x", psam1.receive_data[i]);
    }
    
    printf("\n");
    printf("Cold reset card complete!!\n\n" );
    
    // �������������
    printf("Send get challenge command:\n" );
    re_value = Psam1Pro(com1);
    printf("recevie data:\n");
    
    for (i = 0;i<psam1.receive_len;i++)
    {
        printf(" 0x%x", psam1.receive_data[i]);
    }
    
    printf("\n");
    printf("Test get challenge command OK!!\n\n" );
    
    // ����ѡ�����ļ�����
    printf("Send select main file command:\n" );
    re_value = Psam1Pro(com2);
    printf("recevie data:\n");
    
    for (i = 0;i<psam1.receive_len;i++)
    {
        printf(" 0x%x", psam1.receive_data[i]);
    }
    
    printf("\n");
    printf("Select main file command OK!!\n\n" );
    
    // ���Խ���
    printf("Test done!\n");	
    
    while(1);
    return 0;    //������������ܵľ��棬���򲻻����е�����
    
}