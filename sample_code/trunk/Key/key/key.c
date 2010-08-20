/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  key.c
 *
 *	File Description: Key ��������
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0               2009.04.25       PROCHIP            
 *   		      	
 **************************************************************************************/
#include <stdio.h>

#include "Key.h"
#include"intc.h"


//����ӳ�䣬�Ͱ�����һһ��Ӧ

S8 *KeyMap[]={
                        "*", "Enter",        "Exit",     "Trade",   "F4",
                        "#", "GetPaper", "Delect", "Right",   "F3",
                        "9", "6",             "3",         "Down",  "Up",
                        "8", "5",             "2",         "Left",     "F2",
                        "7", "4",             "1",         "0",         "F1"
                        };


/************************************************************************************
* ������:	   EnableKey
* ��ڲ���:   ��
* ���ز���:   ��
* ��������:   ʹ�ܰ����ж� 
*************************************************************************************/
void EnableKey(void)
{
    unmask_irq(INTSRC_EXINT0);
    unmask_irq(INTSRC_EXINT1);
    unmask_irq(INTSRC_EXINT2);
    unmask_irq(INTSRC_EXINT3);	
    unmask_irq(INTSRC_EXINT4);
	
    irq_enable(INTSRC_EXINT0);
    irq_enable(INTSRC_EXINT1);
    irq_enable(INTSRC_EXINT2);
    irq_enable(INTSRC_EXINT3);
    irq_enable(INTSRC_EXINT4);
}	

/************************************************************************************
* ������:	   DisableKey
* ��ڲ���:   ��
* ���ز���:   ��
* ��������:   ���ΰ����ж� 
*************************************************************************************/
void DisableKey(void)
{
    mask_irq(INTSRC_EXINT0);
    mask_irq(INTSRC_EXINT1);
    mask_irq(INTSRC_EXINT2);
    mask_irq(INTSRC_EXINT3);
    mask_irq(INTSRC_EXINT4);
}	

/************************************************************************************
* ������:	   KeyInit
* ��ڲ���:   ��
* ���ز���:   ��
* ��������:   ������ʼ�������� ���İ����ӿ����ڴ˺����޸�
*************************************************************************************/
void KeyInit(void)                 //ͳһ���   �Ĵ�����д               
{
    DisableKey();
        
    *(RP)GPIO_PORTD_SEL  |= 0x1F ;                  //����GPIO D �飬76��77��78��79��80 ����Ϊͨ����;
    *(RP)GPIO_PORTD_DIR  &= (~0x1F) ;           //����76��77��78��79��80 Ϊ�������	
    *(RP)GPIO_PORTD_DATA &= (~0x1F) ;        //����͵�ƽ
    
    *(RP)GPIO_PORTA_SEL |= 0x001F ;             //����GPIO A ��33��34��35��36��37����Ϊͨ����;
    *(RP)GPIO_PORTA_DIR |= 0x001F ;            // ����Ϊ����
    *(RP)GPIO_PORTA_INTRCTL |= 0x03FF ;       //�ж������ǵ͵�ƽ����
    *(RP)GPIO_PORTA_INCTL |= 0x001F ;       //���뷽ʽΪ�ж�����
    *(RP)GPIO_PORTA_INTRCLR |= 0x001F ;      //����ж�
    *(RP)GPIO_PORTA_INTRCLR = 0x0000 ;     //����ж�	
    
    EnableKey();    
    return ;	
      
}

