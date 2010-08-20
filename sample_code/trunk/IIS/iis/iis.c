/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  iis.c
 *
 *	File Description:   IIS��Ƶ���ŵ�������������
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *     1.0           2009.04.25          lbn            
 *   		      	
 **************************************************************************************/
 
#include	"iis.h"
#include	"intc.h"
#include	"sep4020.h"

/*������Ƶ������*/
#define      SRATE     44                //sample rate 8k,11k,22k,44k

/*������Ƶ����оƬUDA1341��L3����*/
#define      L3C     (1<<4)              //ʹ��GPIO_PORTD4ģ��L3CLOCK�ź�   
#define      L3M     (1<<3)              //ʹ��GPIO_PORTD3ģ��L3MODE�ź�     
#define      L3D     (1<<1)              //ʹ��GPIO_PORTD1ģ��L3DATA�ź�     

/*************************************************************************************************************
* FUNCTION                                                              
*                                                                       
*       L3���ߵĵ�ַģʽ                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	L3���ߵĵ�ַģʽҪ��ͨ��L3����ѡ��һ��ͨ���豸��Ϊ������ݴ��䶨��һ��Ŀ�ļĴ�����
*       
*       ��ַģʽ���ص���ͨ������L3MODE����L3CLOCK�´���8�����������8λ���ݵĴ��䡣
*       
*       UDA1341�ĵ�ַ��000101���ӵ�2λ����7λ�����е�7λ�����λ����2λ�����λ����0λ����1λ�Ǵ������ݵ����͡�
*       
*       ����������STATUS��DATA0��DATA1��
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
*		data = 000101xx + 10           �������ݵ�����ΪSTATUSģʽ   	
*			
*		data = 000101xx + 00	        �������ݵ�����ΪDATA0ģʽ
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	��                
*                                                                       
**************************************************************************************************************/
void _WrL3Addr(U8 data)
{       
    S32 i;
    
    *(RP)(GPIO_PORTD_DATA) &= ~(L3D | L3M) | L3C; //L3D=L, L3M=L(in address mode), L3C=H

    for(i=0; i<8; i++)                            //LSB first  ���������λ
    {
        if(data & 0x1)                            //If data's LSB is 'H'  ���L3D���λΪ��
        {  
           *(RP)(GPIO_PORTD_DATA) |= L3D;         //L3D=H
           
           *(RP)(GPIO_PORTD_DATA) &= ~L3C;        //L3C=L   
            delay(4);            
           *(RP)(GPIO_PORTD_DATA) |= L3C;         //L3C=H
        }
        else                                      //If data's LSB is 'L' ���L3D���λΪ��
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
*       L3�������ݴ���ģʽ                                              
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	���ݴ�������������"L3DATA write"ָ�����ݴ����΢��������UDA1341��"L3DATA peak read"ָ�����ݴ�����"L3DATA write"�෴��
*       
*       ѡ��һ����Ч�ĵ�ַ�������ݴ��䣬ֱ��һ���µ�ַ������֡�
*              
*       ���ݴ��䶼�ǰ��ֽڿ�Ƚ��д��䣬8λΪһ�����ݣ����յ�һ���ֽ����ݾͱ����浽UDA1341�ڡ�
*       
*       ���ݴ��䱾�������ַģʽ�����Ƶģ�ֻ����L3MODE���ߡ�
*                                                                       
* AUTHOR                                                                
*                                                                       
*       PROCHIP             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	U8 data,int halt
*			
*		data            �������ݵ�����ΪSTATUS��DATA0ģʽ   	
*
*		halt            Ϊ�����ݼ������䣬�������ݴ������	
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	��                
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
        if(data & 0x1)                                //if data's LSB is 'H'   ����������λΪ��
        {
           *(RP)(GPIO_PORTD_DATA) |= L3D;             //L3D=H

           *(RP)(GPIO_PORTD_DATA) &= ~L3C;            //L3C=L
            delay(4);
           *(RP)(GPIO_PORTD_DATA) |= L3C ;            //L3C=H                       
        }  
        else                                          //If data's LSB is 'L'   ����������λΪ��
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
*       ��ʼ������оƬcodec                                            
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	�趨L3���߿����źţ���Ե�ַָ��������ݴ���     
*                                                                       
* AUTHOR                                                                
*                                                                       
*       PROCHIP             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	��			
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	��                
*                                                                       
**************************************************************************************************************************/
void InitCodec(void)
{
    //L3D��L3M��L3C�����źŷֱ���ͨ��GPIO_PORTD��1��3��4��ģ����õ��� 
    *(RP)(GPIO_PORTD_SEL) |= 0xd<<1;                     //GPD[4:1] 1 1010
    *(RP)(GPIO_PORTD_DATA) &= ~(L3D) | L3M | L3C;        //Start condition : L3M=H, L3C=H,L3D=L
    *(RP)(GPIO_PORTD_DIR) &= ~(0xd<<1);                  //GPD[4:1]=00_0 Output(L3CLOCK):Output(L3DATA):Output(L3MODE)   
   

    //L3 ���߽ӿ�
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
*       ��ʼ��IIS������                                          
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	���ݲ�ͬ�Ĳ�����������I2S_CTRL       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       PROCHIP             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	��			
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	��                
*                                                                       
*****************************************************************************************************************************/
void InitIIS(void)
{	
   switch(SRATE)
   {
     //���ݲ����ʣ�����IIS�Ŀ��ƼĴ���,���������������������λ��������������Ƶ����λ�����ݴ��䷽�򡢹���ģʽ����˫�����Ͳ����ʵ�,��Ƶ�Ĺ�ʽΪ96M/((0x1f+1)*2)=1500K �� 1500K/32=46.875K (��44.1K�Ĳ�����Ϊ��)  
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
	
    *(RP)I2S_INT |= 0x1<<2;                   //I2S��������,DMA����ʹ��

    *(RP)I2S_CTRL &= ~(STOP(1));              //��ʼ��������            

    return;
}

/*****************************************************************************************************************************
* FUNCTION                                                              
*                                                                       
*       ��ʼ�������������ź�                                          
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	�������ݵ�����������nCSC�˿ڣ�ʵ��¼��������       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       PROCHIP             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	��			
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	��                
*                                                                       
*****************************************************************************************************************************/
void InitDatapath(void)
{
    *(RP)(GPIO_PORTG_SEL) |= 0x1<<11;         //����GPIO_PORTG11Ϊͨ����;
    *(RP)(GPIO_PORTG_DATA) |= 0x1<<11;	      //����GPIO_PORTG11Ϊ�ߵ�ƽ
    *(RP)(GPIO_PORTG_DIR) &= ~(0x1<<11);      //����GPIO_PORTG11Ϊ��� 
   
    return;
}

/*****************************************************************************************************************************
* FUNCTION                                                              
*                                                                       
*       ��ʼ��PWM������                                          
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	���ݲ�ͬ�Ĳ�����Ϊ����оƬcodec���ò�ͬ��ʱ��Ƶ��       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       PROCHIP             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	��			
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	��                
*                                                                       
*****************************************************************************************************************************/
void InitPWM(void)
{
    *(RP)PWM3_CTRL &= 0x00;         //����PWM3���ƼĴ��������������ƽѡ��GPIO �������ѡ��͹���ģʽ
  
    switch(SRATE)                   //���ݲ���������PWM3_DIV���幫ʽΪ96M/(PWM3_DIV �� 2)=12M��12M/256=46.875 (��44.1K�Ĳ�����Ϊ��)
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
    
    //����ռ�ձ�Ϊ50%��ʹ��PWM3
    *(RP)PWM3_PERIOD &= 0x2;        //����PWMͨ��3�����ڼĴ���
    *(RP)PWM3_DATA |= 0x1;          //����PWMͨ��3�����ݼĴ���
    *(RP)PWM_ENABLE |= 0x1<<3;      //ʹ��PWMͨ��3

    return;
}

/*****************************************************************************************************************************
* FUNCTION                                                              
*                                                                       
*       ��ʼ��DMA������                                          
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	�趨DMA��ԴĿ�ĵ�ַ�����ơ����üĴ���       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       PROCHIP             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	��			
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	��                
*                                                                       
*****************************************************************************************************************************/
void InitDMA(void)
{
    write_reg(DMAC_C0SRCADDR, 0x30300000);   //����DMA������Դ��ַ
    write_reg(DMAC_C0DESTADD, I2S_DATA);     //����DMA������Ŀ�ĵ�ַ
    write_reg(DMAC_C0CONTROL, TRANSFERSIZE(0x800) + DI(0) + SI(1) + DESTSIZE(2) +SOURCESIZE(2) + DESTBURST(3) + SOURCEBURST(3)); //control, 0X800, SI, 32, Burst = 4
  	                                         //����DMA������ͨ��0�Ŀ��ƼĴ�������������ߴ硢Դ��Ŀ�ĵ�ַ���ӡ�Դ��Ŀ��(SWidth)�����ȡ�Դ��Ŀ��Burst �������Ŀ��
    write_reg(DMAC_C0CONFIGURATION,0x200B);  //����DMA���������üĴ�����������ǰ����ͨ���š�Դ����š�Ŀ������š����Ʒ�����������ж����Ρ��жϴ������Ρ��������͡�ͨ��ʹ�ܵ�

    return;
}

/*****************************************************************************************************************************
* FUNCTION                                                              
*                                                                       
*       ��Դ������жϴ���                                          
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	Ϊ�����������䣬��DMA������ʹ�ܴ�������жϣ����жϴ�������������DMAͨ����Ϣ����������       
*                                                                       
* AUTHOR                                                                
*                                                                       
*       PROCHIP             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	��			
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	��                
*                                                                       
*****************************************************************************************************************************/
void BufferHanderTr(void)
{
    U32 tmp = read_reg(DMAC_C0SRCADDR);      //DMA������ͨ��0Դ��ַ�Ĵ���     

    if (tmp>=0x31990000)                     //������Դ��С��SDRAM�ڷ�����Ӧ���ڴ�ռ�
    {
       tmp = 0x30300000;                     
    }
		 
    write_reg(DMAC_C0DESTADD, I2S_DATA);     //ͨ��DMA������ͨ��0Ŀ�ĵ�ַ
	
    write_reg(DMAC_C0SRCADDR, tmp);          //����DMA������ͨ��0Դ��ַ
   
    //����DMA������ͨ��0�Ŀ��ƼĴ�������������ߴ硢Դ��Ŀ�ĵ�ַ���ӡ�Դ��Ŀ��(SWidth)�����ȡ�Դ��Ŀ��Burst �������Ŀ��
    write_reg(DMAC_C0CONTROL, TRANSFERSIZE(0x800) + DI(0) + SI(1) + DESTSIZE(2) + SOURCESIZE(2)+DESTBURST(3)+SOURCEBURST(3) );
    
    //����DMA���������üĴ�����������ǰ����ͨ���š�Դ����š�Ŀ������š����Ʒ�����������ж����Ρ��жϴ������Ρ��������͡�ͨ��ʹ�ܵ�
    write_reg(DMAC_C0CONFIGURATION,0x200B); 

    return;
}

/*****************************************************************************************************************************
* FUNCTION                                                              
*                                                                       
*       ��ʼ��DMA��������ж�״̬����Ĵ���                                          
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	�����������ж�״̬�Ĵ����������жϴ���     
*                                                                       
* AUTHOR                                                                
*                                                                       
*       PROCHIP             
*                                                                       
* INPUTS                                                                
*                                                                       
*      	��			
*                                                                       
* OUTPUTS                                                               
*                                                                       
*     	��                
*                                                                       
*****************************************************************************************************************************/
void DMAInt(void)
{   
    BufferHanderTr();            //��Դ������жϴ���  

    //��ϵͳӦ���У����Ҫ���ĳ���ж���Ҫ�ȶԸ�Ϊд1��Ȼ������ٶԸ�λд0
    *(RP)DMAC_INTTCCLEAR |= 0x01;  
    *(RP)DMAC_INTTCCLEAR &= ~(0x01);  
    
    return;
}

/*****************************************************************************************************************************
* FUNCTION                                                              
*                                                                       
*       ��ʱ�������                                          
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*      	�趨ѭ����ʱ     
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
*     	��                
*                                                                       
*****************************************************************************************************************************/
void delay(int n)
{
    int i;
    for(i=0; i<n; i++);		
    
    return;
}