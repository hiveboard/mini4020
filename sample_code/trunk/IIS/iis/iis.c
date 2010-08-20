/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  iis.c
 *
 *	File Description:   IIS音频播放的主调函数描述
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *     1.0           2009.04.25          lbn            
 *   		      	
 **************************************************************************************/
 
#include	"iis.h"
#include	"intc.h"
#include	"sep4020.h"

/*定义音频采样率*/
#define      SRATE     44                //sample rate 8k,11k,22k,44k

/*定义音频解码芯片UDA1341的L3总线*/
#define      L3C     (1<<4)              //使用GPIO_PORTD4模拟L3CLOCK信号   
#define      L3M     (1<<3)              //使用GPIO_PORTD3模拟L3MODE信号     
#define      L3D     (1<<1)              //使用GPIO_PORTD1模拟L3DATA信号     

/*************************************************************************************************************
* FUNCTION                                                              
*                                                                       
*       L3总线的地址模式                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	L3总线的地址模式要求通过L3总线选择一个通信设备，为随后数据传输定义一个目的寄存器。
*       
*       地址模式的特点是通过拉低L3MODE，在L3CLOCK下触发8个脉冲来完成8位数据的传输。
*       
*       UDA1341的地址是000101，从第2位到第7位，其中第7位是最高位，第2位是最低位，第0位到第1位是传输数据的类型。
*       
*       有三种类型STATUS、DATA0和DATA1。
*       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       PROCHIP             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U8 data
*			
*		data = 000101xx + 10           传输数据的类型为STATUS模式   	
*			
*		data = 000101xx + 00	        传输数据的类型为DATA0模式
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	无                
*                                                                       
**************************************************************************************************************/
void _WrL3Addr(U8 data)
{       
    S32 i;
    
    *(RP)(GPIO_PORTD_DATA) &= ~(L3D | L3M) | L3C; //L3D=L, L3M=L(in address mode), L3C=H

    for(i=0; i<8; i++)                            //LSB first  首先是最低位
    {
        if(data & 0x1)                            //If data's LSB is 'H'  如果L3D最低位为高
        {  
           *(RP)(GPIO_PORTD_DATA) |= L3D;         //L3D=H
           
           *(RP)(GPIO_PORTD_DATA) &= ~L3C;        //L3C=L   
            delay(4);            
           *(RP)(GPIO_PORTD_DATA) |= L3C;         //L3C=H
        }
        else                                      //If data's LSB is 'L' 如果L3D最低位为低
        { 
           *(RP)(GPIO_PORTD_DATA) &= ~L3D;        //L3D=L
             
           *(RP)(GPIO_PORTD_DATA) &= ~L3C;        //L3C=L
            delay(4);
           *(RP)(GPIO_PORTD_DATA) |= L3C;         //L3C=H
        }
        
        data >>= 1;
    }
  
    *(RP)(GPIO_PORTD_DATA) |= (L3C | L3M);        //L3M=H,L3C=H      

    return;
}

