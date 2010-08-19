/*
 * (C) Copyright 2003
 * SEP4020RM9200 definitions
 * Author : SEU SEP4020 application group
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
#ifndef SEP4020_H
#define SEP4020_H

#define read_reg(reg)        *(RP)reg
#define write_reg(reg, data) *(RP)reg = data

#define REGW(addr)	(*(volatile unsigned int *)(addr))
#define REGS(addr)	(*(volatile unsigned short *)(addr))
#define REGB(addr)	(*(volatile unsigned char *)(addr))


//*****************************************************************************
//               PERIPHERAL ID DEFINITIONS FOR SEP4020 中断号
//*****************************************************************************
#define SEP4020_ID_EMAC   ((unsigned int) 28) // Ethernet Mac

#define  INTSRC_RTC        31		        
#define  INTSRC_DMAC       30	                
#define  INTSRC_EMI        29
#define  INTSRC_MAC        28
#define  INTSRC_TIMER1     27
#define  INTSRC_TIMER2     26
#define  INTSRC_TIMER3     25
#define  INTSRC_UART0      24
#define  INTSRC_UART1      23
#define  INTSRC_UART2      22
#define  INTSRC_UART3      21
#define  INTSRC_PWM        20
#define  INTSRC_LCDC       19
#define  INTSRC_I2S        18
#define  INTSRC_SSI        17

#define  INTSRC_USB        15
#define  INTSRC_SMC0       14
#define  INTSRC_SMC1       13
#define  INTSRC_SDIO       12  
#define  INTSRC_EXINT10    11              
#define  INTSRC_EXINT9     10              
#define  INTSRC_EXINT8     9               
#define  INTSRC_EXINT7     8               
#define  INTSRC_EXINT6     7               
#define  INTSRC_EXINT5     6               
#define  INTSRC_EXINT4     5               
#define  INTSRC_EXINT3     4               
#define  INTSRC_EXINT2     3               
#define  INTSRC_EXINT1     2               
#define  INTSRC_EXINT0     1
#define  INTSRC_NULL       0


//*****************************************************************************
//               BASE ADDRESS DEFINITIONS FOR SEP4020
// // *****************************************************************************

#define   ESRAM_BASE    0x04000000
#define   INTC_BASE     0x10000000
#define   PMC_BASE      0x10001000
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
#define   MAC_BASE      0x11003000 // (EMACB) Base Address
#define   AMBA_BASE     0x11005000

/*
*  INTC模块
*  基址: 0x10000000
 */

#define INTC_IER                  (REGW(INTC_BASE+0X000))       /* IRQ中断允许寄存器 */
#define INTC_IMR                  (REGW(INTC_BASE+0X008))       /* IRQ中断屏蔽寄存器 */
#define INTC_IFR                  (REGW(INTC_BASE+0X010))       /* IRQ软件强制中断寄存器 */
#define INTC_IRSR                 (REGW(INTC_BASE+0X018))       /* IRQ未处理中断状态寄存器 */
#define INTC_ISR                  (REGW(INTC_BASE+0X020))       /* IRQ中断状态寄存器 */
#define INTC_IMSR                 (REGW(INTC_BASE+0X028))       /* IRQ屏蔽中断状态寄存器 */
#define INTC_IFSR                 (REGW(INTC_BASE+0X030))       /* IRQ中断最终状态寄存器 */
#define INTC_FIER                 (REGW(INTC_BASE+0X0C0))       /* FIQ中断允许寄存器 */
#define INTC_FIMR                 (REGW(INTC_BASE+0X0C4))       /* FIQ中断屏蔽寄存器 */
#define INTC_FIFR                 (REGW(INTC_BASE+0X0C8))       /* FIQ软件强制中断寄存器 */
#define INTC_FIRSR                (REGW(INTC_BASE+0X0CC))       /* FIQ未处理中断状态寄存器 */
#define INTC_FISR                 (REGW(INTC_BASE+0X0D0))       /* FIQ中断状态寄存器 */
#define INTC_FIFSR                (REGW(INTC_BASE+0X0D4))       /* FIQ中断最终状态寄存器 */
#define INTC_IPLR                 (REGW(INTC_BASE+0X0D8))       /* IRQ中断优先级寄存器 */
#define INTC_ICR1                 (REGW(INTC_BASE+0X0DC))       /* IRQ内部中断优先级控制寄存器1 */
#define INTC_ICR2                 (REGW(INTC_BASE+0X0E0))       /* IRQ内部中断优先级控制寄存器2 */
#define INTC_EXICR1               (REGW(INTC_BASE+0X0E4))       /* IRQ外部中断优先级控制寄存器1 */
#define INTC_EXICR2               (REGW(INTC_BASE+0X0E8))       /* IRQ外部中断优先级控制寄存器2 */

