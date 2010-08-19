/*
 *  linux/include/asm-arm/arch-clps711x/hardware.h
 *
 *  This file contains the hardware definitions of the Prospector P720T.
 *
 *  Copyright (C) 2000 Deep Blue Solutions Ltd.
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
#ifndef __ASM_ARCH_HARDWARE_H
#define __ASM_ARCH_HARDWARE_H

#include <linux/config.h>

#define CLPS7111_VIRT_BASE	0xff000000
#define CLPS7111_BASE		CLPS7111_VIRT_BASE

/*
 * The physical addresses that the external chip select signals map to is
 * dependent on the setting of the nMEDCHG signal on EP7211 and EP7212
 * processors.  CONFIG_EP72XX_BOOT_ROM is only available if these
 * processors are in use.
 */
#ifndef CONFIG_EP72XX_ROM_BOOT
#define CS0_PHYS_BASE		(0x00000000)
#define CS1_PHYS_BASE		(0x10000000)
#define CS2_PHYS_BASE		(0x20000000)
#define CS3_PHYS_BASE		(0x30000000)
#define CS4_PHYS_BASE		(0x40000000)
#define CS5_PHYS_BASE		(0x50000000)
#define CS6_PHYS_BASE		(0x60000000)
#define CS7_PHYS_BASE		(0x70000000)
#else
#define CS0_PHYS_BASE		(0x70000000)
#define CS1_PHYS_BASE		(0x60000000)
#define CS2_PHYS_BASE		(0x50000000)
#define CS3_PHYS_BASE		(0x40000000)
#define CS4_PHYS_BASE		(0x30000000)
#define CS5_PHYS_BASE		(0x20000000)
#define CS6_PHYS_BASE		(0x10000000)
#define CS7_PHYS_BASE		(0x00000000)
#endif

#if defined (CONFIG_ARCH_EP7211)

#define EP7211_VIRT_BASE	CLPS7111_VIRT_BASE
#define EP7211_BASE		CLPS7111_VIRT_BASE
#include <asm/hardware/ep7211.h>

#elif defined (CONFIG_ARCH_EP7212)

#define EP7212_VIRT_BASE	CLPS7111_VIRT_BASE
#define EP7212_BASE		CLPS7111_VIRT_BASE
#include <asm/hardware/ep7212.h>


#endif

#define SYSPLD_VIRT_BASE	0xfe000000
#define SYSPLD_BASE		SYSPLD_VIRT_BASE

#ifndef __ASSEMBLER__

#define PCIO_BASE		IO_BASE

#endif


#if  defined (CONFIG_ARCH_AUTCPU12)

#define  CS89712_VIRT_BASE	CLPS7111_VIRT_BASE
#define  CS89712_BASE		CLPS7111_VIRT_BASE

#include <asm/hardware/clps7111.h>
#include <asm/hardware/ep7212.h>
#include <asm/hardware/cs89712.h>

#endif


#if defined (CONFIG_ARCH_CDB89712)

#include <asm/hardware/clps7111.h>
#include <asm/hardware/ep7212.h>
#include <asm/hardware/cs89712.h>

/* dynamic ioremap() areas */
#define FLASH_START      0x00000000
#define FLASH_SIZE       0x800000
#define FLASH_WIDTH      4

#define SRAM_START       0x60000000
#define SRAM_SIZE        0xc000
#define SRAM_WIDTH       4

#define BOOTROM_START    0x70000000
#define BOOTROM_SIZE     0x80
#define BOOTROM_WIDTH    4


/* static cdb89712_map_io() areas */
#define REGISTER_START   0x80000000
#define REGISTER_SIZE    0x4000
#define REGISTER_BASE    0xff000000

#define ETHER_START      0x20000000
#define ETHER_SIZE       0x1000
#define ETHER_BASE       0xfe000000

#endif


#if defined (CONFIG_ARCH_EDB7211)

/*
 * The extra 8 lines of the keyboard matrix are wired to chip select 3 (nCS3) 
 * and repeat across it. This is the mapping for it.
 *
 * In jumpered boot mode, nCS3 is mapped to 0x4000000, not 0x3000000. This 
 * was cause for much consternation and headscratching. This should probably
 * be made a compile/run time kernel option.
 */
#define EP7211_PHYS_EXTKBD		CS3_PHYS_BASE	/* physical */

#define EP7211_VIRT_EXTKBD		(0xfd000000)	/* virtual */


/*
 * The CS8900A ethernet chip has its I/O registers wired to chip select 2 
 * (nCS2). This is the mapping for it.
 *
 * In jumpered boot mode, nCS2 is mapped to 0x5000000, not 0x2000000. This 
 * was cause for much consternation and headscratching. This should probably
 * be made a compile/run time kernel option.
 */
#define EP7211_PHYS_CS8900A		CS2_PHYS_BASE	/* physical */

#define EP7211_VIRT_CS8900A		(0xfc000000)	/* virtual */


/*
 * The two flash banks are wired to chip selects 0 and 1. This is the mapping
 * for them.
 *
 * nCS0 and nCS1 are at 0x70000000 and 0x60000000, respectively, when running
 * in jumpered boot mode.
 */
#define EP7211_PHYS_FLASH1		CS0_PHYS_BASE	/* physical */
#define EP7211_PHYS_FLASH2		CS1_PHYS_BASE	/* physical */

#define EP7211_VIRT_FLASH1		(0xfa000000)	/* virtual */
#define EP7211_VIRT_FLASH2		(0xfb000000)	/* virtual */

#endif /* CONFIG_ARCH_EDB7211 */


/*
 * Relevant bits in port D, which controls power to the various parts of
 * the LCD on the EDB7211.
 */
#define EDB_PD1_LCD_DC_DC_EN	(1<<1)
#define EDB_PD2_LCDEN		(1<<2)
#define EDB_PD3_LCDBL		(1<<3)


/* Clocks */
#define SEP_SLOW_CLOCK		32768		/* slow clock */

#define MM_BASE_V 0xC0000000
#define MM_BASE 0X30000000

/*
 * copyright (c) 2007 by PROCHIP Limited.
 * PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 * material.  All manufacturing, reproduction, use, and sales rights 
 * pertaining to this subject matter are governed by the license agreement.
 * The recipient of this software implicitly accepts the terms of the license.
 *
 * File Name:  sep4020.h
 *
 * File Description: sep4020全部寄存器地址的定义.
 *
 *    Version         Date            Author           
 * ------------------------------------------------------------------------------------    
 *    0.0.1           2007.12.25      Zhang Yang            		      	
 **************************************************************************************
 *
 */

/*
 *  各模块寄存器基值
 */

/* 物理地址 */
#define   ESRAM_BASE    0x04000000
#define   INTC_BASE     0x10000000
#define   PMU_BASE      0x10001000
#define   RTC_BASE      0x10002000
#define   WD_BASE       0x10002000
#define   TIMER_BASE    0x10003000
#define   PWM_BASE      0x10004000
#define   UART0_BASE    0X10005000
#define   UART1_BASE    0X10006000
#define   UART2_BASE    0X10007000
#define   UART3_BASE    0X10008000
#define   SSI_BASE      0X10009000
#define   I2S_BASE      0x1000A000
#define   MMC_BASE      0x1000B000
#define   SMC0_BASE     0x1000C000
#define   SMC1_BASE     0x1000D000
#define   USBD_BASE     0x1000E000
#define   GPIO_BASE     0x1000F000
#define   EMI_BASE      0x11000000
#define   DMAC_BASE     0x11001000
#define   LCDC_BASE     0x11002000
#define   MAC_BASE      0x11003000
#define   AMBA_BASE     0x11005000
#define   S1RV17_BASE     0x2C000000
/* 虚拟地址 */
#define   ESRAM_BASE_V  0xE4000000
#define   INTC_BASE_V   0xE0000000
#define   PMU_BASE_V    0xE0001000
#define   RTC_BASE_V    0xE0002000
#define   WD_BASE_V     0xE0002000
#define   TIMER_BASE_V  0xE0003000
#define   PWM_BASE_V    0xE0004000
#define   UART0_BASE_V  0XE0005000
#define   UART1_BASE_V  0XE0006000
#define   UART2_BASE_V  0XE0007000
#define   UART3_BASE_V  0XE0008000
#define   SSI_BASE_V    0XE0009000
#define   I2S_BASE_V    0xE000A000
#define   MMC_BASE_V    0xE000B000
#define   SMC0_BASE_V   0xE000C000
#define   SMC1_BASE_V   0xE000D000
#define   USBD_BASE_V   0xE000E000
#define   GPIO_BASE_V   0xE000F000
#define   EMI_BASE_V    0xE1000000
#define   DMAC_BASE_V   0xE1001000
#define   LCDC_BASE_V   0xE1002000
#define   MAC_BASE_V    0xE1003000
#define   AMBA_BASE_V   0xE1005000
#define   S1RV17_BASE_V    0xFC000000

/*
 * INTC模块
 * 物理基址: 0x10000000
 * 虚拟基址: 0xE0000000
 */

/* 物理地址 */
#define INTC_IER                  (INTC_BASE+0X000)       /* IRQ中断允许寄存器 */
#define INTC_IMR                  (INTC_BASE+0X008)       /* IRQ中断屏蔽寄存器 */
#define INTC_IFR                  (INTC_BASE+0X010)       /* IRQ软件强制中断寄存器 */
#define INTC_IRSR                 (INTC_BASE+0X018)       /* IRQ未处理中断状态寄存器 */
#define INTC_ISR                  (INTC_BASE+0X020)       /* IRQ中断状态寄存器 */
#define INTC_IMSR                 (INTC_BASE+0X028)       /* IRQ屏蔽中断状态寄存器 */
#define INTC_IFSR                 (INTC_BASE+0X030)       /* IRQ中断最终状态寄存器 */
#define INTC_FIER                 (INTC_BASE+0X0C0)       /* FIQ中断允许寄存器 */
#define INTC_FIMR                 (INTC_BASE+0X0C4)       /* FIQ中断屏蔽寄存器 */
#define INTC_FIFR                 (INTC_BASE+0X0C8)       /* FIQ软件强制中断寄存器 */
#define INTC_FIRSR                (INTC_BASE+0X0CC)       /* FIQ未处理中断状态寄存器 */
#define INTC_FISR                 (INTC_BASE+0X0D0)       /* FIQ中断状态寄存器 */
#define INTC_FIFSR                (INTC_BASE+0X0D4)       /* FIQ中断最终状态寄存器 */
#define INTC_IPLR                 (INTC_BASE+0X0D8)       /* IRQ中断优先级寄存器 */
#define INTC_ICR1                 (INTC_BASE+0X0DC)       /* IRQ内部中断优先级控制寄存器1 */
#define INTC_ICR2                 (INTC_BASE+0X0E0)       /* IRQ内部中断优先级控制寄存器2 */
#define INTC_EXICR1               (INTC_BASE+0X0E4)       /* IRQ外部中断优先级控制寄存器1 */
#define INTC_EXICR2               (INTC_BASE+0X0E8)       /* IRQ外部中断优先级控制寄存器2 */

/* 虚拟地址 */
#define INTC_IER_V                (INTC_BASE_V+0X000)     /* IRQ中断允许寄存器 */
#define INTC_IMR_V                (INTC_BASE_V+0X008)     /* IRQ中断屏蔽寄存器 */
#define INTC_IFR_V                (INTC_BASE_V+0X010)     /* IRQ软件强制中断寄存器 */
#define INTC_IRSR_V               (INTC_BASE_V+0X018)     /* IRQ未处理中断状态寄存器 */
#define INTC_ISR_V                (INTC_BASE_V+0X020)     /* IRQ中断状态寄存器 */
#define INTC_IMSR_V               (INTC_BASE_V+0X028)     /* IRQ屏蔽中断状态寄存器 */
#define INTC_IFSR_V               (INTC_BASE_V+0X030)     /* IRQ中断最终状态寄存器 */
#define INTC_FIER_V               (INTC_BASE_V+0X0C0)     /* FIQ中断允许寄存器 */
#define INTC_FIMR_V               (INTC_BASE_V+0X0C4)     /* FIQ中断屏蔽寄存器 */
#define INTC_FIFR_V               (INTC_BASE_V+0X0C8)     /* FIQ软件强制中断寄存器 */
#define INTC_FIRSR_V              (INTC_BASE_V+0X0CC)     /* FIQ未处理中断状态寄存器 */
#define INTC_FISR_V               (INTC_BASE_V+0X0D0)     /* FIQ中断状态寄存器 */
#define INTC_FIFSR_V              (INTC_BASE_V+0X0D4)     /* FIQ中断最终状态寄存器 */
#define INTC_IPLR_V               (INTC_BASE_V+0X0D8)     /* IRQ中断优先级寄存器 */
#define INTC_ICR1_V               (INTC_BASE_V+0X0DC)     /* IRQ内部中断优先级控制寄存器1 */
#define INTC_ICR2_V               (INTC_BASE_V+0X0E0)     /* IRQ内部中断优先级控制寄存器2 */
#define INTC_EXICR1_V             (INTC_BASE_V+0X0E4)     /* IRQ外部中断优先级控制寄存器1 */
#define INTC_EXICR2_V             (INTC_BASE_V+0X0E8)     /* IRQ外部中断优先级控制寄存器2 */

/*
 * PMU模块
 * 物理基址: 0x10001000
 * 虚拟基址: 0xE0001000
 */

/* 物理地址 */
#define PMU_PLTR                  (PMU_BASE+0X000)        /* PLL的稳定过渡时间 */
#define PMU_PMCR                  (PMU_BASE+0X004)        /* 系统主时钟PLL的控制寄存器 */
#define PMU_PUCR                  (PMU_BASE+0X008)        /* USB时钟PLL的控制寄存器 */
#define PMU_PCSR                  (PMU_BASE+0X00C)        /* 内部模块时钟源供给的控制寄存器 */
#define PMU_PDSLOW                (PMU_BASE+0X010)        /* SLOW状态下时钟的分频因子 */
#define PMU_PMDR                  (PMU_BASE+0X014)        /* 芯片工作模式寄存器 */
#define PMU_RCTR                  (PMU_BASE+0X018)        /* Reset控制寄存器 */
#define PMU_CLRWAKUP              (PMU_BASE+0X01C)        /* WakeUp清除寄存器 */
#define PMU_PDAPB                 (PMU_BASE+0X020)        /* 保留未用(用于扩展总线时钟分频比寄存器) */
#define PMU_PLLTS                 (PMU_BASE+0X024)        /* PLL时钟外部测试输出控制寄存器 */

/* 虚拟地址 */
#define PMU_PLTR_V                (PMU_BASE_V+0X000)      /* PLL的稳定过渡时间 */
#define PMU_PMCR_V                (PMU_BASE_V+0X004)      /* 系统主时钟PLL的控制寄存器 */
#define PMU_PUCR_V                (PMU_BASE_V+0X008)      /* USB时钟PLL的控制寄存器 */
#define PMU_PCSR_V                (PMU_BASE_V+0X00C)      /* 内部模块时钟源供给的控制寄存器 */
#define PMU_PDSLOW_V              (PMU_BASE_V+0X010)      /* SLOW状态下时钟的分频因子 */
#define PMU_PMDR_V                (PMU_BASE_V+0X014)      /* 芯片工作模式寄存器 */
#define PMU_RCTR_V                (PMU_BASE_V+0X018)      /* Reset控制寄存器 */
#define PMU_CLRWAKUP_V            (PMU_BASE_V+0X01C)      /* WakeUp清除寄存器 */
#define PMU_PDAPB_V               (PMU_BASE_V+0X020)      /* 保留未用(用于扩展总线时钟分频比寄存器) */
#define PMU_PLLTS_V               (PMU_BASE_V+0X024)      /* PLL时钟外部测试输出控制寄存器 */


