/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  nand.c
 *
 *	File Description: nandflash ���ܺ���.
 *
 *    Version         Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0           2009.05.06        ch            
 *   		      	
 **************************************************************************************/
#include	<stdio.h>

#include	"sep4020.h"
#include	"intc.h"
#include    "nand.h"

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
  
  for (i=0;i<j;i++)
  {};

}
                                          

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       void InitEmi(void)                                            
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ��ʼ��EMI
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS                                                                
*                                                                       
*      ��                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      ��                 
*                                                                       
***************************************************************/

void InitEmi(void)
{
  *(RP)EMI_NAND_CONF1		 =    0x06302857;
  *(RP)EMI_NAND_CONF2		 =    0x00114353;     //512byte/page,Ӳ��ECC
  
  #ifdef TwoKPage                                     //2K/page, 4 bytes address
  *(RP)EMI_NAND_CONF1		 =    0x06302857;     //4 address       
                                                      //Trr 10 cycles 
                                                      //Tclh 2 cycles 
                                                      //Talh 2 cycles
                                                      //Twh  3 cycles
                                                      //Read_width 10 cycles
                                                      //Writ_width 10 cycles
                                                                                                         
  *(RP)EMI_NAND_CONF2		 =    0x00914353;

  #endif
}	

/***************************************************************
* FUNCTION                                                              
*                                                                       
*       U32 NandWrite(U32 SrcAddr,U32 NandPage)                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ͨ��DMA��Դ��ַ��ʼ��һҳд��nandflash��ָ��ҳ
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS                                                                
*                                                                       
*      U32 SrcAddr       Դ��ַ
*      U32 NandPage      nandflashҳ��        
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      E_HA       
*      E_OK         
*                                                                       
****************************************************************/

U32 NandWrite(U32 SrcAddr,U32 NandPage)
{
  U32 	i,NTM = 0x0;	

  #ifdef TwoKPage                                      //�ж���2K/ҳ��nand������512byte/ҳ
    *(RP)EMI_NAND_ADDR1 = (U32)(NandPage<<16);
    *(RP)DMAC_C0CONTROL = 0x0084149B;                  //word-word burst=4  size=512words+16words
  #else
    *(RP)EMI_NAND_ADDR1 = (U32)(NandPage<<8);
    *(RP)DMAC_C0CONTROL = 0x0021149B;                  //word-word burst=4  size=128words+4words
  #endif                                               //Դ���ӣ�Ŀ�ĺ�Դ�Ĵ�����32bit��Ŀ�ĺ�Դ�ߴ�Ϊ4.
  
  *(RP)DMAC_C0SRCADDR	= SrcAddr;
  *(RP)DMAC_C0DESTADD	= EMI_NAND_DATA;

  *(RP)DMAC_C0CONFIGURATION = 0x300b;                  //Ŀ�������nand��Դ�����MEM�������δ����жϣ����δ����жϣ��������ͣ��洢�������裬ͨ��ʹ�ܡ� 
  
  *(RP)EMI_NAND_COM	    = NAND_CMD_SEQIN;          //��׼nandflash����,NANDʹ��
	
   delay(3000);                                        //��ʱ900us
	
   i = *(RP)DMAC_INTTCSTATUS;                          //ͨ��DMA��������ж�״̬�Ĵ����ж�nandflash�Ƿ���ɲ���
	
	while((i&0x1) != 0x1)			       //û����ɣ������ȴ�һ��ʱ��
	{
		delay(30);                             
		NTM++;
		if(NTM == 0x5) break;
		i = *(RP)DMAC_INTTCSTATUS;
	}	
	
   	if(NTM >= 0x5) return E_HA;			//��ʱ����
	else
	{
	  *(RP)DMAC_INTTCCLEAR=0x1;
	  while((*(RP)DMAC_INTERRORSTATUS)&0x1==0x1)
	  {
	    delay(10);                                    
	  }
	  *(RP)DMAC_INTTCCLEAR=0x0;
	  return E_OK;					//OK! 
        }

}


/*************************************************************
* FUNCTION                                                              
*                                                                       
*       U32 NandRead(U32 DestAddr,U32 NandPage)                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ͨ��DMA��nandflash��ָ��ҳ��Ŀ���ַ
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS                                                                
*                                                                       
*      U32 DestAddr     Ŀ���ַ
*      U32 NandPage     nandflashҳ��          
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      E_HA       
*      E_OK         
*                                                                       
***************************************************************/