/*
 *  PMU模块
 *  基址: 0x10001000
 */

#define PMU_PLTR                  (REGW(PMU_BASE+0X000))        /* PLL的稳定过渡时间 */
#define PMU_PMCR                  (REGW(PMU_BASE+0X004))        /* 系统主时钟PLL的控制寄存器 */
#define PMU_PUCR                  (REGW(PMU_BASE+0X008))        /* USB时钟PLL的控制寄存器 */
#define PMU_PCSR                  (REGW(PMU_BASE+0X00C))        /* 内部模块时钟源供给的控制寄存器 */
#define PMU_PDSLOW                (REGW(PMU_BASE+0X010))        /* SLOW状态下时钟的分频因子 */
#define PMU_PMDR                  (REGW(PMU_BASE+0X014))        /* 芯片工作模式寄存器 */
#define PMU_RCTR                  (REGW(PMU_BASE+0X018))        /* Reset控制寄存器 */
#define PMU_CLRWAKUP              (REGW(PMU_BASE+0X01C))        /* WakeUp清除寄存器 */
#define PMU_PDAPB                 (REGW(PMU_BASE+0X020))        /* 保留未用(用于扩展总线时钟分频比寄存器) */
#define PMU_PLLTS                 (REGW(PMU_BASE+0X024))        /* PLL时钟外部测试输出控制寄存器 */

/*
 *  RTC模块
 *  基址: 0x10002000
 */

#define RTC_STA_YMD               (REGW(RTC_BASE+0X000))        /* 年, 月, 日计数寄存器 */
#define RTC_STA_HMS               (REGW(RTC_BASE+0X004))        /* 小时, 分钟, 秒寄存器 */
#define RTC_ALARM_ALL             (REGW(RTC_BASE+0X008))        /* 定时月, 日, 时, 分寄存器 */
#define RTC_CTR                   (REGW(RTC_BASE+0X00C))        /* 控制寄存器 */
#define RTC_INT_EN                (REGW(RTC_BASE+0X010))        /* 中断使能寄存器 */
#define RTC_INT_STS               (REGW(RTC_BASE+0X014))        /* 中断状态寄存器 */
#define RTC_SAMP                  (REGW(RTC_BASE+0X018))        /* 采样周期寄存器 */
#define RTC_WD_CNT                (REGW(RTC_BASE+0X01C))        /* Watch-Dog计数值寄存器 */
#define RTC_WD_SEV                (REGW(RTC_BASE+0X020))        /* Watch-Dog服务寄存器 */ 
#define RTC_CONFIG_CHECK          (REGW(RTC_BASE+0X024))        /* 配置时间确认寄存器 (在配置时间之前先写0xaaaaaaaa) */
#define RTC_SOFTRESET             (REGW(RTC_BASE+0X028))        /* 软件复位控制寄存器, 4020中被去掉了 */
#define RTC_KEY0                  (REGW(RTC_BASE+0X02C))        /* 密钥寄存器0, 4020中只有这一个寄存器 */
#define RTC_KEY1                  (REGW(RTC_BASE+0X030))        /* 密钥寄存器1 */
#define RTC_KEY2                  (REGW(RTC_BASE+0X034))        /* 密钥寄存器2 */
#define RTC_KEY3                  (REGW(RTC_BASE+0X038))        /* 密钥寄存器3 */
#define RTC_KEY4                  (REGW(RTC_BASE+0X03C))        /* 密钥寄存器4 */
#define RTC_KEY5                  (REGW(RTC_BASE+0X040))        /* 密钥寄存器5 */

/*
 *  TIMER模块
 *  基址: 0x10003000
 */