/*
 * RTC模块
 * 物理基址: 0x10002000
 * 虚拟基址: 0xE0002000
 */

/* 物理地址 */
#define RTC_STA_YMD               (RTC_BASE+0X000)        /* 年, 月, 日计数寄存器 */
#define RTC_STA_HMS               (RTC_BASE+0X004)        /* 小时, 分钟, 秒寄存器 */
#define RTC_ALARM_ALL             (RTC_BASE+0X008)        /* 定时月, 日, 时, 分寄存器 */
#define RTC_CTR                   (RTC_BASE+0X00C)        /* 控制寄存器 */
#define RTC_INT_EN                (RTC_BASE+0X010)        /* 中断使能寄存器 */
#define RTC_INT_STS               (RTC_BASE+0X014)        /* 中断状态寄存器 */
#define RTC_SAMP                  (RTC_BASE+0X018)        /* 采样周期寄存器 */
#define RTC_WD_CNT                (RTC_BASE+0X01C)        /* Watch-Dog计数值寄存器 */
#define RTC_WD_SEV                (RTC_BASE+0X020)        /* Watch-Dog服务寄存器 */ 
#define RTC_CONFIG_CHECK          (RTC_BASE+0X024)        /* 配置时间确认寄存器 (在配置时间之前先写0xaaaaaaaa) */
#define RTC_SOFTRESET             (RTC_BASE+0X028)        /* 软件复位控制寄存器, 4020中被去掉了 */
#define RTC_KEY0                  (RTC_BASE+0X02C)        /* 密钥寄存器0, 4020中只有这一个寄存器 */
#define RTC_KEY1                  (RTC_BASE+0X030)        /* 密钥寄存器1 */
#define RTC_KEY2                  (RTC_BASE+0X034)        /* 密钥寄存器2 */
#define RTC_KEY3                  (RTC_BASE+0X038)        /* 密钥寄存器3 */
#define RTC_KEY4                  (RTC_BASE+0X03C)        /* 密钥寄存器4 */
#define RTC_KEY5                  (RTC_BASE+0X040)        /* 密钥寄存器5 */

/* 虚拟地址 */
#define RTC_STA_YMD_V             (RTC_BASE_V+0X000)      /* 年, 月, 日计数寄存器 */
#define RTC_STA_HMS_V             (RTC_BASE_V+0X004)      /* 小时, 分钟, 秒寄存器 */
#define RTC_ALARM_ALL_V           (RTC_BASE_V+0X008)      /* 定时月, 日, 时, 分寄存器 */
#define RTC_CTR_V                 (RTC_BASE_V+0X00C)      /* 控制寄存器 */
#define RTC_INT_EN_V              (RTC_BASE_V+0X010)      /* 中断使能寄存器 */
#define RTC_INT_STS_V             (RTC_BASE_V+0X014)      /* 中断状态寄存器 */
#define RTC_SAMP_V                (RTC_BASE_V+0X018)      /* 采样周期寄存器 */
#define RTC_WD_CNT_V              (RTC_BASE_V+0X01C)      /* Watch-Dog计数值寄存器 */
#define RTC_WD_SEV_V              (RTC_BASE_V+0X020)      /* Watch-Dog服务寄存器 */ 
#define RTC_CONFIG_CHECK_V        (RTC_BASE_V+0X024)      /* 配置时间确认寄存器 (在配置时间之前先写0xaaaaaaaa) */
#define RTC_SOFTRESET_V           (RTC_BASE_V+0X028)      /* 软件复位控制寄存器, 4020中被去掉了 */
#define RTC_KEY0_V                (RTC_BASE_V+0X02C)      /* 密钥寄存器0, 4020中只有这一个寄存器 */
#define RTC_KEY1_V                (RTC_BASE_V+0X030)      /* 密钥寄存器1 */
#define RTC_KEY2_V                (RTC_BASE_V+0X034)      /* 密钥寄存器2 */
#define RTC_KEY3_V                (RTC_BASE_V+0X038)      /* 密钥寄存器3 */
#define RTC_KEY4_V                (RTC_BASE_V+0X03C)      /* 密钥寄存器4 */
#define RTC_KEY5_V                (RTC_BASE_V+0X040)      /* 密钥寄存器5 */


/*
 * TIMER模块
 * 物理基址: 0x10003000
 * 虚拟基址: 0xE0003000
 */

/* 物理地址 */
#define TIMER_T1LCR               (TIMER_BASE+0X000)      /* 通道1加载计数寄存器 */
#define TIMER_T1CCR               (TIMER_BASE+0X004)      /* 通道1当前计数值寄存器 */
#define TIMER_T1CR                (TIMER_BASE+0X008)      /* 通道1控制寄存器 */
#define TIMER_T1ISCR              (TIMER_BASE+0X00C)      /* 通道1中断状态清除寄存器 */
#define TIMER_T1IMSR              (TIMER_BASE+0X010)      /* 通道1中断屏蔽状态寄存器 */
#define TIMER_T2LCR               (TIMER_BASE+0X020)      /* 通道2加载计数寄存器 */
#define TIMER_T2CCR               (TIMER_BASE+0X024)      /* 通道2当前计数值寄存器 */
#define TIMER_T2CR                (TIMER_BASE+0X028)      /* 通道2控制寄存器 */
#define TIMER_T2ISCR              (TIMER_BASE+0X02C)      /* 通道2中断状态清除寄存器 */
#define TIMER_T2IMSR              (TIMER_BASE+0X030)      /* 通道2中断屏蔽状态寄存器 */
#define TIMER_T3LCR               (TIMER_BASE+0X040)      /* 通道3加载计数寄存器 */
#define TIMER_T3CCR               (TIMER_BASE+0X044)      /* 通道3当前计数值寄存器 */
#define TIMER_T3CR                (TIMER_BASE+0X048)      /* 通道3控制寄存器 */
#define TIMER_T3ISCR              (TIMER_BASE+0X04C)      /* 通道3中断状态清除寄存器 */
#define TIMER_T3IMSR              (TIMER_BASE+0X050)      /* 通道3中断屏蔽状态寄存器 */
#define TIMER_T3CAPR              (TIMER_BASE+0X054)      /* 通道3捕获寄存器 */
#define TIMER_T4LCR               (TIMER_BASE+0X060)      /* 通道4加载计数寄存器 */
#define TIMER_T4CCR               (TIMER_BASE+0X064)      /* 通道4当前计数值寄存器 */
#define TIMER_T4CR                (TIMER_BASE+0X068)      /* 通道4控制寄存器 */
#define TIMER_T4ISCR              (TIMER_BASE+0X06C)      /* 通道4中断状态清除寄存器 */
#define TIMER_T4IMSR              (TIMER_BASE+0X070)      /* 通道4中断屏蔽状态寄存器 */
#define TIMER_T4CAPR              (TIMER_BASE+0X074)      /* 通道4捕获寄存器 */
#define TIMER_T5LCR               (TIMER_BASE+0X080)      /* 通道5加载计数寄存器 */
#define TIMER_T5CCR               (TIMER_BASE+0X084)      /* 通道5当前计数值寄存器 */
#define TIMER_T5CR                (TIMER_BASE+0X088)      /* 通道5控制寄存器 */
#define TIMER_T5ISCR              (TIMER_BASE+0X08C)      /* 通道5中断状态清除寄存器 */
#define TIMER_T5IMSR              (TIMER_BASE+0X090)      /* 通道5中断屏蔽状态寄存器 */
#define TIMER_T5CAPR              (TIMER_BASE+0X094)      /* 通道5捕获寄存器 */
#define TIMER_T6LCR               (TIMER_BASE+0X0A0)      /* 通道6加载计数寄存器 */
#define TIMER_T6CCR               (TIMER_BASE+0X0A4)      /* 通道6当前计数值寄存器 */
#define TIMER_T6CR                (TIMER_BASE+0X0A8)      /* 通道6控制寄存器 */
#define TIMER_T6ISCR              (TIMER_BASE+0X0AC)      /* 通道6中断状态清除寄存器 */
#define TIMER_T6IMSR              (TIMER_BASE+0X0B0)      /* 通道6中断屏蔽状态寄存器 */
#define TIMER_T6CAPR              (TIMER_BASE+0X0B4)      /* 通道6捕获寄存器 */
#define TIMER_T7LCR               (TIMER_BASE+0X0C0)      /* 通道7加载计数寄存器 */
#define TIMER_T7CCR               (TIMER_BASE+0X0C4)      /* 通道7当前计数值寄存器 */
#define TIMER_T7CR                (TIMER_BASE+0X0C8)      /* 通道7控制寄存器 */
#define TIMER_T7ISCR              (TIMER_BASE+0X0CC)      /* 通道7中断状态清除寄存器 */
#define TIMER_T7IMSR              (TIMER_BASE+0X0D0)      /* 通道7中断屏蔽状态寄存器 */
#define TIMER_T8LCR               (TIMER_BASE+0X0E0)      /* 通道8加载计数寄存器 */
#define TIMER_T8CCR               (TIMER_BASE+0X0E4)      /* 通道8当前计数值寄存器 */
#define TIMER_T8CR                (TIMER_BASE+0X0E8)      /* 通道8控制寄存器 */
#define TIMER_T8ISCR              (TIMER_BASE+0X0EC)      /* 通道8中断状态清除寄存器 */
#define TIMER_T8IMSR              (TIMER_BASE+0X0F0)      /* 通道8中断屏蔽状态寄存器 */
#define TIMER_T9LCR               (TIMER_BASE+0X100)      /* 通道9加载计数寄存器 */
#define TIMER_T9CCR               (TIMER_BASE+0X104)      /* 通道9当前计数值寄存器 */
#define TIMER_T9CR                (TIMER_BASE+0X108)      /* 通道9控制寄存器 */
#define TIMER_T9ISCR              (TIMER_BASE+0X10C)      /* 通道9中断状态清除寄存器 */
#define TIMER_T9IMSR              (TIMER_BASE+0X110)      /* 通道9中断屏蔽状态寄存器 */
#define TIMER_T10LCR              (TIMER_BASE+0X120)      /* 通道10加载计数寄存器 */
#define TIMER_T10CCR              (TIMER_BASE+0X124)      /* 通道10当前计数值寄存器 */
#define TIMER_T10CR               (TIMER_BASE+0X128)      /* 通道10控制寄存器 */
#define TIMER_T10ISCR             (TIMER_BASE+0X12C)      /* 通道10中断状态清除寄存器 */
#define TIMER_T10IMSR             (TIMER_BASE+0X130)      /* 通道10中断屏蔽状态寄存器 */
#define TIMER_TIMSR               (TIMER_BASE+0X140)      /* TIMER中断屏蔽状态寄存器 */
#define TIMER_TISCR               (TIMER_BASE+0X144)      /* TIMER中断状态清除寄存器 */
#define TIMER_TISR                (TIMER_BASE+0X148)      /* TIMER中断状态寄存器 */

/* 虚拟地址 */
#define TIMER_T1LCR_V             (TIMER_BASE_V+0X000)    /* 通道1加载计数寄存器 */
#define TIMER_T1CCR_V             (TIMER_BASE_V+0X004)    /* 通道1当前计数值寄存器 */
#define TIMER_T1CR_V              (TIMER_BASE_V+0X008)    /* 通道1控制寄存器 */
#define TIMER_T1ISCR_V            (TIMER_BASE_V+0X00C)    /* 通道1中断状态清除寄存器 */
#define TIMER_T1IMSR_V            (TIMER_BASE_V+0X010)    /* 通道1中断屏蔽状态寄存器 */
#define TIMER_T2LCR_V             (TIMER_BASE_V+0X020)    /* 通道2加载计数寄存器 */
#define TIMER_T2CCR_V             (TIMER_BASE_V+0X024)    /* 通道2当前计数值寄存器 */
#define TIMER_T2CR_V              (TIMER_BASE_V+0X028)    /* 通道2控制寄存器 */
#define TIMER_T2ISCR_V            (TIMER_BASE_V+0X02C)    /* 通道2中断状态清除寄存器 */
#define TIMER_T2IMSR_V            (TIMER_BASE_V+0X030)    /* 通道2中断屏蔽状态寄存器 */
#define TIMER_T3LCR_V             (TIMER_BASE_V+0X040)    /* 通道3加载计数寄存器 */
#define TIMER_T3CCR_V             (TIMER_BASE_V+0X044)    /* 通道3当前计数值寄存器 */
#define TIMER_T3CR_V              (TIMER_BASE_V+0X048)    /* 通道3控制寄存器 */
#define TIMER_T3ISCR_V            (TIMER_BASE_V+0X04C)    /* 通道3中断状态清除寄存器 */
#define TIMER_T3IMSR_V            (TIMER_BASE_V+0X050)    /* 通道3中断屏蔽状态寄存器 */
#define TIMER_T3CAPR_V            (TIMER_BASE_V+0X054)    /* 通道3捕获寄存器 */
#define TIMER_T4LCR_V             (TIMER_BASE_V+0X060)    /* 通道4加载计数寄存器 */
#define TIMER_T4CCR_V             (TIMER_BASE_V+0X064)    /* 通道4当前计数值寄存器 */
#define TIMER_T4CR_V              (TIMER_BASE_V+0X068)    /* 通道4控制寄存器 */
#define TIMER_T4ISCR_V            (TIMER_BASE_V+0X06C)    /* 通道4中断状态清除寄存器 */
#define TIMER_T4IMSR_V            (TIMER_BASE_V+0X070)    /* 通道4中断屏蔽状态寄存器 */
#define TIMER_T4CAPR_V            (TIMER_BASE_V+0X074)    /* 通道4捕获寄存器 */
#define TIMER_T5LCR_V             (TIMER_BASE_V+0X080)    /* 通道5加载计数寄存器 */
#define TIMER_T5CCR_V             (TIMER_BASE_V+0X084)    /* 通道5当前计数值寄存器 */
#define TIMER_T5CR_V              (TIMER_BASE_V+0X088)    /* 通道5控制寄存器 */
#define TIMER_T5ISCR_V            (TIMER_BASE_V+0X08C)    /* 通道5中断状态清除寄存器 */
#define TIMER_T5IMSR_V            (TIMER_BASE_V+0X090)    /* 通道5中断屏蔽状态寄存器 */
#define TIMER_T5CAPR_V            (TIMER_BASE_V+0X094)    /* 通道5捕获寄存器 */
#define TIMER_T6LCR_V             (TIMER_BASE_V+0X0A0)    /* 通道6加载计数寄存器 */
#define TIMER_T6CCR_V             (TIMER_BASE_V+0X0A4)    /* 通道6当前计数值寄存器 */
#define TIMER_T6CR_V              (TIMER_BASE_V+0X0A8)    /* 通道6控制寄存器 */
#define TIMER_T6ISCR_V            (TIMER_BASE_V+0X0AC)    /* 通道6中断状态清除寄存器 */
#define TIMER_T6IMSR_V            (TIMER_BASE_V+0X0B0)    /* 通道6中断屏蔽状态寄存器 */
#define TIMER_T6CAPR_V            (TIMER_BASE_V+0X0B4)    /* 通道6捕获寄存器 */
#define TIMER_T7LCR_V             (TIMER_BASE_V+0X0C0)    /* 通道7加载计数寄存器 */
#define TIMER_T7CCR_V             (TIMER_BASE_V+0X0C4)    /* 通道7当前计数值寄存器 */
#define TIMER_T7CR_V              (TIMER_BASE_V+0X0C8)    /* 通道7控制寄存器 */
#define TIMER_T7ISCR_V            (TIMER_BASE_V+0X0CC)    /* 通道7中断状态清除寄存器 */
#define TIMER_T7IMSR_V            (TIMER_BASE_V+0X0D0)    /* 通道7中断屏蔽状态寄存器 */
#define TIMER_T8LCR_V             (TIMER_BASE_V+0X0E0)    /* 通道8加载计数寄存器 */
#define TIMER_T8CCR_V             (TIMER_BASE_V+0X0E4)    /* 通道8当前计数值寄存器 */
#define TIMER_T8CR_V              (TIMER_BASE_V+0X0E8)    /* 通道8控制寄存器 */
#define TIMER_T8ISCR_V            (TIMER_BASE_V+0X0EC)    /* 通道8中断状态清除寄存器 */
#define TIMER_T8IMSR_V            (TIMER_BASE_V+0X0F0)    /* 通道8中断屏蔽状态寄存器 */
#define TIMER_T9LCR_V             (TIMER_BASE_V+0X100)    /* 通道9加载计数寄存器 */
#define TIMER_T9CCR_V             (TIMER_BASE_V+0X104)    /* 通道9当前计数值寄存器 */
#define TIMER_T9CR_V              (TIMER_BASE_V+0X108)    /* 通道9控制寄存器 */
#define TIMER_T9ISCR_V            (TIMER_BASE_V+0X10C)    /* 通道9中断状态清除寄存器 */
#define TIMER_T9IMSR_V            (TIMER_BASE_V+0X110)    /* 通道9中断屏蔽状态寄存器 */
#define TIMER_T10LCR_V            (TIMER_BASE_V+0X120)    /* 通道10加载计数寄存器 */
#define TIMER_T10CCR_V            (TIMER_BASE_V+0X124)    /* 通道10当前计数值寄存器 */
#define TIMER_T10CR_V             (TIMER_BASE_V+0X128)    /* 通道10控制寄存器 */
#define TIMER_T10ISCR_V           (TIMER_BASE_V+0X12C)    /* 通道10中断状态清除寄存器 */
#define TIMER_T10IMSR_V           (TIMER_BASE_V+0X130)    /* 通道10中断屏蔽状态寄存器 */
#define TIMER_TIMSR_V             (TIMER_BASE_V+0X140)    /* TIMER中断屏蔽状态寄存器 */
#define TIMER_TISCR_V             (TIMER_BASE_V+0X144)    /* TIMER中断状态清除寄存器 */
#define TIMER_TISR_V              (TIMER_BASE_V+0X148)    /* TIMER中断状态寄存器 */