/****************************************************************************************************************************
* FUNCTION                                                              
*                                                                       
*       L3总线数据传输模式                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	数据传输有两个方向，"L3DATA write"指出数据传输从微处理器到UDA1341，"L3DATA peak read"指出数据传输与"L3DATA write"相反。
*       
*       选择一个有效的地址进行数据传输，直到一个新地址命令出现。
*              
*       数据传输都是按字节宽度进行传输，8位为一组数据，接收到一个字节数据就被保存到UDA1341内。
*       
*       数据传输本质上与地址模式是相似的，只不过L3MODE拉高。
*                                                                       
* AUTHOR                                                                
*                                                                       
*       PROCHIP             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U8 data,int halt
*			
*		data            传输数据的类型为STATUS或DATA0模式   	
*
*		halt            为零数据继续传输，非零数据传输结束	
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	无                
*                                                                       
*****************************************************************************************************************************/
void _WrL3Data(U8 data,int halt)
{
   
    S32 i;

    if(halt)
    {
        *(RP)(GPIO_PORTD_DATA) &= ~(L3D | L3M) | L3C; //L3C=H(while tstp, L3 interface halt condition)                                 
    }

    *(RP)(GPIO_PORTD_DATA) |= (L3C | L3M);            //L3M=H(in data transfer mode)  
                                 
    for(i=0; i<8; i++)
    {
        if(data & 0x1)                                //if data's LSB is 'H'   如果数据最低位为高
        {
           *(RP)(GPIO_PORTD_DATA) |= L3D;             //L3D=H

           *(RP)(GPIO_PORTD_DATA) &= ~L3C;            //L3C=L
            delay(4);
           *(RP)(GPIO_PORTD_DATA) |= L3C ;            //L3C=H                       
        }  
        else                                          //If data's LSB is 'L'   如果数据最低位为低
        {  
           *(RP)(GPIO_PORTD_DATA) &= ~L3D;            //L3D=L

           *(RP)(GPIO_PORTD_DATA) &= ~L3C;            //L3C=L
            delay(4);
           *(RP)(GPIO_PORTD_DATA) |= L3C;             //L3C=H                         
        }
        
        data >>= 1;                                   //For check next bit
    }
  
    *(RP)(GPIO_PORTD_DATA) |= (L3C | L3M);            //L3M=H,L3C=H

   return;
}

/*************************************************************************************************************************
* FUNCTION                                                              
*                                                                       
*       初始化解码芯片codec                                            
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	设定L3总线控制信号，针对地址指令进行数据传输     
*                                                                       
* AUTHOR                                                                
*                                                                       
*       PROCHIP             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	无			
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	无                
*                                                                       
**************************************************************************************************************************/
void InitCodec(void)
{
    //L3D、L3M和L3C三个信号分别是通过GPIO_PORTD的1、3、4脚模拟而得到的 
    *(RP)(GPIO_PORTD_SEL) |= 0xd<<1;                     //GPD[4:1] 1 1010
    *(RP)(GPIO_PORTD_DATA) &= ~(L3D) | L3M | L3C;        //Start condition : L3M=H, L3C=H,L3D=L
    *(RP)(GPIO_PORTD_DIR) &= ~(0xd<<1);                  //GPD[4:1]=00_0 Output(L3CLOCK):Output(L3DATA):Output(L3MODE)   
   

    //L3 总线接口
    _WrL3Addr(0x14 + 2);	    //STATUS (000101xx+10)
    _WrL3Data(0x60,0);          //0,1,10, 000,0 : Status 0,Reset,256fs,IIS-bus,no DC-filtering
 
    _WrL3Addr(0x14 + 2);	    //STATUS (000101xx+10)
    _WrL3Data(0x20,0);	        //0,0,10, 000,0 : Status 0,No reset,256fs,IIS-bus,no DC-filtering
	
    _WrL3Addr(0x14 + 2);	    //STATUS (000101xx+10)
    _WrL3Data(0xc1,0);	        //1,1,0,0, 0,0,01 : Status 1,Gain of DAC 6 dB,Gain of ADC 0dB,ADC non-inverting,DAC non-inverting,Single speed playback,ADC-Off DAC-On
							
    _WrL3Addr(0x14 + 0);	    //DATA0 (000101xx+00)
    _WrL3Data(0x0f,0);	        //00,00 ffff  : Data0, Volume control (6 bits)  -14dB    

    _WrL3Addr(0x14 + 0);	    //DATA0 (000101xx+00)
    _WrL3Data(0x7b,0);	        //01,11 10,11 : Data0, Bass Boost 18~24dB, Treble 6dB
	
    _WrL3Addr(0x14 + 0);        //DATA0 (000101xx+00)
    _WrL3Data(0x83,0);          //10,0,0 0,0,11:Data0, peak detection position, de-emphasis (2 bits), mute,  mode switch (2 bits)
	
    return;
}