#define TIMER_T1LCR               (REGW(TIMER_BASE+0X000))      /* 通道1加载计数寄存器 */
#define TIMER_T1CCR               (REGW(TIMER_BASE+0X004))      /* 通道1当前计数值寄存器 */
#define TIMER_T1CR                (REGW(TIMER_BASE+0X008))      /* 通道1控制寄存器 */
#define TIMER_T1ISCR              (REGW(TIMER_BASE+0X00C))      /* 通道1中断状态清除寄存器 */
#define TIMER_T1IMSR              (REGW(TIMER_BASE+0X010))      /* 通道1中断屏蔽状态寄存器 */
#define TIMER_T2LCR               (REGW(TIMER_BASE+0X020))      /* 通道2加载计数寄存器 */
#define TIMER_T2CCR               (REGW(TIMER_BASE+0X024))      /* 通道2当前计数值寄存器 */
#define TIMER_T2CR                (REGW(TIMER_BASE+0X028))      /* 通道2控制寄存器 */
#define TIMER_T2ISCR              (REGW(TIMER_BASE+0X02C))      /* 通道2中断状态清除寄存器 */
#define TIMER_T2IMSR              (REGW(TIMER_BASE+0X030))      /* 通道2中断屏蔽状态寄存器 */
#define TIMER_T3LCR               (REGW(TIMER_BASE+0X040))      /* 通道3加载计数寄存器 */
#define TIMER_T3CCR               (REGW(TIMER_BASE+0X044))      /* 通道3当前计数值寄存器 */
#define TIMER_T3CR                (REGW(TIMER_BASE+0X048))      /* 通道3控制寄存器 */
#define TIMER_T3ISCR              (REGW(TIMER_BASE+0X04C))      /* 通道3中断状态清除寄存器 */
#define TIMER_T3IMSR              (REGW(TIMER_BASE+0X050))      /* 通道3中断屏蔽状态寄存器 */
#define TIMER_T3CAPR              (REGW(TIMER_BASE+0X054))      /* 通道3捕获寄存器 */
#define TIMER_T4LCR               (REGW(TIMER_BASE+0X060))      /* 通道4加载计数寄存器 */
#define TIMER_T4CCR               (REGW(TIMER_BASE+0X064))      /* 通道4当前计数值寄存器 */
#define TIMER_T4CR                (REGW(TIMER_BASE+0X068))      /* 通道4控制寄存器 */
#define TIMER_T4ISCR              (REGW(TIMER_BASE+0X06C))      /* 通道4中断状态清除寄存器 */
#define TIMER_T4IMSR              (REGW(TIMER_BASE+0X070))      /* 通道4中断屏蔽状态寄存器 */
#define TIMER_T4CAPR              (REGW(TIMER_BASE+0X074))      /* 通道4捕获寄存器 */
#define TIMER_T5LCR               (REGW(TIMER_BASE+0X080))      /* 通道5加载计数寄存器 */
#define TIMER_T5CCR               (REGW(TIMER_BASE+0X084))      /* 通道5当前计数值寄存器 */
#define TIMER_T5CR                (REGW(TIMER_BASE+0X088))      /* 通道5控制寄存器 */
#define TIMER_T5ISCR              (REGW(TIMER_BASE+0X08C))      /* 通道5中断状态清除寄存器 */
#define TIMER_T5IMSR              (REGW(TIMER_BASE+0X090))      /* 通道5中断屏蔽状态寄存器 */
#define TIMER_T5CAPR              (REGW(TIMER_BASE+0X094))      /* 通道5捕获寄存器 */
#define TIMER_T6LCR               (REGW(TIMER_BASE+0X0A0))      /* 通道6加载计数寄存器 */
#define TIMER_T6CCR               (REGW(TIMER_BASE+0X0A4))      /* 通道6当前计数值寄存器 */
#define TIMER_T6CR                (REGW(TIMER_BASE+0X0A8))      /* 通道6控制寄存器 */
#define TIMER_T6ISCR              (REGW(TIMER_BASE+0X0AC))      /* 通道6中断状态清除寄存器 */
#define TIMER_T6IMSR              (REGW(TIMER_BASE+0X0B0))      /* 通道6中断屏蔽状态寄存器 */
#define TIMER_T6CAPR              (REGW(TIMER_BASE+0X0B4))      /* 通道6捕获寄存器 */
#define TIMER_T7LCR               (REGW(TIMER_BASE+0X0C0))      /* 通道7加载计数寄存器 */
#define TIMER_T7CCR               (REGW(TIMER_BASE+0X0C4))      /* 通道7当前计数值寄存器 */
#define TIMER_T7CR                (REGW(TIMER_BASE+0X0C8))      /* 通道7控制寄存器 */
#define TIMER_T7ISCR              (REGW(TIMER_BASE+0X0CC))      /* 通道7中断状态清除寄存器 */
#define TIMER_T7IMSR              (REGW(TIMER_BASE+0X0D0))      /* 通道7中断屏蔽状态寄存器 */
#define TIMER_T8LCR               (REGW(TIMER_BASE+0X0E0))      /* 通道8加载计数寄存器 */
#define TIMER_T8CCR               (REGW(TIMER_BASE+0X0E4))      /* 通道8当前计数值寄存器 */
#define TIMER_T8CR                (REGW(TIMER_BASE+0X0E8))      /* 通道8控制寄存器 */
#define TIMER_T8ISCR              (REGW(TIMER_BASE+0X0EC))      /* 通道8中断状态清除寄存器 */
#define TIMER_T8IMSR              (REGW(TIMER_BASE+0X0F0))      /* 通道8中断屏蔽状态寄存器 */
#define TIMER_T9LCR               (REGW(TIMER_BASE+0X100))      /* 通道9加载计数寄存器 */
#define TIMER_T9CCR               (REGW(TIMER_BASE+0X104))      /* 通道9当前计数值寄存器 */
#define TIMER_T9CR                (REGW(TIMER_BASE+0X108))      /* 通道9控制寄存器 */
#define TIMER_T9ISCR              (REGW(TIMER_BASE+0X10C))      /* 通道9中断状态清除寄存器 */
#define TIMER_T9IMSR              (REGW(TIMER_BASE+0X110))      /* 通道9中断屏蔽状态寄存器 */
#define TIMER_T10LCR              (REGW(TIMER_BASE+0X120))      /* 通道10加载计数寄存器 */
#define TIMER_T10CCR              (REGW(TIMER_BASE+0X124))      /* 通道10当前计数值寄存器 */
#define TIMER_T10CR               (REGW(TIMER_BASE+0X128))      /* 通道10控制寄存器 */
#define TIMER_T10ISCR             (REGW(TIMER_BASE+0X12C))      /* 通道10中断状态清除寄存器 */
#define TIMER_T10IMSR             (REGW(TIMER_BASE+0X130))      /* 通道10中断屏蔽状态寄存器 */
#define TIMER_TIMSR               (REGW(TIMER_BASE+0X140))      /* TIMER中断屏蔽状态寄存器 */
#define TIMER_TISCR               (REGW(TIMER_BASE+0X144))      /* TIMER中断状态清除寄存器 */
#define TIMER_TISR                (REGW(TIMER_BASE+0X148))      /* TIMER中断状态寄存器 */

