/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  mac.h
 *
 *	File Description: mac功能函数声明。
 *
 *    Version         Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0          2009.05.08          CH           
 *   		      	
 *************************************************************************************/
#ifndef  __MAC_H
#define  __MAC_H
 
#define	   BDNUM_RX	    1
#define    BDNUM_TX     1                           //描述符数目
#define    MAC_TXRAM    0x30020000                  //发送描述符缓冲区首地址       
#define    MAC_RXRAM    0x30020800                  //接收描述符缓冲区首地址
#define    RX_LENG      200              			//4的整数倍
#define    TX_LENG      200

void delay (U32 j);
void packet_creat(U32 len);
void phy_hard_reset( int mii_hard_rst);
void mii_reg_read(void);
void write_mii_reg(U16 address, U16 data);
void phy_selfloop_back(void);
void mac_int(void);

#endif