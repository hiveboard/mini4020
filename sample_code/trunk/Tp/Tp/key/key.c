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
/*���������С���ĵ���*/
#define  TPMIN 	            400
#define  TPMAX 	            3800

U16 xpix ,ypix ,zpix ;
/******************************************************************
*��������DisableTp,EnableTp
*��ڲ�������
*���ز�������
*�������ܣ���ʼ��touchpad
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
	*(RP)GPIO_PORTD_DATA &= ~(0x1<<3); //cs Ƭѡ�ź�����,ʹ��ADS 7846
	return ;
	}
	

 void CSH (void)
{
	*(RP)GPIO_PORTD_DATA |= 0x1<<3;		//cs Ƭѡ�ź�����  ����ʹ��ADS 7846
	return ;
}	  
/******************************************************************
*��������Delay10MicroSeconds
*��ڲ�����
*���ز�������
*�������ܣ���ʱ
*******************************************************************/

void Delay(int n)
{
    int i;
    
    for(i=0;i<n;i++);
    
    return ;
}  
/******************************************************************
*��������TouchpanelInit
*��ڲ�������
*���ز�������
*�������ܣ���ʼ��touchpanel
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
   	// �ж�����,��ʼ���ж�
    *(RP)GPIO_PORTD_SEL	|= 0x1<<3;   	//portD3����Ϊͨ�ã���ͨio�� 
    *(RP)GPIO_PORTD_DIR  &= ~(0x1<<3);	// portD3(SSI_CS1) ���
    CSH();
    //����CS��
    
    *(RP)SSI_SSIENR = 0x0; //����ǰ����ֹ��SSI
    
    *(RP)SSI_CONTROL0 = 0x07;  //SPIЭ�飬����֡����Ϊ8,ֹͣλΪ�͵�ƽ
    *(RP)SSI_CONTROL1 = 0x00; //����֡����Ϊ1
//    *(RP)SSI_MWCR	  = 0x4;  
    *(RP)SSI_BAUDR  = 0x1000; //���ò�����
    *(RP)SSI_TXFTLR = 0x1;  //������ֵΪ1
    *(RP)SSI_RXFTLR = 0x0;  //������ֵΪ1
//    *(RP)SSI_IMR	= 0x1f; //  �������ж�
    *(RP)SSI_IMR	= 0x0;  //�����ж� 
    *(RP)SSI_DMACR  =0x0;//δʹ��DMA
    *(RP)SSI_DR =0x0;
    *(RP)SSI_SER =0x0;
    
    
    
    *(RP)SSI_SSIENR = 0x1; //������ʹ��ssi
	irq_enable(INTSRC_EXINT8);     
	unmask_irq(INTSRC_EXINT8);  
	return ;

} 
/******************************************************************
*��������SendCommand
*��ڲ�����ADCommand
*���ز�����data
*�������ܣ���ADS7846��������񷵻�λ�ò���
*******************************************************************/

  unsigned short  SendCommand  (U8 ADCommand)
{	
	U16 location;
	*(RP)SSI_SSIENR = 0x0; 
	*(RP)SSI_SER = 0x0;    
	*(RP)SSI_CONTROL0 = 0x07;//����֡����Ϊ8
	CSL();//ʹ��ADS7846	
	*(RP)SSI_SSIENR = 0x1; 
	*(RP)SSI_SER = 0x2;    	
	*(RP)SSI_DR = ADCommand;
	while (!(*(RP)SSI_SR & 0x4));
	while (*(RP)SSI_SR & 0x1);	

	*(RP)SSI_SSIENR = 0x0; 
	*(RP)SSI_SER = 0x0;    
	*(RP)SSI_CONTROL0 = 0x0c;//����֡Ϊ12
	*(RP)SSI_SSIENR = 0x1; //ʹ��SSI
	*(RP)SSI_SER = 0x2;    //ʹ�ܴ��豸����ʼ��������
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
       X[i] = SendCommand(X_LOCATION_CMD);  //��ȡX����10������ֵ
       Y[i] = SendCommand(Y_LOCATION_CMD);  //��ȡY����10������ֵ  
    }
    
    //��X��10������ֵ����С��������
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
     
    //ȡ���е�6������ֵ����ƽ��
    for(i=2; i<8; i++) 
    {   
       X_value  += X[i];
     }
       X_value = X_value /6;      
    
    Delay(9);
    
    //�������������X������� 
    if (X_value<TPMAX && X_value>TPMIN)
    {	
       xpix = 240-240*(X_value-TPMIN)/(TPMAX-TPMIN);
    }                               
    
    //��Y��10������ֵ����С��������
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
       
    //ȡ���е�6������ֵ����ƽ��
    for(i=2; i<8; i++) 
    {
       Y_value += Y[i];
	}
       Y_value = Y_value/6;  
    
    //�������������Y�������
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
*��������tp_handler
*��ڲ�������
*���ز�������
*�������ܣ�touch�жϴ���
*******************************************************************/
void Tp_handler(void)
{         
	Readxy();//��ȡ����������
	*(RP)GPIO_PORTA_INTRCLR |= 0x100;                 
    *(RP)GPIO_PORTA_INTRCLR &= 0x000;                 //����ж�
	return;
}
