/*************************************************************************************
 *	Copyright (c) 2009 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Name:  NorFlash.h
 *
 *	File Description: NorFlash����ͷ�ļ�
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

#define FALSE                   0             // ������ֵ
#define TRUE                    1             // �ɹ�����ֵ 

typedef unsigned char            BYTE;           // BYTE is 8-bit in length
typedef unsigned short int      WORD;        // WORD is 16-bit in length
typedef unsigned long int       Uint32;        // Uint32 is 32-bit in length

#ifndef NOR_TEST
	#define		ERASENUM		39                                 /* ������С����Ҫ���ݾ�����¼���ݴ�С����õ� */
	#define		SIZE1			0x10000			     /* ��һ����¼���ļ���С����λΪ��ʹ�õ�Flash�ֳ� */
	#define		SIZE2			0x98000			     /* �ڶ�����¼���ļ�(kernel code)��С����λΪ��ʹ�õ�Flash�ֳ� */
#else
	#define		ERASENUM		0x2				     /* ���Բ�����С��������Ҫ����ѡ�� */
	#define		SIZE1			0x20000			     /* �������ݵĴ�С����λΪ��ʹ�õ�Flash�ֳ� */
	#define 		SIZE2			SIZE1
#endif
	
#define		ERASESIZE		0x2000				     /* ÿ�β����Ĵ�С������flash���ϵõ�����������䶯 */	
#define		DataHead	                0x30001000		            /* �����¼���ݵ��ڴ��ַ���û��ɸı� */
#define		NorHead			0x20000000			     /* ��Ҫ��¼��Nor Flash�е�Ŀ���ַ��ȡ�����û���Ҫ��bootloader������ */
#define		kernelHead		0x201f0000			     /* ��Ҫ��¼kernel��Nor Flash�е�Ŀ���ַ��ȡ�����û���Ҫ��bootloader������ */


#define sysAddress(offset) (volatile U16*)(system_base|offset<<1)      //Norflashʡ����A0,��Ҫ����

#define MAX_TIMEOUT	0x07FFFFFF   // A ceiling constant used by Check_Toggle_,Ready() and Check_Data_Polling().The user should modify this constant accordingly.
                          
#define GE01

#ifdef GE01

	/*��NorFlash_32Bit����������Ϊ32λNorFlash������Ϊ16λNorFlash*/

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

/********************************** 32λNorFlash����******************/

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