/*
 *  UART0模块
 *  基址: 0x10005000
 */

#define UART0_DLBL                (REGW(UART0_BASE+0X000))      /* 波特率设置低八位寄存器 */
#define UART0_RXFIFO              (REGB(UART0_BASE+0X000))      /* 接收FIFO */
#define UART0_TXFIFO              (REGB(UART0_BASE+0X000))      /* 发送FIFO */
#define UART0_DLBH                (REGW(UART0_BASE+0X004))      /* 波特率设置高八位寄存器 */
#define UART0_IER                 (REGW(UART0_BASE+0X004))      /* 中断使能寄存器 */
#define UART0_IIR                 (REGW(UART0_BASE+0X008))      /* 中断识别寄存器 */
#define UART0_FCR                 (REGW(UART0_BASE+0X008))      /* FIFO控制寄存器 */
#define UART0_LCR                 (REGW(UART0_BASE+0X00C))      /* 行控制寄存器 */
#define UART0_MCR                 (REGW(UART0_BASE+0X010))      /* Modem控制寄存器 */
#define UART0_LSR                 (REGW(UART0_BASE+0X014))      /* 行状态寄存器 */
#define UART0_MSR                 (REGW(UART0_BASE+0X018))      /* Modem状态寄存器 */

/*
 *  EMI模块
 *  基址: 0x11000000
 */

