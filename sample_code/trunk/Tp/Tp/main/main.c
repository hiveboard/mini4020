/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  main.c
 *
 *	File Description: Key 演示程序主函数
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
	
    CSL;                            //片选信号拉低，使能ADS7846
   
    Delay(40);	
	
    for(i=0; i<8; i++)  
    {
	   CLKL;                        //时钟信号拉低                                    
	
	   Delay(10);	
		
	   if(command & 0x80)
	       DATAH;                   //触发一个脉冲
 	   else 
 	       DATAL;
 	       
 	   Delay(10);                
			
       CLKH;                        //时钟信号拉高
	
	   Delay(20);
	   
       command <<= 1;	          	
    }
    
    CLKL;
	
    Delay(80);

    for(i=0; i<12; i++)
    {
	   CLKL;                        //时钟信号拉低
		
       Delay(40);	
		
	   if(*(RP)GPIO_PORTD_DATA&0x1)
	   {
	     data |= 0x1;               //输出一个数据
       }
		
       data <<= 1;                
		
		
	   CLKH;                        //时钟信号拉高
		
	   Delay(40);	
    }
    
    CLKL;
	
    Delay(40);	
	
    CSH;                            //SSI_CS1片选信号拉高
	
    printf("gpio_read is %x\n",data);    
}

void ssi_read(void)
{
		int i;
		*(RP)GPIO_PORTD_SEL &= ~(0x13);	
	
		CSL();//使能ADS7846
		
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
		*(RP)SSI_SSIENR =0x1; //使能SSI
		*(RP)SSI_SER =0x2;    //使能从设备，开始传输命令
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
    return 0 ;  //避免编译器warning ，永远不会到达这里
}
