/*************************************************************************************
*	Copyright (c) 2005 by National ASIC System Engineering Research Center.
*	PROPRIETARY RIGHTS of ASIC are involved in the subject matter of this 
*	material.  All manufacturing, reproduction, use, and sales rights 
*	pertaining to this subject matter are governed by the license agreement.
*	The recipient of this software implicitly accepts the terms of the license.
*
*	File Name: lcdc.h
*
*	File Description:
*			The file define some macro definition used in lcdc.c file.
*		
*	Created by spone <spone_awp@yahoo.com.cn>, 2005-03-22
**************************************************************************************/
#ifndef __LCD_H
#define __LCD_H


#include "sep4020.h"


/*	definition of LCDC registers  address   */
                                                                           
#define 		VS_BASE			0x301f0000			//定义图像数据在存储器SDRAM中的起始地址



//------------------------------------------------------------------------------


#define LCDWIDTH	320          //定义可画图形的x轴边界
#define LCDHEIGHT	240          //定义刻画图形的y轴边界

//Macro for SIZE register setting
#define XMAX	((LCDWIDTH/16)	<< 20)
#define YMAX	(LCDHEIGHT)

//Macro for PCR register setting
#define TFT		   ((U32)1 << 31)
#define COLOR	   (1 << 30)
#define PBSIZE	   (0 << 28)
#define BPIX	   (4 << 25)
#define PIXPOL	   (0 << 24)
#define FLMPOL	   (1 << 23)
#define LPPOL	   (1 << 22)
#define CLKPOL	   (1 << 21)
#define OEPOL	   (0 << 20)
#define END_SEL	   (0 << 18)
#define ACD_SEL	   (0 << 15)
#define ACD		   (0 << 8 )
#define PCD		   (10)

//Macro for HCR regiter setting
#define H_WIDTH		((U32)46 << 26)
#define H_WAIT_1	(20 << 8)
#define H_WAIT_2	(20)

//Macro for VCR register setting
#define V_WIDTH				((U32)10<< 26)
#define PASS_FRAME_WAIT		(0 <<16)
#define V_WAIT_1			(3 << 8)
#define V_WAIT_2			(3)




//Macro for PWMR register setting
#define SCR		(0 << 9)
#define CC_EN	(0 << 8)
#define PW		(64)

//Macro for DMACR register setting
#define BL		((U32)1 << 31)
#define HM		(7 << 16)
#define TM		(3)



/* declaration of function */
void InitLcdc(void);
void ShowPics(const unsigned char *pic);


#endif