#define EMI_CSACONF               (REGW(EMI_BASE+0X000))        /* CSA参数配置寄存器 */
#define EMI_CSBCONF               (REGW(EMI_BASE+0X004))        /* CSB参数配置寄存器 */
#define EMI_CSCCONF               (REGW(EMI_BASE+0X008))        /* CSC参数配置寄存器 */
#define EMI_CSDCONF               (REGW(EMI_BASE+0X00C))        /* CSD参数配置寄存器 */
#define EMI_CSECONF               (REGW(EMI_BASE+0X010))        /* CSE参数配置寄存器 */
#define EMI_CSFCONF               (REGW(EMI_BASE+0X014))        /* CSF参数配置寄存器 */
#define EMI_SDCONF1               (REGW(EMI_BASE+0X018))        /* SDRAM时序配置寄存器1 */
#define EMI_SDCONF2               (REGW(EMI_BASE+0X01C))        /* SDRAM时序配置寄存器2, SDRAM初始化用到的配置信息 */
#define EMI_REMAPCONF             (REGW(EMI_BASE+0X020))        /* 片选空间及地址映射REMAP配置寄存器 */
#define EMI_NAND_ADDR1            (REGW(EMI_BASE+0X100))        /* NAND FLASH的地址寄存器1 */
#define EMI_NAND_COM              (REGW(EMI_BASE+0X104))        /* NAND FLASH的控制字寄存器 */
#define EMI_NAND_STA              (REGW(EMI_BASE+0X10C))        /* NAND FLASH的状态寄存器 */
#define EMI_ERR_ADDR1             (REGW(EMI_BASE+0X110))        /* 读操作出错的地址寄存器1 */
#define EMI_ERR_ADDR2             (REGW(EMI_BASE+0X114))        /* 读操作出错的地址寄存器2 */
#define EMI_NAND_CONF1            (REGW(EMI_BASE+0X118))        /* NAND FLASH的配置器存器1 */
#define EMI_NAND_INTR             (REGW(EMI_BASE+0X11C))        /* NAND FLASH中断寄存器 */
#define EMI_NAND_ECC              (REGW(EMI_BASE+0X120))        /* ECC校验完成寄存器 */
#define EMI_NAND_IDLE             (REGW(EMI_BASE+0X124))        /* NAND FLASH空闲寄存器 */
#define EMI_NAND_CONF2            (REGW(EMI_BASE+0X128))        /* NAND FLASH的配置器存器2 */
#define EMI_NAND_ADDR2            (REGW(EMI_BASE+0X12C))        /* NAND FLASH的地址寄存器2 */
#define EMI_NAND_ID		    (REGW(EMI_BASE+0X130))
#define EMI_NAND_DATA             (REGW(EMI_BASE+0X200))        /* NAND FLASH的数据寄存器 */


#define EMI_NAND_ADDR1_RAW	    (EMI_BASE+0X100)        /* NAND FLASH的地址寄存器1 */
#define EMI_NAND_COM_RAW	    (EMI_BASE+0X104)        /* NAND FLASH的控制字寄存器 */
#define EMI_NAND_STA_RAW	    (EMI_BASE+0X10C)        /* NAND FLASH的状态寄存器 */
#define EMI_ERR_ADDR1_RAW	    (EMI_BASE+0X110)        /* 读操作出错的地址寄存器1 */
#define EMI_ERR_ADDR2_RAW	    (EMI_BASE+0X114)        /* 读操作出错的地址寄存器2 */
#define EMI_NAND_CONF1_RAW	    (EMI_BASE+0X118)        /* NAND FLASH的配置器存器1 */
#define EMI_NAND_INTR_RAW	    (EMI_BASE+0X11C)        /* NAND FLASH中断寄存器 */
#define EMI_NAND_ECC_RAW	    (EMI_BASE+0X120)        /* ECC校验完成寄存器 */
#define EMI_NAND_IDLE_RAW	    (EMI_BASE+0X124)        /* NAND FLASH空闲寄存器 */
#define EMI_NAND_CONF2_RAW	    (EMI_BASE+0X128)        /* NAND FLASH的配置器存器2 */
#define EMI_NAND_ADDR2_RAW	    (EMI_BASE+0X12C)        /* NAND FLASH的地址寄存器2 */
#define EMI_NAND_ID_RAW		    (EMI_BASE+0X130)
#define EMI_NAND_DATA_RAW         (EMI_BASE+0X200)        /* NAND FLASH的数据寄存器 */
/*
 *  DMAC模块
 *  基址: 0x11001000
 */