U32 NandRead(U32 DestAddr,U32 NandPage)
{
  U32  i,NTM = 0x0; 
  
  #ifdef TwoKPage
    *(RP)EMI_NAND_ADDR1	 = (U32)(NandPage<<16);
    *(RP)DMAC_C0CONTROL  = 0x0084249b;			       //word-word burst=4  size=512words+16words
  #else
    *(RP)EMI_NAND_ADDR1  = (U32)(NandPage<<8);
    *(RP)DMAC_C0CONTROL  = 0x0021249b;			       //word-word burst=4  size=128words+4words
  #endif
  
  *(RP)DMAC_C0SRCADDR	 = EMI_NAND_DATA;
  *(RP)DMAC_C0DESTADD	 = DestAddr;

  *(RP)DMAC_C0CONFIGURATION = 0x31d;			       //��DMAͨ��                   		
  
  *(RP)EMI_NAND_COM	    = NAND_CMD_READ0;                  //nandflashʹ��
  
  delay(3000);                                            
  
  i = *(RP)EMI_NAND_IDLE;                        	       //ͨ��nandflash���мĴ����ж�nandflash�Ƿ���ɲ���
	while((i&0x1) != 0x1)				       //û����ɣ������ȴ�һ��ʱ��
	{  
		delay(30);                                     
		NTM++;
		if(NTM == 0x5) break;
		i = *(RP)EMI_NAND_IDLE;
	}
	
	if(NTM >= 0x5) return E_HA;			      //��ʱ����
	return E_OK;					      //OK!
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       U32 NandErase(U32 NandPage)                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ����nandflashָ��ҳ���ڵ�BLOCK
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS                                                                
*                                                                       
*      U32 nand_page                 
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      E_HA
*	   E_OK                  
*                                                                       
**************************************************************/

U32 NandErase(U32 NandPage)
{
  U32 i,NTM = 0x0;
  #ifdef TwoKPage
    *(RP)EMI_NAND_ADDR1 = (U32)(NandPage);
  #else
    *(RP)EMI_NAND_ADDR1 = (U32)(NandPage);
  #endif
	
  *(RP)EMI_NAND_COM		= NAND_CMD_ERASE;           //��׼nandflash��������
  
  delay(1000);                                              //��ʱ
  
  i = *(RP)EMI_NAND_IDLE; 
                          		                    //ͨ��nandflash���мĴ����ж�nandflash�Ƿ���ɲ���
	while((i&0x1) != 0x1)
	{  
		delay(200);						                   
		NTM++;
		if(NTM == 0x5) break;
		i = *(RP)EMI_NAND_IDLE; 
	}
	
	if(NTM >= 0x5) return E_HA;			    //��ʱ����
												
	else return E_OK;											//OK!
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       U32 NandReadIdle(void)                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ��nandflash��ID
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS                                                                
*                                                                       
*      ��                 
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      E_HA
*	   E_OK                  
*                                                                       
*************************************************************/

U32 NandReadId(void)
{
  U32 i,NTM = 0x0;
  *(RP)EMI_NAND_COM		 = NAND_CMD_READID;
  
	i = *(RP)EMI_NAND_IDLE;
	                         		                        //ͨ��nandflash���мĴ����ж�nandflash�Ƿ���ɲ���
	while((i&0x1) != 0x1)
	{  
		delay(30);						//û����ɣ������ȴ�һ��ʱ��
		NTM++;
		if(NTM == 0x5) break;
		i = *(RP)EMI_NAND_IDLE; 
	}
	
	if(NTM >= 0x5) return E_HA;					//��ʱ����
												
	return E_OK;							//OK!
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       U32 NandReadStatus(void)                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ��nandflash��Status
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS                                                                
*                                                                       
*      ��                 
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      E_HA
*	   E_OK                  
*                                                                       
*************************************************************/

U32 NandReadStatus(void)
{
  U32 i,NTM = 0x0;
  *(RP)EMI_NAND_COM		 = NAND_CMD_STATUS;
	i = *(RP)EMI_NAND_IDLE;                         		//ͨ��nandflash���мĴ����ж�nandflash�Ƿ���ɲ���
	
	while((i&0x1) != 0x1)
	{  
		delay(30);						//û����ɣ������ȴ�һ��ʱ��
		NTM++;
		if(NTM == 0x5) break;
		i = *(RP)EMI_NAND_IDLE; 
	}
	
	if(NTM >= 0x5) return E_HA;					//��ʱ����
												
	return E_OK;							//OK! 
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       CompareMem                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       �Ƚ�2�����ݿ�������Ƿ�һ�¡�
*                                                                       
* AUTHOR                                                                
*                                                                       
*       wuer              
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 src,  ��һ�����ݿ���׵�ַ
*		U32 dest, �ڶ������ݿ���׵�ַ
*		U32 len�� �Ƚϵ����ݿ��С        
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      ��  �������ӡ��Ϣ��              
*                                                                       
***************************************************************/
void CompareMem(U32 src, U32 dest,U32 len)
{
	U32 i,a,b;
	
	for(i=0;i<len;i++)
	{
		a = *(RP8)src;
		b = *(RP8)dest;
    	
    	if(a != b)
    	{
    		printf("Data different at 0x%lx\n",dest);
    	}
    	
    	src ++;
    	dest ++;	
	}
	
	printf("Compare data finished\n");
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       void InitGpio(void)                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ��nandflash��WP#
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS                                                                
*                                                                       
*      ��                 
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      ��                 
*                                                                       
**************************************************************/
void InitGpio(void)
{
    *(RP)GPIO_PORTG_SEL |= 0x0080;	
    *(RP)GPIO_PORTG_DATA |= 0x0080;
    *(RP)GPIO_PORTG_DIR &= ~0x0080;
}

