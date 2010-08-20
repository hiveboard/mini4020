#include "intc.h"
#include "key.h"
#include <stdio.h>
#include	"sep4020.h"
#define X_LOCATION_CMD  0x90
#define Y_LOCATION_CMD  0xd0

#define PEN_UP          0
#define PEN_UNSURE      1 
#define PEN_DOWN        2 
#define  LCDWIDTH	        320
#define  LCDHEIGHT	        240
/*定义采样最小最大的点数*/
#define  TPMIN 	            400
#define  TPMAX 	            3800

U16 xpix ,ypix ,zpix ;
/******************************************************************
*函数名：DisableTp,EnableTp
*入口参数：无
*返回参数：无
*函数功能：初始化touchpad
*******************************************************************/

/*void DisableTp(void)
	{	unmask_irq(INTSRC_EXINT8);
	irq_disable(INTSRC_EXINT8);
	}

void EnableTp(void)
	{	
	irq_enable(INTSRC_EXINT8);
	}   */
	
 void CSL (void) 
{
	*(RP)GPIO_PORTD_DATA &= ~(0x1<<3); //cs 片选信号拉低,使能ADS 7846
	return ;
	}
	

 void CSH (void)
{
	*(RP)GPIO_PORTD_DATA |= 0x1<<3;		//cs 片选信号拉高  ，不使能ADS 7846
	return ;
}	  
/******************************************************************
*函数名：Delay10MicroSeconds
*入口参数：
*返回参数：无
*函数功能：延时
*******************************************************************/

void Delay(int n)
{
    int i;
    
    for(i=0;i<n;i++);
    
    return ;
}  
/******************************************************************
*函数名：TouchpanelInit
*入口参数：无
*返回参数：无
*函数功能：初始化touchpanel
*******************************************************************/
void TouchpanelInit(void)
{	mask_irq(INTSRC_EXINT8); 
    zpix = 4;//4 means up
    xpix = 0;
    ypix = 0; 
    
    
    
	*(RP)GPIO_PORTA_SEL |= 0x100;
	*(RP)GPIO_PORTA_DIR |= 0x100;  //	EXINT8
	*(RP)GPIO_PORTA_INCTL |= 0x100;	                 
	*(RP)GPIO_PORTA_INTRCTL |= 0x30000;                 
    *(RP)GPIO_PORTA_INTRCLR |= 0x100;  // pull up               
   	*(RP)GPIO_PORTA_INTRCLR &= 0x0;   // down
   	// 中断设置,初始化中断
    *(RP)GPIO_PORTD_SEL	|= 0x1<<3;   	//portD3设置为通用（普通io） 
    *(RP)GPIO_PORTD_DIR  &= ~(0x1<<3);	// portD3(SSI_CS1) 输出
    CSH();
    //设置CS线
    
    *(RP)SSI_SSIENR = 0x0; //设置前紧禁止能SSI
    
    *(RP)SSI_CONTROL0 = 0x07;  //SPI协议，数据帧长度为8,停止位为低电平
    *(RP)SSI_CONTROL1 = 0x00; //数据帧数量为1
//    *(RP)SSI_MWCR	  = 0x4;  
    *(RP)SSI_BAUDR  = 0x1000; //设置波特率
    *(RP)SSI_TXFTLR = 0x1;  //发送阈值为1
    *(RP)SSI_RXFTLR = 0x0;  //接受阈值为1
//    *(RP)SSI_IMR	= 0x1f; //  不屏蔽中断
    *(RP)SSI_IMR	= 0x0;  //屏蔽中断 
    *(RP)SSI_DMACR  =0x0;//未使能DMA
    *(RP)SSI_DR =0x0;
    *(RP)SSI_SER =0x0;
    
    
    
    *(RP)SSI_SSIENR = 0x1; //设置完使能ssi
	irq_enable(INTSRC_EXINT8);     
	unmask_irq(INTSRC_EXINT8);  
	return ;

} 
/******************************************************************
*函数名：SendCommand
*入口参数：ADCommand
*返回参数：data
*函数功能：向ADS7846输入命令，获返回位置参数
*******************************************************************/

  unsigned short  SendCommand  (U8 ADCommand)
{	
	U16 location;
	*(RP)SSI_SSIENR = 0x0; 
	*(RP)SSI_SER = 0x0;    
	*(RP)SSI_CONTROL0 = 0x07;//数据帧长度为8
	CSL();//使能ADS7846	
	*(RP)SSI_SSIENR = 0x1; 
	*(RP)SSI_SER = 0x2;    	
	*(RP)SSI_DR = ADCommand;
	while (!(*(RP)SSI_SR & 0x4));
	while (*(RP)SSI_SR & 0x1);	

	*(RP)SSI_SSIENR = 0x0; 
	*(RP)SSI_SER = 0x0;    
	*(RP)SSI_CONTROL0 = 0x0c;//数据帧为12
	*(RP)SSI_SSIENR = 0x1; //使能SSI
	*(RP)SSI_SER = 0x2;    //使能从设备，开始传输命令
	*(RP)SSI_DR = 0x0;
	while (!(*(RP)SSI_SR & 0x4));
	while (*(RP)SSI_SR & 0x1);
	location=*(RP)SSI_DR;
	CSH(); 
//	printf("%x \n",location);
	return location;  
}
	