/*
 *  SD模块
 *  基址: 0x1000B000
 */
 
#define MMC_CLOCK_CONTROL 		(MMC_BASE+0x00)
#define MMC_SOFTWARE_RESET		(MMC_BASE+0X04)
#define MMC_ARGUMENT     		(MMC_BASE+0X08)
#define MMC_COMMAND       	      (MMC_BASE+0X0C)
#define MMC_BLOCK_SIZE            	(MMC_BASE+0X10)
#define MMC_BLOCK_COUNT    		(MMC_BASE+0X14)
#define MMC_TRANSFER_MODE  		(MMC_BASE+0X18)
#define MMC_RESPONSE0    		(MMC_BASE+0X1c)
#define MMC_RESPONSE1    		(MMC_BASE+0X20)
#define MMC_RESPONSE2    		(MMC_BASE+0X24)
#define MMC_RESPONSE3    		(MMC_BASE+0X28)
#define MMC_READ_TIMEOUT_CONTROL  	(MMC_BASE+0X2c)
#define MMC_INTERRUPT_STATUS      	(MMC_BASE+0X30)
#define MMC_INTERRUPT_STATUS_MASK 	(MMC_BASE+0X34)
#define MMC_READ_BUFER_ACCESS     	(MMC_BASE+0X38)
#define MMC_WRITE_BUFER_ACCESS    	(MMC_BASE+0X3c)

/*
 *  SD模块
 *  虚拟基址: 0xE000B000
 */
 
#define MMC_CLOCK_CONTROL_V  		(MMC_BASE_V+0x00)
#define MMC_SOFTWARE_RESET_V 		(MMC_BASE_V+0X04)
#define MMC_ARGUMENT_V     		(MMC_BASE_V+0X08)
#define MMC_COMMAND_V       	      (MMC_BASE_V+0X0C)
#define MMC_BLOCK_SIZE_V            (MMC_BASE_V+0X10)
#define MMC_BLOCK_COUNT_V    		(MMC_BASE_V+0X14)
#define MMC_TRANSFER_MODE_V  		(MMC_BASE_V+0X18)
#define MMC_RESPONSE0_V    		(MMC_BASE_V+0X1c)
#define MMC_RESPONSE1_V    		(MMC_BASE_V+0X20)
#define MMC_RESPONSE2_V    		(MMC_BASE_V+0X24)
#define MMC_RESPONSE3_V    		(MMC_BASE_V+0X28)
#define MMC_READ_TIMEOUT_CONTROL_V  (MMC_BASE_V+0X2c)
#define MMC_INTERRUPT_STATUS_V      (MMC_BASE_V+0X30)
#define MMC_INTERRUPT_STATUS_MASK_V (MMC_BASE_V+0X34)
#define MMC_READ_BUFER_ACCESS_V     (MMC_BASE_V+0X38)
#define MMC_WRITE_BUFER_ACCESS_V    (MMC_BASE_V+0X3c)

/*
 * PWM模块
 * 物理基址: 0x10004000
 * 虚拟基址: 0xE0004000
 */

/* 物理地址 */
#define PWM1_CTRL                 (PWM_BASE+0X000)        /* PWM1控制寄存器 */
#define PWM1_DIV                  (PWM_BASE+0X004)        /* PWM1分频寄存器 */
#define PWM1_PERIOD               (PWM_BASE+0X008)        /* PWM1周期寄存器 */
#define PWM1_DATA                 (PWM_BASE+0X00C)        /* PWM1数据寄存器 */
#define PWM1_CNT                  (PWM_BASE+0X010)        /* PWM1计数寄存器 */
#define PWM1_STATUS               (PWM_BASE+0X014)        /* PWM1状态寄存器 */
#define PWM2_CTRL                 (PWM_BASE+0X020)        /* PWM2控制寄存器 */
#define PWM2_DIV                  (PWM_BASE+0X024)        /* PWM2分频寄存器 */
#define PWM2_PERIOD               (PWM_BASE+0X028)        /* PWM2周期寄存器 */
#define PWM2_DATA                 (PWM_BASE+0X02C)        /* PWM2数据寄存器 */
#define PWM2_CNT                  (PWM_BASE+0X030)        /* PWM2计数寄存器 */
#define PWM2_STATUS               (PWM_BASE+0X034)        /* PWM2状态寄存器 */
#define PWM3_CTRL                 (PWM_BASE+0X040)        /* PWM3控制寄存器 */
#define PWM3_DIV                  (PWM_BASE+0X044)        /* PWM3分频寄存器 */
#define PWM3_PERIOD               (PWM_BASE+0X048)        /* PWM3周期寄存器 */
#define PWM3_DATA                 (PWM_BASE+0X04C)        /* PWM3数据寄存器 */
#define PWM3_CNT                  (PWM_BASE+0X050)        /* PWM3计数寄存器 */
#define PWM3_STATUS               (PWM_BASE+0X054)        /* PWM3状态寄存器 */
#define PWM4_CTRL                 (PWM_BASE+0X060)        /* PWM4控制寄存器 */
#define PWM4_DIV                  (PWM_BASE+0X064)        /* PWM4分频寄存器 */
#define PWM4_PERIOD               (PWM_BASE+0X068)        /* PWM4周期寄存器 */
#define PWM4_DATA                 (PWM_BASE+0X06C)        /* PWM4数据寄存器 */
#define PWM4_CNT                  (PWM_BASE+0X070)        /* PWM4计数寄存器 */
#define PWM4_STATUS               (PWM_BASE+0X074)        /* PWM4状态寄存器 */
#define PWM_INTMASK               (PWM_BASE+0X080)        /* PWM中断屏蔽寄存器 */
#define PWM_INT                   (PWM_BASE+0X084)        /* PWM中断寄存器 */
#define PWM_ENABLE                (PWM_BASE+0X088)        /* PWM使能寄存器 */

/* 虚拟地址 */
#define PWM1_CTRL_V               (PWM_BASE_V+0X000)      /* PWM1控制寄存器 */
#define PWM1_DIV_V                (PWM_BASE_V+0X004)      /* PWM1分频寄存器 */
#define PWM1_PERIOD_V             (PWM_BASE_V+0X008)      /* PWM1周期寄存器 */
#define PWM1_DATA_V               (PWM_BASE_V+0X00C)      /* PWM1数据寄存器 */
#define PWM1_CNT_V                (PWM_BASE_V+0X010)      /* PWM1计数寄存器 */
#define PWM1_STATUS_V             (PWM_BASE_V+0X014)      /* PWM1状态寄存器 */
#define PWM2_CTRL_V               (PWM_BASE_V+0X020)      /* PWM2控制寄存器 */
#define PWM2_DIV_V                (PWM_BASE_V+0X024)      /* PWM2分频寄存器 */
#define PWM2_PERIOD_V             (PWM_BASE_V+0X028)      /* PWM2周期寄存器 */
#define PWM2_DATA_V               (PWM_BASE_V+0X02C)      /* PWM2数据寄存器 */
#define PWM2_CNT_V                (PWM_BASE_V+0X030)      /* PWM2计数寄存器 */
#define PWM2_STATUS_V             (PWM_BASE_V+0X034)      /* PWM2状态寄存器 */
#define PWM3_CTRL_V               (PWM_BASE_V+0X040)      /* PWM3控制寄存器 */
#define PWM3_DIV_V                (PWM_BASE_V+0X044)      /* PWM3分频寄存器 */
#define PWM3_PERIOD_V             (PWM_BASE_V+0X048)      /* PWM3周期寄存器 */
#define PWM3_DATA_V               (PWM_BASE_V+0X04C)      /* PWM3数据寄存器 */
#define PWM3_CNT_V                (PWM_BASE_V+0X050)      /* PWM3计数寄存器 */
#define PWM3_STATUS_V             (PWM_BASE_V+0X054)      /* PWM3状态寄存器 */
#define PWM4_CTRL_V               (PWM_BASE_V+0X060)      /* PWM4控制寄存器 */
#define PWM4_DIV_V                (PWM_BASE_V+0X064)      /* PWM4分频寄存器 */
#define PWM4_PERIOD_V             (PWM_BASE_V+0X068)      /* PWM4周期寄存器 */
#define PWM4_DATA_V               (PWM_BASE_V+0X06C)      /* PWM4数据寄存器 */
#define PWM4_CNT_V                (PWM_BASE_V+0X070)      /* PWM4计数寄存器 */
#define PWM4_STATUS_V             (PWM_BASE_V+0X074)      /* PWM4状态寄存器 */
#define PWM_INTMASK_V             (PWM_BASE_V+0X080)      /* PWM中断屏蔽寄存器 */
#define PWM_INT_V                 (PWM_BASE_V+0X084)      /* PWM中断寄存器 */
#define PWM_ENABLE_V              (PWM_BASE_V+0X088)      /* PWM使能寄存器 */


/*
 * UART0模块
 * 物理基址: 0x10005000
 * 虚拟基址: 0xE0005000
 */

/* 物理地址 */
#define UART0_DLBL                (UART0_BASE+0X000)      /* 波特率设置低八位寄存器 */
#define UART0_RXFIFO              (UART0_BASE+0X000)      /* 接收FIFO */
#define UART0_TXFIFO              (UART0_BASE+0X000)      /* 发送FIFO */
#define UART0_DLBH                (UART0_BASE+0X004)      /* 波特率设置高八位寄存器 */
#define UART0_IER                 (UART0_BASE+0X004)      /* 中断使能寄存器 */
#define UART0_IIR                 (UART0_BASE+0X008)      /* 中断识别寄存器 */
#define UART0_FCR                 (UART0_BASE+0X008)      /* FIFO控制寄存器 */
#define UART0_LCR                 (UART0_BASE+0X00C)      /* 行控制寄存器 */
#define UART0_MCR                 (UART0_BASE+0X010)      /* Modem控制寄存器 */
#define UART0_LSR                 (UART0_BASE+0X014)      /* 行状态寄存器 */
#define UART0_MSR                 (UART0_BASE+0X018)      /* Modem状态寄存器 */

/* 虚拟地址 */
#define UART0_DLBL_V              (UART0_BASE_V+0X000)    /* 波特率设置低八位寄存器 */
#define UART0_RXFIFO_V            (UART0_BASE_V+0X000)    /* 接收FIFO */
#define UART0_TXFIFO_V            (UART0_BASE_V+0X000)    /* 发送FIFO */
#define UART0_DLBH_V              (UART0_BASE_V+0X004)    /* 波特率设置高八位寄存器 */
#define UART0_IER_V               (UART0_BASE_V+0X004)    /* 中断使能寄存器 */
#define UART0_IIR_V               (UART0_BASE_V+0X008)    /* 中断识别寄存器 */
#define UART0_FCR_V               (UART0_BASE_V+0X008)    /* FIFO控制寄存器 */
#define UART0_LCR_V               (UART0_BASE_V+0X00C)    /* 行控制寄存器 */
#define UART0_MCR_V               (UART0_BASE_V+0X010)    /* Modem控制寄存器 */
#define UART0_LSR_V               (UART0_BASE_V+0X014)    /* 行状态寄存器 */
#define UART0_MSR_V               (UART0_BASE_V+0X018)    /* Modem状态寄存器 */


/*
 * UART1模块
 * 物理基址: 0x10006000
 * 虚拟基址: 0xE0006000
 */

/* 物理地址 */
#define UART1_DLBL                (UART1_BASE+0X000)      /* 波特率设置低八位寄存器 */
#define UART1_RXFIFO              (UART1_BASE+0X000)      /* 接收FIFO */
#define UART1_TXFIFO              (UART1_BASE+0X000)      /* 发送FIFO */
#define UART1_DLBH                (UART1_BASE+0X004)      /* 波特率设置高八位寄存器 */
#define UART1_IER                 (UART1_BASE+0X004)      /* 中断使能寄存器 */
#define UART1_IIR                 (UART1_BASE+0X008)      /* 中断识别寄存器 */
#define UART1_FCR                 (UART1_BASE+0X008)      /* FIFO控制寄存器 */
#define UART1_LCR                 (UART1_BASE+0X00C)      /* 行控制寄存器 */
#define UART1_MCR                 (UART1_BASE+0X010)      /* Modem控制寄存器 */
#define UART1_LSR                 (UART1_BASE+0X014)      /* 行状态寄存器 */
#define UART1_MSR                 (UART1_BASE+0X018)      /* Modem状态寄存器 */

