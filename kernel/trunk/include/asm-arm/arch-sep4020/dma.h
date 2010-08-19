/*
 *  linux/include/asm-arm/arch-clps711x/dma.h
 *
 *  Copyright (C) 1997,1998 Russell King
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef __ASM_ARCH_DMA_H
#define __ASM_ARCH_DMA_H

//#define MAX_DMA_ADDRESS		0xffffffff

//#define MAX_DMA_CHANNELS	6
struct sep4020dma_param
{
    u32 srcadd;
    u32 destadd;

    u8 channelnum;

    //control reg
    u32 transize;
    u32  destinc;
    u32  srcinc;
    u32  destsize;
    u32  srcsize;
    u32  destburst;
    u32  srcburst;

    //configuration reg
    u32 destnum;
    u32 srcnum;
    u32 controller;
    u32 compintmask; //1:mask 0:unmask
    u32 errintmask;  //1:mask 0:unmask
    u32 transtype;
    //u32 channelenable; //1: enable, 0: disable 
};

//size
#define SIZEBYTE 0x0
#define SIZEHALFWORD 0x1
#define SIZEWORD 0x2

//burstsize
#define BURST1 0x1
#define BURST4 0x3
#define BURST8 0x5
#define BURST16 0x7

//  外设号
#define DMAMEM 0x0
#define DMAUSB 0x1
#define DMAI2S 0x4
#define DMASSI 0x5
#define DMANAND 0x6
#define DMAMMC 0x7
#define DMAEXTERN 0x8

#define DMACONTROL 0   // dma控制
#define PERIPHCONTROL 1 //外设控制

//transtype
#define MEMTOMEM 0x0
#define MENTOPERIPH 0x1
#define PERIPHTOMEM 0x2

extern int sep4020_dma_set(struct sep4020dma_param *dma);




#endif  