#define DMAC_INTSTATUS            (REGW(DMAC_BASE+0X020))       /* DAMC中断状态寄存器。 */
#define DMAC_INTTCSTATUS          (REGW(DMAC_BASE+0X050))       /* DMAC传输完成中断状态寄存器 */
#define DMAC_INTTCCLEAR           (REGW(DMAC_BASE+0X060))       /* DMAC传输完成中断状态清除寄存器 */
#define DMAC_INTERRORSTATUS       (REGW(DMAC_BASE+0X080))       /* DMAC传输错误中断状态寄存器 */
#define DMAC_INTERRCLR            (REGW(DMAC_BASE+0X090))       /* DMAC传输错误中断状态清除寄存器 */
#define DMAC_ENBLDCHNS            (REGW(DMAC_BASE+0X0B0))       /* DMAC通道使能状态寄存器 */
#define DMAC_C0SRCADDR            (REGW(DMAC_BASE+0X000))      /* DMAC道0源地址寄存器 */
#define DMAC_C0DESTADD            (REGW(DMAC_BASE+0X004))       /* DMAC道0目的地址寄存器 */
#define DMAC_C0CONTROL            (REGW(DMAC_BASE+0X00C))       /* DMAC道0控制寄存器 */
#define DMAC_C0CONFIGURATION      (REGW(DMAC_BASE+0X010))       /* DMAC道0配置寄存器 */
#define DMAC_C0DESCRIPTOR         (REGW(DMAC_BASE+0X01C))       /* DMAC道0链表地址寄存器 */
#define DMAC_C1SRCADDR            (REGW(DMAC_BASE+0X100))       /* DMAC道1源地址寄存器 */
#define DMAC_C1DESTADDR           (REGW(DMAC_BASE+0X104))       /* DMAC道1目的地址寄存器 */
#define DMAC_C1CONTROL            (REGW(DMAC_BASE+0X10C))       /* DMAC道1控制寄存器 */
#define DMAC_C1CONFIGURATION      (REGW(DMAC_BASE+0X110))       /* DMAC道1配置寄存器 */
#define DMAC_C1DESCRIPTOR         (REGW(DMAC_BASE+0X114))       /* DMAC道1链表地址寄存器 */
#define DMAC_C2SRCADDR            (REGW(DMAC_BASE+0X200))       /* DMAC道2源地址寄存器 */
#define DMAC_C2DESTADDR           (REGW(DMAC_BASE+0X204))       /* DMAC道2目的地址寄存器 */
#define DMAC_C2CONTROL            (REGW(DMAC_BASE+0X20C))       /* DMAC道2控制寄存器 */
#define DMAC_C2CONFIGURATION      (REGW(DMAC_BASE+0X210))       /* DMAC道2配置寄存器 */
#define DMAC_C2DESCRIPTOR         (REGW(DMAC_BASE+0X214))       /* DMAC道2链表地址寄存器 */
#define DMAC_C3SRCADDR            (REGW(DMAC_BASE+0X300))       /* DMAC道3源地址寄存器 */
#define DMAC_C3DESTADDR           (REGW(DMAC_BASE+0X304))       /* DMAC道3目的地址寄存器 */
#define DMAC_C3CONTROL            (REGW(DMAC_BASE+0X30C))       /* DMAC道3控制寄存器 */
#define DMAC_C3CONFIGURATION      (REGW(DMAC_BASE+0X310))       /* DMAC道3配置寄存器 */
#define DMAC_C3DESCRIPTOR         (REGW(DMAC_BASE+0X314))       /* DMAC道3链表地址寄存器 */
#define DMAC_C4SRCADDR            (REGW(DMAC_BASE+0X400))       /* DMAC道4源地址寄存器 */
#define DMAC_C4DESTADDR           (REGW(DMAC_BASE+0X404))       /* DMAC道4目的地址寄存器 */
#define DMAC_C4CONTROL            (REGW(DMAC_BASE+0X40C))       /* DMAC道4控制寄存器 */
#define DMAC_C4CONFIGURATION      (REGW(DMAC_BASE+0X410))       /* DMAC道4配置寄存器 */
#define DMAC_C4DESCRIPTOR         (REGW(DMAC_BASE+0X414))       /* DMAC道4链表地址寄存器 */
#define DMAC_C5SRCADDR            (REGW(DMAC_BASE+0X500))       /* DMAC道5源地址寄存器 */
#define DMAC_C5DESTADDR           (REGW(DMAC_BASE+0X504))       /* DMAC道5目的地址寄存器 */
#define DMAC_C5CONTROL            (REGW(DMAC_BASE+0X50C))       /* DMAC道5控制寄存器 */
#define DMAC_C5CONFIGURATION      (REGW(DMAC_BASE+0X510))       /* DMAC道5配置寄存器 */
#define DMAC_C5DESCRIPTOR         (REGW(DMAC_BASE+0X514))       /* DMAC道5链表地址寄存器 */

