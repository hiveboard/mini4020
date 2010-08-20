/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  main.c
 *
 *	File Description: ������, ʵ�־��幦��.
 *
 *    Version         Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0           2009.05.08          CH           
 *   		      	
 *************************************************************************************/

#include	<stdio.h>

#include	"sep4020.h"
#include	"intc.h"
#include	"sd.h"


int main(void)
{
    int i;
    U32 head;
    U32 rca;
    
    //**************************��ʼ��һ������****************************************
    head = MEM_BLOCK_BEGIN;      
    for(i=0; i<BLOCK_SIZE; i++)
    {
	*(RP)head = i;
	head = head + 4;
    }
	
    //********************************��ʼ��SD��**************************************
    //�õ�SD����rca��ַ�ţ��������������ʱ��Ҫ�������͵�ַ��
    rca = InitialSd();
     
     
    //********************************��ʼ��FIFO**************************************
    FifoReset();
    

    //********************************��д���Կ�ʼ************************************    
    SdSingleW1r1(rca);			//���� һλдһλ��
  
    SdSingleW1r4(rca);			//���� һλд��λ��
   
    SdSingleW4r1(rca);			//���� ��λдһλ��
   
    SdSingleW4r4(rca);			//���� ��λд��λ��
  
    SdMultiW1r1(rca); 			//��� һλдһλ��
  
    SdMultiW1r4(rca); 			//��� һλд��λ��
   
    SdMultiW4r1(rca); 			//��� ��λдһλ��

    SdMultiW4r4(rca); 			//��� ��λд��λ��

    while(1);
    
    return 0;
}