/* 虚拟地址 */
#define UART1_DLBL_V              (UART1_BASE_V+0X000)    /* 波特率设置低八位寄存器 */
#define UART1_RXFIFO_V            (UART1_BASE_V+0X000)    /* 接收FIFO */
#define UART1_TXFIFO_V            (UART1_BASE_V+0X000)    /* 发送FIFO */
#define UART1_DLBH_V              (UART1_BASE_V+0X004)    /* 波特率设置高八位寄存器 */
#define UART1_IER_V               (UART1_BASE_V+0X004)    /* 中断使能寄存器 */
#define UART1_IIR_V               (UART1_BASE_V+0X008)    /* 中断识别寄存器 */
#define UART1_FCR_V               (UART1_BASE_V+0X008)    /* FIFO控制寄存器 */
#define UART1_LCR_V               (UART1_BASE_V+0X00C)    /* 行控制寄存器 */
#define UART1_MCR_V               (UART1_BASE_V+0X010)    /* Modem控制寄存器 */
#define UART1_LSR_V               (UART1_BASE_V+0X014)    /* 行状态寄存器 */
#define UART1_MSR_V               (UART1_BASE_V+0X018)    /* Modem状态寄存器 */


/*
 * UART2模块
 * 物理基址: 0x10007000
 * 虚拟基址: 0xE0007000
 */

/* 物理地址 */
#define UART2_DLBL                (UART2_BASE+0X000)      /* 波特率设置低八位寄存器 */
#define UART2_RXFIFO              (UART2_BASE+0X000)      /* 接收FIFO */
#define UART2_TXFIFO              (UART2_BASE+0X000)      /* 发送FIFO */
#define UART2_DLBH                (UART2_BASE+0X004)      /* 波特率设置高八位寄存器 */
#define UART2_IER                 (UART2_BASE+0X004)      /* 中断使能寄存器 */
#define UART2_IIR                 (UART2_BASE+0X008)      /* 中断识别寄存器 */
#define UART2_FCR                 (UART2_BASE+0X008)      /* FIFO控制寄存器 */
#define UART2_LCR                 (UART2_BASE+0X00C)      /* 行控制寄存器 */
#define UART2_MCR                 (UART2_BASE+0X010)      /* Modem控制寄存器 */
#define UART2_LSR                 (UART2_BASE+0X014)      /* 行状态寄存器 */
#define UART2_MSR                 (UART2_BASE+0X018)      /* Modem状态寄存器 */

/* 虚拟地址 */
#define UART2_DLBL_V              (UART2_BASE_V+0X000)    /* 波特率设置低八位寄存器 */
#define UART2_RXFIFO_V            (UART2_BASE_V+0X000)    /* 接收FIFO */
#define UART2_TXFIFO_V            (UART2_BASE_V+0X000)    /* 发送FIFO */
#define UART2_DLBH_V              (UART2_BASE_V+0X004)    /* 波特率设置高八位寄存器 */
#define UART2_IER_V               (UART2_BASE_V+0X004)    /* 中断使能寄存器 */
#define UART2_IIR_V               (UART2_BASE_V+0X008)    /* 中断识别寄存器 */
#define UART2_FCR_V               (UART2_BASE_V+0X008)    /* FIFO控制寄存器 */
#define UART2_LCR_V               (UART2_BASE_V+0X00C)    /* 行控制寄存器 */
#define UART2_MCR_V               (UART2_BASE_V+0X010)    /* Modem控制寄存器 */
#define UART2_LSR_V               (UART2_BASE_V+0X014)    /* 行状态寄存器 */
#define UART2_MSR_V               (UART2_BASE_V+0X018)    /* Modem状态寄存器 */


/*
 * UART3模块
 * 物理基址: 0x10008000
 * 虚拟基址: 0xE0008000
 */

/* 物理地址 */
#define UART3_DLBL                (UART3_BASE+0X000)      /* 波特率设置低八位寄存器 */
#define UART3_RXFIFO              (UART3_BASE+0X000)      /* 接收FIFO */
#define UART3_TXFIFO              (UART3_BASE+0X000)      /* 发送FIFO */
#define UART3_DLBH                (UART3_BASE+0X004)      /* 波特率设置高八位寄存器 */
#define UART3_IER                 (UART3_BASE+0X004)      /* 中断使能寄存器 */
#define UART3_IIR                 (UART3_BASE+0X008)      /* 中断识别寄存器 */
#define UART3_FCR                 (UART3_BASE+0X008)      /* FIFO控制寄存器 */
#define UART3_LCR                 (UART3_BASE+0X00C)      /* 行控制寄存器 */
#define UART3_MCR                 (UART3_BASE+0X010)      /* Modem控制寄存器 */
#define UART3_LSR                 (UART3_BASE+0X014)      /* 行状态寄存器 */
#define UART3_MSR                 (UART3_BASE+0X018)      /* Modem状态寄存器 */

/* 虚拟地址 */
#define UART3_DLBL_V              (UART3_BASE_V+0X000)    /* 波特率设置低八位寄存器 */
#define UART3_RXFIFO_V            (UART3_BASE_V+0X000)    /* 接收FIFO */
#define UART3_TXFIFO_V            (UART3_BASE_V+0X000)    /* 发送FIFO */
#define UART3_DLBH_V              (UART3_BASE_V+0X004)    /* 波特率设置高八位寄存器 */
#define UART3_IER_V               (UART3_BASE_V+0X004)    /* 中断使能寄存器 */
#define UART3_IIR_V               (UART3_BASE_V+0X008)    /* 中断识别寄存器 */
#define UART3_FCR_V               (UART3_BASE_V+0X008)    /* FIFO控制寄存器 */
#define UART3_LCR_V               (UART3_BASE_V+0X00C)    /* 行控制寄存器 */
#define UART3_MCR_V               (UART3_BASE_V+0X010)    /* Modem控制寄存器 */
#define UART3_LSR_V               (UART3_BASE_V+0X014)    /* 行状态寄存器 */
#define UART3_MSR_V               (UART3_BASE_V+0X018)    /* Modem状态寄存器 */


/*
 * SSI模块
 * 物理基址: 0x10009000
 * 虚拟基址: 0xE0009000
 */

/* 物理地址 */
#define SSI_CONTROL0              (SSI_BASE+0X000)        /* 控制寄存器0 */
#define SSI_CONTROL1              (SSI_BASE+0X004)        /* 控制寄存器1 */
#define SSI_SSIENR                (SSI_BASE+0X008)        /* SSI使能寄存器 */
#define SSI_MWCR                  (SSI_BASE+0X00C)        /* Microwire控制寄存器 */
#define SSI_SER                   (SSI_BASE+0X010)        /* 从设备使能寄存器 */
#define SSI_BAUDR                 (SSI_BASE+0X014)        /* 波特率设置寄存器 */
#define SSI_TXFTLR                (SSI_BASE+0X018)        /* 发送FIFO阈值寄存器 */
#define SSI_RXFTLR                (SSI_BASE+0X01C)        /* 接收FIFO阈值寄存器 */
#define SSI_TXFLR                 (SSI_BASE+0X020)        /* 发送FIFO状态寄存器 */
#define SSI_RXFLR                 (SSI_BASE+0X024)        /* 接收FIFO状态寄存器 */
#define SSI_SR                    (SSI_BASE+0X028)        /* 状态寄存器 */
#define SSI_IMR                   (SSI_BASE+0X02C)        /* 中断屏蔽寄存器 */
#define SSI_ISR                   (SSI_BASE+0X030)        /* 中断最终状态寄存器 */
#define SSI_RISR                  (SSI_BASE+0X034)        /* 中断原始状态寄存器 */
#define SSI_TXOICR                (SSI_BASE+0X038)        /* 发送FIFO上溢中断清除寄存器 */
#define SSI_RXOICR                (SSI_BASE+0X03C)        /* 接收FIFO上溢中断清除寄存器 */
#define SSI_RXUICR                (SSI_BASE+0X040)        /* 接收FIFO下溢中断清除寄存器 */
#define SSI_ICR                   (SSI_BASE+0X02C)        /* 中断清除寄存器 */
#define SSI_DMACR                 (SSI_BASE+0X04C)        /* DMA控制寄存器 */
#define SSI_DMATDLR               (SSI_BASE+0X050)        /* DMA发送状态寄存器 */
#define SSI_DMARDLR               (SSI_BASE+0X054)        /* DMA接收状态寄存器 */
#define SSI_DR                    (SSI_BASE+0X060)        /* 数据寄存器 */

/* 虚拟地址 */
#define SSI_CONTROL0_V            (SSI_BASE_V+0X000)      /* 控制寄存器0 */
#define SSI_CONTROL1_V            (SSI_BASE_V+0X004)      /* 控制寄存器1 */
#define SSI_SSIENR_V              (SSI_BASE_V+0X008)      /* SSI使能寄存器 */
#define SSI_MWCR_V                (SSI_BASE_V+0X00C)      /* Microwire控制寄存器 */
#define SSI_SER_V                 (SSI_BASE_V+0X010)      /* 从设备使能寄存器 */
#define SSI_BAUDR_V               (SSI_BASE_V+0X014)      /* 波特率设置寄存器 */
#define SSI_TXFTLR_V              (SSI_BASE_V+0X018)      /* 发送FIFO阈值寄存器 */
#define SSI_RXFTLR_V              (SSI_BASE_V+0X01C)      /* 接收FIFO阈值寄存器 */
#define SSI_TXFLR_V               (SSI_BASE_V+0X020)      /* 发送FIFO状态寄存器 */
#define SSI_RXFLR_V               (SSI_BASE_V+0X024)      /* 接收FIFO状态寄存器 */
#define SSI_SR_V                  (SSI_BASE_V+0X028)      /* 状态寄存器 */
#define SSI_IMR_V                 (SSI_BASE_V+0X02C)      /* 中断屏蔽寄存器 */
#define SSI_ISR_V                 (SSI_BASE_V+0X030)      /* 中断最终状态寄存器 */
#define SSI_RISR_V                (SSI_BASE_V+0X034)      /* 中断原始状态寄存器 */
#define SSI_TXOICR_V              (SSI_BASE_V+0X038)      /* 发送FIFO上溢中断清除寄存器 */
#define SSI_RXOICR_V              (SSI_BASE_V+0X03C)      /* 接收FIFO上溢中断清除寄存器 */
#define SSI_RXUICR_V              (SSI_BASE_V+0X040)      /* 接收FIFO下溢中断清除寄存器 */
#define SSI_ICR_V                 (SSI_BASE_V+0X02C)      /* 中断清除寄存器 */
#define SSI_DMACR_V               (SSI_BASE_V+0X04C)      /* DMA控制寄存器 */
#define SSI_DMATDLR_V             (SSI_BASE_V+0X050)      /* DMA发送状态寄存器 */
#define SSI_DMARDLR_V             (SSI_BASE_V+0X054)      /* DMA接收状态寄存器 */
#define SSI_DR_V                  (SSI_BASE_V+0X060)      /* 数据寄存器 */


/*
 * I2S模块
 * 物理基址: 0x1000A000
 * 虚拟基址: 0xE000A000
 */

/* 物理地址 */
#define I2S_CTRL                  (I2S_BASE+0X000)        /* I2S控制寄存器 */
#define I2S_DATA                  (I2S_BASE+0X004)        /* I2S数据寄存器 */
#define I2S_INT                   (I2S_BASE+0X008)        /* I2S中断寄存器 */
#define I2S_STATUS                (I2S_BASE+0X00C)        /* I2S状态寄存器 */

/* 虚拟地址 */
#define I2S_CTRL_V                (I2S_BASE_V+0X000)      /* I2S控制寄存器 */
#define I2S_DATA_V                (I2S_BASE_V+0X004)      /* I2S数据寄存器 */
#define I2S_INT_V                 (I2S_BASE_V+0X008)      /* I2S中断寄存器 */
#define I2S_STATUS_V              (I2S_BASE_V+0X00C)      /* I2S状态寄存器 */


/*
 * GPIO模块
 * 物理基址: 0x1000B000
 * 虚拟基址: 0xE000B000
 */

/* 物理地址 */
#define GPIO_DBCLK_DIV            (GPIO_BASE+0X000)       /* 去毛刺采用时钟分频比配置寄存器 */
#define GPIO_PORTA_DIR            (GPIO_BASE+0X004)       /* A组端口输入输出方向配置寄存器 */
#define GPIO_PORTA_SEL            (GPIO_BASE+0X008)       /* A组端口通用用途选择配置寄存器 */
#define GPIO_PORTA_INCTL          (GPIO_BASE+0X00C)       /* A组端口通用用途输入时类型配置寄存器 */
#define GPIO_PORTA_INTRCTL        (GPIO_BASE+0X010)       /* A组端口中断触发类型配置寄存器 */
#define GPIO_PORTA_INTRCLR        (GPIO_BASE+0X014)       /* A组端口通用用途中断清除配置寄存器 */
#define GPIO_PORTA_DATA           (GPIO_BASE+0X018)       /* A组端口通用用途数据配置寄存器 */
#define GPIO_PORTB_DIR            (GPIO_BASE+0X01C)       /* B组端口输入输出方向配置寄存器 */
#define GPIO_PORTB_SEL            (GPIO_BASE+0X020)       /* B组端口通用用途选择配置寄存器 */
#define GPIO_PORTB_DATA           (GPIO_BASE+0X024)       /* B组端口通用用途数据配置寄存器 */
#define GPIO_PORTC_DIR            (GPIO_BASE+0X028)       /* C组端口输入输出方向配置寄存器 */
#define GPIO_PORTC_SEL            (GPIO_BASE+0X02C)       /* C组端口通用用途选择配置寄存器 */
#define GPIO_PORTC_DATA           (GPIO_BASE+0X030)       /* C组端口通用用途数据配置寄存器 */
#define GPIO_PORTD_DIR            (GPIO_BASE+0X034)       /* D组端口输入输出方向配置寄存器 */
#define GPIO_PORTD_SEL            (GPIO_BASE+0X038)       /* D组端口通用用途选择配置寄存器 */
#define GPIO_PORTD_SPECII         (GPIO_BASE+0X03C)       /* D组端口专用用途2选择配置寄存器 */
#define GPIO_PORTD_DATA           (GPIO_BASE+0X040)       /* D组端口通用用途数据配置寄存器 */
#define GPIO_PORTE_DIR            (GPIO_BASE+0X044)       /* E组端口输入输出方向配置寄存器 */
#define GPIO_PORTE_SEL            (GPIO_BASE+0X048)       /* E组端口通用用途选择配置寄存器 */
#define GPIO_PORTE_DATA           (GPIO_BASE+0X04C)       /* E组端口通用用途数据配置寄存器 */
#define GPIO_PORTF_DIR            (GPIO_BASE+0X050)       /* F组端口输入输出方向配置寄存器 */
#define GPIO_PORTF_SEL            (GPIO_BASE+0X054)       /* F组端口通用用途选择配置寄存器 */
#define GPIO_PORTF_INCTL          (GPIO_BASE+0X058)       /* F组端口通用用途输入时类型配置寄存器 */
#define GPIO_PORTF_INTRCTL        (GPIO_BASE+0X05C)       /* F组端口中断触发类型配置寄存器 */
#define GPIO_PORTF_INTRCLR        (GPIO_BASE+0X060)       /* F组端口通用用途中断清除配置寄存器 */
#define GPIO_PORTF_DATA           (GPIO_BASE+0X064)       /* F组端口通用用途数据配置寄存器 */
#define GPIO_PORTG_DIR            (GPIO_BASE+0X068)       /* G组端口输入输出方向配置寄存器 */
#define GPIO_PORTG_SEL            (GPIO_BASE+0X06C)       /* G组端口通用用途选择配置寄存器 */
#define GPIO_PORTG_DATA           (GPIO_BASE+0X070)       /* G组端口通用用途数据配置寄存器 */
#define GPIO_PORTH_DIR            (GPIO_BASE+0X07C)       /* H组端口输入输出方向配置寄存器 */
#define GPIO_PORTH_SEL            (GPIO_BASE+0X078)       /* H组端口通用用途选择配置寄存器 */
#define GPIO_PORTH_DATA           (GPIO_BASE+0X07C)       /* H组端口通用用途数据配置寄存器 */
#define GPIO_PORTI_DIR            (GPIO_BASE+0X080)       /* I组端口输入输出方向配置寄存器 */
#define GPIO_PORTI_SEL            (GPIO_BASE+0X084)       /* I组端口通用用途选择配置寄存器 */
#define GPIO_PORTI_DATA           (GPIO_BASE+0X088)       /* I组端口通用用途数据配置寄存器 */