// ========== Register definition for EMACB peripheral ========== 
#define MAC_CTRL                  (REGW(MAC_BASE+0X000))        /* MAC控制寄存器 */
#define MAC_INTSRC                (REGW(MAC_BASE+0X004))        /* MAC中断源寄存器 */
#define MAC_INTMASK               (REGW(MAC_BASE+0X008))        /* MAC中断屏蔽寄存器 */
#define MAC_IPGT                  (REGW(MAC_BASE+0X00C))        /* 连续帧间隔寄存器 */
#define MAC_IPGR1                 (REGW(MAC_BASE+0X010))        /* 等待窗口寄存器 */
#define MAC_IPGR2                 (REGW(MAC_BASE+0X014))        /* 等待窗口寄存器 */
#define MAC_PACKETLEN             (REGW(MAC_BASE+0X018))        /* 帧长度寄存器 */
#define MAC_COLLCONF              (REGW(MAC_BASE+0X01C))        /* 碰撞重发寄存器 */
#define MAC_TXBD_NUM              (REGW(MAC_BASE+0X020))        /* 发送描述符寄存器 */
#define MAC_FLOWCTRL              (REGW(MAC_BASE+0X024))        /* 流控寄存器 */
#define MAC_MII_CTRL              (REGW(MAC_BASE+0X028))        /* PHY控制寄存器 */
#define MAC_MII_CMD               (REGW(MAC_BASE+0X02C))        /* PHY命令寄存器 */
#define MAC_MII_ADDRESS           (REGW(MAC_BASE+0X030))        /* PHY地址寄存器 */
#define MAC_MII_TXDATA            (REGW(MAC_BASE+0X034))        /* PHY写数据寄存器 */
#define MAC_MII_RXDATA            (REGW(MAC_BASE+0X038))        /* PHY读数据寄存器 */
#define MAC_MII_STATUS            (REGW(MAC_BASE+0X03C))        /* PHY状态寄存器 */
#define MAC_ADDR0                 (REGW(MAC_BASE+0X040))        /* MAC地址寄存器 */
#define MAC_ADDR1                 (REGW(MAC_BASE+0X044))        /* MAC地址寄存器 */
#define MAC_HASH0                 (REGW(MAC_BASE+0X048))        /* MAC HASH寄存器 */
#define MAC_HASH1                 (REGW(MAC_BASE+0X04C))        /* MAC HASH寄存器 */
#define MAC_TXPAUSE              (REGW(MAC_BASE+0X050))        /* MAC控制帧寄存器 */
#define MAC_TX_BD                (MAC_BASE+0x400)      /*发送描述符*/
#define MAC_RX_BD                (MAC_BASE+0x600)       /*接收描述符*/

/*
*  GPIO模块
*  基址: 0x1000B000
 */