/*****************************************************************************************************************************
* FUNCTION                                                              
*                                                                       
*       初始化IIS控制器                                          
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	根据不同的采样率来设置I2S_CTRL       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       PROCHIP             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	无			
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	无                
*                                                                       
*****************************************************************************************************************************/
void InitIIS(void)
{	
   switch(SRATE)
   {
     //根据采样率，配置IIS的控制寄存器,包括静音、发声、软件复位、左右声道、音频数据位宽、数据传输方向、工作模式、单双声道和采样率等,分频的公式为96M/((0x1f+1)*2)=1500K ， 1500K/32=46.875K (以44.1K的采样率为例)  
      case 8:  
           write_reg(I2S_CTRL, MUTE(0) + RESET_1(0) + ALIGN(0) + WS(0) + WID(1) + DIR(0)+ MODE(0) + MONO(0) + STOP(1) + DIV(0xaf)); //8.15k,8.5kbest
	   break;	
      case 11:
           write_reg(I2S_CTRL, MUTE(0) + RESET_1(0) + ALIGN(0) + WS(0) + WID(1) + DIR(0)+ MODE(0) + MONO(0) + STOP(1) + DIV(0x7f)); //11k,11.7kbest
	   break;	
      case 22: 
           write_reg(I2S_CTRL, MUTE(0) + RESET_1(0) + ALIGN(0) + WS(0) + WID(1) + DIR(0)+ MODE(0) + MONO(0) + STOP(1) + DIV(0x3f)); //23.4kbest
           break;    
      case 44: 
           write_reg(I2S_CTRL, MUTE(0) + RESET_1(0) + ALIGN(0) + WS(0) + WID(1) + DIR(0)+ MODE(0) + MONO(0) + STOP(1) + DIV(0x1f)); //46.875kbest
	   break;
   }
	
    *(RP)I2S_INT |= 0x1<<2;                   //I2S发送数据,DMA传输使能

    *(RP)I2S_CTRL &= ~(STOP(1));              //开始播放音乐            

    return;
}

/*****************************************************************************************************************************
* FUNCTION                                                              
*                                                                       
*       初始化数据流控制信号                                          
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	根据数据的流向，来控制nCSC端口，实现录放音操作       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       PROCHIP             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	无			
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	无                
*                                                                       
*****************************************************************************************************************************/
void InitDatapath(void)
{
    *(RP)(GPIO_PORTG_SEL) |= 0x1<<11;         //配置GPIO_PORTG11为通用用途
    *(RP)(GPIO_PORTG_DATA) |= 0x1<<11;	      //配置GPIO_PORTG11为高电平
    *(RP)(GPIO_PORTG_DIR) &= ~(0x1<<11);      //配置GPIO_PORTG11为输出 
   
    return;
}

/*****************************************************************************************************************************
* FUNCTION                                                              
*                                                                       
*       初始化PWM控制器                                          
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	根据不同的采样率为解码芯片codec配置不同的时钟频率       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       PROCHIP             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	无			
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	无                
*                                                                       
*****************************************************************************************************************************/
void InitPWM(void)
{
    *(RP)PWM3_CTRL &= 0x00;         //配置PWM3控制寄存器，包括输出电平选择、GPIO 输入输出选择和工作模式
  
    switch(SRATE)                   //根据采样率配置PWM3_DIV具体公式为96M/(PWM3_DIV × 2)=12M，12M/256=46.875 (以44.1K的采样率为例)
    {
      case 8:
           *(RP)PWM3_DIV = 0x17;   
           break;  
      case 11:
           *(RP)PWM3_DIV = 0x11;    
           break;  
      case 22:
           *(RP)PWM3_DIV = 0x8;    
           break;  
      case 44:
           *(RP)PWM3_DIV = 0x4;    
           break;
     }
    
    //配置占空比为50%并使能PWM3
    *(RP)PWM3_PERIOD &= 0x2;        //配置PWM通道3的周期寄存器
    *(RP)PWM3_DATA |= 0x1;          //配置PWM通道3的数据寄存器
    *(RP)PWM_ENABLE |= 0x1<<3;      //使能PWM通道3

    return;
}