/* 虚拟地址 */
#define GPIO_DBCLK_DIV_V          (GPIO_BASE_V+0X000)     /* 去毛刺采用时钟分频比配置寄存器 */
#define GPIO_PORTA_DIR_V          (GPIO_BASE_V+0X004)     /* A组端口输入输出方向配置寄存器 */
#define GPIO_PORTA_SEL_V          (GPIO_BASE_V+0X008)     /* A组端口通用用途选择配置寄存器 */
#define GPIO_PORTA_INCTL_V        (GPIO_BASE_V+0X00C)     /* A组端口通用用途输入时类型配置寄存器 */
#define GPIO_PORTA_INTRCTL_V      (GPIO_BASE_V+0X010)     /* A组端口中断触发类型配置寄存器 */
#define GPIO_PORTA_INTRCLR_V      (GPIO_BASE_V+0X014)     /* A组端口通用用途中断清除配置寄存器 */
#define GPIO_PORTA_DATA_V         (GPIO_BASE_V+0X018)     /* A组端口通用用途数据配置寄存器 */
#define GPIO_PORTB_DIR_V          (GPIO_BASE_V+0X01C)     /* B组端口输入输出方向配置寄存器 */
#define GPIO_PORTB_SEL_V          (GPIO_BASE_V+0X020)     /* B组端口通用用途选择配置寄存器 */
#define GPIO_PORTB_DATA_V         (GPIO_BASE_V+0X024)     /* B组端口通用用途数据配置寄存器 */
#define GPIO_PORTC_DIR_V          (GPIO_BASE_V+0X028)     /* C组端口输入输出方向配置寄存器 */
#define GPIO_PORTC_SEL_V          (GPIO_BASE_V+0X02C)     /* C组端口通用用途选择配置寄存器 */
#define GPIO_PORTC_DATA_V         (GPIO_BASE_V+0X030)     /* C组端口通用用途数据配置寄存器 */
#define GPIO_PORTD_DIR_V          (GPIO_BASE_V+0X034)     /* D组端口输入输出方向配置寄存器 */
#define GPIO_PORTD_SEL_V          (GPIO_BASE_V+0X038)     /* D组端口通用用途选择配置寄存器 */
#define GPIO_PORTD_SPECII_V       (GPIO_BASE_V+0X03C)     /* D组端口专用用途2选择配置寄存器 */
#define GPIO_PORTD_DATA_V         (GPIO_BASE_V+0X040)     /* D组端口通用用途数据配置寄存器 */
#define GPIO_PORTE_DIR_V          (GPIO_BASE_V+0X044)     /* E组端口输入输出方向配置寄存器 */
#define GPIO_PORTE_SEL_V          (GPIO_BASE_V+0X048)     /* E组端口通用用途选择配置寄存器 */
#define GPIO_PORTE_DATA_V         (GPIO_BASE_V+0X04C)     /* E组端口通用用途数据配置寄存器 */
#define GPIO_PORTF_DIR_V          (GPIO_BASE_V+0X050)     /* F组端口输入输出方向配置寄存器 */
#define GPIO_PORTF_SEL_V          (GPIO_BASE_V+0X054)     /* F组端口通用用途选择配置寄存器 */
#define GPIO_PORTF_INCTL_V        (GPIO_BASE_V+0X058)     /* F组端口通用用途输入时类型配置寄存器 */
#define GPIO_PORTF_INTRCTL_V      (GPIO_BASE_V+0X05C)     /* F组端口中断触发类型配置寄存器 */
#define GPIO_PORTF_INTRCLR_V      (GPIO_BASE_V+0X060)     /* F组端口通用用途中断清除配置寄存器 */
#define GPIO_PORTF_DATA_V         (GPIO_BASE_V+0X064)     /* F组端口通用用途数据配置寄存器 */
#define GPIO_PORTG_DIR_V          (GPIO_BASE_V+0X068)     /* G组端口输入输出方向配置寄存器 */
#define GPIO_PORTG_SEL_V          (GPIO_BASE_V+0X06C)     /* G组端口通用用途选择配置寄存器 */
#define GPIO_PORTG_DATA_V         (GPIO_BASE_V+0X070)     /* G组端口通用用途数据配置寄存器 */
#define GPIO_PORTH_DIR_V          (GPIO_BASE_V+0X07C)     /* H组端口输入输出方向配置寄存器 */
#define GPIO_PORTH_SEL_V          (GPIO_BASE_V+0X078)     /* H组端口通用用途选择配置寄存器 */
#define GPIO_PORTH_DATA_V         (GPIO_BASE_V+0X07C)     /* H组端口通用用途数据配置寄存器 */
#define GPIO_PORTI_DIR_V          (GPIO_BASE_V+0X080)     /* I组端口输入输出方向配置寄存器 */
#define GPIO_PORTI_SEL_V          (GPIO_BASE_V+0X084)     /* I组端口通用用途选择配置寄存器 */
#define GPIO_PORTI_DATA_V         (GPIO_BASE_V+0X088)     /* I组端口通用用途数据配置寄存器 */


/*
 * SMC0模块
 * 物理基址: 0x1000C000
 * 虚拟基址: 0xE000C000
 */

/* 物理地址 */
#define SMC0_CTRL                 (SMC0_BASE+0X000)       /* SMC0控制寄存器 */
#define SMC0_FD                   (SMC0_BASE+0X004)       /* SMC0基本单元时间寄存器 */
#define SMC0_CT                   (SMC0_BASE+0X008)       /* SMC0字符传输时间寄存器 */
#define SMC0_BT                   (SMC0_BASE+0X00C)       /* SMC0块传输时间寄存器 */
#define SMC0_TX                   (SMC0_BASE+0X010)       /* 发送fifo */
#define SMC0_RX			  (SMC0_BASE+0X14)        /* 接受fifo */
#define SMC0_INT_MASK             (SMC0_BASE+0X1C)	  /* 中断屏蔽 */
#define SMC0_STATUS		  (SMC0_BASE+0X20)        /* 状态寄存器*/
#define SMC0_TX_TRIG              (SMC0_BASE+0X24)        /* 传输tx_fifo中的字节数*/ 

	
/* 虚拟地址 */
#define SMC0_CTRL_V                 (SMC0_BASE_V+0X000)       /* SMC0控制寄存器 */
#define SMC0_FD_V                   (SMC0_BASE_V+0X004)       /* SMC0基本单元时间寄存器 */
#define SMC0_CT_V                   (SMC0_BASE_V+0X008)       /* SMC0字符传输时间寄存器 */
#define SMC0_BT_V                   (SMC0_BASE_V+0X00C)       /* SMC0块传输时间寄存器 */
#define SMC0_TX_V                   (SMC0_BASE_V+0X010)       /* 发送fifo */
#define SMC0_RX_V        	    (SMC0_BASE_V+0X14)        /* 接受fifo */
#define SMC0_INT_MASK_V             (SMC0_BASE_V+0X1C)	      /* 中断屏蔽 */
#define SMC0_STATUS_V		    (SMC0_BASE_V+0X20)        /* 状态寄存器*/
#define SMC0_TX_TRIG_V              (SMC0_BASE_V+0X24)        /* 传输tx_fifo中的字节数*/ 



/*
 * SMC1模块
 * 物理基址: 0x1000D000
 * 虚拟基址: 0xE000D000
 */

/* 物理地址 */
#define SMC1_CTRL                 (SMC1_BASE+0X000)       /* SMC1控制寄存器 */
#define SMC1_FD                   (SMC1_BASE+0X004)       /* SMC1基本单元时间寄存器 */
#define SMC1_CT                   (SMC1_BASE+0X008)       /* SMC1字符传输时间寄存器 */
#define SMC1_BT                   (SMC1_BASE+0X00C)       /* SMC1块传输时间寄存器 */
#define SMC1_TX                   (SMC1_BASE+0X010)       /* 发送fifo */
#define SMC1_RX			  (SMC1_BASE+0X14)        /* 接受fifo */
#define SMC1_INT_MASK             (SMC1_BASE+0X1C)	  /* 中断屏蔽 */
#define SMC1_STATUS		  (SMC1_BASE+0X20)        /* 状态寄存器*/
#define SMC1_TX_TRIG              (SMC1_BASE+0X24)        /* 传输tx_fifo中的字节数*/ 

/* 虚拟地址 */
#define SMC1_CTRL_V                 (SMC1_BASE_V+0X000)       /* SMC1控制寄存器 */
#define SMC1_FD_V                   (SMC1_BASE_V+0X004)       /* SMC1基本单元时间寄存器 */
#define SMC1_CT_V                   (SMC1_BASE_V+0X008)       /* SMC1字符传输时间寄存器 */
#define SMC1_BT_V                   (SMC1_BASE_V+0X00C)       /* SMC1块传输时间寄存器 */
#define SMC1_TX_V                   (SMC1_BASE_V+0X010)       /* 发送fifo */
#define SMC1_RX_V		    (SMC1_BASE_V+0X14)        /* 接受fifo */
#define SMC1_INT_MASK_V             (SMC1_BASE_V+0X1C)	      /* 中断屏蔽 */
#define SMC1_STATUS_V		    (SMC1_BASE_V+0X20)        /* 状态寄存器*/
#define SMC1_TX_TRIG_V              (SMC1_BASE_V+0X24)        /* 传输tx_fifo中的字节数*/ 



/*
 * USBD模块
 * 物理基址: 0x1000E000
 * 虚拟基址: 0xE000E000
 */

