/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  main.c
 *
 *	File Description: 主函数, 实现具体功能.
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
*       比较2块数据块的内容是否一致。
*                                                                       
* AUTHOR                                                                
*                                                                       
*       wuer              
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U32 src,  第一块数据块的首地址
*	    U32 dest, 第二块数据块的首地址
*	    U32 len， 比较的数据块大小        
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      无  （出错打印信息）              
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
*       int SendCmdWaitRespIni(U16 Cmd,U32 Arg,U16 trans_mode,U16 blk_len,U16 nob,U16 int_mask)                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       向SD控制器发送命令，等待SD控制器确认收到命令
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS                                                                
*                                                                       
*	    Cmd:			命令编号;
*	    Arg:			命令参数;
*	    TransMode       SD传输模式;
*	    BlkLen:		    数据块长度;
*	    Nob:			数据块数目;
*	    IntMask:		中断允许标志;                 
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      *(RP)SDC_RESPONSE0   SD命令响应                  
*                                                                       
*************************************************/
int SendCmdWaitRespIni(U16 Cmd,U32 Arg,U16 TransMode,U16 BlkLen,U16 Nob,U16 IntMask)
{
    U32 i;
    
    *(RP)SDC_CLOCK_CONTROL=0Xff00;                  //配置SD时钟，512分频,关闭SD 时钟
    
    delay(10);
    *(RP)SDC_CLOCK_CONTROL=0Xff04;                  //打开SD时钟，512分频,开启SD 时钟
    
    delay(10);
    *(RP)SDC_INTERRUPT_STATUS_MASK=IntMask;         //中断状态屏蔽寄存器赋值
    
    delay(10);
    *(RP)SDC_TRANSFER_MODE=TransMode;               //传输模式选择寄存器赋值
    
    delay(10);
    *(RP)SDC_BLOCK_SIZE=BlkLen;                     //数据块长度寄存器赋值
    
    delay(10);
    *(RP)SDC_BLOCK_COUNT=Nob;                       //数据块数目寄存器赋值
    
    delay(10);
    *(RP)SDC_ARGUMENT=Arg;                          //命令参数寄存器赋值
    
    delay(10);
    *(RP)SDC_COMMAND=Cmd;                           //命令控制寄存器赋值
    
    delay(10);                                  
   
    i = *(RP)SDC_INTERRUPT_STATUS & 0x1000;
	
    delay(10);
	
    while(i != 0x1000)                             //判断：是否命令发送完毕，并且收到响应
    {
	   i = *(RP)SDC_INTERRUPT_STATUS & 0x1000;
    }
    delay(160);

    return *(RP)SDC_RESPONSE0;                     //返回命令反馈信息
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       int SendCmdWaitResp(U16 Cmd,U32 Arg,U16 TransMode,U16 BlkLen,U16 Nob,U16 IntMask)                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       向SD控制器发送命令并传送数据，等待SD控制器确认收到命令
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS                                                                
*                                                                       
*	    Cmd:			命令编号;
*	    Arg:			命令参数;
*	    TransMode       SD传输模式;
*	    BlkLen:		    数据块长度;
*	    Nob:			数据块数目;
*	    IntMask:		中断允许标志;                 
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      *(RP)SDC_RESPONSE0   SD命令响应                  
*                                                                       
*************************************************/
int SendCmdWaitResp(U16 Cmd,U32 Arg,U16 TransMode,U16 BlkLen,U16 Nob,U16 IntMask)
{
   U32 i;
    
   *(RP)SDC_CLOCK_CONTROL=0Xff00;               //配置SD时钟
   delay(10000);
   
   *(RP)SDC_CLOCK_CONTROL=0Xff04;               //打开SD时钟		
   delay(10000);
    
    
    *(RP)SDC_INTERRUPT_STATUS_MASK=IntMask;     //中断状态屏蔽寄存器赋值
    *(RP)SDC_TRANSFER_MODE=TransMode;           //传输模式选择寄存器赋值
    *(RP)SDC_BLOCK_SIZE=BlkLen;                 //数据块长度寄存器赋值
    *(RP)SDC_BLOCK_COUNT=Nob;                   //数据块数目寄存器赋值
    *(RP)SDC_ARGUMENT=Arg;                      //命令参数寄存器赋值
    *(RP)SDC_COMMAND=Cmd;                       //命令控制寄存器赋值
    
    delay(10);    
        
    i = *(RP)SDC_INTERRUPT_STATUS & 0x1040;
	
    while(i != 0x1040)                         //判断：命令发送完毕，并且收到响应，数据传输完毕。这三项是否已经都完成。
    {
	i = *(RP)SDC_INTERRUPT_STATUS & 0x1040;
    }
    
    delay(1000);
    
    return *(RP)SDC_RESPONSE0;                  //返回命令反馈信息
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       int SendCmdWaitIdle(U16 Cmd,U32 Arg,U16 IntMask )                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       向SD控制器发送命令，等待SD控制器确认收到命令
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS                                                                
*                                                                       
*	    Cmd:			命令编号;
*	    Arg:			命令参数;  
*	    IntMask:		中断允许标志;                 
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      *(RP)SDC_RESPONSE0   SD命令响应                  
*                                                                       
*************************************************/
int SendCmdWaitIdle(U16 Cmd,U32 Arg,U16 IntMask )
{
    int i;

    *(RP)SDC_CLOCK_CONTROL=0Xff00;                       //配置SD时钟
    
    delay(10);
    *(RP)SDC_CLOCK_CONTROL=0Xff04;                       //打开SD时钟
       
    delay(10);
    *(RP)SDC_COMMAND=Cmd;                                //命令控制寄存器赋值
    
    delay(10);                          
    *(RP)SDC_INTERRUPT_STATUS_MASK=IntMask;              //中断状态屏蔽寄存器赋值
    
    delay(10);
    *(RP)SDC_ARGUMENT=Arg;                               //命令参数寄存器赋值
    
    delay(10);
	
    i = *(RP)SDC_INTERRUPT_STATUS & 0x1000;
	
    while(i != 0x1000)                                  //判断：是否命令发送完毕，并且收到响应
    {
	i = *(RP)SDC_INTERRUPT_STATUS & 0x1000;
    }
    
    delay(10);
        	                	             
    return *(RP)SDC_RESPONSE0;                           //返回命令反馈信息以及数值1
    return 1;  
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       void wait()                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       等待
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS                                                                
*       无                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*       无                  
*                                                                       
**************************************************************/
void wait()
{
	 U32 resp;
	 
	 SendCmdWaitRespIni(0x1aa,0X0,0X0,0x0,0x0,0X0);        //CMD13（SD 命令代号），命令发送使能控制 ，无数据传输，48 位短反馈
	 
	 resp = *(RP)SDC_INTERRUPT_STATUS & 0x1000;
	 
	 while (resp != 0x1000)                                //判断：是否命令发送完毕，并且收到响应
	 {
	   SendCmdWaitRespIni(0x1aa,0X0,0X0,0x0,0x0,0X0);      //CMD13 ,读card status register.

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
*       擦除
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
*       无                  
*                                                                       
*************************************************/
void erase(U32 startaddr,U32 endaddr)
{
	   U32 saddr = startaddr;
	   U32 eaddr = endaddr;
	   
	   delay(100);
	   SendCmdWaitRespIni(0x40a,saddr,0X0,0x0,0x0,0X0);      //CMD35 配置起始地址
	   
	   delay(100);
	   SendCmdWaitRespIni(0x42a,eaddr,0X0,0x0,0x0,0X0);      //CMD36 配置结束地址
	   
	   delay(100);
	   SendCmdWaitRespIni(0x4cb,0X0,0X0,0x0,0x0,0X0);        //CMD38 执行擦除操作
	   
	   delay(1000);
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       U32 InitialSd()                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       初始化SD卡
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS 
*                                                               
*       无                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*       rca    SD卡的rca地址号                  
*                                                                       
*************************************************/
U32 InitialSd(void)
{
    U32 rca;
    
    *(RP)SDC_SOFTWARE_RESET=0x0;                   //触发软复位,对其写0是进行reset 
    
    delay(200);
    *(RP)SDC_SOFTWARE_RESET=0x1;                   //不触发软复位
    
    delay(200);
 
    SendCmdWaitIdle(0x08,0x0,0xfff);               //CMD0，命令发送使能
       
    delay(8000);  
    
    do
    {
    	SendCmdWaitIdle(0x6ea,0x0,0xfff);          //CMD55,以切换到ACMD命令
    	
    	delay(3);  
    	SendCmdWaitIdle(0x52a,0x80ff8000,0xfff);   //ACMD41，向SD控制器发送命令，等待SD控制器确认收到命令
       
    }while(*(RP)SDC_RESPONSE0<0X80008000);

    
    SendCmdWaitRespIni(0x49,0X0,0X0,0x0,0x0,0Xfff);//CMD2，发送CID
    
    SendCmdWaitRespIni(0x6a,0X0,0X0,0x0,0x0,0Xfff);//CMD3，询问卡片发出新的相关地址
    
    rca=*(RP)SDC_RESPONSE0;
    
    SendCmdWaitRespIni(0xea,rca,0X0,0x0,0x0,0Xfff);//CMD7，设置选择性的相关参数
    
    return rca;   
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       void SdSingleW1r1(U32 rca)                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       单块一位写一位读
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS 
*                                                               
*       rca    SD卡的rca地址号                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*       无                  
*                                                                       
*************************************************/
void SdSingleW1r1(U32 rca)
{
    U32 complete;
    erase(0x0,0x4000);//擦除
    
    //***********************配置DMA2进行一位写*************************
    *(RP)DMAC_C2SRCADDR  = MEM_BLOCK_BEGIN;                  // DMAC道2源地址赋为0x30200000
    *(RP)DMAC_C2DESTADDR = SDC_WRITE_BUFER_ACCESS;           // DMAC道2目的地址赋为发送FIFO的地址
    *(RP)DMAC_C2CONTROL  = 0x20149b;                         //传输尺寸0x080,源地址增加目的地址不增加，传输宽度32bit，传输的数目4                                                 				
    *(RP)DMAC_C2CONFIGURATION = 0x380b;	                     //不屏蔽传输中断，屏蔽错误中断，通道使能，传输类型：存储器到外设							       				 	    
    
   
    //配置SD卡位单线传输模式
    SendCmdWaitIdle(0x6ea,rca,0xfff);                        //CMD55，以切换到ACMD命令
    SendCmdWaitIdle(0xca,0x0,0xfff);                         //ACMD6，定义数据线宽度，48 位短反馈，无数据传输
 
    //发送单块写命令   
    SendCmdWaitResp(0x30e,0x00000000,0X2,0x0200,0x1,0Xfff);  //CMD24  1bit mode
    
    complete = *(RP)SDC_INTERRUPT_STATUS;
    
    while((complete |0xfffffffe) !=0xfffffffe)               //响应超时错误
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    } 
    
    delay(500);
    
    //***********************配置DMA2进行一位读*************************						
    *(RP)DMAC_C2SRCADDR  = SDC_READ_BUFER_ACCESS;           // DMAC道2源地址赋为0x1000B038
    *(RP)DMAC_C2DESTADDR = END(0);                          // DMAC道2目的地址赋为0x30220000
    *(RP)DMAC_C2CONTROL  = 0x20249b;                        //传输尺寸0x080,源地址不增加目的地址增加，传输宽度32bit，传输的数目4                                           				
    *(RP)DMAC_C2CONFIGURATION = 0x38d;                      //源外设为MMC，不屏蔽传输中断，屏蔽错误中断，通道使能，传输类型：外设到存储器	
	
    //发送单块读命令 
    SendCmdWaitResp(0x22e,0x00000000,0X0,0x0200,0x1,0Xfff); //CMD17 1bit mode
    
    complete = *(RP)SDC_INTERRUPT_STATUS;
   
    while((complete |0xfffffffd) !=0xfffffffd)              //写数据CRC 校验出错
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    } 
    
    CompareMem(MEM_BLOCK_BEGIN,END(0),0x200);               // 比较2块数据块的内容是否一致，长度512Byte
    printf("SdSingleW1r1 Ok\n\n"); 
}


/*************************************************************
* FUNCTION                                                              
*                                                                       
*       void SdSingleW1r4(U32 rca)                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       单块一位写四位读
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS 
*                                                               
*       rca    SD卡的rca地址号                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*       无                  
*                                                                       
*************************************************/
void SdSingleW1r4(U32 rca)
{
    U32 complete;
    erase(0x0,0x4000);//擦除
    
    //***********************配置DMA2进行一位写*************************
    *(RP)DMAC_C2SRCADDR = MEM_BLOCK_BEGIN;                   //DMAC道2源地址赋为0x30200000
    *(RP)DMAC_C2DESTADDR = SDC_WRITE_BUFER_ACCESS;           //DMAC道2目的地址赋为发送FIFO的地址
    *(RP)DMAC_C2CONTROL = 0x20149b;                          //传输尺寸0x080,源地址增加目的地址不增加，传输宽度32bit，传输的数目4                                                				
    *(RP)DMAC_C2CONFIGURATION = 0x380b;	                     //不屏蔽传输中断，屏蔽错误中断，通道使能，传输类型：存储器到外设							       				 	    
    
    SendCmdWaitIdle(0x6ea,rca,0xfff);                        //CMD55
    SendCmdWaitIdle(0xca,0x0,0xfff);                         //ACMD6
       
    SendCmdWaitResp(0x30e,0x00000000,0X2,0x0200,0x1,0Xfff);  //CMD24  1bit mode
 
    complete = *(RP)SDC_INTERRUPT_STATUS;
    while((complete |0xfffffffe) !=0xfffffffe)               //响应超时错误
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    } 
    
    delay(5000);
   

    //***********************配置DMA2进行四位读*************************
    *(RP)DMAC_C2SRCADDR  = SDC_READ_BUFER_ACCESS;           //DMAC道2源地址赋为0x1000B038
    *(RP)DMAC_C2DESTADDR = END(1);                          //DMAC道2目的地址赋为0x30230000
    *(RP)DMAC_C2CONTROL  = 0x20249b;                        //传输尺寸0x080,源地址不增加目的地址增加，传输宽度32bit，传输的数目4
    *(RP)DMAC_C2CONFIGURATION = 0x38d;                      //源外设为MMC，不屏蔽传输中断，屏蔽错误中断，通道使能，传输类型：外设到存储器	
	
    SendCmdWaitIdle(0x6ea,rca,0xfff);                       //CMD55，以切换到ACMD命令
    SendCmdWaitIdle(0xca,0x2,0xfff);                        //ACMD6，定义数据线宽度，48 位短反馈，无数据传输
   						
    SendCmdWaitResp(0x22e,0x00000000,0X1,0x0200,0x1,0Xfff); //CMD17 4bit mode
    
    complete = *(RP)SDC_INTERRUPT_STATUS;
    
    while((complete |0xfffffffd) !=0xfffffffd)              //写数据CRC 校验出错
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    }
     
    CompareMem(MEM_BLOCK_BEGIN,END(1),0x200);               // 比较2块数据块的内容是否一致，长度512Byte
    printf("SdSingleW1r4 Ok\n\n"); 
    
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       void SdSingleW4r1(U32 rca)                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       单块四位写一位读
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS 
*                                                               
*       rca    SD卡的rca地址号                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*       无                  
*                                                                       
*************************************************/
void SdSingleW4r1(U32 rca)
{
    U32 complete;
    erase(0x0,0x4000);//擦除
    
    //***********************配置DMA2进行四位写*************************
    *(RP)DMAC_C2SRCADDR  = MEM_BLOCK_BEGIN;                  //DMAC道2源地址赋为0x30200000
    *(RP)DMAC_C2DESTADDR = SDC_WRITE_BUFER_ACCESS;           //DMAC道2目的地址赋为发送FIFO的地址
    *(RP)DMAC_C2CONTROL  = 0x20149b;                         //传输尺寸0x080,源地址增加目的地址不增加，传输宽度32bit，传输的数目4                                                  				
    *(RP)DMAC_C2CONFIGURATION = 0x380b;	                     //不屏蔽传输中断，屏蔽错误中断，通道使能，传输类型：存储器到外设							       				 	    
    
    
    SendCmdWaitIdle(0x6ea,rca,0xfff);                        //CMD55，以切换到ACMD命令
    SendCmdWaitIdle(0xca,0x2,0xfff);                         //ACMD6，定义数据线宽度，48 位短反馈，无数据传输
    
  
    SendCmdWaitResp(0x30e,0x00000000,0X3,0x0200,0x1,0Xfff);  //CMD24  1bit mode
    
 
    complete = *(RP)SDC_INTERRUPT_STATUS;
    
    while((complete |0xfffffffe) !=0xfffffffe)               //响应超时错误
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    } 
 	
 
    delay(5000);
   
   //***********************配置DMA2进行一位读*************************
    *(RP)DMAC_C2SRCADDR  = SDC_READ_BUFER_ACCESS;             //DMAC道2源地址赋为0x1000B038
    *(RP)DMAC_C2DESTADDR = END(2);                            //DMAC道2目的地址赋为0x30240000
    *(RP)DMAC_C2CONTROL  = 0x20249b;                          //传输尺寸0x080,源地址不增加目的地址增加，传输宽度32bit，传输的数目4                                                 				
    *(RP)DMAC_C2CONFIGURATION = 0x38d;	                      //源外设为MMC，不屏蔽传输中断，屏蔽错误中断，通道使能，传输类型：外设到存储器
	
	
    SendCmdWaitIdle(0x6ea,rca,0xfff);                         //CMD55
    SendCmdWaitIdle(0xca,0x0,0xfff);                          //ACMD6
    						
    SendCmdWaitResp(0x22e,0x00000000,0X0,0x0200,0x1,0Xfff);   //CMD17 1bit mode
    
    complete = *(RP)SDC_INTERRUPT_STATUS;
    while((complete |0xfffffffd) !=0xfffffffd)                //写数据CRC 校验出错
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    } 
    
    CompareMem(MEM_BLOCK_BEGIN,END(2),0x200);                 //比较2块数据块的内容是否一致，长度512Byte
    printf("SdSingleW4r1 Ok\n\n"); 
    
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       void SdSingleW4r4(U32 rca)                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       单块四位写四位读
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS 
*                                                               
*       rca    SD卡的rca地址号                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*       无                  
*                                                                       
*************************************************/
void SdSingleW4r4(U32 rca)
{
    U32 complete;
    erase(0x0,0x4000);//擦除
    
    //***********************配置DMA2进行四位写*************************
    *(RP)DMAC_C2SRCADDR  = MEM_BLOCK_BEGIN;                    //DMAC道2源地址赋为0x30200000
    *(RP)DMAC_C2DESTADDR = SDC_WRITE_BUFER_ACCESS;             //DMAC道2目的地址赋为发送FIFO的地址
    *(RP)DMAC_C2CONTROL  = 0x20149b;                           //传输尺寸0x080,源地址增加目的地址不增加，传输宽度32bit，传输的数目4                                                  				
    *(RP)DMAC_C2CONFIGURATION = 0x380b;	                       //不屏蔽传输中断，屏蔽错误中断，通道使能，传输类型：存储器到外设							       				 	    
    
    SendCmdWaitIdle(0x6ea,rca,0xfff);                          //CMD55，以切换到ACMD命令
    SendCmdWaitIdle(0xca,0x2,0xfff);                           //ACMD6，定义数据线宽度，48 位短反馈，无数据传输
    
    SendCmdWaitResp(0x30e,0x00000000,0X3,0x0200,0x1,0Xfff);    //CMD24  1bit mode
    
    complete = *(RP)SDC_INTERRUPT_STATUS;
    while ((complete & 0x00000040) != 0x00000040)              //数据传输未完
    {
      complete = *(RP)SDC_INTERRUPT_STATUS;
    } 
    
    while((complete |0xfffffffe) != 0xfffffffe)                //响应超时错误
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    } 
 
    delay(5000);
   
   //***********************配置DMA2进行四位读*************************
    *(RP)DMAC_C2SRCADDR  = SDC_READ_BUFER_ACCESS;              //DMAC道2源地址赋为0x1000B038
    *(RP)DMAC_C2DESTADDR = END(3);                             //DMAC道2目的地址赋为0x30250000
    *(RP)DMAC_C2CONTROL  = 0x20249b;                           //传输尺寸0x080,源地址不增加目的地址增加，传输宽度32bit，传输的数目4                                                  				
    *(RP)DMAC_C2CONFIGURATION = 0x38d;	                       //源外设为MMC，不屏蔽传输中断，屏蔽错误中断，通道使能，传输类型：外设到存储器
	
    SendCmdWaitIdle(0x6ea,rca,0xfff);                          //CMD55，以切换到ACMD命令
    SendCmdWaitIdle(0xca,0x2,0xfff);                           //ACMD6，定义数据线宽度，48 位短反馈，无数据传输
   
    						
    SendCmdWaitResp(0x22e,0x00000000,0X1,0x0200,0x1,0Xfff);    //CMD17 1bit mode
    
    complete = *(RP)SDC_INTERRUPT_STATUS;
    while((complete |0xfffffffd) != 0xfffffffd)                //写数据CRC 校验出错 
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    }
     
    CompareMem(MEM_BLOCK_BEGIN,END(3),0x200);                  //比较2块数据块的内容是否一致，长度512Byte
    printf("SdSingleW4r4 Ok\n\n"); 
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       void SdMultiW1r1(U32 rca)                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       多块一位写一位读
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS 
*                                                               
*       rca    SD卡的rca地址号                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*       无                  
*                                                                       
*************************************************/
void SdMultiW1r1(U32 rca)
{
    U32 complete;
    erase(0x0,0x4000);//擦除
    
    //***********************配置DMA2进行一位写*************************
    *(RP)DMAC_C2SRCADDR = MEM_BLOCK_BEGIN;                      //DMAC道2源地址赋为0x30200000
    *(RP)DMAC_C2DESTADDR = SDC_WRITE_BUFER_ACCESS;              //DMAC道2目的地址赋为发送FIFO的地址
    *(RP)DMAC_C2CONTROL = 0x80149b;                             //传输尺寸0x200,源地址增加目的地址不增加，传输宽度32bit，传输的数目4                				
    *(RP)DMAC_C2CONFIGURATION = 0x380b;		                //不屏蔽传输中断，屏蔽错误中断，通道使能，传输类型：存储器到外设						       				 	    
  
    SendCmdWaitIdle(0x6ea,rca,0xfff);                           //CMD55，切换到ACMD命令
    SendCmdWaitIdle(0xca,0x0,0xfff);                            //ACMD6，配置传输模式为单线模式
      
    SendCmdWaitResp(0x32e,0,0X2,0x0200,0x4,0Xfff);              // CMD25 多块写命令 1bit mode
    
    SendCmdWaitIdle(0x18a,0x0,0xfff);                           //stop CMD12  多块传输需要发送stop命令以停止传输
    
    complete = *(RP)SDC_INTERRUPT_STATUS;
    while((complete |0xfffffffe) != 0xfffffffe)                 //响应超时错误
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    } 
    
    delay(5000);
    //***********************配置DMA2进行一位读*************************	
    *(RP)DMAC_C2SRCADDR  = SDC_READ_BUFER_ACCESS;               //DMAC道2源地址赋为0x1000B038
    *(RP)DMAC_C2DESTADDR = END(4);                              //DMAC道2目的地址赋为0x30250000
    *(RP)DMAC_C2CONTROL  = 0x80249b;                            //传输尺寸0x200,源地址不增加目的地址增加，传输宽度32bit，传输的数目4                 				
    *(RP)DMAC_C2CONFIGURATION = 0x38d;	                        //源外设为MMC，不屏蔽传输中断，屏蔽错误中断，通道使能，传输类型：外设到存储器
	
    SendCmdWaitIdle(0x6ea,rca,0xfff);                           //CMD55，切换到ACMD命令
    SendCmdWaitIdle(0xca,0x0,0xfff);                            //ACMD6，配置传输模式为单线模式
    					
    SendCmdWaitResp(0x24e,0,0X0,0x0200,0x4,0Xfff);              //CMD18  多块读命令  1bit mode
    
    SendCmdWaitIdle(0x18a,0x0,0xfff);                           //stop CMD12，多块传输需要发送stop命令以停止传输
    
    complete = *(RP)SDC_INTERRUPT_STATUS;
    while((complete |0xfffffffd) != 0xfffffffd)                 //写数据CRC 校验出错
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    } 
    
    CompareMem(MEM_BLOCK_BEGIN,END(4),0x200);                   //比较2块数据块的内容是否一致，长度512Byte
    printf("SdMultiW1r1 Ok\n\n"); 
   
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       void SdMultiW1r4(U32 rca)                                              
*                                                                       
* DESCRIPTION                                                           
*       多块一位写四位读
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS 
*                                                               
*       rca    SD卡的rca地址号                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*       无                  
*                                                                       
*************************************************/
void SdMultiW1r4(U32 rca)
{
    U32 complete;
    erase(0x0,0x4000);//擦除
    
    //***********************配置DMA2进行一位写*************************
    *(RP)DMAC_C2SRCADDR = MEM_BLOCK_BEGIN;                      //DMAC道2源地址赋为0x30200000
    *(RP)DMAC_C2DESTADDR = SDC_WRITE_BUFER_ACCESS;              //DMAC道2目的地址赋为发送FIFO的地址
    *(RP)DMAC_C2CONTROL = 0x80149b;                             //传输尺寸0x200,源地址增加目的地址不增加，传输宽度32bit，传输的数目4                                                   				
    *(RP)DMAC_C2CONFIGURATION = 0x380b;	                        //不屏蔽传输中断，屏蔽错误中断，通道使能，传输类型：存储器到外设							       				 	    
  
    SendCmdWaitIdle(0x6ea,rca,0xfff);                           //CMD55
    SendCmdWaitIdle(0xca,0x0,0xfff);                            //ACMD6，配置传输模式为单线模式
      
    SendCmdWaitResp(0x32e,0,0X2,0x0200,0x4,0Xfff);              // CMD25 1bit mode
  
    complete = *(RP)SDC_INTERRUPT_STATUS;
    while ((complete & 0x00000040) != 0x00000040)               //数据传输未完
    {
      complete = *(RP)SDC_INTERRUPT_STATUS;
    } 
   
    while((complete |0xfffffffe) != 0xfffffffe)                 //响应超时错误
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    } 
    
    delay(5000);
    
    SendCmdWaitIdle(0x18a,0x0,0xfff);                           //stop CMD12
    
     //***********************配置DMA2进行四位读*************************
 
    *(RP)DMAC_C2SRCADDR  = SDC_READ_BUFER_ACCESS;              //DMAC道2源地址赋为0x1000B038
    *(RP)DMAC_C2DESTADDR = END(5);                             //DMAC道2目的地址赋为0x30260000
    *(RP)DMAC_C2CONTROL  = 0x80249b;                           //传输尺寸0x200,源地址不增加目的地址增加，传输宽度32bit，传输的数目4                                                                           				
    *(RP)DMAC_C2CONFIGURATION = 0x38d;	                       //源外设为MMC，不屏蔽传输中断，屏蔽错误中断，通道使能，传输类型：外设到存储器                       
	
    SendCmdWaitIdle(0x6ea,rca,0xfff);                          //CMD55
    SendCmdWaitIdle(0xca, 0x2,0xfff);                          //ACMD6，配置传输模式为单线模式
						
    SendCmdWaitResp(0x24e,0,0X1,0x0200,0x4,0Xfff);             // CMD18 4bit mode
    
    complete = *(RP)SDC_INTERRUPT_STATUS;;
    while ((complete & 0x00000040) != 0x00000040)              //数据传输未完
    {
      complete = *(RP)SDC_INTERRUPT_STATUS;
    }
    
    complete = *(RP)SDC_INTERRUPT_STATUS;
    while((complete |0xfffffffd) != 0xfffffffd)                //写数据CRC 校验出错
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    } 
    
    SendCmdWaitIdle(0x18a,0x0,0xfff);                          //stop CMD12
    CompareMem(MEM_BLOCK_BEGIN,END(5),0x200);                  //比较2块数据块的内容是否一致，长度512Byte 
    printf("SdMultiW1r4 Ok\n\n");              
    
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       void SdMultiW4r1(U32 rca)                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       多块四位写一位读
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS 
*                                                               
*       rca    SD卡的rca地址号                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*       无                  
*                                                                       
*************************************************/
void SdMultiW4r1(U32 rca)
{
    U32 complete;
    erase(0x0,0x4000);//擦除
    
    //***********************配置DMA2进行四位写*************************
    *(RP)DMAC_C2SRCADDR = MEM_BLOCK_BEGIN;                    //DMAC道2源地址赋为0x30200000
    *(RP)DMAC_C2DESTADDR = SDC_WRITE_BUFER_ACCESS;            //DMAC道2目的地址赋为发送FIFO的地址
    *(RP)DMAC_C2CONTROL = 0x80149b;                           //传输尺寸0x200,源地址增加目的地址不增加，传输宽度32bit，传输的数目4                                                  				
    *(RP)DMAC_C2CONFIGURATION = 0x380b;		                  //不屏蔽传输中断，屏蔽错误中断，通道使能，传输类型：存储器到外设						       				 	    
  
    SendCmdWaitIdle(0x6ea,rca,0xfff);                         //CMD55
    SendCmdWaitIdle(0xca,0x2,0xfff);                          //ACMD6
      
    SendCmdWaitResp(0x32e,0X0,0X3,0x0200,0x4,0Xfff);          // CMD25 4bit mode
  
    complete = *(RP)SDC_INTERRUPT_STATUS;
    while ((complete & 0x00000040) != 0x00000040)             //数据传输未完
    {
      complete = *(RP)SDC_INTERRUPT_STATUS;
    } 
   
    while((complete |0xfffffffe) != 0xfffffffe)               //响应超时错误
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    } 
    delay(5000);
    
    SendCmdWaitIdle(0x18a,0x0,0xfff);                         //stop CMD12
    
    //***********************配置DMA2进行一位读*************************
    *(RP)DMAC_C2SRCADDR  = SDC_READ_BUFER_ACCESS;             //DMAC道2源地址赋为0x1000B038
    *(RP)DMAC_C2DESTADDR = END(6);                            //DMAC道2目的地址赋为0x30270000
    *(RP)DMAC_C2CONTROL  = 0x80249b;                          //传输尺寸0x200,源地址不增加目的地址增加，传输宽度32bit，传输的数目4                                                 				
    *(RP)DMAC_C2CONFIGURATION = 0x38d;	                      //源外设为MMC，不屏蔽传输中断，屏蔽错误中断，通道使能，传输类型：外设到存储器
	
	
    SendCmdWaitIdle(0x6ea,rca,0xfff);                         //CMD55
    SendCmdWaitIdle(0xca,0x0,0xfff);                          //ACMD6
							
    SendCmdWaitResp(0x24e,0X0,0X0,0x0200,0x4,0Xfff);          //CMD18 1bit mode
    
    complete = *(RP)SDC_INTERRUPT_STATUS;;
    while ((complete & 0x00000040) != 0x00000040)             //数据传输未完
    {
      complete = *(RP)SDC_INTERRUPT_STATUS;
    }
   
    while((complete |0xfffffffd) != 0xfffffffd)               //写数据CRC 校验出错
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    } 
    
    SendCmdWaitIdle(0x18a,0x0,0xfff);                         //stop CMD12
    CompareMem(MEM_BLOCK_BEGIN,END(6),0x200);                 //比较2块数据块的内容是否一致，长度512Byte 
    printf("SdMultiW4r1 Ok\n\n");
    
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       void SdMultiW4r4(U32 rca)                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       多块四位写四位读
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS 
*                                                               
*       rca    SD卡的rca地址号                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*       无                  
*                                                                       
*************************************************/
void SdMultiW4r4(U32 rca)
{
    U32 complete;
    erase(0x0,0x4000);//擦除
    
    //***********************配置DMA2进行四位写*************************
    *(RP)DMAC_C2SRCADDR = MEM_BLOCK_BEGIN;                    //DMAC道2源地址赋为0x30200000
    *(RP)DMAC_C2DESTADDR = SDC_WRITE_BUFER_ACCESS;            //DMAC道2目的地址赋为发送FIFO的地址
    *(RP)DMAC_C2CONTROL = 0x80149b;                           //传输尺寸0x200,源地址增加目的地址不增加，传输宽度32bit，传输的数目4                                                				
    *(RP)DMAC_C2CONFIGURATION = 0x380b;	                      //不屏蔽传输中断，屏蔽错误中断，通道使能，传输类型：存储器到外设							       				 	    
  
    SendCmdWaitIdle(0x6ea,rca,0xfff);                         //CMD55
    SendCmdWaitIdle(0xca,0x2,0xfff);                          //ACMD6
      
    SendCmdWaitResp(0x32e,0X0,0X3,0x0200,0x4,0Xfff);          // CMD25 4bit mode
  
    complete = *(RP)SDC_INTERRUPT_STATUS;
    while ((complete & 0x00000040) != 0x00000040)             //数据传输未完
    {
      complete = *(RP)SDC_INTERRUPT_STATUS;
    } 
    
    complete = *(RP)SDC_INTERRUPT_STATUS;
    while((complete |0xfffffffe) != 0xfffffffe)               //响应超时错误
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    } 
    
    delay(5000);
    
    SendCmdWaitIdle(0x18a,0x0,0xfff);                         //stop CMD12
    
    //***********************配置DMA2进行四位读*************************
    *(RP)DMAC_C2SRCADDR  = SDC_READ_BUFER_ACCESS;             //DMAC道2源地址赋为0x1000B038
    *(RP)DMAC_C2DESTADDR = END(7);                            //DMAC道2目的地址赋为0x30270000                          
    *(RP)DMAC_C2CONTROL  = 0x80249b;                          //传输尺寸0x200,源地址不增加目的地址增加，传输宽度32bit，传输的数目4                                                				
    *(RP)DMAC_C2CONFIGURATION = 0x38d;	                      //源外设为MMC，不屏蔽传输中断，屏蔽错误中断，通道使能，传输类型：外设到存储器
	
    SendCmdWaitIdle(0x6ea,rca,0xfff);                         //CMD55
    SendCmdWaitIdle(0xca, 0x2,0xfff);                         //ACMD6
	
    	
    					
    SendCmdWaitResp(0x24e,0,0X1,0x0200,0x4,0Xfff);            // CMD18 4bit mode
    
    complete = *(RP)SDC_INTERRUPT_STATUS;;                    //中断状态寄存器内容赋值
    while ((complete & 0x00000040) != 0x00000040)             //数据传输未完
    {
      complete = *(RP)SDC_INTERRUPT_STATUS;
    }
    
    while((complete |0xfffffffd) != 0xfffffffd)               //写数据CRC 校验出错
    {
      printf("CRC ERROR");
      complete = *(RP)SDC_INTERRUPT_STATUS;
      break;
    } 
    
    SendCmdWaitIdle(0x18a,0x0,0xfff);                         //stop CMD12
    CompareMem(MEM_BLOCK_BEGIN,END(7),0x200);                 //比较2块数据块的内容是否一致，长度512Byte
    printf("SdMultiW4r4 Ok\n\n");
    
}

/*************************************************************
* FUNCTION                                                              
*                                                                       
*       void  FifoReset()                                             
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       FIFO初始化
*                                                                       
* AUTHOR                                                                
*                                                                       
*       yao                   
*                                                                       
* INPUTS 
*                                                               
*       无                
*                                                                       
* OUTPUTS                                                               
*                                                                       
*       无                  
*                                                                       
*************************************************/
void  FifoReset(void)
{
    U32 tmp;
    *(RP)SDC_CLOCK_CONTROL=0X0000;             		       //配置SD时钟
    delay(10);
    
    *(RP)SDC_CLOCK_CONTROL=0X0004;                         //打开SD时钟
    delay(2);
    
    while((*(RP)SDC_INTERRUPT_STATUS & 0x200)!=0x200)      //接收FIFO非空
    {
    	tmp = *(RP)SDC_READ_BUFER_ACCESS;
    	delay(10);
    }
    
    while((*(RP)SDC_INTERRUPT_STATUS & 0x800)!=0x800)      //发送FIFO非空
    {
    	*(RP)SDC_WRITE_BUFER_ACCESS = tmp;
    	delay(10);
    }

}
