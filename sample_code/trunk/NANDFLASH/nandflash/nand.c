/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  nand.c
 *
 *	File Description: nandflash 功能函数.
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
*       延时
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
*       无                  
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
*       初始化EMI
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS                                                                
*                                                                       
*      无                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      无                 
*                                                                       
***************************************************************/

void InitEmi(void)
{
  *(RP)EMI_NAND_CONF1		 =    0x06302857;
  *(RP)EMI_NAND_CONF2		 =    0x00114353;     //512byte/page,硬件ECC
  
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
*       通过DMA把源地址开始的一页写入nandflash的指定页
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS                                                                
*                                                                       
*      U32 SrcAddr       源地址
*      U32 NandPage      nandflash页码        
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

  #ifdef TwoKPage                                      //判断是2K/页的nand，还是512byte/页
    *(RP)EMI_NAND_ADDR1 = (U32)(NandPage<<16);
    *(RP)DMAC_C0CONTROL = 0x0084149B;                  //word-word burst=4  size=512words+16words
  #else
    *(RP)EMI_NAND_ADDR1 = (U32)(NandPage<<8);
    *(RP)DMAC_C0CONTROL = 0x0021149B;                  //word-word burst=4  size=128words+4words
  #endif                                               //源增加，目的和源的传输宽度32bit，目的和源尺寸为4.
  
  *(RP)DMAC_C0SRCADDR	= SrcAddr;
  *(RP)DMAC_C0DESTADD	= EMI_NAND_DATA;

  *(RP)DMAC_C0CONFIGURATION = 0x300b;                  //目的外设号nand，源外设号MEM，不屏蔽传输中断，屏蔽错误中断，传输类型：存储器到外设，通道使能。 
  
  *(RP)EMI_NAND_COM	    = NAND_CMD_SEQIN;          //标准nandflash命令,NAND使能
	
   delay(3000);                                        //延时900us
	
   i = *(RP)DMAC_INTTCSTATUS;                          //通过DMA传输完成中断状态寄存器判断nandflash是否完成操作
	
	while((i&0x1) != 0x1)			       //没有完成，继续等待一段时间
	{
		delay(30);                             
		NTM++;
		if(NTM == 0x5) break;
		i = *(RP)DMAC_INTTCSTATUS;
	}	
	
   	if(NTM >= 0x5) return E_HA;			//超时返错
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
*       通过DMA读nandflash的指定页到目标地址
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS                                                                
*                                                                       
*      U32 DestAddr     目标地址
*      U32 NandPage     nandflash页码          
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

  *(RP)DMAC_C0CONFIGURATION = 0x31d;			       //打开DMA通道                   		
  
  *(RP)EMI_NAND_COM	    = NAND_CMD_READ0;                  //nandflash使能
  
  delay(3000);                                            
  
  i = *(RP)EMI_NAND_IDLE;                        	       //通过nandflash空闲寄存器判断nandflash是否完成操作
	while((i&0x1) != 0x1)				       //没有完成，继续等待一段时间
	{  
		delay(30);                                     
		NTM++;
		if(NTM == 0x5) break;
		i = *(RP)EMI_NAND_IDLE;
	}
	
	if(NTM >= 0x5) return E_HA;			      //超时返错
	return E_OK;					      //OK!
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       U32 NandErase(U32 NandPage)                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       擦除nandflash指定页所在的BLOCK
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
	
  *(RP)EMI_NAND_COM		= NAND_CMD_ERASE;           //标准nandflash擦除命令
  
  delay(1000);                                              //延时
  
  i = *(RP)EMI_NAND_IDLE; 
                          		                    //通过nandflash空闲寄存器判断nandflash是否完成操作
	while((i&0x1) != 0x1)
	{  
		delay(200);						                   
		NTM++;
		if(NTM == 0x5) break;
		i = *(RP)EMI_NAND_IDLE; 
	}
	
	if(NTM >= 0x5) return E_HA;			    //超时返错
												
	else return E_OK;											//OK!
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       U32 NandReadIdle(void)                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       读nandflash的ID
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS                                                                
*                                                                       
*      无                 
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
	                         		                        //通过nandflash空闲寄存器判断nandflash是否完成操作
	while((i&0x1) != 0x1)
	{  
		delay(30);						//没有完成，继续等待一段时间
		NTM++;
		if(NTM == 0x5) break;
		i = *(RP)EMI_NAND_IDLE; 
	}
	
	if(NTM >= 0x5) return E_HA;					//超时返错
												
	return E_OK;							//OK!
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       U32 NandReadStatus(void)                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       读nandflash的Status
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS                                                                
*                                                                       
*      无                 
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
	i = *(RP)EMI_NAND_IDLE;                         		//通过nandflash空闲寄存器判断nandflash是否完成操作
	
	while((i&0x1) != 0x1)
	{  
		delay(30);						//没有完成，继续等待一段时间
		NTM++;
		if(NTM == 0x5) break;
		i = *(RP)EMI_NAND_IDLE; 
	}
	
	if(NTM >= 0x5) return E_HA;					//超时返错
												
	return E_OK;							//OK! 
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       CompareMem                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       比较2块数据块的内容是否一致。
*                                                                       
* AUTHOR                                                                
*                                                                       
*       wuer              
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 src,  第一块数据块的首地址
*		U32 dest, 第二块数据块的首地址
*		U32 len， 比较的数据块大小        
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      无  （出错打印信息）              
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
*       打开nandflash的WP#
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS                                                                
*                                                                       
*      无                 
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      无                 
*                                                                       
**************************************************************/
void InitGpio(void)
{
    *(RP)GPIO_PORTG_SEL |= 0x0080;	
    *(RP)GPIO_PORTG_DATA |= 0x0080;
    *(RP)GPIO_PORTG_DIR &= ~0x0080;
}