/* 物理地址 */
#define USBD_PROTOCOLINTR         (USBD_BASE+0X000)       /* USB协议中断寄存器 */
#define USBD_INTRMASK             (USBD_BASE+0X004)       /* USB中断屏蔽寄存器 */
#define USBD_INTRCTRL             (USBD_BASE+0X008)       /* USB中断类型控制寄存器 */
#define USBD_EPINFO               (USBD_BASE+0X00C)       /* USB活动端点状态寄存器 */
#define USBD_BCONFIGURATIONVALUE  (USBD_BASE+0X010)       /* SET_CCONFIGURATION记录 */
#define USBD_BMATTRIBUTES         (USBD_BASE+0X014)       /* 当前配置属性寄存器 */
#define USBD_DEVSPEED             (USBD_BASE+0X018)       /* 当前设备工作速度寄存器 */
#define USBD_FRAMENUMBER          (USBD_BASE+0X01C)       /* 记录当前SOF包内的帧号 */
#define USBD_EPTRANSACTIONS0      (USBD_BASE+0X020)       /* 记录下次要求的传输次数 */
#define USBD_EPTRANSACTIONS1      (USBD_BASE+0X024)       /* 记录下次要求的传输次数 */
#define USBD_APPIFUPDATE          (USBD_BASE+0X028)       /* 接口号快速更新寄存器 */
#define USBD_CFGINTERFACE0        (USBD_BASE+0X02C)       /* 记录接口的值 */
#define USBD_CFGINTERFACE1        (USBD_BASE+0X030)       /* 记录接口的值 */
#define USBD_CFGINTERFACE2        (USBD_BASE+0X034)       /* 记录接口的值 */
#define USBD_CFGINTERFACE3        (USBD_BASE+0X038)       /* 记录接口的值 */
#define USBD_CFGINTERFACE4        (USBD_BASE+0X03C)       /* 记录接口的值 */
#define USBD_CFGINTERFACE5        (USBD_BASE+0X040)       /* 记录接口的值 */
#define USBD_CFGINTERFACE6        (USBD_BASE+0X044)       /* 记录接口的值 */
#define USBD_CFGINTERFACE7        (USBD_BASE+0X048)       /* 记录接口的值 */
#define USBD_CFGINTERFACE8        (USBD_BASE+0X04C)       /* 记录接口的值 */
#define USBD_CFGINTERFACE9        (USBD_BASE+0X050)       /* 记录接口的值 */
#define USBD_CFGINTERFACE10       (USBD_BASE+0X054)       /* 记录接口的值 */
#define USBD_CFGINTERFACE11       (USBD_BASE+0X058)       /* 记录接口的值 */
#define USBD_CFGINTERFACE12       (USBD_BASE+0X05C)       /* 记录接口的值 */
#define USBD_CFGINTERFACE13       (USBD_BASE+0X060)       /* 记录接口的值 */
#define USBD_CFGINTERFACE14       (USBD_BASE+0X064)       /* 记录接口的值 */
#define USBD_CFGINTERFACE15       (USBD_BASE+0X068)       /* 记录接口的值 */
#define USBD_CFGINTERFACE16       (USBD_BASE+0X06C)       /* 记录接口的值 */
#define USBD_CFGINTERFACE17       (USBD_BASE+0X070)       /* 记录接口的值 */
#define USBD_CFGINTERFACE18       (USBD_BASE+0X074)       /* 记录接口的值 */
#define USBD_CFGINTERFACE19       (USBD_BASE+0X078)       /* 记录接口的值 */
#define USBD_CFGINTERFACE20       (USBD_BASE+0X07C)       /* 记录接口的值 */
#define USBD_CFGINTERFACE21       (USBD_BASE+0X080)       /* 记录接口的值 */
#define USBD_CFGINTERFACE22       (USBD_BASE+0X084)       /* 记录接口的值 */
#define USBD_CFGINTERFACE23       (USBD_BASE+0X088)       /* 记录接口的值 */
#define USBD_CFGINTERFACE24       (USBD_BASE+0X08C)       /* 记录接口的值 */
#define USBD_CFGINTERFACE25       (USBD_BASE+0X090)       /* 记录接口的值 */
#define USBD_CFGINTERFACE26       (USBD_BASE+0X094)       /* 记录接口的值 */
#define USBD_CFGINTERFACE27       (USBD_BASE+0X098)       /* 记录接口的值 */
#define USBD_CFGINTERFACE28       (USBD_BASE+0X09C)       /* 记录接口的值 */
#define USBD_CFGINTERFACE29       (USBD_BASE+0X0A0)       /* 记录接口的值 */
#define USBD_CFGINTERFACE30       (USBD_BASE+0X0A4)       /* 记录接口的值 */
#define USBD_CFGINTERFACE31       (USBD_BASE+0X0A8)       /* 记录接口的值 */
#define USBD_PKTPASSEDCTRL        (USBD_BASE+0X0AC)       /* 记录成功接收的包数 */
#define USBD_PKTDROPPEDCTRL       (USBD_BASE+0X0B0)       /* 记录丢失的包数 */
#define USBD_CRCERRCTRL           (USBD_BASE+0X0B4)       /* 记录CRC错误的包数 */
#define USBD_BITSTUFFERRCTRL      (USBD_BASE+0X0B8)       /* 记录位填充错误的包数 */
#define USBD_PIDERRCTRL           (USBD_BASE+0X0BC)       /* 记录PID错误的包数 */
#define USBD_FRAMINGERRCTL        (USBD_BASE+0X0C0)       /* 记录有SYNC和EOP的包数 */
#define USBD_TXPKTCTRL            (USBD_BASE+0X0C4)       /* 记录发送包的数量 */
#define USBD_STATCTRLOV           (USBD_BASE+0X0C8)       /* 记录统计寄存器溢出情况 */
#define USBD_TXLENGTH             (USBD_BASE+0X0CC)       /* 记录每次IN传输事务包长度 */
#define USBD_RXLENGTH             (USBD_BASE+0X0D0)       /* 记录OUT传输事务包长度 */
#define USBD_RESUME               (USBD_BASE+0X0D4)       /* USB唤醒寄存器 */
#define USBD_READFLAG             (USBD_BASE+0X0D8)       /* 读异步状态寄存器标志 */
#define USBD_RECEIVETYPE          (USBD_BASE+0X0DC)       /* 传输状态寄存器 */
#define USBD_APPLOCK              (USBD_BASE+0X0E0)       /* 锁信号寄存器 */
#define USBD_EP0OUTADDR           (USBD_BASE+0X100)       /* 端点0端点号和方向 */
#define USBD_EP0OUTBMATTR         (USBD_BASE+0X104)       /* 端点0类型寄存器 */
#define USBD_EP0OUTMAXPKTSIZE     (USBD_BASE+0X108)       /* 端点0最大包尺寸寄存器 */
#define USBD_EP0OUTIFNUM          (USBD_BASE+0X10C)       /* 端点0接口号寄存器 */
#define USBD_EP0OUTSTAT           (USBD_BASE+0X110)       /* 端点0状态寄存器 */
#define USBD_EP0OUTBMREQTYPE      (USBD_BASE+0X114)       /* 端点0 SETUP事务请求类 */
#define USBD_EP0OUTBREQUEST       (USBD_BASE+0X118)       /* 端点0 SETUP事务请求内容 */
#define USBD_EP0OUTWVALUE         (USBD_BASE+0X11C)       /* 端点0 SETUP事务请求值 */
#define USBD_EP0OUTWINDEX         (USBD_BASE+0X120)       /* 端点0 SETUP事务请求索引 */
#define USBD_EP0OUTWLENGTH        (USBD_BASE+0X124)       /* 端点0 SETUP事务请求长度 */
#define USBD_EP0OUTSYNCHFRAME     (USBD_BASE+0X128)       /* 端点0同步包帧号 */
#define USBD_EP1OUTADDR           (USBD_BASE+0X12C)       /* 端点1输出端点号和方向 */
#define USBD_EP1OUTBMATTR         (USBD_BASE+0X130)       /* 端点1输出类型寄存器 */
#define USBD_EP1OUTMAXPKTSIZE     (USBD_BASE+0X134)       /* 端点1输出最大包尺寸寄存器 */
#define USBD_EP1OUTIFNUM          (USBD_BASE+0X138)       /* 端点1输出接口号寄存器 */
#define USBD_EP1OUTSTAT           (USBD_BASE+0X13C)       /* 端点1输出状态寄存器 */
#define USBD_EP1OUTBMREQTYPE      (USBD_BASE+0X140)       /* 端点1输出SETUP事务请求类型 */
#define USBD_EP1OUTBREQUEST       (USBD_BASE+0X144)       /* 端点1输出SETUP事务请求内容 */
#define USBD_EP1OUTWVALUE         (USBD_BASE+0X148)       /* 端点1输出SETUP事务请求值 */
#define USBD_EP1OUTWINDX          (USBD_BASE+0X14C)       /* 端点1输出SETUP事务请求索引 */
#define USBD_EP1OUTWLENGH         (USBD_BASE+0X150)       /* 端点1输出SETUP事务请求域长度 */
#define USBD_EP1OUTSYNCHFRAME     (USBD_BASE+0X154)       /* 端点1输出同步包帧号 */
#define USBD_EP1INADDR            (USBD_BASE+0X158)       /* 端点1输入端点号和方向 */
#define USBD_EP1INBMATTR          (USBD_BASE+0X15C)       /* 端点1输入类型寄存器 */
#define USBD_EP1INMAXPKTSIZE      (USBD_BASE+0X160)       /* 端点1输入最大包尺寸寄存器 */
#define USBD_EP1INIFNUM           (USBD_BASE+0X164)       /* 端点1输入接口号寄存器 */
#define USBD_EP1INSTAT            (USBD_BASE+0X168)       /* 端点1输入状态寄存器 */
#define USBD_EP1INBMREQTYPE       (USBD_BASE+0X16C)       /* 端点1输入SETUP事务请求类型 */
#define USBD_EP1INBREQUEST        (USBD_BASE+0X170)       /* 端点1输入SETUP事务请求内容 */
#define USBD_EP1INWVALUE          (USBD_BASE+0X174)       /* 端点1输入SETUP事务请求值 */
#define USBD_EP1INWINDEX          (USBD_BASE+0X178)       /* 端点1输入SETUP事务请求索引 */
#define USBD_EP1INWLENGTH         (USBD_BASE+0X17C)       /* 端点1输入SETUP事务请求域长度 */
#define USBD_EP1INSYNCHFRAME      (USBD_BASE+0X180)       /* 端点1输入同步包帧号 */
#define USBD_EP2OUTADDR           (USBD_BASE+0X184)       /* 端点2输出端点号和方向 */
#define USBD_EP2OUTBMATTR         (USBD_BASE+0X188)       /* 端点2输出类型寄存器 */
#define USBD_EP2OUTMAXPKTSIZE     (USBD_BASE+0X18C)       /* 端点2输出最大包尺寸寄存器 */
#define USBD_EP2OUTIFNUM          (USBD_BASE+0X190)       /* 端点2输出接口号寄存器 */
#define USBD_EP2OUTSTAT           (USBD_BASE+0X194)       /* 端点2输出状态寄存器 */
#define USBD_EP2OUTBMREQTYPE      (USBD_BASE+0X198)       /* 端点2输出SETUP事务请求类型 */
#define USBD_EP2OUTBREQUEST       (USBD_BASE+0X19C)       /* 端点2输出SETUP事务请求内容 */
#define USBD_EP2OUTWVALUE         (USBD_BASE+0X1A0)       /* 端点2输出SETUP事务请求值 */
#define USBD_EP2OUTWINDEX         (USBD_BASE+0X1A4)       /* 端点2输出SETUP事务请求索引 */
#define USBD_EP2OUTWLENGTH        (USBD_BASE+0X1A8)       /* 端点2输出SETUP事务请求域长度 */
#define USBD_EP2OUTSYNCHFRAME     (USBD_BASE+0X1AC)       /* 端点2输出同步包帧号 */
#define USBD_EP2INADDR            (USBD_BASE+0X1B0)       /* 端点2输入端点号和方向 */
#define USBD_EP2INBMATTR          (USBD_BASE+0X1B4)       /* 端点2输入类型寄存器 */
#define USBD_EP2INMAXPKTSIZE      (USBD_BASE+0X1B8)       /* 端点2输入最大包尺寸寄存器 */
#define USBD_EP2INIFNUM           (USBD_BASE+0X1BC)       /* 端点2输入接口号寄存器 */
#define USBD_EP2INSTAT            (USBD_BASE+0X1C0)       /* 端点2输入状态寄存器 */
#define USBD_EP2INBMREQTYPE       (USBD_BASE+0X1C4)       /* 端点2输入SETUP事务请求类型 */
#define USBD_EP2INBREQUEST        (USBD_BASE+0X1C8)       /* 端点2输入SETUP事务请求内容 */
#define USBD_EP2INWVALUE          (USBD_BASE+0X1CC)       /* 端点2输入SETUP事务请求值 */
#define USBD_EP2INWINDEX          (USBD_BASE+0X1D0)       /* 端点2输入SETUP事务请求索引 */
#define USBD_EP2INWLENGTH         (USBD_BASE+0X1D4)       /* 端点2输入SETUP事务请求域长度 */
#define USBD_EP2INSYNCHFRAME      (USBD_BASE+0X1D8)       /* 端点2输入同步包帧号 */
#define USBD_RXFIFO               (USBD_BASE+0X200)       /* 接受FIFO */
#define USBD_TXFIFO               (USBD_BASE+0X300)       /* 发送FIFO */

