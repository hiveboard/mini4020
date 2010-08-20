/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  lcd.c
 *
 *	File Description:   lcd的函数文件
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
*       LCDC初始化函数
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong              
*                                                                       
* INPUTS                                                                
*                                                                       
*      无
*   
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      无   
*
***************************************************************/
void InitLcdc(void)
{
     /*Turn on the light the back_led for TFT Moudle*/
    *(RP)GPIO_PORTC_SEL  |= 0X0008;      //Portc8设置为通用口
    *(RP)GPIO_PORTC_DIR  &= (~0X0008);   //Portc8设置为输出
    *(RP)GPIO_PORTC_DATA |= 0X0008;      //Portc8输出高电平
    
    memset((char*)VS_BASE,0xff,320*240*2);      //初始化数据帧的缓存区

    *(RP)LCDC_LECR = 0x00000000;     			//禁用LCDC
	
    /* config the register of lcdc */
  
	    *(RP)LCDC_SSA = VS_BASE;				//lcd数据帧的起始地址0x301f0000
    	*(RP)LCDC_SIZE = YMAX | XMAX;
		*(RP)LCDC_PCR = TFT|COLOR|PBSIZE|BPIX|PIXPOL|FLMPOL|LPPOL|CLKPOL|OEPOL|END_SEL|ACD_SEL|ACD|PCD;//面板寄存器配置
		*(RP)LCDC_HCR = H_WIDTH|H_WAIT_1|H_WAIT_2;                                                     //水平配置寄存器
		*(RP)LCDC_VCR = V_WIDTH|PASS_FRAME_WAIT|V_WAIT_1|V_WAIT_2;                                     //垂直配置寄存器
		*(RP)LCDC_PWMR = SCR|CC_EN|PW;                                                                 //对比度控制寄存器                                                                //
		*(RP)LCDC_DMACR = BL|HM|TM;                                                                    //DMA控制寄存器
    
    *(RP)LCDC_LECR = 0x00000001;    		   //使能LCDC
    *(RP)LCDC_LCDISREN = 0x00000000;		   //中断在加载帧的最后一个或第一个数据时设置，到LCD之间会有一个延时

}



/*************************************************************
* FUNCTION                                                              
*                                                                       
*       ShowPics                                             
*                                                                       
* DESCRIPTION                                                           
*                                                                       
*       图片显示处理函数
*                                                                       
* AUTHOR                                                                
*                                                                       
*       bigstrong              
*                                                                       
* INPUTS                                                                
*                                                                       
*      const unsigned char *pic    数据帧的首指针
*   
*                                                                       
* OUTPUTS                                                               
*                                                                       
*      无   
*
***************************************************************/
void ShowPics(const unsigned char *pic)          
{
  memcpy((char*)VS_BASE,pic,320*240*2);
}
