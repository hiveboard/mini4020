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
 *    1.0          2009.05.08          CH           
 *   		      	
 *************************************************************************************/

#include	<stdio.h>

#include	"sep4020.h"
#include	"intc.h"
#include	"sd.h"

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
*	    U32 dest, �ڶ������ݿ���׵�ַ
*	    U32 len�� �Ƚϵ����ݿ��С        
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      ��  �������ӡ��Ϣ��              
*                                                                       
***************************************************************/
void CompareMem(U32 src, U32 dest,U32 len)
{
	U32 i;
	U32 a,b;
	
	for(i=0; i<len; i++)
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
*       int SendCmdWaitRespIni(U16 Cmd,U32 Arg,U16 trans_mode,U16 blk_len,U16 nob,U16 int_mask)                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ��SD��������������ȴ�SD������ȷ���յ�����
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS                                                                
*                                                                       
*	    Cmd:			������;
*	    Arg:			�������;
*	    TransMode       SD����ģʽ;
*	    BlkLen:		    ���ݿ鳤��;
*	    Nob:			���ݿ���Ŀ;
*	    IntMask:		�ж������־;                 
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      *(RP)SDC_RESPONSE0   SD������Ӧ                  
*                                                                       
*************************************************/
int SendCmdWaitRespIni(U16 Cmd,U32 Arg,U16 TransMode,U16 BlkLen,U16 Nob,U16 IntMask)
{
    U32 i;
    
    *(RP)SDC_CLOCK_CONTROL=0Xff00;                  //����SDʱ�ӣ�512��Ƶ,�ر�SD ʱ��
    
    delay(10);
    *(RP)SDC_CLOCK_CONTROL=0Xff04;                  //��SDʱ�ӣ�512��Ƶ,����SD ʱ��
    
    delay(10);
    *(RP)SDC_INTERRUPT_STATUS_MASK=IntMask;         //�ж�״̬���μĴ�����ֵ
    
    delay(10);
    *(RP)SDC_TRANSFER_MODE=TransMode;               //����ģʽѡ��Ĵ�����ֵ
    
    delay(10);
    *(RP)SDC_BLOCK_SIZE=BlkLen;                     //���ݿ鳤�ȼĴ�����ֵ
    
    delay(10);
    *(RP)SDC_BLOCK_COUNT=Nob;                       //���ݿ���Ŀ�Ĵ�����ֵ
    
    delay(10);
    *(RP)SDC_ARGUMENT=Arg;                          //��������Ĵ�����ֵ
    
    delay(10);
    *(RP)SDC_COMMAND=Cmd;                           //������ƼĴ�����ֵ
    
    delay(10);                                  
   
    i = *(RP)SDC_INTERRUPT_STATUS & 0x1000;
	
    delay(10);
	
    while(i != 0x1000)                             //�жϣ��Ƿ��������ϣ������յ���Ӧ
    {
	   i = *(RP)SDC_INTERRUPT_STATUS & 0x1000;
    }
    delay(160);

    return *(RP)SDC_RESPONSE0;                     //�����������Ϣ
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       int SendCmdWaitResp(U16 Cmd,U32 Arg,U16 TransMode,U16 BlkLen,U16 Nob,U16 IntMask)                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ��SD��������������������ݣ��ȴ�SD������ȷ���յ�����
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS                                                                
*                                                                       
*	    Cmd:			������;
*	    Arg:			�������;
*	    TransMode       SD����ģʽ;
*	    BlkLen:		    ���ݿ鳤��;
*	    Nob:			���ݿ���Ŀ;
*	    IntMask:		�ж������־;                 
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      *(RP)SDC_RESPONSE0   SD������Ӧ                  
*                                                                       
*************************************************/
int SendCmdWaitResp(U16 Cmd,U32 Arg,U16 TransMode,U16 BlkLen,U16 Nob,U16 IntMask)
{
   U32 i;
    
   *(RP)SDC_CLOCK_CONTROL=0Xff00;               //����SDʱ��
   delay(10000);
   
   *(RP)SDC_CLOCK_CONTROL=0Xff04;               //��SDʱ��		
   delay(10000);
    
    
    *(RP)SDC_INTERRUPT_STATUS_MASK=IntMask;     //�ж�״̬���μĴ�����ֵ
    *(RP)SDC_TRANSFER_MODE=TransMode;           //����ģʽѡ��Ĵ�����ֵ
    *(RP)SDC_BLOCK_SIZE=BlkLen;                 //���ݿ鳤�ȼĴ�����ֵ
    *(RP)SDC_BLOCK_COUNT=Nob;                   //���ݿ���Ŀ�Ĵ�����ֵ
    *(RP)SDC_ARGUMENT=Arg;                      //��������Ĵ�����ֵ
    *(RP)SDC_COMMAND=Cmd;                       //������ƼĴ�����ֵ
    
    delay(10);    
        
    i = *(RP)SDC_INTERRUPT_STATUS & 0x1040;
	
    while(i != 0x1040)                         //�жϣ��������ϣ������յ���Ӧ�����ݴ�����ϡ��������Ƿ��Ѿ�����ɡ�
    {
	i = *(RP)SDC_INTERRUPT_STATUS & 0x1040;
    }
    
    delay(1000);
    
    return *(RP)SDC_RESPONSE0;                  //�����������Ϣ
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       int SendCmdWaitIdle(U16 Cmd,U32 Arg,U16 IntMask )                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ��SD��������������ȴ�SD������ȷ���յ�����
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS                                                                
*                                                                       
*	    Cmd:			������;
*	    Arg:			�������;  
*	    IntMask:		�ж������־;                 
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      *(RP)SDC_RESPONSE0   SD������Ӧ                  
*                                                                       
*************************************************/
int SendCmdWaitIdle(U16 Cmd,U32 Arg,U16 IntMask )
{
    int i;

    *(RP)SDC_CLOCK_CONTROL=0Xff00;                       //����SDʱ��
    
    delay(10);
    *(RP)SDC_CLOCK_CONTROL=0Xff04;                       //��SDʱ��
       
    delay(10);
    *(RP)SDC_COMMAND=Cmd;                                //������ƼĴ�����ֵ
    
    delay(10);                          
    *(RP)SDC_INTERRUPT_STATUS_MASK=IntMask;              //�ж�״̬���μĴ�����ֵ
    
    delay(10);
    *(RP)SDC_ARGUMENT=Arg;                               //��������Ĵ�����ֵ
    
    delay(10);
	
    i = *(RP)SDC_INTERRUPT_STATUS & 0x1000;
	
    while(i != 0x1000)                                  //�жϣ��Ƿ��������ϣ������յ���Ӧ
    {
	i = *(RP)SDC_INTERRUPT_STATUS & 0x1000;
    }
    
    delay(10);
        	                	             
    return *(RP)SDC_RESPONSE0;                           //�����������Ϣ�Լ���ֵ1
    return 1;  
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       void wait()                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       �ȴ�
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS                                                                
*       ��                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*       ��                  
*                                                                       
**************************************************************/
void wait()
{
	 U32 resp;
	 
	 SendCmdWaitRespIni(0x1aa,0X0,0X0,0x0,0x0,0X0);        //CMD13��SD ������ţ��������ʹ�ܿ��� �������ݴ��䣬48 λ�̷���
	 
	 resp = *(RP)SDC_INTERRUPT_STATUS & 0x1000;
	 
	 while (resp != 0x1000)                                //�жϣ��Ƿ��������ϣ������յ���Ӧ
	 {
	   SendCmdWaitRespIni(0x1aa,0X0,0X0,0x0,0x0,0X0);      //CMD13 ,��card status register.

	   resp = *(RP)SDC_INTERRUPT_STATUS & 0x1000;
	 }
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       void erase()                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ����
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS
*                                                                
*       startaddr
*        endaddr                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*       ��                  
*                                                                       
*************************************************/
void erase(U32 startaddr,U32 endaddr)
{
	   U32 saddr = startaddr;
	   U32 eaddr = endaddr;
	   
	   delay(100);
	   SendCmdWaitRespIni(0x40a,saddr,0X0,0x0,0x0,0X0);      //CMD35 ������ʼ��ַ
	   
	   delay(100);
	   SendCmdWaitRespIni(0x42a,eaddr,0X0,0x0,0x0,0X0);      //CMD36 ���ý�����ַ
	   
	   delay(100);
	   SendCmdWaitRespIni(0x4cb,0X0,0X0,0x0,0x0,0X0);        //CMD38 ִ�в�������
	   
	   delay(1000);
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       U32 InitialSd()                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ��ʼ��SD��
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS 
*                                                               
*       ��                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*       rca    SD����rca��ַ��                  
*                                                                       
*************************************************/
U32 InitialSd(void)
{
    U32 rca;
    
    *(RP)SDC_SOFTWARE_RESET=0x0;                   //������λ,����д0�ǽ���reset 
    
    delay(200);
    *(RP)SDC_SOFTWARE_RESET=0x1;                   //��������λ
    
    delay(200);
 
    SendCmdWaitIdle(0x08,0x0,0xfff);               //CMD0�������ʹ��
       
    delay(8000);  
    
    do
    {
    	SendCmdWaitIdle(0x6ea,0x0,0xfff);          //CMD55,���л���ACMD����
    	
    	delay(3);  
    	SendCmdWaitIdle(0x52a,0x80ff8000,0xfff);   //ACMD41����SD��������������ȴ�SD������ȷ���յ�����
       
    }while(*(RP)SDC_RESPONSE0<0X80008000);

    
    SendCmdWaitRespIni(0x49,0X0,0X0,0x0,0x0,0Xfff);//CMD2������CID
    
    SendCmdWaitRespIni(0x6a,0X0,0X0,0x0,0x0,0Xfff);//CMD3��ѯ�ʿ�Ƭ�����µ���ص�ַ
    
    rca=*(RP)SDC_RESPONSE0;
    
    SendCmdWaitRespIni(0xea,rca,0X0,0x0,0x0,0Xfff);//CMD7������ѡ���Ե���ز���
    
    return rca;   
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       void SdSingleW1r1(U32 rca)                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ����һλдһλ��
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS 
*                                                               
*       rca    SD����rca��ַ��                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*       ��                  
*                                                                       
*************************************************/
void SdSingleW1r1(U32 rca)
{
    U32 complete;
    erase(0x0,0x4000);//����
    
    //***********************����DMA2����һλд*************************
    *(RP)DMAC_C2SRCADDR  = MEM_BLOCK_BEGIN;                  // DMAC��2Դ��ַ��Ϊ0x30200000
    *(RP)DMAC_C2DESTADDR = SDC_WRITE_BUFER_ACCESS;           // DMAC��2Ŀ�ĵ�ַ��Ϊ����FIFO�ĵ�ַ
    *(RP)DMAC_C2CONTROL  = 0x20149b;                         //����ߴ�0x080,Դ��ַ����Ŀ�ĵ�ַ�����ӣ�������32bit���������Ŀ4                                                 				
    *(RP)DMAC_C2CONFIGURATION = 0x380b;	                     //�����δ����жϣ����δ����жϣ�ͨ��ʹ�ܣ��������ͣ��洢��������							       				 	    
    
   
    //����SD��λ���ߴ���ģʽ
    SendCmdWaitIdle(0x6ea,rca,0xfff);                        //CMD55�����л���ACMD����
    SendCmdWaitIdle(0xca,0x0,0xfff);                         //ACMD6�����������߿�ȣ�48 λ�̷����������ݴ���
 
    //���͵���д����   
    SendCmdWaitResp(0x30e,0x00000000,0X2,0x0200,0x1,0Xfff);  //CMD24  1bit mode
    
    complete = *(RP)SDC_INTERRUPT_STATUS;
    
    while((complete |0xfffffffe) !=0xfffffffe)               //��Ӧ��ʱ����
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    } 
    
    delay(500);
    
    //***********************����DMA2����һλ��*************************						
    *(RP)DMAC_C2SRCADDR  = SDC_READ_BUFER_ACCESS;           // DMAC��2Դ��ַ��Ϊ0x1000B038
    *(RP)DMAC_C2DESTADDR = END(0);                          // DMAC��2Ŀ�ĵ�ַ��Ϊ0x30220000
    *(RP)DMAC_C2CONTROL  = 0x20249b;                        //����ߴ�0x080,Դ��ַ������Ŀ�ĵ�ַ���ӣ�������32bit���������Ŀ4                                           				
    *(RP)DMAC_C2CONFIGURATION = 0x38d;                      //Դ����ΪMMC�������δ����жϣ����δ����жϣ�ͨ��ʹ�ܣ��������ͣ����赽�洢��	
	
    //���͵�������� 
    SendCmdWaitResp(0x22e,0x00000000,0X0,0x0200,0x1,0Xfff); //CMD17 1bit mode
    
    complete = *(RP)SDC_INTERRUPT_STATUS;
   
    while((complete |0xfffffffd) !=0xfffffffd)              //д����CRC У�����
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    } 
    
    CompareMem(MEM_BLOCK_BEGIN,END(0),0x200);               // �Ƚ�2�����ݿ�������Ƿ�һ�£�����512Byte
    printf("SdSingleW1r1 Ok\n\n"); 
}


/*************************************************************
* FUNCTION                                                              
*                                                                       
*       void SdSingleW1r4(U32 rca)                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ����һλд��λ��
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS 
*                                                               
*       rca    SD����rca��ַ��                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*       ��                  
*                                                                       
*************************************************/
void SdSingleW1r4(U32 rca)
{
    U32 complete;
    erase(0x0,0x4000);//����
    
    //***********************����DMA2����һλд*************************
    *(RP)DMAC_C2SRCADDR = MEM_BLOCK_BEGIN;                   //DMAC��2Դ��ַ��Ϊ0x30200000
    *(RP)DMAC_C2DESTADDR = SDC_WRITE_BUFER_ACCESS;           //DMAC��2Ŀ�ĵ�ַ��Ϊ����FIFO�ĵ�ַ
    *(RP)DMAC_C2CONTROL = 0x20149b;                          //����ߴ�0x080,Դ��ַ����Ŀ�ĵ�ַ�����ӣ�������32bit���������Ŀ4                                                				
    *(RP)DMAC_C2CONFIGURATION = 0x380b;	                     //�����δ����жϣ����δ����жϣ�ͨ��ʹ�ܣ��������ͣ��洢��������							       				 	    
    
    SendCmdWaitIdle(0x6ea,rca,0xfff);                        //CMD55
    SendCmdWaitIdle(0xca,0x0,0xfff);                         //ACMD6
       
    SendCmdWaitResp(0x30e,0x00000000,0X2,0x0200,0x1,0Xfff);  //CMD24  1bit mode
 
    complete = *(RP)SDC_INTERRUPT_STATUS;
    while((complete |0xfffffffe) !=0xfffffffe)               //��Ӧ��ʱ����
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    } 
    
    delay(5000);
   

    //***********************����DMA2������λ��*************************
    *(RP)DMAC_C2SRCADDR  = SDC_READ_BUFER_ACCESS;           //DMAC��2Դ��ַ��Ϊ0x1000B038
    *(RP)DMAC_C2DESTADDR = END(1);                          //DMAC��2Ŀ�ĵ�ַ��Ϊ0x30230000
    *(RP)DMAC_C2CONTROL  = 0x20249b;                        //����ߴ�0x080,Դ��ַ������Ŀ�ĵ�ַ���ӣ�������32bit���������Ŀ4
    *(RP)DMAC_C2CONFIGURATION = 0x38d;                      //Դ����ΪMMC�������δ����жϣ����δ����жϣ�ͨ��ʹ�ܣ��������ͣ����赽�洢��	
	
    SendCmdWaitIdle(0x6ea,rca,0xfff);                       //CMD55�����л���ACMD����
    SendCmdWaitIdle(0xca,0x2,0xfff);                        //ACMD6�����������߿�ȣ�48 λ�̷����������ݴ���
   						
    SendCmdWaitResp(0x22e,0x00000000,0X1,0x0200,0x1,0Xfff); //CMD17 4bit mode
    
    complete = *(RP)SDC_INTERRUPT_STATUS;
    
    while((complete |0xfffffffd) !=0xfffffffd)              //д����CRC У�����
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    }
     
    CompareMem(MEM_BLOCK_BEGIN,END(1),0x200);               // �Ƚ�2�����ݿ�������Ƿ�һ�£�����512Byte
    printf("SdSingleW1r4 Ok\n\n"); 
    
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       void SdSingleW4r1(U32 rca)                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ������λдһλ��
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS 
*                                                               
*       rca    SD����rca��ַ��                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*       ��                  
*                                                                       
*************************************************/
void SdSingleW4r1(U32 rca)
{
    U32 complete;
    erase(0x0,0x4000);//����
    
    //***********************����DMA2������λд*************************
    *(RP)DMAC_C2SRCADDR  = MEM_BLOCK_BEGIN;                  //DMAC��2Դ��ַ��Ϊ0x30200000
    *(RP)DMAC_C2DESTADDR = SDC_WRITE_BUFER_ACCESS;           //DMAC��2Ŀ�ĵ�ַ��Ϊ����FIFO�ĵ�ַ
    *(RP)DMAC_C2CONTROL  = 0x20149b;                         //����ߴ�0x080,Դ��ַ����Ŀ�ĵ�ַ�����ӣ�������32bit���������Ŀ4                                                  				
    *(RP)DMAC_C2CONFIGURATION = 0x380b;	                     //�����δ����жϣ����δ����жϣ�ͨ��ʹ�ܣ��������ͣ��洢��������							       				 	    
    
    
    SendCmdWaitIdle(0x6ea,rca,0xfff);                        //CMD55�����л���ACMD����
    SendCmdWaitIdle(0xca,0x2,0xfff);                         //ACMD6�����������߿�ȣ�48 λ�̷����������ݴ���
    
  
    SendCmdWaitResp(0x30e,0x00000000,0X3,0x0200,0x1,0Xfff);  //CMD24  1bit mode
    
 
    complete = *(RP)SDC_INTERRUPT_STATUS;
    
    while((complete |0xfffffffe) !=0xfffffffe)               //��Ӧ��ʱ����
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    } 
 	
 
    delay(5000);
   
   //***********************����DMA2����һλ��*************************
    *(RP)DMAC_C2SRCADDR  = SDC_READ_BUFER_ACCESS;             //DMAC��2Դ��ַ��Ϊ0x1000B038
    *(RP)DMAC_C2DESTADDR = END(2);                            //DMAC��2Ŀ�ĵ�ַ��Ϊ0x30240000
    *(RP)DMAC_C2CONTROL  = 0x20249b;                          //����ߴ�0x080,Դ��ַ������Ŀ�ĵ�ַ���ӣ�������32bit���������Ŀ4                                                 				
    *(RP)DMAC_C2CONFIGURATION = 0x38d;	                      //Դ����ΪMMC�������δ����жϣ����δ����жϣ�ͨ��ʹ�ܣ��������ͣ����赽�洢��
	
	
    SendCmdWaitIdle(0x6ea,rca,0xfff);                         //CMD55
    SendCmdWaitIdle(0xca,0x0,0xfff);                          //ACMD6
    						
    SendCmdWaitResp(0x22e,0x00000000,0X0,0x0200,0x1,0Xfff);   //CMD17 1bit mode
    
    complete = *(RP)SDC_INTERRUPT_STATUS;
    while((complete |0xfffffffd) !=0xfffffffd)                //д����CRC У�����
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    } 
    
    CompareMem(MEM_BLOCK_BEGIN,END(2),0x200);                 //�Ƚ�2�����ݿ�������Ƿ�һ�£�����512Byte
    printf("SdSingleW4r1 Ok\n\n"); 
    
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       void SdSingleW4r4(U32 rca)                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ������λд��λ��
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS 
*                                                               
*       rca    SD����rca��ַ��                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*       ��                  
*                                                                       
*************************************************/
void SdSingleW4r4(U32 rca)
{
    U32 complete;
    erase(0x0,0x4000);//����
    
    //***********************����DMA2������λд*************************
    *(RP)DMAC_C2SRCADDR  = MEM_BLOCK_BEGIN;                    //DMAC��2Դ��ַ��Ϊ0x30200000
    *(RP)DMAC_C2DESTADDR = SDC_WRITE_BUFER_ACCESS;             //DMAC��2Ŀ�ĵ�ַ��Ϊ����FIFO�ĵ�ַ
    *(RP)DMAC_C2CONTROL  = 0x20149b;                           //����ߴ�0x080,Դ��ַ����Ŀ�ĵ�ַ�����ӣ�������32bit���������Ŀ4                                                  				
    *(RP)DMAC_C2CONFIGURATION = 0x380b;	                       //�����δ����жϣ����δ����жϣ�ͨ��ʹ�ܣ��������ͣ��洢��������							       				 	    
    
    SendCmdWaitIdle(0x6ea,rca,0xfff);                          //CMD55�����л���ACMD����
    SendCmdWaitIdle(0xca,0x2,0xfff);                           //ACMD6�����������߿�ȣ�48 λ�̷����������ݴ���
    
    SendCmdWaitResp(0x30e,0x00000000,0X3,0x0200,0x1,0Xfff);    //CMD24  1bit mode
    
    complete = *(RP)SDC_INTERRUPT_STATUS;
    while ((complete & 0x00000040) != 0x00000040)              //���ݴ���δ��
    {
      complete = *(RP)SDC_INTERRUPT_STATUS;
    } 
    
    while((complete |0xfffffffe) != 0xfffffffe)                //��Ӧ��ʱ����
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    } 
 
    delay(5000);
   
   //***********************����DMA2������λ��*************************
    *(RP)DMAC_C2SRCADDR  = SDC_READ_BUFER_ACCESS;              //DMAC��2Դ��ַ��Ϊ0x1000B038
    *(RP)DMAC_C2DESTADDR = END(3);                             //DMAC��2Ŀ�ĵ�ַ��Ϊ0x30250000
    *(RP)DMAC_C2CONTROL  = 0x20249b;                           //����ߴ�0x080,Դ��ַ������Ŀ�ĵ�ַ���ӣ�������32bit���������Ŀ4                                                  				
    *(RP)DMAC_C2CONFIGURATION = 0x38d;	                       //Դ����ΪMMC�������δ����жϣ����δ����жϣ�ͨ��ʹ�ܣ��������ͣ����赽�洢��
	
    SendCmdWaitIdle(0x6ea,rca,0xfff);                          //CMD55�����л���ACMD����
    SendCmdWaitIdle(0xca,0x2,0xfff);                           //ACMD6�����������߿�ȣ�48 λ�̷����������ݴ���
   
    						
    SendCmdWaitResp(0x22e,0x00000000,0X1,0x0200,0x1,0Xfff);    //CMD17 1bit mode
    
    complete = *(RP)SDC_INTERRUPT_STATUS;
    while((complete |0xfffffffd) != 0xfffffffd)                //д����CRC У����� 
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    }
     
    CompareMem(MEM_BLOCK_BEGIN,END(3),0x200);                  //�Ƚ�2�����ݿ�������Ƿ�һ�£�����512Byte
    printf("SdSingleW4r4 Ok\n\n"); 
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       void SdMultiW1r1(U32 rca)                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ���һλдһλ��
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS 
*                                                               
*       rca    SD����rca��ַ��                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*       ��                  
*                                                                       
*************************************************/
void SdMultiW1r1(U32 rca)
{
    U32 complete;
    erase(0x0,0x4000);//����
    
    //***********************����DMA2����һλд*************************
    *(RP)DMAC_C2SRCADDR = MEM_BLOCK_BEGIN;                      //DMAC��2Դ��ַ��Ϊ0x30200000
    *(RP)DMAC_C2DESTADDR = SDC_WRITE_BUFER_ACCESS;              //DMAC��2Ŀ�ĵ�ַ��Ϊ����FIFO�ĵ�ַ
    *(RP)DMAC_C2CONTROL = 0x80149b;                             //����ߴ�0x200,Դ��ַ����Ŀ�ĵ�ַ�����ӣ�������32bit���������Ŀ4                				
    *(RP)DMAC_C2CONFIGURATION = 0x380b;		                //�����δ����жϣ����δ����жϣ�ͨ��ʹ�ܣ��������ͣ��洢��������						       				 	    
  
    SendCmdWaitIdle(0x6ea,rca,0xfff);                           //CMD55���л���ACMD����
    SendCmdWaitIdle(0xca,0x0,0xfff);                            //ACMD6�����ô���ģʽΪ����ģʽ
      
    SendCmdWaitResp(0x32e,0,0X2,0x0200,0x4,0Xfff);              // CMD25 ���д���� 1bit mode
    
    SendCmdWaitIdle(0x18a,0x0,0xfff);                           //stop CMD12  ��鴫����Ҫ����stop������ֹͣ����
    
    complete = *(RP)SDC_INTERRUPT_STATUS;
    while((complete |0xfffffffe) != 0xfffffffe)                 //��Ӧ��ʱ����
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    } 
    
    delay(5000);
    //***********************����DMA2����һλ��*************************	
    *(RP)DMAC_C2SRCADDR  = SDC_READ_BUFER_ACCESS;               //DMAC��2Դ��ַ��Ϊ0x1000B038
    *(RP)DMAC_C2DESTADDR = END(4);                              //DMAC��2Ŀ�ĵ�ַ��Ϊ0x30250000
    *(RP)DMAC_C2CONTROL  = 0x80249b;                            //����ߴ�0x200,Դ��ַ������Ŀ�ĵ�ַ���ӣ�������32bit���������Ŀ4                 				
    *(RP)DMAC_C2CONFIGURATION = 0x38d;	                        //Դ����ΪMMC�������δ����жϣ����δ����жϣ�ͨ��ʹ�ܣ��������ͣ����赽�洢��
	
    SendCmdWaitIdle(0x6ea,rca,0xfff);                           //CMD55���л���ACMD����
    SendCmdWaitIdle(0xca,0x0,0xfff);                            //ACMD6�����ô���ģʽΪ����ģʽ
    					
    SendCmdWaitResp(0x24e,0,0X0,0x0200,0x4,0Xfff);              //CMD18  ��������  1bit mode
    
    SendCmdWaitIdle(0x18a,0x0,0xfff);                           //stop CMD12����鴫����Ҫ����stop������ֹͣ����
    
    complete = *(RP)SDC_INTERRUPT_STATUS;
    while((complete |0xfffffffd) != 0xfffffffd)                 //д����CRC У�����
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    } 
    
    CompareMem(MEM_BLOCK_BEGIN,END(4),0x200);                   //�Ƚ�2�����ݿ�������Ƿ�һ�£�����512Byte
    printf("SdMultiW1r1 Ok\n\n"); 
   
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       void SdMultiW1r4(U32 rca)                                              
*                                                                       
* DESCRIPTION                                                           
*       ���һλд��λ��
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS 
*                                                               
*       rca    SD����rca��ַ��                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*       ��                  
*                                                                       
*************************************************/
void SdMultiW1r4(U32 rca)
{
    U32 complete;
    erase(0x0,0x4000);//����
    
    //***********************����DMA2����һλд*************************
    *(RP)DMAC_C2SRCADDR = MEM_BLOCK_BEGIN;                      //DMAC��2Դ��ַ��Ϊ0x30200000
    *(RP)DMAC_C2DESTADDR = SDC_WRITE_BUFER_ACCESS;              //DMAC��2Ŀ�ĵ�ַ��Ϊ����FIFO�ĵ�ַ
    *(RP)DMAC_C2CONTROL = 0x80149b;                             //����ߴ�0x200,Դ��ַ����Ŀ�ĵ�ַ�����ӣ�������32bit���������Ŀ4                                                   				
    *(RP)DMAC_C2CONFIGURATION = 0x380b;	                        //�����δ����жϣ����δ����жϣ�ͨ��ʹ�ܣ��������ͣ��洢��������							       				 	    
  
    SendCmdWaitIdle(0x6ea,rca,0xfff);                           //CMD55
    SendCmdWaitIdle(0xca,0x0,0xfff);                            //ACMD6�����ô���ģʽΪ����ģʽ
      
    SendCmdWaitResp(0x32e,0,0X2,0x0200,0x4,0Xfff);              // CMD25 1bit mode
  
    complete = *(RP)SDC_INTERRUPT_STATUS;
    while ((complete & 0x00000040) != 0x00000040)               //���ݴ���δ��
    {
      complete = *(RP)SDC_INTERRUPT_STATUS;
    } 
   
    while((complete |0xfffffffe) != 0xfffffffe)                 //��Ӧ��ʱ����
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    } 
    
    delay(5000);
    
    SendCmdWaitIdle(0x18a,0x0,0xfff);                           //stop CMD12
    
     //***********************����DMA2������λ��*************************
 
    *(RP)DMAC_C2SRCADDR  = SDC_READ_BUFER_ACCESS;              //DMAC��2Դ��ַ��Ϊ0x1000B038
    *(RP)DMAC_C2DESTADDR = END(5);                             //DMAC��2Ŀ�ĵ�ַ��Ϊ0x30260000
    *(RP)DMAC_C2CONTROL  = 0x80249b;                           //����ߴ�0x200,Դ��ַ������Ŀ�ĵ�ַ���ӣ�������32bit���������Ŀ4                                                                           				
    *(RP)DMAC_C2CONFIGURATION = 0x38d;	                       //Դ����ΪMMC�������δ����жϣ����δ����жϣ�ͨ��ʹ�ܣ��������ͣ����赽�洢��                       
	
    SendCmdWaitIdle(0x6ea,rca,0xfff);                          //CMD55
    SendCmdWaitIdle(0xca, 0x2,0xfff);                          //ACMD6�����ô���ģʽΪ����ģʽ
						
    SendCmdWaitResp(0x24e,0,0X1,0x0200,0x4,0Xfff);             // CMD18 4bit mode
    
    complete = *(RP)SDC_INTERRUPT_STATUS;;
    while ((complete & 0x00000040) != 0x00000040)              //���ݴ���δ��
    {
      complete = *(RP)SDC_INTERRUPT_STATUS;
    }
    
    complete = *(RP)SDC_INTERRUPT_STATUS;
    while((complete |0xfffffffd) != 0xfffffffd)                //д����CRC У�����
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    } 
    
    SendCmdWaitIdle(0x18a,0x0,0xfff);                          //stop CMD12
    CompareMem(MEM_BLOCK_BEGIN,END(5),0x200);                  //�Ƚ�2�����ݿ�������Ƿ�һ�£�����512Byte 
    printf("SdMultiW1r4 Ok\n\n");              
    
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       void SdMultiW4r1(U32 rca)                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       �����λдһλ��
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS 
*                                                               
*       rca    SD����rca��ַ��                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*       ��                  
*                                                                       
*************************************************/
void SdMultiW4r1(U32 rca)
{
    U32 complete;
    erase(0x0,0x4000);//����
    
    //***********************����DMA2������λд*************************
    *(RP)DMAC_C2SRCADDR = MEM_BLOCK_BEGIN;                    //DMAC��2Դ��ַ��Ϊ0x30200000
    *(RP)DMAC_C2DESTADDR = SDC_WRITE_BUFER_ACCESS;            //DMAC��2Ŀ�ĵ�ַ��Ϊ����FIFO�ĵ�ַ
    *(RP)DMAC_C2CONTROL = 0x80149b;                           //����ߴ�0x200,Դ��ַ����Ŀ�ĵ�ַ�����ӣ�������32bit���������Ŀ4                                                  				
    *(RP)DMAC_C2CONFIGURATION = 0x380b;		                  //�����δ����жϣ����δ����жϣ�ͨ��ʹ�ܣ��������ͣ��洢��������						       				 	    
  
    SendCmdWaitIdle(0x6ea,rca,0xfff);                         //CMD55
    SendCmdWaitIdle(0xca,0x2,0xfff);                          //ACMD6
      
    SendCmdWaitResp(0x32e,0X0,0X3,0x0200,0x4,0Xfff);          // CMD25 4bit mode
  
    complete = *(RP)SDC_INTERRUPT_STATUS;
    while ((complete & 0x00000040) != 0x00000040)             //���ݴ���δ��
    {
      complete = *(RP)SDC_INTERRUPT_STATUS;
    } 
   
    while((complete |0xfffffffe) != 0xfffffffe)               //��Ӧ��ʱ����
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    } 
    delay(5000);
    
    SendCmdWaitIdle(0x18a,0x0,0xfff);                         //stop CMD12
    
    //***********************����DMA2����һλ��*************************
    *(RP)DMAC_C2SRCADDR  = SDC_READ_BUFER_ACCESS;             //DMAC��2Դ��ַ��Ϊ0x1000B038
    *(RP)DMAC_C2DESTADDR = END(6);                            //DMAC��2Ŀ�ĵ�ַ��Ϊ0x30270000
    *(RP)DMAC_C2CONTROL  = 0x80249b;                          //����ߴ�0x200,Դ��ַ������Ŀ�ĵ�ַ���ӣ�������32bit���������Ŀ4                                                 				
    *(RP)DMAC_C2CONFIGURATION = 0x38d;	                      //Դ����ΪMMC�������δ����жϣ����δ����жϣ�ͨ��ʹ�ܣ��������ͣ����赽�洢��
	
	
    SendCmdWaitIdle(0x6ea,rca,0xfff);                         //CMD55
    SendCmdWaitIdle(0xca,0x0,0xfff);                          //ACMD6
							
    SendCmdWaitResp(0x24e,0X0,0X0,0x0200,0x4,0Xfff);          //CMD18 1bit mode
    
    complete = *(RP)SDC_INTERRUPT_STATUS;;
    while ((complete & 0x00000040) != 0x00000040)             //���ݴ���δ��
    {
      complete = *(RP)SDC_INTERRUPT_STATUS;
    }
   
    while((complete |0xfffffffd) != 0xfffffffd)               //д����CRC У�����
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    } 
    
    SendCmdWaitIdle(0x18a,0x0,0xfff);                         //stop CMD12
    CompareMem(MEM_BLOCK_BEGIN,END(6),0x200);                 //�Ƚ�2�����ݿ�������Ƿ�һ�£�����512Byte 
    printf("SdMultiW4r1 Ok\n\n");
    
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       void SdMultiW4r4(U32 rca)                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       �����λд��λ��
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS 
*                                                               
*       rca    SD����rca��ַ��                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*       ��                  
*                                                                       
*************************************************/
void SdMultiW4r4(U32 rca)
{
    U32 complete;
    erase(0x0,0x4000);//����
    
    //***********************����DMA2������λд*************************
    *(RP)DMAC_C2SRCADDR = MEM_BLOCK_BEGIN;                    //DMAC��2Դ��ַ��Ϊ0x30200000
    *(RP)DMAC_C2DESTADDR = SDC_WRITE_BUFER_ACCESS;            //DMAC��2Ŀ�ĵ�ַ��Ϊ����FIFO�ĵ�ַ
    *(RP)DMAC_C2CONTROL = 0x80149b;                           //����ߴ�0x200,Դ��ַ����Ŀ�ĵ�ַ�����ӣ�������32bit���������Ŀ4                                                				
    *(RP)DMAC_C2CONFIGURATION = 0x380b;	                      //�����δ����жϣ����δ����жϣ�ͨ��ʹ�ܣ��������ͣ��洢��������							       				 	    
  
    SendCmdWaitIdle(0x6ea,rca,0xfff);                         //CMD55
    SendCmdWaitIdle(0xca,0x2,0xfff);                          //ACMD6
      
    SendCmdWaitResp(0x32e,0X0,0X3,0x0200,0x4,0Xfff);          // CMD25 4bit mode
  
    complete = *(RP)SDC_INTERRUPT_STATUS;
    while ((complete & 0x00000040) != 0x00000040)             //���ݴ���δ��
    {
      complete = *(RP)SDC_INTERRUPT_STATUS;
    } 
    
    complete = *(RP)SDC_INTERRUPT_STATUS;
    while((complete |0xfffffffe) != 0xfffffffe)               //��Ӧ��ʱ����
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    } 
    
    delay(5000);
    
    SendCmdWaitIdle(0x18a,0x0,0xfff);                         //stop CMD12
    
    //***********************����DMA2������λ��*************************
    *(RP)DMAC_C2SRCADDR  = SDC_READ_BUFER_ACCESS;             //DMAC��2Դ��ַ��Ϊ0x1000B038
    *(RP)DMAC_C2DESTADDR = END(7);                            //DMAC��2Ŀ�ĵ�ַ��Ϊ0x30270000                          
    *(RP)DMAC_C2CONTROL  = 0x80249b;                          //����ߴ�0x200,Դ��ַ������Ŀ�ĵ�ַ���ӣ�������32bit���������Ŀ4                                                				
    *(RP)DMAC_C2CONFIGURATION = 0x38d;	                      //Դ����ΪMMC�������δ����жϣ����δ����жϣ�ͨ��ʹ�ܣ��������ͣ����赽�洢��
	
    SendCmdWaitIdle(0x6ea,rca,0xfff);                         //CMD55
    SendCmdWaitIdle(0xca, 0x2,0xfff);                         //ACMD6
	
    	
    					
    SendCmdWaitResp(0x24e,0,0X1,0x0200,0x4,0Xfff);            // CMD18 4bit mode
    
    complete = *(RP)SDC_INTERRUPT_STATUS;;                    //�ж�״̬�Ĵ������ݸ�ֵ
    while ((complete & 0x00000040) != 0x00000040)             //���ݴ���δ��
    {
      complete = *(RP)SDC_INTERRUPT_STATUS;
    }
    
    while((complete |0xfffffffd) != 0xfffffffd)               //д����CRC У�����
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    } 
    
    SendCmdWaitIdle(0x18a,0x0,0xfff);                         //stop CMD12
    CompareMem(MEM_BLOCK_BEGIN,END(7),0x200);                 //�Ƚ�2�����ݿ�������Ƿ�һ�£�����512Byte
    printf("SdMultiW4r4 Ok\n\n");
    
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       void  FifoReset()                                             
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       FIFO��ʼ��
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS 
*                                                               
*       ��                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*       ��                  
*                                                                       
*************************************************/
void  FifoReset(void)
{
    U32 tmp;
    *(RP)SDC_CLOCK_CONTROL=0X0000;             		       //����SDʱ��
    delay(10);
    
    *(RP)SDC_CLOCK_CONTROL=0X0004;                         //��SDʱ��
    delay(2);
    
    while((*(RP)SDC_INTERRUPT_STATUS & 0x200)!=0x200)      //����FIFO�ǿ�
    {
    	tmp = *(RP)SDC_READ_BUFER_ACCESS;
    	delay(10);
    }
    
    while((*(RP)SDC_INTERRUPT_STATUS & 0x800)!=0x800)      //����FIFO�ǿ�
    {
    	*(RP)SDC_WRITE_BUFER_ACCESS = tmp;
    	delay(10);
    }

}
