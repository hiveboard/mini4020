/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  main.c
 *
 *	File Description: Key ��ʾ����������
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0               2009.04.25       PROCHIP            
 *   		      	
 **************************************************************************************/

#include <stdio.h>

#include "sep4020.h"
#include "intc.h"
#include "key.h"
/*void init_gpio(void)
{
   *(RP)GPIO_PORTD_SEL |= 0x1A;                
   *(RP)GPIO_PORTD_DATA &= ~(0x1A); 
   *(RP)GPIO_PORTD_DIR |= 0x01;
}

void gpio_read(void)
{

    int i=0,data=0;
    unsigned char command=0x90;
    
    init_gpio();
	
    CSL;                            //Ƭѡ�ź����ͣ�ʹ��ADS7846
   
    Delay(40);	
	
    for(i=0; i<8; i++)  
    {
	   CLKL;                        //ʱ���ź�����                                    
	
	   Delay(10);	
		
	   if(command & 0x80)
	       DATAH;                   //����һ������
 	   else 
 	       DATAL;
 	       
 	   Delay(10);                
			
       CLKH;                        //ʱ���ź�����
	
	   Delay(20);
	   
       command <<= 1;	          	
    }
    
    CLKL;
	
    Delay(80);

    for(i=0; i<12; i++)
    {
	   CLKL;                        //ʱ���ź�����
		
       Delay(40);	
		
	   if(*(RP)GPIO_PORTD_DATA&0x1)
	   {
	     data |= 0x1;               //���һ������
       }
		
       data <<= 1;                
		
		
	   CLKH;                        //ʱ���ź�����
		
	   Delay(40);	
    }
    
    CLKL;
	
    Delay(40);	
	
    CSH;                            //SSI_CS1Ƭѡ�ź�����
	
    printf("gpio_read is %x\n",data);    
}

void ssi_read(void)
{
		int i;
		*(RP)GPIO_PORTD_SEL &= ~(0x13);	
	
		CSL();//ʹ��ADS7846
		
		*(RP)SSI_SSIENR =0x0; 
		*(RP)SSI_SER =0x0;    
		*(RP)SSI_CONTROL0 = 0x007;
		*(RP)SSI_SSIENR =0x1; 
		*(RP)SSI_SER =0x2;    
		
		*(RP)SSI_DR=0x90;
		while (!(*(RP)SSI_SR & 0x4));
		while (*(RP)SSI_SR & 0x1);

		*(RP)SSI_SSIENR =0x0; 
		*(RP)SSI_SER =0x0;    
		*(RP)SSI_CONTROL0 = 0x00B;
		*(RP)SSI_SSIENR =0x1; //ʹ��SSI
		*(RP)SSI_SER =0x2;    //ʹ�ܴ��豸����ʼ��������
		*(RP)SSI_DR=0x00;
		while (!(*(RP)SSI_SR & 0x4));
		while (*(RP)SSI_SR & 0x1);
	
		CSH();
		
		printf("SSI_DR is %x\n",*(RP)SSI_DR);


}
*/

int main(void)
{
	printf("Please touch!\n ");
    TouchpanelInit();
    
    //gpio_read();
    
   // mask_irq(INTSRC_EXINT8);
      
    while(1);
    return 0 ;  //���������warning ����Զ���ᵽ������
}