/************************************************************************************
* ������:	   WriteRow
* ��ڲ���:   index �кţ�HighLow ��ƽ 
* ���ز���:   ��
* ��������:   ���ð�������ŵ�ƽ
*************************************************************************************/
void WriteRow(S32 index,S32 HighLow)
{ 
    switch(index)
    {
        case 0:
            if(HighLow) *(RP)GPIO_PORTD_DATA |= 0x01;else *(RP)GPIO_PORTD_DATA &= ~0x01;
            break;
        case 1: 
            if(HighLow) *(RP)GPIO_PORTD_DATA |= 0x02;else *(RP)GPIO_PORTD_DATA &= ~0x02;
            break;
        case 2:
            if(HighLow) *(RP)GPIO_PORTD_DATA |= 0x04;else *(RP)GPIO_PORTD_DATA &= ~0x04;
            break;
        case 3:
            if(HighLow) *(RP)GPIO_PORTD_DATA |= 0x08;else *(RP)GPIO_PORTD_DATA &= ~0x08;
            break;
        case 4:
            if(HighLow) *(RP)GPIO_PORTD_DATA |= 0x10;else *(RP)GPIO_PORTD_DATA &= ~0x10;
            break;
        default:
            break;
    }
        
    return ;
}

/************************************************************************************
* ������:	   ReadCol
* ��ڲ���:   index �к� 
* ���ز���:   �������ƽֻ
* ��������:   ��ȡ�������ƽ
*************************************************************************************/
S32 ReadCol(S32 index)										
{
    return (( *(RP)GPIO_PORTA_DATA >>  index   ) & 0x1);
}

/************************************************************************************
* ������:	   EntIntKey
* ��ڲ���:   ��
* ���ز���:   ��
* ��������:   �����жϴ���
*************************************************************************************/
void EntIntKey(void)  //�ӷ��� 0506 

{
    S32 Keynum = 0;  //��ֵ
    S32 i = 0;
    S32 j = 0;
    S32 col = 0, row = 0;
    S32 InitCol=0;
    S32 ColCount=0,RowCount=0;
    
    DisableKey();	//�����ΰ�����������ʶ��
    
    //����ж�
    *(RP)GPIO_PORTA_INTRCLR |= 0x001F;            //����ж�
    *(RP)GPIO_PORTA_INTRCLR  = 0x0000;           //����ж�
    
    //ͨ����ȡ�ж�״̬�Ĵ����ж���һ�в����ж�
    InitCol = (*(RP)(INTC_ISR)>>1) & 0x001F ; 
    
    for( i = 0 ; i < 5 ; i ++)
    {
        if( (InitCol >> i) & 0x01 )
        {
            col = i ;
            ColCount ++;
        }
    }


	//ɨ�裬ͨ��������PD0~PD4 ����ߵ�ƽ����ȡ����
    if ((!ReadCol(col)))
    {
        for(i=0;i<5;i++) 
        {
            WriteRow(i,1);
            Delay10MicroSeconds(10);
            
            if( ReadCol( col ) )
            {
                row = i ;
                RowCount++;
            }
            
        WriteRow(i,0);
        }
    } 

     if(ColCount==1 && RowCount==1)   //ColCount==1 && RowCount==1 ��ʾ�жϽ���
     {
        Keynum = col*5 + row;	                 // �õ��е�ַ,Ȼ��������ֵ
        
        // �ȵ�����̧����ʾһ�ΰ�����ɣ���ӡ����Ӧ�İ�������
        
        j = *(RP)GPIO_PORTA_DATA ; 
        
        while(j != 0x1F)
        {
            Delay10MicroSeconds(10) ;
            j =  *(RP)GPIO_PORTA_DATA ;
        }
        
        printf("0x%ld \n",Keynum );
        printf("Key_value =%s\n",KeyMap[Keynum]);
        
    }
    
    EnableKey(); //������ϣ��򿪰���
    
    return ;

}

/************************************************************************************
* ������:	   Delay10MicroSeconds
* ��ڲ���:   n ��ʱ��λ��
* ���ز���:   ��
* ��������:   ��ʱ��������λʱ��Ϊ10΢�룬�����ڴ�û�в���ϵͳ����ʹ�ò���ϵͳʱ��
                   ����ʹ��ϵͳ��ʱ��������ش�����Ҫ���жϴ����н�����ʱ
*************************************************************************************/
void Delay10MicroSeconds(S32 n)
{
    S32 i;
    
    for(i=0;i<(n*100);i++);
    
    return ;
}