/* 虚拟地址 */
#define USBD_PROTOCOLINTR_V         (USBD_BASE_V+0X000)       /* USB协议中断寄存器 */
#define USBD_INTRMASK_V             (USBD_BASE_V+0X004)       /* USB中断屏蔽寄存器 */
#define USBD_INTRCTRL_V             (USBD_BASE_V+0X008)       /* USB中断类型控制寄存器 */
#define USBD_EPINFO_V               (USBD_BASE_V+0X00C)       /* USB活动端点状态寄存器 */
#define USBD_BCONFIGURATIONVALUE_V  (USBD_BASE_V+0X010)       /* SET_CCONFIGURATION记录 */
#define USBD_BMATTRIBUTES_V         (USBD_BASE_V+0X014)       /* 当前配置属性寄存器 */
#define USBD_DEVSPEED_V             (USBD_BASE_V+0X018)       /* 当前设备工作速度寄存器 */
#define USBD_FRAMENUMBER_V          (USBD_BASE_V+0X01C)       /* 记录当前SOF包内的帧号 */
#define USBD_EPTRANSACTIONS0_V      (USBD_BASE_V+0X020)       /* 记录下次要求的传输次数 */
#define USBD_EPTRANSACTIONS1_V      (USBD_BASE_V+0X024)       /* 记录下次要求的传输次数 */
#define USBD_APPIFUPDATE_V          (USBD_BASE_V+0X028)       /* 接口号快速更新寄存器 */
#define USBD_CFGINTERFACE0_V        (USBD_BASE_V+0X02C)       /* 记录接口的值 */
#define USBD_CFGINTERFACE1_V        (USBD_BASE_V+0X030)       /* 记录接口的值 */
#define USBD_CFGINTERFACE2_V        (USBD_BASE_V+0X034)       /* 记录接口的值 */
#define USBD_CFGINTERFACE3_V        (USBD_BASE_V+0X038)       /* 记录接口的值 */
#define USBD_CFGINTERFACE4_V        (USBD_BASE_V+0X03C)       /* 记录接口的值 */
#define USBD_CFGINTERFACE5_V        (USBD_BASE_V+0X040)       /* 记录接口的值 */
#define USBD_CFGINTERFACE6_V        (USBD_BASE_V+0X044)       /* 记录接口的值 */
#define USBD_CFGINTERFACE7_V        (USBD_BASE_V+0X048)       /* 记录接口的值 */
#define USBD_CFGINTERFACE8_V        (USBD_BASE_V+0X04C)       /* 记录接口的值 */
#define USBD_CFGINTERFACE9_V        (USBD_BASE_V+0X050)       /* 记录接口的值 */
#define USBD_CFGINTERFACE10_V       (USBD_BASE_V+0X054)       /* 记录接口的值 */
#define USBD_CFGINTERFACE11_V       (USBD_BASE_V+0X058)       /* 记录接口的值 */
#define USBD_CFGINTERFACE12_V       (USBD_BASE_V+0X05C)       /* 记录接口的值 */
#define USBD_CFGINTERFACE13_V       (USBD_BASE_V+0X060)       /* 记录接口的值 */
#define USBD_CFGINTERFACE14_V       (USBD_BASE_V+0X064)       /* 记录接口的值 */
#define USBD_CFGINTERFACE15_V       (USBD_BASE_V+0X068)       /* 记录接口的值 */
#define USBD_CFGINTERFACE16_V       (USBD_BASE_V+0X06C)       /* 记录接口的值 */
#define USBD_CFGINTERFACE17_V       (USBD_BASE_V+0X070)       /* 记录接口的值 */
#define USBD_CFGINTERFACE18_V       (USBD_BASE_V+0X074)       /* 记录接口的值 */
#define USBD_CFGINTERFACE19_V       (USBD_BASE_V+0X078)       /* 记录接口的值 */
#define USBD_CFGINTERFACE20_V       (USBD_BASE_V+0X07C)       /* 记录接口的值 */
#define USBD_CFGINTERFACE21_V       (USBD_BASE_V+0X080)       /* 记录接口的值 */
#define USBD_CFGINTERFACE22_V       (USBD_BASE_V+0X084)       /* 记录接口的值 */
#define USBD_CFGINTERFACE23_V       (USBD_BASE_V+0X088)       /* 记录接口的值 */
#define USBD_CFGINTERFACE24_V       (USBD_BASE_V+0X08C)       /* 记录接口的值 */
#define USBD_CFGINTERFACE25_V       (USBD_BASE_V+0X090)       /* 记录接口的值 */
#define USBD_CFGINTERFACE26_V       (USBD_BASE_V+0X094)       /* 记录接口的值 */
#define USBD_CFGINTERFACE27_V       (USBD_BASE_V+0X098)       /* 记录接口的值 */
#define USBD_CFGINTERFACE28_V       (USBD_BASE_V+0X09C)       /* 记录接口的值 */
#define USBD_CFGINTERFACE29_V       (USBD_BASE_V+0X0A0)       /* 记录接口的值 */
#define USBD_CFGINTERFACE30_V       (USBD_BASE_V+0X0A4)       /* 记录接口的值 */
#define USBD_CFGINTERFACE31_V       (USBD_BASE_V+0X0A8)       /* 记录接口的值 */
#define USBD_PKTPASSEDCTRL_V        (USBD_BASE_V+0X0AC)       /* 记录成功接收的包数 */
#define USBD_PKTDROPPEDCTRL_V       (USBD_BASE_V+0X0B0)       /* 记录丢失的包数 */
#define USBD_CRCERRCTRL_V           (USBD_BASE_V+0X0B4)       /* 记录CRC错误的包数 */
#define USBD_BITSTUFFERRCTRL_V      (USBD_BASE_V+0X0B8)       /* 记录位填充错误的包数 */
#define USBD_PIDERRCTRL_V           (USBD_BASE_V+0X0BC)       /* 记录PID错误的包数 */
#define USBD_FRAMINGERRCTL_V        (USBD_BASE_V+0X0C0)       /* 记录有SYNC和EOP的包数 */
#define USBD_TXPKTCTRL_V            (USBD_BASE_V+0X0C4)       /* 记录发送包的数量 */
#define USBD_STATCTRLOV_V           (USBD_BASE_V+0X0C8)       /* 记录统计寄存器溢出情况 */
#define USBD_TXLENGTH_V             (USBD_BASE_V+0X0CC)       /* 记录每次IN传输事务包长度 */
#define USBD_RXLENGTH_V             (USBD_BASE_V+0X0D0)       /* 记录OUT传输事务包长度 */
#define USBD_RESUME_V               (USBD_BASE_V+0X0D4)       /* USB唤醒寄存器 */
#define USBD_READFLAG_V             (USBD_BASE_V+0X0D8)       /* 读异步状态寄存器标志 */
#define USBD_RECEIVETYPE_V          (USBD_BASE_V+0X0DC)       /* 传输状态寄存器 */
#define USBD_APPLOCK_V              (USBD_BASE_V+0X0E0)       /* 锁信号寄存器 */
#define USBD_EP0OUTADDR_V           (USBD_BASE_V+0X100)       /* 端点0端点号和方向 */
#define USBD_EP0OUTBMATTR_V         (USBD_BASE_V+0X104)       /* 端点0类型寄存器 */
#define USBD_EP0OUTMAXPKTSIZE_V     (USBD_BASE_V+0X108)       /* 端点0最大包尺寸寄存器 */
#define USBD_EP0OUTIFNUM_V          (USBD_BASE_V+0X10C)       /* 端点0接口号寄存器 */
#define USBD_EP0OUTSTAT_V           (USBD_BASE_V+0X110)       /* 端点0状态寄存器 */
#define USBD_EP0OUTBMREQTYPE_V      (USBD_BASE_V+0X114)       /* 端点0 SETUP事务请求类 */
#define USBD_EP0OUTBREQUEST_V       (USBD_BASE_V+0X118)       /* 端点0 SETUP事务请求内容 */
#define USBD_EP0OUTWVALUE_V         (USBD_BASE_V+0X11C)       /* 端点0 SETUP事务请求值 */
#define USBD_EP0OUTWINDEX_V         (USBD_BASE_V+0X120)       /* 端点0 SETUP事务请求索引 */
#define USBD_EP0OUTWLENGTH_V        (USBD_BASE_V+0X124)       /* 端点0 SETUP事务请求长度 */
#define USBD_EP0OUTSYNCHFRAME_V     (USBD_BASE_V+0X128)       /* 端点0同步包帧号 */
#define USBD_EP1OUTADDR_V           (USBD_BASE_V+0X12C)       /* 端点1输出端点号和方向 */
#define USBD_EP1OUTBMATTR_V         (USBD_BASE_V+0X130)       /* 端点1输出类型寄存器 */
#define USBD_EP1OUTMAXPKTSIZE_V     (USBD_BASE_V+0X134)       /* 端点1输出最大包尺寸寄存器 */
#define USBD_EP1OUTIFNUM_V          (USBD_BASE_V+0X138)       /* 端点1输出接口号寄存器 */
#define USBD_EP1OUTSTAT_V           (USBD_BASE_V+0X13C)       /* 端点1输出状态寄存器 */
#define USBD_EP1OUTBMREQTYPE_V      (USBD_BASE_V+0X140)       /* 端点1输出SETUP事务请求类型 */
#define USBD_EP1OUTBREQUEST_V       (USBD_BASE_V+0X144)       /* 端点1输出SETUP事务请求内容 */
#define USBD_EP1OUTWVALUE_V         (USBD_BASE_V+0X148)       /* 端点1输出SETUP事务请求值 */
#define USBD_EP1OUTWINDX_V          (USBD_BASE_V+0X14C)       /* 端点1输出SETUP事务请求索引 */
#define USBD_EP1OUTWLENGH_V         (USBD_BASE_V+0X150)       /* 端点1输出SETUP事务请求域长度 */
#define USBD_EP1OUTSYNCHFRAME_V     (USBD_BASE_V+0X154)       /* 端点1输出同步包帧号 */
#define USBD_EP1INADDR_V            (USBD_BASE_V+0X158)       /* 端点1输入端点号和方向 */
#define USBD_EP1INBMATTR_V          (USBD_BASE_V+0X15C)       /* 端点1输入类型寄存器 */
#define USBD_EP1INMAXPKTSIZE_V      (USBD_BASE_V+0X160)       /* 端点1输入最大包尺寸寄存器 */
#define USBD_EP1INIFNUM_V           (USBD_BASE_V+0X164)       /* 端点1输入接口号寄存器 */
#define USBD_EP1INSTAT_V            (USBD_BASE_V+0X168)       /* 端点1输入状态寄存器 */
#define USBD_EP1INBMREQTYPE_V       (USBD_BASE_V+0X16C)       /* 端点1输入SETUP事务请求类型 */
#define USBD_EP1INBREQUEST_V        (USBD_BASE_V+0X170)       /* 端点1输入SETUP事务请求内容 */
#define USBD_EP1INWVALUE_V          (USBD_BASE_V+0X174)       /* 端点1输入SETUP事务请求值 */
#define USBD_EP1INWINDEX_V          (USBD_BASE_V+0X178)       /* 端点1输入SETUP事务请求索引 */
#define USBD_EP1INWLENGTH_V         (USBD_BASE_V+0X17C)       /* 端点1输入SETUP事务请求域长度 */
#define USBD_EP1INSYNCHFRAME_V      (USBD_BASE_V+0X180)       /* 端点1输入同步包帧号 */
#define USBD_EP2OUTADDR_V           (USBD_BASE_V+0X184)       /* 端点2输出端点号和方向 */
#define USBD_EP2OUTBMATTR_V         (USBD_BASE_V+0X188)       /* 端点2输出类型寄存器 */
#define USBD_EP2OUTMAXPKTSIZE_V     (USBD_BASE_V+0X18C)       /* 端点2输出最大包尺寸寄存器 */
#define USBD_EP2OUTIFNUM_V          (USBD_BASE_V+0X190)       /* 端点2输出接口号寄存器 */
#define USBD_EP2OUTSTAT_V           (USBD_BASE_V+0X194)       /* 端点2输出状态寄存器 */
#define USBD_EP2OUTBMREQTYPE_V      (USBD_BASE_V+0X198)       /* 端点2输出SETUP事务请求类型 */
#define USBD_EP2OUTBREQUEST_V       (USBD_BASE_V+0X19C)       /* 端点2输出SETUP事务请求内容 */
#define USBD_EP2OUTWVALUE_V         (USBD_BASE_V+0X1A0)       /* 端点2输出SETUP事务请求值 */
#define USBD_EP2OUTWINDEX_V         (USBD_BASE_V+0X1A4)       /* 端点2输出SETUP事务请求索引 */
#define USBD_EP2OUTWLENGTH_V        (USBD_BASE_V+0X1A8)       /* 端点2输出SETUP事务请求域长度 */
#define USBD_EP2OUTSYNCHFRAME_V     (USBD_BASE_V+0X1AC)       /* 端点2输出同步包帧号 */
#define USBD_EP2INADDR_V            (USBD_BASE_V+0X1B0)       /* 端点2输入端点号和方向 */
#define USBD_EP2INBMATTR_V          (USBD_BASE_V+0X1B4)       /* 端点2输入类型寄存器 */
#define USBD_EP2INMAXPKTSIZE_V      (USBD_BASE_V+0X1B8)       /* 端点2输入最大包尺寸寄存器 */
#define USBD_EP2INIFNUM_V           (USBD_BASE_V+0X1BC)       /* 端点2输入接口号寄存器 */
#define USBD_EP2INSTAT_V            (USBD_BASE_V+0X1C0)       /* 端点2输入状态寄存器 */
#define USBD_EP2INBMREQTYPE_V       (USBD_BASE_V+0X1C4)       /* 端点2输入SETUP事务请求类型 */
#define USBD_EP2INBREQUEST_V        (USBD_BASE_V+0X1C8)       /* 端点2输入SETUP事务请求内容 */
#define USBD_EP2INWVALUE_V          (USBD_BASE_V+0X1CC)       /* 端点2输入SETUP事务请求值 */
#define USBD_EP2INWINDEX_V          (USBD_BASE_V+0X1D0)       /* 端点2输入SETUP事务请求索引 */
#define USBD_EP2INWLENGTH_V         (USBD_BASE_V+0X1D4)       /* 端点2输入SETUP事务请求域长度 */
#define USBD_EP2INSYNCHFRAME_V      (USBD_BASE_V+0X1D8)       /* 端点2输入同步包帧号 */
#define USBD_RXFIFO_V               (USBD_BASE_V+0X200)       /* 接受FIFO */
#define USBD_TXFIFO_V               (USBD_BASE_V+0X300)       /* 发送FIFO */


/*
 * EMI模块
 * 物理基址: 0x11000000
 * 虚拟基址: 0xE1000000
 */

/* 物理地址 */
#define EMI_CSACONF               (EMI_BASE+0X000)        /* CSA参数配置寄存器 */
#define EMI_CSBCONF               (EMI_BASE+0X004)        /* CSB参数配置寄存器 */
#define EMI_CSCCONF               (EMI_BASE+0X008)        /* CSC参数配置寄存器 */
#define EMI_CSDCONF               (EMI_BASE+0X00C)        /* CSD参数配置寄存器 */
#define EMI_CSECONF               (EMI_BASE+0X010)        /* CSE参数配置寄存器 */
#define EMI_CSFCONF               (EMI_BASE+0X014)        /* CSF参数配置寄存器 */
#define EMI_SDCONF1               (EMI_BASE+0X018)        /* SDRAM时序配置寄存器1 */
#define EMI_SDCONF2               (EMI_BASE+0X01C)        /* SDRAM时序配置寄存器2, SDRAM初始化用到的配置信息 */
#define EMI_REMAPCONF             (EMI_BASE+0X020)        /* 片选空间及地址映射REMAP配置寄存器 */
#define EMI_NAND_ADDR1            (EMI_BASE+0X100)        /* NAND FLASH的地址寄存器1 */
#define EMI_NAND_COM              (EMI_BASE+0X104)        /* NAND FLASH的控制字寄存器 */
#define EMI_NAND_STA              (EMI_BASE+0X10C)        /* NAND FLASH的状态寄存器 */
#define EMI_ERR_ADDR1             (EMI_BASE+0X110)        /* 读操作出错的地址寄存器1 */
#define EMI_ERR_ADDR2             (EMI_BASE+0X114)        /* 读操作出错的地址寄存器2 */
#define EMI_NAND_CONF1            (EMI_BASE+0X118)        /* NAND FLASH的配置器存器1 */
#define EMI_NAND_INTR             (EMI_BASE+0X11C)        /* NAND FLASH中断寄存器 */
#define EMI_NAND_ECC              (EMI_BASE+0X120)        /* ECC校验完成寄存器 */
#define EMI_NAND_IDLE             (EMI_BASE+0X124)        /* NAND FLASH空闲寄存器 */
#define EMI_NAND_CONF2            (EMI_BASE+0X128)        /* NAND FLASH的配置器存器2 */
#define EMI_NAND_ADDR2            (EMI_BASE+0X12C)        /* NAND FLASH的地址寄存器2 */
#define EMI_NAND_DATA             (EMI_BASE+0X200)        /* NAND FLASH的数据寄存器 */

/* 虚拟地址 */
#define EMI_CSACONF_V             (EMI_BASE_V+0X000)      /* CSA参数配置寄存器 */
#define EMI_CSBCONF_V             (EMI_BASE_V+0X004)      /* CSB参数配置寄存器 */
#define EMI_CSCCONF_V             (EMI_BASE_V+0X008)      /* CSC参数配置寄存器 */
#define EMI_CSDCONF_V             (EMI_BASE_V+0X00C)      /* CSD参数配置寄存器 */
#define EMI_CSECONF_V             (EMI_BASE_V+0X010)      /* CSE参数配置寄存器 */
#define EMI_CSFCONF_V             (EMI_BASE_V+0X014)      /* CSF参数配置寄存器 */
#define EMI_SDCONF1_V             (EMI_BASE_V+0X018)      /* SDRAM时序配置寄存器1 */
#define EMI_SDCONF2_V             (EMI_BASE_V+0X01C)      /* SDRAM时序配置寄存器2, SDRAM初始化用到的配置信息 */
#define EMI_REMAPCONF_V           (EMI_BASE_V+0X020)      /* 片选空间及地址映射REMAP配置寄存器 */
#define EMI_NAND_ADDR1_V          (EMI_BASE_V+0X100)      /* NAND FLASH的地址寄存器1 */
#define EMI_NAND_COM_V            (EMI_BASE_V+0X104)      /* NAND FLASH的控制字寄存器 */
#define EMI_NAND_STA_V            (EMI_BASE_V+0X10C)      /* NAND FLASH的状态寄存器 */
#define EMI_ERR_ADDR1_V           (EMI_BASE_V+0X110)      /* 读操作出错的地址寄存器1 */
#define EMI_ERR_ADDR2_V           (EMI_BASE_V+0X114)      /* 读操作出错的地址寄存器2 */
#define EMI_NAND_CONF1_V          (EMI_BASE_V+0X118)      /* NAND FLASH的配置器存器1 */
#define EMI_NAND_INTR_V           (EMI_BASE_V+0X11C)      /* NAND FLASH中断寄存器 */
#define EMI_NAND_ECC_V            (EMI_BASE_V+0X120)      /* ECC校验完成寄存器 */
#define EMI_NAND_IDLE_V           (EMI_BASE_V+0X124)      /* NAND FLASH空闲寄存器 */
#define EMI_NAND_CONF2_V          (EMI_BASE_V+0X128)      /* NAND FLASH的配置器存器2 */
#define EMI_NAND_ADDR2_V          (EMI_BASE_V+0X12C)      /* NAND FLASH的地址寄存器2 */
#define EMI_NAND_ID_V		  (EMI_BASE_V+0X130)	  /* NAND FLASH的ID寄存器 */
#define EMI_NAND_DATA_V           (EMI_BASE_V+0X200)      /* NAND FLASH的数据寄存器 */


/*
 * DMAC模块
 * 物理基址: 0x11001000
 * 虚拟基址: 0xE1001000
 */

/* 物理地址 */
#define DMAC_INTSTATUS            (DMAC_BASE+0X020)       /* DAMC中断状态寄存器。 */
#define DMAC_INTTCSTATUS          (DMAC_BASE+0X050)       /* DMAC传输完成中断状态寄存器 */
#define DMAC_INTTCCLEAR           (DMAC_BASE+0X060)       /* DMAC传输完成中断状态清除寄存器 */
#define DMAC_INTERRORSTATUS       (DMAC_BASE+0X080)       /* DMAC传输错误中断状态寄存器 */
#define DMAC_INTINTERRCLR         (DMAC_BASE+0X090)       /* DMAC传输错误中断状态清除寄存器 */
#define DMAC_ENBLDCHNS            (DMAC_BASE+0X0B0)       /* DMAC通道使能状态寄存器 */
#define DMAC_C0SRCADDR            (DMAC_BASE+0X000)       /* DMAC道0源地址寄存器 */
#define DMAC_C0DESTADD            (DMAC_BASE+0X004)       /* DMAC道0目的地址寄存器 */
#define DMAC_C0CONTROL            (DMAC_BASE+0X00C)       /* DMAC道0控制寄存器 */
#define DMAC_C0CONFIGURATION      (DMAC_BASE+0X010)       /* DMAC道0配置寄存器 */
#define DMAC_C0DESCRIPTOR         (DMAC_BASE+0X01C)       /* DMAC道0链表地址寄存器 */
#define DMAC_C1SRCADDR            (DMAC_BASE+0X100)       /* DMAC道1源地址寄存器 */
#define DMAC_C1DESTADDR           (DMAC_BASE+0X104)       /* DMAC道1目的地址寄存器 */
#define DMAC_C1CONTROL            (DMAC_BASE+0X10C)       /* DMAC道1控制寄存器 */
#define DMAC_C1CONFIGURATION      (DMAC_BASE+0X110)       /* DMAC道1配置寄存器 */
#define DMAC_C1DESCRIPTOR         (DMAC_BASE+0X114)       /* DMAC道1链表地址寄存器 */
#define DMAC_C2SRCADDR            (DMAC_BASE+0X200)       /* DMAC道2源地址寄存器 */
#define DMAC_C2DESTADDR           (DMAC_BASE+0X204)       /* DMAC道2目的地址寄存器 */
#define DMAC_C2CONTROL            (DMAC_BASE+0X20C)       /* DMAC道2控制寄存器 */
#define DMAC_C2CONFIGURATION      (DMAC_BASE+0X210)       /* DMAC道2配置寄存器 */
#define DMAC_C2DESCRIPTOR         (DMAC_BASE+0X214)       /* DMAC道2链表地址寄存器 */
#define DMAC_C3SRCADDR            (DMAC_BASE+0X300)       /* DMAC道3源地址寄存器 */
#define DMAC_C3DESTADDR           (DMAC_BASE+0X304)       /* DMAC道3目的地址寄存器 */
#define DMAC_C3CONTROL            (DMAC_BASE+0X30C)       /* DMAC道3控制寄存器 */
#define DMAC_C3CONFIGURATION      (DMAC_BASE+0X310)       /* DMAC道3配置寄存器 */
#define DMAC_C3DESCRIPTOR         (DMAC_BASE+0X314)       /* DMAC道3链表地址寄存器 */
#define DMAC_C4SRCADDR            (DMAC_BASE+0X400)       /* DMAC道4源地址寄存器 */
#define DMAC_C4DESTADDR           (DMAC_BASE+0X404)       /* DMAC道4目的地址寄存器 */
#define DMAC_C4CONTROL            (DMAC_BASE+0X40C)       /* DMAC道4控制寄存器 */
#define DMAC_C4CONFIGURATION      (DMAC_BASE+0X410)       /* DMAC道4配置寄存器 */
#define DMAC_C4DESCRIPTOR         (DMAC_BASE+0X414)       /* DMAC道4链表地址寄存器 */
#define DMAC_C5SRCADDR            (DMAC_BASE+0X500)       /* DMAC道5源地址寄存器 */
#define DMAC_C5DESTADDR           (DMAC_BASE+0X504)       /* DMAC道5目的地址寄存器 */
#define DMAC_C5CONTROL            (DMAC_BASE+0X50C)       /* DMAC道5控制寄存器 */
#define DMAC_C5CONFIGURATION      (DMAC_BASE+0X510)       /* DMAC道5配置寄存器 */
#define DMAC_C5DESCRIPTOR         (DMAC_BASE+0X514)       /* DMAC道5链表地址寄存器 */

