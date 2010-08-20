/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  lcd.c
 *
 *	File Description:   lcd�ĺ����ļ�
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0          2009.04.25            Jack           
 *   		      	
 ***************************************************************************************/
#include <stdio.h>
#include <string.h>

#include "lcdc.h"
#include "sep4020.h"
/*************************************************************
* FUNCTION                                                              
*                                                                       
*       init_lcdc                                           
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       LCDC��ʼ������
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong              
*                                                                       
* INPUTS                                                                
*                                                                       
*      ��
*   
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      ��   
*
***************************************************************/
void InitLcdc(void)
{
     /*Turn on the light the back_led for TFT Moudle*/
    *(RP)GPIO_PORTC_SEL  |= 0X0008;      //Portc8����Ϊͨ�ÿ�
    *(RP)GPIO_PORTC_DIR  &= (~0X0008);   //Portc8����Ϊ���
    *(RP)GPIO_PORTC_DATA |= 0X0008;      //Portc8����ߵ�ƽ
    
    memset((char*)VS_BASE,0xff,320*240*2);      //��ʼ������֡�Ļ�����

    *(RP)LCDC_LECR = 0x00000000;     			//����LCDC
	
    /* config the register of lcdc */
  
	    *(RP)LCDC_SSA = VS_BASE;				//lcd����֡����ʼ��ַ0x301f0000
    	*(RP)LCDC_SIZE = YMAX | XMAX;
		*(RP)LCDC_PCR = TFT|COLOR|PBSIZE|BPIX|PIXPOL|FLMPOL|LPPOL|CLKPOL|OEPOL|END_SEL|ACD_SEL|ACD|PCD;//���Ĵ�������
		*(RP)LCDC_HCR = H_WIDTH|H_WAIT_1|H_WAIT_2;                                                     //ˮƽ���üĴ���
		*(RP)LCDC_VCR = V_WIDTH|PASS_FRAME_WAIT|V_WAIT_1|V_WAIT_2;                                     //��ֱ���üĴ���
		*(RP)LCDC_PWMR = SCR|CC_EN|PW;                                                                 //�Աȶȿ��ƼĴ���                                                                //
		*(RP)LCDC_DMACR = BL|HM|TM;                                                                    //DMA���ƼĴ���
    
    *(RP)LCDC_LECR = 0x00000001;    		   //ʹ��LCDC
    *(RP)LCDC_LCDISREN = 0x00000000;		   //�ж��ڼ���֡�����һ�����һ������ʱ���ã���LCD֮�����һ����ʱ

}



/*************************************************************
* FUNCTION                                                              
*                                                                       
*       ShowPics                                             
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       ͼƬ��ʾ������
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong              
*                                                                       
* INPUTS                                                                
*                                                                       
*      const unsigned char *pic    ����֡����ָ��
*   
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      ��   
*
***************************************************************/
void ShowPics(const unsigned char *pic)          
{
  memcpy((char*)VS_BASE,pic,320*240*2);
}
