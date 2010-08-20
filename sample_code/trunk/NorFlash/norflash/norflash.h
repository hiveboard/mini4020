/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  NorFlash.h
 *
 *	File Description: NorFlash驱动头文件
 *
 *    Version           Date            Author           
 *------------------------------------------------------------------------------------    
 *    1.0               2009.04.25       PROCHIP            
 *   		      	
 **************************************************************************************/

#ifndef __NORFLASH_H
#define __NORFLASH_H
 
#include "sep4020.h"
#include <stdio.h>
#include "config.h"

#define FALSE                   0             // 出错返回值
#define TRUE                    1             // 成功返回值 

typedef unsigned char            BYTE;           // BYTE is 8-bit in length
typedef unsigned short int      WORD;        // WORD is 16-bit in length
typedef unsigned long int       Uint32;        // Uint32 is 32-bit in length

#ifndef NOR_TEST
	#define		ERASENUM		39                                 /* 擦除大小，需要根据具体烧录内容大小计算得到 */
	#define		SIZE1			0x10000			     /* 第一次烧录的文件大小，单位为所使用的Flash字长 */
	#define		SIZE2			0x98000			     /* 第二次烧录的文件(kernel code)大小，单位为所使用的Flash字长 */
#else
	#define		ERASENUM		0x2				     /* 测试擦除大小，根据需要自行选择 */
	#define		SIZE1			0x20000			     /* 测试数据的大小，单位为所使用的Flash字长 */
	#define 		SIZE2			SIZE1
#endif
	
#define		ERASESIZE		0x2000				     /* 每次擦除的大小，根据flash资料得到，不可随意变动 */	
#define		DataHead	                0x30001000		            /* 存放烧录内容的内存地址，用户可改变 */
#define		NorHead			0x20000000			     /* 需要烧录的Nor Flash中的目标地址，取决于用户需要和bootloader的内容 */
#define		kernelHead		0x201f0000			     /* 需要烧录kernel的Nor Flash中的目标地址，取决于用户需要和bootloader的内容 */


#define sysAddress(offset) (volatile U16*)(system_base|offset<<1)      //Norflash省掉了A0,需要左移

#define MAX_TIMEOUT	0x07FFFFFF   // A ceiling constant used by Check_Toggle_,Ready() and Check_Data_Polling().The user should modify this constant accordingly.
                          
#define GE01

#ifdef GE01

	/*宏NorFlash_32Bit，若定义了为32位NorFlash，否则为16位NorFlash*/

	#define NorFlash_32Bit
	
#endif

#define SECTOR_SIZE            2048        // Must be 2048 words for 39VF160X
#define BLOCK_SIZE              32768      // Must be 32K words for 39VF160X

#define SST_ID                      0x00BF     // SST Manufacturer's ID code
#define SST_39VF1601           0x234B   // SST39VF1601 device code
#define SST_39VF1602           0x234A   // SST39VF1602 device code


extern void Delay10MicroSeconds(S32 n);
extern void Delay30NanoSeconds(S32 n);


#ifdef NorFlash_32Bit

/********************************** 32位NorFlash操作******************/

S32 CheckSST39VF160X(void);
void CfiQuery(WORD*);
void SecIDQuery(WORD*, WORD*);
S32 EraseOneSector(Uint32);
S32 EraseOneBlock (Uint32);
S32 EraseEntireChip(void);
S32 ProgramOneWord (WORD*, Uint32);
S32 ProgramOneSector (WORD*, Uint32);
S32 ProgramOneBlock (WORD *Src, U32 Dst);
S32 SecIDLockStatus(void);
S32 UserSecIDWordPro (WORD*, WORD*, S32);
void UserSecIDLockOut (void);
void EraseSuspend (void);
void EraseResume (void);
S32 CheckToggleReady (Uint32);
S32 CheckDataPolling (Uint32, WORD);
S32 Check(U32 head1, U32 head2, U32 num);

#endif

#endif