#define GPIO_DBCLK_DIV            (REGW(GPIO_BASE+0X000))       /* 去毛刺采用时钟分频比配置寄存器 */
#define GPIO_PORTA_DIR            (REGW(GPIO_BASE+0X004))       /* A组端口输入输出方向配置寄存器 */
#define GPIO_PORTA_SEL            (REGW(GPIO_BASE+0X008))       /* A组端口通用用途选择配置寄存器 */
#define GPIO_PORTA_INCTL          (REGW(GPIO_BASE+0X00C))       /* A组端口通用用途输入时类型配置寄存器 */
#define GPIO_PORTA_INTRCTL        (REGW(GPIO_BASE+0X010))       /* A组端口中断触发类型配置寄存器 */
#define GPIO_PORTA_INTRCLR        (REGW(GPIO_BASE+0X014))       /* A组端口通用用途中断清除配置寄存器 */
#define GPIO_PORTA_DATA           (REGW(GPIO_BASE+0X018))       /* A组端口通用用途数据配置寄存器 */
#define GPIO_PORTB_DIR            (REGW(GPIO_BASE+0X01C))       /* B组端口输入输出方向配置寄存器 */
#define GPIO_PORTB_SEL            (REGW(GPIO_BASE+0X020))       /* B组端口通用用途选择配置寄存器 */
#define GPIO_PORTB_DATA           (REGW(GPIO_BASE+0X024))       /* B组端口通用用途数据配置寄存器 */
#define GPIO_PORTC_DIR            (REGW(GPIO_BASE+0X028))       /* C组端口输入输出方向配置寄存器 */
#define GPIO_PORTC_SEL            (REGW(GPIO_BASE+0X02C))       /* C组端口通用用途选择配置寄存器 */
#define GPIO_PORTC_DATA           (REGW(GPIO_BASE+0X030))       /* C组端口通用用途数据配置寄存器 */
#define GPIO_PORTD_DIR            (REGW(GPIO_BASE+0X034))       /* D组端口输入输出方向配置寄存器 */
#define GPIO_PORTD_SEL            (REGW(GPIO_BASE+0X038))       /* D组端口通用用途选择配置寄存器 */
#define GPIO_PORTD_SPECII         (REGW(GPIO_BASE+0X03C))       /* D组端口专用用途2选择配置寄存器 */
#define GPIO_PORTD_DATA           (REGW(GPIO_BASE+0X040))       /* D组端口通用用途数据配置寄存器 */
#define GPIO_PORTE_DIR            (REGW(GPIO_BASE+0X044))       /* E组端口输入输出方向配置寄存器 */
#define GPIO_PORTE_SEL            (REGW(GPIO_BASE+0X048))       /* E组端口通用用途选择配置寄存器 */
#define GPIO_PORTE_DATA           (REGW(GPIO_BASE+0X04C))       /* E组端口通用用途数据配置寄存器 */
#define GPIO_PORTF_DIR            (REGW(GPIO_BASE+0X050))       /* F组端口输入输出方向配置寄存器 */
#define GPIO_PORTF_SEL            (REGW(GPIO_BASE+0X054))       /* F组端口通用用途选择配置寄存器 */
#define GPIO_PORTF_INCTL          (REGW(GPIO_BASE+0X058))       /* F组端口通用用途输入时类型配置寄存器 */
#define GPIO_PORTF_INTRCTL        (REGW(GPIO_BASE+0X05C))       /* F组端口中断触发类型配置寄存器 */
#define GPIO_PORTF_INTRCLR        (REGW(GPIO_BASE+0X060))       /* F组端口通用用途中断清除配置寄存器 */
#define GPIO_PORTF_DATA           (REGW(GPIO_BASE+0X064))       /* F组端口通用用途数据配置寄存器 */
#define GPIO_PORTG_DIR            (REGW(GPIO_BASE+0X068))       /* G组端口输入输出方向配置寄存器 */
#define GPIO_PORTG_SEL            (REGW(GPIO_BASE+0X06C))       /* G组端口通用用途选择配置寄存器 */
#define GPIO_PORTG_DATA           (REGW(GPIO_BASE+0X070))       /* G组端口通用用途数据配置寄存器 */
#define GPIO_PORTH_DIR            (REGW(GPIO_BASE+0X074))       /* H组端口输入输出方向配置寄存器 */
#define GPIO_PORTH_SEL            (REGW(GPIO_BASE+0X078))       /* H组端口通用用途选择配置寄存器 */
#define GPIO_PORTH_DATA           (REGW(GPIO_BASE+0X07C))       /* H组端口通用用途数据配置寄存器 */
#define GPIO_PORTI_DIR            (REGW(GPIO_BASE+0X080))       /* I组端口输入输出方向配置寄存器 */
#define GPIO_PORTI_SEL            (REGW(GPIO_BASE+0X084))       /* I组端口通用用途选择配置寄存器 */
#define GPIO_PORTI_DATA           (REGW(GPIO_BASE+0X088))       /* I组端口通用用途数据配置寄存器 */



#define UTXH0		(*(volatile unsigned char *)(0x10005000))
#define NAND_READ_ID	9
#define NAND_READ_STATUS 10
#endif
