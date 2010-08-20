/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  mac.c
 *
 *	File Description: mac���ܺ���.
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

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       void delay (U32 j)                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ��ʱ
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS                                                                
*       U32 j                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*       ��                  
*                                                                       
**************************************************************/

void delay (U32 j)
{ 
  U32 i;
  
  for (i=0; i<j; i++);
  {};

}
  
  
/****************************************
	macros  for write and read registers
****************************************/

#define write_reg(reg, data) \
        *(RP)reg = data
   
	
#define read_reg(reg)   *(RP)reg

//**********************************************//
//�ó�����Ϊ����������һ��MAC֡ ǰ��6���ֽ�ΪĿ��֡��MAC��ַ �������ֽ�Ϊ����MAC�ĵ�ַ ����Ϊ���� 
void packet_creat(U32 len)
{
	U32 i;
	U8 *point;
	U8 *p = (U8 *)malloc(sizeof(char)*len);  	        //SET A NEW MEMERY ROOM
	point = p;
	
	if(p == NULL) exit(1);
	
	memset(p,0x00,sizeof(char)*len);  			//INITIAL THE DEFAULT VALUE
	
	p[i++]=0x00;
	p[i++]=0x17;
	p[i++]=0x31;
	p[i++]=0x17;
	p[i++]=0x9B;
	p[i++]=0xD2;
	
	for(i=6; i<12; i++)p[i] = 0x05;  				
	
	// set packet length
	*(RP16)(p + 12) = len-14;
    
	for(i=14; i<len; i++)      
	{
	  p[i] = 0xaa;
	}
	 
	for(i=0; i<len; i++)      
	{
	    *(RP8)(MAC_TXRAM+i) = *point++;
	}
}



void phy_hard_reset( int mii_hard_rst)    
{
  if (mii_hard_rst == 1)
    {
      *(RP)GPIO_PORTH_SEL  |= 0X0040;              //ͨ����;�������üĴ���                
      *(RP)GPIO_PORTH_DATA |= 0X0040; 
      *(RP)GPIO_PORTH_DIR  &= (~0X0040);           //ͨ����;���

                              
    }
  else 
    {
      *(RP)GPIO_PORTH_SEL  |= 0X0040;
      *(RP)GPIO_PORTH_DATA &= (~0X0040); 
      *(RP)GPIO_PORTH_DIR  &= (~0X0040);
      

    }

}


void mii_reg_read()
{
	int i;
	U32 phyreg[25];
	
	for(i=0; i<25; i++)
	{
		write_reg(MAC_MII_ADDRESS, (i<<8) | 0x1f);  //12:8 PHY�ڲ��ļĴ�����ַ    4:0 PHY��ַ

		write_reg(MAC_MII_CMD, 0x02);               //��PHY�Ĵ���
		delay (10000);
		
		phyreg[i] = read_reg(MAC_MII_RXDATA);       //15:0 PHY�Ĵ���������ʱ������ֵ
	}
}


	


//************** write MII reg **************//
//*******************************************//

void write_mii_reg(U16 address, U16 data)   
{

	U32 mii_status;

	write_reg(MAC_MII_ADDRESS, (U32)(address) | 0x1f); //12��8 PHY�ڲ��ļĴ�����ַ  4��0 PHY��ַ
	write_reg(MAC_MII_TXDATA, (U32)data);              //15:0 PHY�Ĵ���д����ʱд���ֵ
	write_reg(MAC_MII_CMD, 0x4);                       //bit 2   дPHY�Ĵ���
	delay(8000);
	
	while(1) 
	{
		mii_status = read_reg(MAC_MII_STATUS);
		if ((mii_status & 0x2) == 0) break;       //��ǰ��д������û����ɣ�MAC��Ҫ�ȴ�BUSYλ�����Ժ��ٽ����µ�PHY�Ĵ�����д��
	}
	
	return;	
}



void phy_selfloop_back()
{
	
    U32 i;
    U32 temp;
	
    packet_creat(0x200);
    
    write_reg(MAC_PACKETLEN, 0x400600);    //��С֡����64�ֽ�, ���֡������1518�ֽڣ����ϱ���ֽڣ��Ϊ1536�ֽ�
    
    write_reg(MAC_MII_CTRL, 0x120);         //bit 8: ���͹���֡��PHYʱ���Ƿ���Ҫ���PREAMBLE�� 
                                           //����֡����ʱ�ӷ�Ƶ����MDC��Ƶ�ʵ���HCLK/MII_CLKDIV  (bit 7:0 MII_CLKDIV
	
    write_mii_reg(0x0000, 0x4100);        //speed,100M, loopback

    delay (10000);
	
    for(i=0; i<BDNUM_TX; i++)               //�����������Ĵ���
    {
	  write_reg((MAC_BD+i*8), 0x00);
	  write_reg((MAC_BD+i*8+4), 0x00); 
    }		
	
	
    for(i=0;i<BDNUM_RX;i++)                //�����������Ĵ���
    {
	  write_reg(((MAC_BD+64*8)+i*8), 0x00);
	  write_reg(((MAC_BD+64*8)+i*8+4), 0x00);
    }	
	
    *(RP)MAC_ADDR0 = 0x05050505; 
    *(RP)MAC_ADDR1 = 0x05050505;
    
    write_reg(MAC_INTMASK, 0x00);           //�ж�����				
  
    for(i = 0; i < BDNUM_RX; i++)
    {
      write_reg(((MAC_BD+64*8)+ i*8 + 4),(MAC_RXRAM+i*RX_LENG)); 		
    }

    for(i = 0; i < BDNUM_TX; i++)
    {
      write_reg(((MAC_BD + 4) + i*8),(MAC_TXRAM + i*TX_LENG) );			
    }   
 
    for(i = 0; i < BDNUM_RX; i++)
    {
	  	if (i == BDNUM_RX - 1)
		  	write_reg(((MAC_BD+64*8)+ i*8), 0xe000);
	  	
	  	else
		  	write_reg(((MAC_BD+64*8)+ i*8), 0xc000);
     }
	
     for(i = 0; i < BDNUM_TX; i++)
     {
	  	if(i == BDNUM_TX - 1)
	  		{
	    		write_reg((MAC_BD + i*8), 0x0200f000);  
	    	 
	  		}
	  	
	  	else
	  		{
	    		write_reg((MAC_BD + i*8), 0x00bcd000);   
	        
	  		}
      }	
	
	
	write_reg(MAC_CTRL, 0xa423);                            
	
	{
	  while(1)
		{
			delay (100000);
			mii_reg_read();
			
			temp = *(RP)(MAC_BD+8*64);
			if(!(temp & 0x8000)) break;
		}
	}
		
	printf("PHY lookback ok,MAC test allright!!!\n");
}




void mac_int(void)
{
  
}