/*****************************************************************************************************************************
* FUNCTION                                                              
*                                                                       
*       初始化DMA控制器                                          
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	设定DMA的源目的地址、控制、配置寄存器       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       PROCHIP             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	无			
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	无                
*                                                                       
*****************************************************************************************************************************/
void InitDMA(void)
{
    write_reg(DMAC_C0SRCADDR, 0x30300000);   //配置DMA控制器源地址
    write_reg(DMAC_C0DESTADD, I2S_DATA);     //配置DMA控制器目的地址
    write_reg(DMAC_C0CONTROL, TRANSFERSIZE(0x800) + DI(0) + SI(1) + DESTSIZE(2) +SOURCESIZE(2) + DESTBURST(3) + SOURCEBURST(3)); //control, 0X800, SI, 32, Burst = 4
  	                                         //配置DMA控制器通道0的控制寄存器，包括传输尺寸、源与目的地址增加、源与目的(SWidth)传输宽度、源与目的Burst 传输的数目等
    write_reg(DMAC_C0CONFIGURATION,0x200B);  //配置DMA控制器配置寄存器，包括当前链表通道号、源外设号、目的外设号、控制方、传输完成中断屏蔽、中断错误屏蔽、传输类型、通道使能等

    return;
}

/*****************************************************************************************************************************
* FUNCTION                                                              
*                                                                       
*       音源传输的中断处理                                          
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	为了能连续传输，在DMA传输中使能传输完成中断，在中断处理函数中再设置DMA通道信息，继续传输       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       PROCHIP             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	无			
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	无                
*                                                                       
*****************************************************************************************************************************/
void BufferHanderTr(void)
{
    U32 tmp = read_reg(DMAC_C0SRCADDR);      //DMA控制器通道0源地址寄存器     

    if (tmp>=0x31990000)                     //根据音源大小在SDRAM内分配相应的内存空间
    {
       tmp = 0x30300000;                     
    }
		 
    write_reg(DMAC_C0DESTADD, I2S_DATA);     //通过DMA控制器通道0目的地址
	
    write_reg(DMAC_C0SRCADDR, tmp);          //配置DMA控制器通道0源地址
   
    //配置DMA控制器通道0的控制寄存器，包括传输尺寸、源与目的地址增加、源与目的(SWidth)传输宽度、源与目的Burst 传输的数目等
    write_reg(DMAC_C0CONTROL, TRANSFERSIZE(0x800) + DI(0) + SI(1) + DESTSIZE(2) + SOURCESIZE(2)+DESTBURST(3)+SOURCEBURST(3) );
    
    //配置DMA控制器配置寄存器，包括当前链表通道号、源外设号、目的外设号、控制方、传输完成中断屏蔽、中断错误屏蔽、传输类型、通道使能等
    write_reg(DMAC_C0CONFIGURATION,0x200B); 

    return;
}

/*****************************************************************************************************************************
* FUNCTION                                                              
*                                                                       
*       初始化DMA传输完成中断状态清除寄存器                                          
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	清除传输完成中断状态寄存器，进入中断处理     
*                                                                       
* AUTHOR                                                                
*                                                                       
*       PROCHIP             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	无			
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	无                
*                                                                       
*****************************************************************************************************************************/
void DMAInt(void)
{   
    BufferHanderTr();            //音源传输的中断处理  

    //在系统应用中，如果要清除某个中断需要先对该为写1，然后接着再对该位写0
    *(RP)DMAC_INTTCCLEAR |= 0x01;  
    *(RP)DMAC_INTTCCLEAR &= ~(0x01);  
    
    return;
}

/*****************************************************************************************************************************
* FUNCTION                                                              
*                                                                       
*       延时处理机制                                          
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	设定循环延时     
*                                                                       
* AUTHOR                                                                
*                                                                       
*       PROCHIP             
*                                                                       
* INPUTS                                                                
*                                                                       
*       n			
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	无                
*                                                                       
*****************************************************************************************************************************/
void delay(int n)
{
    int i;
    for(i=0; i<n; i++);		
    
    return;
}