/* 虚拟地址 */
#define DMAC_INTSTATUS_V          (DMAC_BASE_V+0X020)     /* DAMC中断状态寄存器。 */
#define DMAC_INTTCSTATUS_V        (DMAC_BASE_V+0X050)     /* DMAC传输完成中断状态寄存器 */
#define DMAC_INTTCCLEAR_V         (DMAC_BASE_V+0X060)     /* DMAC传输完成中断状态清除寄存器 */
#define DMAC_INTERRORSTATUS_V     (DMAC_BASE_V+0X080)     /* DMAC传输错误中断状态寄存器 */
#define DMAC_INTINTERRCLR_V       (DMAC_BASE_V+0X090)     /* DMAC传输错误中断状态清除寄存器 */
#define DMAC_ENBLDCHNS_V          (DMAC_BASE_V+0X0B0)     /* DMAC通道使能状态寄存器 */
#define DMAC_C0SRCADDR_V          (DMAC_BASE_V+0X000)     /* DMAC道0源地址寄存器 */
#define DMAC_C0DESTADD_V          (DMAC_BASE_V+0X004)     /* DMAC道0目的地址寄存器 */
#define DMAC_C0CONTROL_V          (DMAC_BASE_V+0X00C)     /* DMAC道0控制寄存器 */
#define DMAC_C0CONFIGURATION_V    (DMAC_BASE_V+0X010)     /* DMAC道0配置寄存器 */
#define DMAC_C0DESCRIPTOR_V       (DMAC_BASE_V+0X01C)     /* DMAC道0链表地址寄存器 */
#define DMAC_C1SRCADDR_V          (DMAC_BASE_V+0X100)     /* DMAC道1源地址寄存器 */
#define DMAC_C1DESTADDR_V         (DMAC_BASE_V+0X104)     /* DMAC道1目的地址寄存器 */
#define DMAC_C1CONTROL_V          (DMAC_BASE_V+0X10C)     /* DMAC道1控制寄存器 */
#define DMAC_C1CONFIGURATION_V    (DMAC_BASE_V+0X110)     /* DMAC道1配置寄存器 */
#define DMAC_C1DESCRIPTOR_V       (DMAC_BASE_V+0X114)     /* DMAC道1链表地址寄存器 */
#define DMAC_C2SRCADDR_V          (DMAC_BASE_V+0X200)     /* DMAC道2源地址寄存器 */
#define DMAC_C2DESTADDR_V         (DMAC_BASE_V+0X204)     /* DMAC道2目的地址寄存器 */
#define DMAC_C2CONTROL_V          (DMAC_BASE_V+0X20C)     /* DMAC道2控制寄存器 */
#define DMAC_C2CONFIGURATION_V    (DMAC_BASE_V+0X210)     /* DMAC道2配置寄存器 */
#define DMAC_C2DESCRIPTOR_V       (DMAC_BASE_V+0X214)     /* DMAC道2链表地址寄存器 */
#define DMAC_C3SRCADDR_V          (DMAC_BASE_V+0X300)     /* DMAC道3源地址寄存器 */
#define DMAC_C3DESTADDR_V         (DMAC_BASE_V+0X304)     /* DMAC道3目的地址寄存器 */
#define DMAC_C3CONTROL_V          (DMAC_BASE_V+0X30C)     /* DMAC道3控制寄存器 */
#define DMAC_C3CONFIGURATION_V    (DMAC_BASE_V+0X310)     /* DMAC道3配置寄存器 */
#define DMAC_C3DESCRIPTOR_V       (DMAC_BASE_V+0X314)     /* DMAC道3链表地址寄存器 */
#define DMAC_C4SRCADDR_V          (DMAC_BASE_V+0X400)     /* DMAC道4源地址寄存器 */
#define DMAC_C4DESTADDR_V         (DMAC_BASE_V+0X404)     /* DMAC道4目的地址寄存器 */
#define DMAC_C4CONTROL_V          (DMAC_BASE_V+0X40C)     /* DMAC道4控制寄存器 */
#define DMAC_C4CONFIGURATION_V    (DMAC_BASE_V+0X410)     /* DMAC道4配置寄存器 */
#define DMAC_C4DESCRIPTOR_V       (DMAC_BASE_V+0X414)     /* DMAC道4链表地址寄存器 */
#define DMAC_C5SRCADDR_V          (DMAC_BASE_V+0X500)     /* DMAC道5源地址寄存器 */
#define DMAC_C5DESTADDR_V         (DMAC_BASE_V+0X504)     /* DMAC道5目的地址寄存器 */
#define DMAC_C5CONTROL_V          (DMAC_BASE_V+0X50C)     /* DMAC道5控制寄存器 */
#define DMAC_C5CONFIGURATION_V    (DMAC_BASE_V+0X510)     /* DMAC道5配置寄存器 */
#define DMAC_C5DESCRIPTOR_V       (DMAC_BASE_V+0X514)     /* DMAC道5链表地址寄存器 */


/*
 * LCDC模块
 * 物理基址: 0x11002000
 * 虚拟基址: 0xE1002000
 */

/* 物理地址 */
#define LCDC_SSA                  (LCDC_BASE+0X000)       /* 屏幕起始地址寄存器 */
#define LCDC_SIZE                 (LCDC_BASE+0X004)       /* 屏幕尺寸寄存器 */
#define LCDC_PCR                  (LCDC_BASE+0X008)       /* 面板配置寄存器 */
#define LCDC_HCR                  (LCDC_BASE+0X00C)       /* 水平配置寄存器 */
#define LCDC_VCR                  (LCDC_BASE+0X010)       /* 垂直配置寄存器 */
#define LCDC_PWMR                 (LCDC_BASE+0X014)       /* PWM对比度控制寄存器 */
#define LCDC_LECR                 (LCDC_BASE+0X018)       /* 使能控制寄存器 */
#define LCDC_DMACR                (LCDC_BASE+0X01C)       /* DMA控制寄存器 */
#define LCDC_LCDISREN             (LCDC_BASE+0X020)       /* 中断使能寄存器 */
#define LCDC_LCDISR               (LCDC_BASE+0X024)       /* 中断状态寄存器 */
#define LCDC_LGPMR                (LCDC_BASE+0X040)       /* 灰度调色映射寄存器组 (16个32bit寄存器) */

/* 虚拟地址 */
#define LCDC_SSA_V                (LCDC_BASE_V+0X000)     /* 屏幕起始地址寄存器 */
#define LCDC_SIZE_V               (LCDC_BASE_V+0X004)     /* 屏幕尺寸寄存器 */
#define LCDC_PCR_V                (LCDC_BASE_V+0X008)     /* 面板配置寄存器 */
#define LCDC_HCR_V                (LCDC_BASE_V+0X00C)     /* 水平配置寄存器 */
#define LCDC_VCR_V                (LCDC_BASE_V+0X010)     /* 垂直配置寄存器 */
#define LCDC_PWMR_V               (LCDC_BASE_V+0X014)     /* PWM对比度控制寄存器 */
#define LCDC_LECR_V               (LCDC_BASE_V+0X018)     /* 使能控制寄存器 */
#define LCDC_DMACR_V              (LCDC_BASE_V+0X01C)     /* DMA控制寄存器 */
#define LCDC_LCDISREN_V           (LCDC_BASE_V+0X020)     /* 中断使能寄存器 */
#define LCDC_LCDISR_V             (LCDC_BASE_V+0X024)     /* 中断状态寄存器 */
#define LCDC_LGPMR_V              (LCDC_BASE_V+0X040)     /* 灰度调色映射寄存器组 (16个32bit寄存器) */


/*
 * MAC模块
 * 物理基址: 0x11003000
 * 虚拟基址: 0xE1003000
 */

/* 物理地址 */
#define MAC_CTRL                  (MAC_BASE+0X000)        /* MAC控制寄存器 */
#define MAC_INTSRC                (MAC_BASE+0X004)        /* MAC中断源寄存器 */
#define MAC_INTMASK               (MAC_BASE+0X008)        /* MAC中断屏蔽寄存器 */
#define MAC_IPGT                  (MAC_BASE+0X00C)        /* 连续帧间隔寄存器 */
#define MAC_IPGR1                 (MAC_BASE+0X010)        /* 等待窗口寄存器 */
#define MAC_IPGR2                 (MAC_BASE+0X014)        /* 等待窗口寄存器 */
#define MAC_PACKETLEN             (MAC_BASE+0X018)        /* 帧长度寄存器 */
#define MAC_COLLCONF              (MAC_BASE+0X01C)        /* 碰撞重发寄存器 */
#define MAC_TXBD_NUM              (MAC_BASE+0X020)        /* 发送描述符寄存器 */
#define MAC_FLOWCTRL              (MAC_BASE+0X024)        /* 流控寄存器 */
#define MAC_MII_CTRL              (MAC_BASE+0X028)        /* PHY控制寄存器 */
#define MAC_MII_CMD               (MAC_BASE+0X02C)        /* PHY命令寄存器 */
#define MAC_MII_ADDRESS           (MAC_BASE+0X030)        /* PHY地址寄存器 */
#define MAC_MII_TXDATA            (MAC_BASE+0X034)        /* PHY写数据寄存器 */
#define MAC_MII_RXDATA            (MAC_BASE+0X038)        /* PHY读数据寄存器 */
#define MAC_MII_STATUS            (MAC_BASE+0X03C)        /* PHY状态寄存器 */
#define MAC_ADDR0                 (MAC_BASE+0X040)        /* MAC地址寄存器 */
#define MAC_ADDR1                 (MAC_BASE+0X044)        /* MAC地址寄存器 */
#define MAC_HASH0                 (MAC_BASE+0X048)        /* MAC HASH寄存器 */
#define MAC_HASH1                 (MAC_BASE+0X04C)        /* MAC HASH寄存器 */
#define MAC_TXPAUSE               (MAC_BASE+0X050)        /* MAC控制帧寄存器 */
#define MAC_TX_BD                 (MAC_BASE+0X400)        /* MAC发送描述符 */
#define MAC_RX_BD                 (MAC_BASE+0X600)        /* MAC接收描述符 */

/* 虚拟地址 */
#define MAC_CTRL_V                (MAC_BASE_V+0X000)      /* MAC控制寄存器 */
#define MAC_INTSRC_V              (MAC_BASE_V+0X004)      /* MAC中断源寄存器 */
#define MAC_INTMASK_V             (MAC_BASE_V+0X008)      /* MAC中断屏蔽寄存器 */
#define MAC_IPGT_V                (MAC_BASE_V+0X00C)      /* 连续帧间隔寄存器 */
#define MAC_IPGR1_V               (MAC_BASE_V+0X010)      /* 等待窗口寄存器 */
#define MAC_IPGR2_V               (MAC_BASE_V+0X014)      /* 等待窗口寄存器 */
#define MAC_PACKETLEN_V           (MAC_BASE_V+0X018)      /* 帧长度寄存器 */
#define MAC_COLLCONF_V            (MAC_BASE_V+0X01C)      /* 碰撞重发寄存器 */
#define MAC_TXBD_NUM_V            (MAC_BASE_V+0X020)      /* 发送描述符寄存器 */
#define MAC_FLOWCTRL_V            (MAC_BASE_V+0X024)      /* 流控寄存器 */
#define MAC_MII_CTRL_V            (MAC_BASE_V+0X028)      /* PHY控制寄存器 */
#define MAC_MII_CMD_V             (MAC_BASE_V+0X02C)      /* PHY命令寄存器 */
#define MAC_MII_ADDRESS_V         (MAC_BASE_V+0X030)      /* PHY地址寄存器 */
#define MAC_MII_TXDATA_V          (MAC_BASE_V+0X034)      /* PHY写数据寄存器 */
#define MAC_MII_RXDATA_V          (MAC_BASE_V+0X038)      /* PHY读数据寄存器 */
#define MAC_MII_STATUS_V          (MAC_BASE_V+0X03C)      /* PHY状态寄存器 */
#define MAC_ADDR0_V               (MAC_BASE_V+0X040)      /* MAC地址寄存器 */
#define MAC_ADDR1_V               (MAC_BASE_V+0X044)      /* MAC地址寄存器 */
#define MAC_HASH0_V               (MAC_BASE_V+0X048)      /* MAC HASH寄存器 */
#define MAC_HASH1_V               (MAC_BASE_V+0X04C)      /* MAC HASH寄存器 */
#define MAC_TXPAUSE_V             (MAC_BASE_V+0X050)      /* MAC控制帧寄存器 */
#define MAC_TX_BD_V               (MAC_BASE_V+0X400)        /* MAC发送描述符 */
#define MAC_RX_BD_V               (MAC_BASE_V+0X600)        /* MAC接收描述符 */

#define INT_ENABLE(n) *(volatile unsigned int*)INTC_IMR_V&=~(1<<n)
#define INT_DISABLE(n) *(volatile unsigned int*)INTC_IMR_V |= (1<<(n))
#define CLEAR_PEND_INT(n) IntPend = (1<<(n))
#define SET_PEND_INT(n) IntPndTst |= (1<<(n))



#define IO_BASE         0xe0000000                      // VA of IO
#define IO_SIZE         0x01006000                      // How much?
#define IO_START        INTC_BASE                       // PA of IO
#define NAND_READ_ID	9
#define NAND_READ_STATUS 10
#define PMCR 0X400B
#define UARTCLK (PMCR-0X4000)*8000000
#define MHz 1000000
#define fMCLK_MHz UARTCLK
#define fMCLK (fMCLK_MHz / MHz)
#define MCLK2 (fMCLK_MHz / 2)


#endif