static void Readxy(void)
{
    U16 xpix, ypix;
/*    U16 X_value = 0,Y_value = 0;
    U16 i,j,t,X[10],Y[10];  
 
    for(i=0; i<10; i++)
    { 
       X[i] = SendCommand(X_LOCATION_CMD);  //读取X轴上10个采样值
       Y[i] = SendCommand(Y_LOCATION_CMD);  //读取Y轴上10个采样值  
    }
    
    //对X轴10个采样值按大小进行排序
    for(i=0; i<10; i++)
    { 
      for(j=0; j<10-i; j++) 
      {
         if(X[j]>X[j+1])
         {  
	        t = X[j];
	        X[j] = X[j+1];
	        X[j+1] = t;
          }
       }  
     }
     
    //取其中的6个采样值进行平均
    for(i=2; i<8; i++) 
    {   
       X_value  += X[i];
     }
       X_value = X_value /6;      
    
    Delay(9);
    
    //计算出触点所在X轴的坐标 
    if (X_value<TPMAX && X_value>TPMIN)
    {	
       xpix = 240-240*(X_value-TPMIN)/(TPMAX-TPMIN);
    }                               
    
    //对Y轴10个采样值按大小进行排序
    for(i=0; i<10; i++)
    {
      for(j=0; j<10-i; j++) 
      {	
         if(Y[j]>Y[j+1])
	     {  
	        t = Y[j];
	        Y[j] = Y[j+1];
	        Y[j+1] = t;
	      }  
       }	  
     }
       
    //取其中的6个采样值进行平均
    for(i=2; i<8; i++) 
    {
       Y_value += Y[i];
	}
       Y_value = Y_value/6;  
    
    //计算出触点所在Y轴的坐标
    if (Y_value<TPMAX && Y_value>TPMIN)
    {
       ypix = 320*(Y_value-TPMIN)/(TPMAX-TPMIN);  
    }  */
    xpix=SendCommand(X_LOCATION_CMD);
    ypix=SendCommand(Y_LOCATION_CMD);
     	
    printf("X is %d   Y is %d\n",xpix,ypix);
    
    return;
}                      


/******************************************************************
*函数名：tp_handler
*入口参数：无
*返回参数：无
*函数功能：touch中断处理
*******************************************************************/
void Tp_handler(void)
{         
	Readxy();//读取触摸屏坐标
	*(RP)GPIO_PORTA_INTRCLR |= 0x100;                 
    *(RP)GPIO_PORTA_INTRCLR &= 0x000;                 //清除中断
	return;
}
