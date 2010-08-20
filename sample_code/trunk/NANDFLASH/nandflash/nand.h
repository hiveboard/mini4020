/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  nand.h
 *
 *	File Description: nandflash 功能函数声明.
 *
 *    Version         Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0           2009.05.06        ch            
 *   		      	
 **************************************************************************************/
#ifndef _NAND_H
#define _NAND_H

#include 	"sep4020.h"

#define MEM_BLOCK_BEGIN         0x30300000
							
#define NAND_CMD_ERASE  	0x80000060
#define NAND_CMD_STATUS		0x80000070
#define NAND_CMD_READID		0x80000090
#define NAND_CMD_READ1		0x80000001
#define NAND_CMD_READECC	0x80000050             //标准nandflash命令

#ifdef TwoKPage
  #define NAND_CMD_READ0   	0xc0003000              
  #define NAND_CMD_SEQIN	0xc0001080
#else
  #define NAND_CMD_READ0  	0x80000000
  #define NAND_CMD_SEQIN	0x80000080
#endif
 
void InitEmi(void);
U32  NandWrite(U32 SrcAddr,U32 NandPage);
U32  NandRead(U32 DestAddr,U32 NandPage);
U32  NandErase(U32 NandPage);
U32  NandReadId(void);
U32  NandReadStatus(void);
void CompareMem(U32 src, U32 dest,U32 len);
void InitGpio(void);

#endif
