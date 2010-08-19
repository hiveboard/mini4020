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
//               PERIPHERAL ID DEFINITIONS FOR SEP4020 �жϺ�
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
*  INTCģ��
*  ��ַ: 0x10000000
 */

#define INTC_IER                  (REGW(INTC_BASE+0X000))       /* IRQ�ж�����Ĵ��� */
#define INTC_IMR                  (REGW(INTC_BASE+0X008))       /* IRQ�ж����μĴ��� */
#define INTC_IFR                  (REGW(INTC_BASE+0X010))       /* IRQ���ǿ���жϼĴ��� */
#define INTC_IRSR                 (REGW(INTC_BASE+0X018))       /* IRQδ�����ж�״̬�Ĵ��� */
#define INTC_ISR                  (REGW(INTC_BASE+0X020))       /* IRQ�ж�״̬�Ĵ��� */
#define INTC_IMSR                 (REGW(INTC_BASE+0X028))       /* IRQ�����ж�״̬�Ĵ��� */
#define INTC_IFSR                 (REGW(INTC_BASE+0X030))       /* IRQ�ж�����״̬�Ĵ��� */
#define INTC_FIER                 (REGW(INTC_BASE+0X0C0))       /* FIQ�ж�����Ĵ��� */
#define INTC_FIMR                 (REGW(INTC_BASE+0X0C4))       /* FIQ�ж����μĴ��� */
#define INTC_FIFR                 (REGW(INTC_BASE+0X0C8))       /* FIQ���ǿ���жϼĴ��� */
#define INTC_FIRSR                (REGW(INTC_BASE+0X0CC))       /* FIQδ�����ж�״̬�Ĵ��� */
#define INTC_FISR                 (REGW(INTC_BASE+0X0D0))       /* FIQ�ж�״̬�Ĵ��� */
#define INTC_FIFSR                (REGW(INTC_BASE+0X0D4))       /* FIQ�ж�����״̬�Ĵ��� */
#define INTC_IPLR                 (REGW(INTC_BASE+0X0D8))       /* IRQ�ж����ȼ��Ĵ��� */
#define INTC_ICR1                 (REGW(INTC_BASE+0X0DC))       /* IRQ�ڲ��ж����ȼ����ƼĴ���1 */
#define INTC_ICR2                 (REGW(INTC_BASE+0X0E0))       /* IRQ�ڲ��ж����ȼ����ƼĴ���2 */
#define INTC_EXICR1               (REGW(INTC_BASE+0X0E4))       /* IRQ�ⲿ�ж����ȼ����ƼĴ���1 */
#define INTC_EXICR2               (REGW(INTC_BASE+0X0E8))       /* IRQ�ⲿ�ж����ȼ����ƼĴ���2 */

/*
 *  PMUģ��
 *  ��ַ: 0x10001000
 */

#define PMU_PLTR                  (REGW(PMU_BASE+0X000))        /* PLL���ȶ�����ʱ�� */
#define PMU_PMCR                  (REGW(PMU_BASE+0X004))        /* ϵͳ��ʱ��PLL�Ŀ��ƼĴ��� */
#define PMU_PUCR                  (REGW(PMU_BASE+0X008))        /* USBʱ��PLL�Ŀ��ƼĴ��� */
#define PMU_PCSR                  (REGW(PMU_BASE+0X00C))        /* �ڲ�ģ��ʱ��Դ�����Ŀ��ƼĴ��� */
#define PMU_PDSLOW                (REGW(PMU_BASE+0X010))        /* SLOW״̬��ʱ�ӵķ�Ƶ���� */
#define PMU_PMDR                  (REGW(PMU_BASE+0X014))        /* оƬ����ģʽ�Ĵ��� */
#define PMU_RCTR                  (REGW(PMU_BASE+0X018))        /* Reset���ƼĴ��� */
#define PMU_CLRWAKUP              (REGW(PMU_BASE+0X01C))        /* WakeUp����Ĵ��� */
#define PMU_PDAPB                 (REGW(PMU_BASE+0X020))        /* ����δ��(������չ����ʱ�ӷ�Ƶ�ȼĴ���) */
#define PMU_PLLTS                 (REGW(PMU_BASE+0X024))        /* PLLʱ���ⲿ����������ƼĴ��� */

/*
 *  RTCģ��
 *  ��ַ: 0x10002000
 */

#define RTC_STA_YMD               (REGW(RTC_BASE+0X000))        /* ��, ��, �ռ����Ĵ��� */
#define RTC_STA_HMS               (REGW(RTC_BASE+0X004))        /* Сʱ, ����, ��Ĵ��� */
#define RTC_ALARM_ALL             (REGW(RTC_BASE+0X008))        /* ��ʱ��, ��, ʱ, �ּĴ��� */
#define RTC_CTR                   (REGW(RTC_BASE+0X00C))        /* ���ƼĴ��� */
#define RTC_INT_EN                (REGW(RTC_BASE+0X010))        /* �ж�ʹ�ܼĴ��� */
#define RTC_INT_STS               (REGW(RTC_BASE+0X014))        /* �ж�״̬�Ĵ��� */
#define RTC_SAMP                  (REGW(RTC_BASE+0X018))        /* �������ڼĴ��� */
#define RTC_WD_CNT                (REGW(RTC_BASE+0X01C))        /* Watch-Dog����ֵ�Ĵ��� */
#define RTC_WD_SEV                (REGW(RTC_BASE+0X020))        /* Watch-Dog����Ĵ��� */ 
#define RTC_CONFIG_CHECK          (REGW(RTC_BASE+0X024))        /* ����ʱ��ȷ�ϼĴ��� (������ʱ��֮ǰ��д0xaaaaaaaa) */
#define RTC_SOFTRESET             (REGW(RTC_BASE+0X028))        /* �����λ���ƼĴ���, 4020�б�ȥ���� */
#define RTC_KEY0                  (REGW(RTC_BASE+0X02C))        /* ��Կ�Ĵ���0, 4020��ֻ����һ���Ĵ��� */
#define RTC_KEY1                  (REGW(RTC_BASE+0X030))        /* ��Կ�Ĵ���1 */
#define RTC_KEY2                  (REGW(RTC_BASE+0X034))        /* ��Կ�Ĵ���2 */
#define RTC_KEY3                  (REGW(RTC_BASE+0X038))        /* ��Կ�Ĵ���3 */
#define RTC_KEY4                  (REGW(RTC_BASE+0X03C))        /* ��Կ�Ĵ���4 */
#define RTC_KEY5                  (REGW(RTC_BASE+0X040))        /* ��Կ�Ĵ���5 */

/*
 *  TIMERģ��
 *  ��ַ: 0x10003000
 */

#define TIMER_T1LCR               (REGW(TIMER_BASE+0X000))      /* ͨ��1���ؼ����Ĵ��� */
#define TIMER_T1CCR               (REGW(TIMER_BASE+0X004))      /* ͨ��1��ǰ����ֵ�Ĵ��� */
#define TIMER_T1CR                (REGW(TIMER_BASE+0X008))      /* ͨ��1���ƼĴ��� */
#define TIMER_T1ISCR              (REGW(TIMER_BASE+0X00C))      /* ͨ��1�ж�״̬����Ĵ��� */
#define TIMER_T1IMSR              (REGW(TIMER_BASE+0X010))      /* ͨ��1�ж�����״̬�Ĵ��� */
#define TIMER_T2LCR               (REGW(TIMER_BASE+0X020))      /* ͨ��2���ؼ����Ĵ��� */
#define TIMER_T2CCR               (REGW(TIMER_BASE+0X024))      /* ͨ��2��ǰ����ֵ�Ĵ��� */
#define TIMER_T2CR                (REGW(TIMER_BASE+0X028))      /* ͨ��2���ƼĴ��� */
#define TIMER_T2ISCR              (REGW(TIMER_BASE+0X02C))      /* ͨ��2�ж�״̬����Ĵ��� */
#define TIMER_T2IMSR              (REGW(TIMER_BASE+0X030))      /* ͨ��2�ж�����״̬�Ĵ��� */
#define TIMER_T3LCR               (REGW(TIMER_BASE+0X040))      /* ͨ��3���ؼ����Ĵ��� */
#define TIMER_T3CCR               (REGW(TIMER_BASE+0X044))      /* ͨ��3��ǰ����ֵ�Ĵ��� */
#define TIMER_T3CR                (REGW(TIMER_BASE+0X048))      /* ͨ��3���ƼĴ��� */
#define TIMER_T3ISCR              (REGW(TIMER_BASE+0X04C))      /* ͨ��3�ж�״̬����Ĵ��� */
#define TIMER_T3IMSR              (REGW(TIMER_BASE+0X050))      /* ͨ��3�ж�����״̬�Ĵ��� */
#define TIMER_T3CAPR              (REGW(TIMER_BASE+0X054))      /* ͨ��3����Ĵ��� */
#define TIMER_T4LCR               (REGW(TIMER_BASE+0X060))      /* ͨ��4���ؼ����Ĵ��� */
#define TIMER_T4CCR               (REGW(TIMER_BASE+0X064))      /* ͨ��4��ǰ����ֵ�Ĵ��� */
#define TIMER_T4CR                (REGW(TIMER_BASE+0X068))      /* ͨ��4���ƼĴ��� */
#define TIMER_T4ISCR              (REGW(TIMER_BASE+0X06C))      /* ͨ��4�ж�״̬����Ĵ��� */
#define TIMER_T4IMSR              (REGW(TIMER_BASE+0X070))      /* ͨ��4�ж�����״̬�Ĵ��� */
#define TIMER_T4CAPR              (REGW(TIMER_BASE+0X074))      /* ͨ��4����Ĵ��� */
#define TIMER_T5LCR               (REGW(TIMER_BASE+0X080))      /* ͨ��5���ؼ����Ĵ��� */
#define TIMER_T5CCR               (REGW(TIMER_BASE+0X084))      /* ͨ��5��ǰ����ֵ�Ĵ��� */
#define TIMER_T5CR                (REGW(TIMER_BASE+0X088))      /* ͨ��5���ƼĴ��� */
#define TIMER_T5ISCR              (REGW(TIMER_BASE+0X08C))      /* ͨ��5�ж�״̬����Ĵ��� */
#define TIMER_T5IMSR              (REGW(TIMER_BASE+0X090))      /* ͨ��5�ж�����״̬�Ĵ��� */
#define TIMER_T5CAPR              (REGW(TIMER_BASE+0X094))      /* ͨ��5����Ĵ��� */
#define TIMER_T6LCR               (REGW(TIMER_BASE+0X0A0))      /* ͨ��6���ؼ����Ĵ��� */
#define TIMER_T6CCR               (REGW(TIMER_BASE+0X0A4))      /* ͨ��6��ǰ����ֵ�Ĵ��� */
#define TIMER_T6CR                (REGW(TIMER_BASE+0X0A8))      /* ͨ��6���ƼĴ��� */
#define TIMER_T6ISCR              (REGW(TIMER_BASE+0X0AC))      /* ͨ��6�ж�״̬����Ĵ��� */
#define TIMER_T6IMSR              (REGW(TIMER_BASE+0X0B0))      /* ͨ��6�ж�����״̬�Ĵ��� */
#define TIMER_T6CAPR              (REGW(TIMER_BASE+0X0B4))      /* ͨ��6����Ĵ��� */
#define TIMER_T7LCR               (REGW(TIMER_BASE+0X0C0))      /* ͨ��7���ؼ����Ĵ��� */
#define TIMER_T7CCR               (REGW(TIMER_BASE+0X0C4))      /* ͨ��7��ǰ����ֵ�Ĵ��� */
#define TIMER_T7CR                (REGW(TIMER_BASE+0X0C8))      /* ͨ��7���ƼĴ��� */
#define TIMER_T7ISCR              (REGW(TIMER_BASE+0X0CC))      /* ͨ��7�ж�״̬����Ĵ��� */
#define TIMER_T7IMSR              (REGW(TIMER_BASE+0X0D0))      /* ͨ��7�ж�����״̬�Ĵ��� */
#define TIMER_T8LCR               (REGW(TIMER_BASE+0X0E0))      /* ͨ��8���ؼ����Ĵ��� */
#define TIMER_T8CCR               (REGW(TIMER_BASE+0X0E4))      /* ͨ��8��ǰ����ֵ�Ĵ��� */
#define TIMER_T8CR                (REGW(TIMER_BASE+0X0E8))      /* ͨ��8���ƼĴ��� */
#define TIMER_T8ISCR              (REGW(TIMER_BASE+0X0EC))      /* ͨ��8�ж�״̬����Ĵ��� */
#define TIMER_T8IMSR              (REGW(TIMER_BASE+0X0F0))      /* ͨ��8�ж�����״̬�Ĵ��� */
#define TIMER_T9LCR               (REGW(TIMER_BASE+0X100))      /* ͨ��9���ؼ����Ĵ��� */
#define TIMER_T9CCR               (REGW(TIMER_BASE+0X104))      /* ͨ��9��ǰ����ֵ�Ĵ��� */
#define TIMER_T9CR                (REGW(TIMER_BASE+0X108))      /* ͨ��9���ƼĴ��� */
#define TIMER_T9ISCR              (REGW(TIMER_BASE+0X10C))      /* ͨ��9�ж�״̬����Ĵ��� */
#define TIMER_T9IMSR              (REGW(TIMER_BASE+0X110))      /* ͨ��9�ж�����״̬�Ĵ��� */
#define TIMER_T10LCR              (REGW(TIMER_BASE+0X120))      /* ͨ��10���ؼ����Ĵ��� */
#define TIMER_T10CCR              (REGW(TIMER_BASE+0X124))      /* ͨ��10��ǰ����ֵ�Ĵ��� */
#define TIMER_T10CR               (REGW(TIMER_BASE+0X128))      /* ͨ��10���ƼĴ��� */
#define TIMER_T10ISCR             (REGW(TIMER_BASE+0X12C))      /* ͨ��10�ж�״̬����Ĵ��� */
#define TIMER_T10IMSR             (REGW(TIMER_BASE+0X130))      /* ͨ��10�ж�����״̬�Ĵ��� */
#define TIMER_TIMSR               (REGW(TIMER_BASE+0X140))      /* TIMER�ж�����״̬�Ĵ��� */
#define TIMER_TISCR               (REGW(TIMER_BASE+0X144))      /* TIMER�ж�״̬����Ĵ��� */
#define TIMER_TISR                (REGW(TIMER_BASE+0X148))      /* TIMER�ж�״̬�Ĵ��� */

/*
 *  UART0ģ��
 *  ��ַ: 0x10005000
 */

#define UART0_DLBL                (REGW(UART0_BASE+0X000))      /* ���������õͰ�λ�Ĵ��� */
#define UART0_RXFIFO              (REGB(UART0_BASE+0X000))      /* ����FIFO */
#define UART0_TXFIFO              (REGB(UART0_BASE+0X000))      /* ����FIFO */
#define UART0_DLBH                (REGW(UART0_BASE+0X004))      /* ���������ø߰�λ�Ĵ��� */
#define UART0_IER                 (REGW(UART0_BASE+0X004))      /* �ж�ʹ�ܼĴ��� */
#define UART0_IIR                 (REGW(UART0_BASE+0X008))      /* �ж�ʶ��Ĵ��� */
#define UART0_FCR                 (REGW(UART0_BASE+0X008))      /* FIFO���ƼĴ��� */
#define UART0_LCR                 (REGW(UART0_BASE+0X00C))      /* �п��ƼĴ��� */
#define UART0_MCR                 (REGW(UART0_BASE+0X010))      /* Modem���ƼĴ��� */
#define UART0_LSR                 (REGW(UART0_BASE+0X014))      /* ��״̬�Ĵ��� */
#define UART0_MSR                 (REGW(UART0_BASE+0X018))      /* Modem״̬�Ĵ��� */

/*
 *  EMIģ��
 *  ��ַ: 0x11000000
 */

#define EMI_CSACONF               (REGW(EMI_BASE+0X000))        /* CSA�������üĴ��� */
#define EMI_CSBCONF               (REGW(EMI_BASE+0X004))        /* CSB�������üĴ��� */
#define EMI_CSCCONF               (REGW(EMI_BASE+0X008))        /* CSC�������üĴ��� */
#define EMI_CSDCONF               (REGW(EMI_BASE+0X00C))        /* CSD�������üĴ��� */
#define EMI_CSECONF               (REGW(EMI_BASE+0X010))        /* CSE�������üĴ��� */
#define EMI_CSFCONF               (REGW(EMI_BASE+0X014))        /* CSF�������üĴ��� */
#define EMI_SDCONF1               (REGW(EMI_BASE+0X018))        /* SDRAMʱ�����üĴ���1 */
#define EMI_SDCONF2               (REGW(EMI_BASE+0X01C))        /* SDRAMʱ�����üĴ���2, SDRAM��ʼ���õ���������Ϣ */
#define EMI_REMAPCONF             (REGW(EMI_BASE+0X020))        /* Ƭѡ�ռ估��ַӳ��REMAP���üĴ��� */
#define EMI_NAND_ADDR1            (REGW(EMI_BASE+0X100))        /* NAND FLASH�ĵ�ַ�Ĵ���1 */
#define EMI_NAND_COM              (REGW(EMI_BASE+0X104))        /* NAND FLASH�Ŀ����ּĴ��� */
#define EMI_NAND_STA              (REGW(EMI_BASE+0X10C))        /* NAND FLASH��״̬�Ĵ��� */
#define EMI_ERR_ADDR1             (REGW(EMI_BASE+0X110))        /* ����������ĵ�ַ�Ĵ���1 */
#define EMI_ERR_ADDR2             (REGW(EMI_BASE+0X114))        /* ����������ĵ�ַ�Ĵ���2 */
#define EMI_NAND_CONF1            (REGW(EMI_BASE+0X118))        /* NAND FLASH������������1 */
#define EMI_NAND_INTR             (REGW(EMI_BASE+0X11C))        /* NAND FLASH�жϼĴ��� */
#define EMI_NAND_ECC              (REGW(EMI_BASE+0X120))        /* ECCУ����ɼĴ��� */
#define EMI_NAND_IDLE             (REGW(EMI_BASE+0X124))        /* NAND FLASH���мĴ��� */
#define EMI_NAND_CONF2            (REGW(EMI_BASE+0X128))        /* NAND FLASH������������2 */
#define EMI_NAND_ADDR2            (REGW(EMI_BASE+0X12C))        /* NAND FLASH�ĵ�ַ�Ĵ���2 */
#define EMI_NAND_ID		    (REGW(EMI_BASE+0X130))
#define EMI_NAND_DATA             (REGW(EMI_BASE+0X200))        /* NAND FLASH�����ݼĴ��� */


#define EMI_NAND_ADDR1_RAW	    (EMI_BASE+0X100)        /* NAND FLASH�ĵ�ַ�Ĵ���1 */
#define EMI_NAND_COM_RAW	    (EMI_BASE+0X104)        /* NAND FLASH�Ŀ����ּĴ��� */
#define EMI_NAND_STA_RAW	    (EMI_BASE+0X10C)        /* NAND FLASH��״̬�Ĵ��� */
#define EMI_ERR_ADDR1_RAW	    (EMI_BASE+0X110)        /* ����������ĵ�ַ�Ĵ���1 */
#define EMI_ERR_ADDR2_RAW	    (EMI_BASE+0X114)        /* ����������ĵ�ַ�Ĵ���2 */
#define EMI_NAND_CONF1_RAW	    (EMI_BASE+0X118)        /* NAND FLASH������������1 */
#define EMI_NAND_INTR_RAW	    (EMI_BASE+0X11C)        /* NAND FLASH�жϼĴ��� */
#define EMI_NAND_ECC_RAW	    (EMI_BASE+0X120)        /* ECCУ����ɼĴ��� */
#define EMI_NAND_IDLE_RAW	    (EMI_BASE+0X124)        /* NAND FLASH���мĴ��� */
#define EMI_NAND_CONF2_RAW	    (EMI_BASE+0X128)        /* NAND FLASH������������2 */
#define EMI_NAND_ADDR2_RAW	    (EMI_BASE+0X12C)        /* NAND FLASH�ĵ�ַ�Ĵ���2 */
#define EMI_NAND_ID_RAW		    (EMI_BASE+0X130)
#define EMI_NAND_DATA_RAW         (EMI_BASE+0X200)        /* NAND FLASH�����ݼĴ��� */
/*
 *  DMACģ��
 *  ��ַ: 0x11001000
 */

#define DMAC_INTSTATUS            (REGW(DMAC_BASE+0X020))       /* DAMC�ж�״̬�Ĵ����� */
#define DMAC_INTTCSTATUS          (REGW(DMAC_BASE+0X050))       /* DMAC��������ж�״̬�Ĵ��� */
#define DMAC_INTTCCLEAR           (REGW(DMAC_BASE+0X060))       /* DMAC��������ж�״̬����Ĵ��� */
#define DMAC_INTERRORSTATUS       (REGW(DMAC_BASE+0X080))       /* DMAC��������ж�״̬�Ĵ��� */
#define DMAC_INTERRCLR            (REGW(DMAC_BASE+0X090))       /* DMAC��������ж�״̬����Ĵ��� */
#define DMAC_ENBLDCHNS            (REGW(DMAC_BASE+0X0B0))       /* DMACͨ��ʹ��״̬�Ĵ��� */
#define DMAC_C0SRCADDR            (REGW(DMAC_BASE+0X000))      /* DMAC��0Դ��ַ�Ĵ��� */
#define DMAC_C0DESTADD            (REGW(DMAC_BASE+0X004))       /* DMAC��0Ŀ�ĵ�ַ�Ĵ��� */
#define DMAC_C0CONTROL            (REGW(DMAC_BASE+0X00C))       /* DMAC��0���ƼĴ��� */
#define DMAC_C0CONFIGURATION      (REGW(DMAC_BASE+0X010))       /* DMAC��0���üĴ��� */
#define DMAC_C0DESCRIPTOR         (REGW(DMAC_BASE+0X01C))       /* DMAC��0�����ַ�Ĵ��� */
#define DMAC_C1SRCADDR            (REGW(DMAC_BASE+0X100))       /* DMAC��1Դ��ַ�Ĵ��� */
#define DMAC_C1DESTADDR           (REGW(DMAC_BASE+0X104))       /* DMAC��1Ŀ�ĵ�ַ�Ĵ��� */
#define DMAC_C1CONTROL            (REGW(DMAC_BASE+0X10C))       /* DMAC��1���ƼĴ��� */
#define DMAC_C1CONFIGURATION      (REGW(DMAC_BASE+0X110))       /* DMAC��1���üĴ��� */
#define DMAC_C1DESCRIPTOR         (REGW(DMAC_BASE+0X114))       /* DMAC��1�����ַ�Ĵ��� */
#define DMAC_C2SRCADDR            (REGW(DMAC_BASE+0X200))       /* DMAC��2Դ��ַ�Ĵ��� */
#define DMAC_C2DESTADDR           (REGW(DMAC_BASE+0X204))       /* DMAC��2Ŀ�ĵ�ַ�Ĵ��� */
#define DMAC_C2CONTROL            (REGW(DMAC_BASE+0X20C))       /* DMAC��2���ƼĴ��� */
#define DMAC_C2CONFIGURATION      (REGW(DMAC_BASE+0X210))       /* DMAC��2���üĴ��� */
#define DMAC_C2DESCRIPTOR         (REGW(DMAC_BASE+0X214))       /* DMAC��2�����ַ�Ĵ��� */
#define DMAC_C3SRCADDR            (REGW(DMAC_BASE+0X300))       /* DMAC��3Դ��ַ�Ĵ��� */
#define DMAC_C3DESTADDR           (REGW(DMAC_BASE+0X304))       /* DMAC��3Ŀ�ĵ�ַ�Ĵ��� */
#define DMAC_C3CONTROL            (REGW(DMAC_BASE+0X30C))       /* DMAC��3���ƼĴ��� */
#define DMAC_C3CONFIGURATION      (REGW(DMAC_BASE+0X310))       /* DMAC��3���üĴ��� */
#define DMAC_C3DESCRIPTOR         (REGW(DMAC_BASE+0X314))       /* DMAC��3�����ַ�Ĵ��� */
#define DMAC_C4SRCADDR            (REGW(DMAC_BASE+0X400))       /* DMAC��4Դ��ַ�Ĵ��� */
#define DMAC_C4DESTADDR           (REGW(DMAC_BASE+0X404))       /* DMAC��4Ŀ�ĵ�ַ�Ĵ��� */
#define DMAC_C4CONTROL            (REGW(DMAC_BASE+0X40C))       /* DMAC��4���ƼĴ��� */
#define DMAC_C4CONFIGURATION      (REGW(DMAC_BASE+0X410))       /* DMAC��4���üĴ��� */
#define DMAC_C4DESCRIPTOR         (REGW(DMAC_BASE+0X414))       /* DMAC��4�����ַ�Ĵ��� */
#define DMAC_C5SRCADDR            (REGW(DMAC_BASE+0X500))       /* DMAC��5Դ��ַ�Ĵ��� */
#define DMAC_C5DESTADDR           (REGW(DMAC_BASE+0X504))       /* DMAC��5Ŀ�ĵ�ַ�Ĵ��� */
#define DMAC_C5CONTROL            (REGW(DMAC_BASE+0X50C))       /* DMAC��5���ƼĴ��� */
#define DMAC_C5CONFIGURATION      (REGW(DMAC_BASE+0X510))       /* DMAC��5���üĴ��� */
#define DMAC_C5DESCRIPTOR         (REGW(DMAC_BASE+0X514))       /* DMAC��5�����ַ�Ĵ��� */

// ========== Register definition for EMACB peripheral ========== 
#define MAC_CTRL                  (REGW(MAC_BASE+0X000))        /* MAC���ƼĴ��� */
#define MAC_INTSRC                (REGW(MAC_BASE+0X004))        /* MAC�ж�Դ�Ĵ��� */
#define MAC_INTMASK               (REGW(MAC_BASE+0X008))        /* MAC�ж����μĴ��� */
#define MAC_IPGT                  (REGW(MAC_BASE+0X00C))        /* ����֡����Ĵ��� */
#define MAC_IPGR1                 (REGW(MAC_BASE+0X010))        /* �ȴ����ڼĴ��� */
#define MAC_IPGR2                 (REGW(MAC_BASE+0X014))        /* �ȴ����ڼĴ��� */
#define MAC_PACKETLEN             (REGW(MAC_BASE+0X018))        /* ֡���ȼĴ��� */
#define MAC_COLLCONF              (REGW(MAC_BASE+0X01C))        /* ��ײ�ط��Ĵ��� */
#define MAC_TXBD_NUM              (REGW(MAC_BASE+0X020))        /* �����������Ĵ��� */
#define MAC_FLOWCTRL              (REGW(MAC_BASE+0X024))        /* ���ؼĴ��� */
#define MAC_MII_CTRL              (REGW(MAC_BASE+0X028))        /* PHY���ƼĴ��� */
#define MAC_MII_CMD               (REGW(MAC_BASE+0X02C))        /* PHY����Ĵ��� */
#define MAC_MII_ADDRESS           (REGW(MAC_BASE+0X030))        /* PHY��ַ�Ĵ��� */
#define MAC_MII_TXDATA            (REGW(MAC_BASE+0X034))        /* PHYд���ݼĴ��� */
#define MAC_MII_RXDATA            (REGW(MAC_BASE+0X038))        /* PHY�����ݼĴ��� */
#define MAC_MII_STATUS            (REGW(MAC_BASE+0X03C))        /* PHY״̬�Ĵ��� */
#define MAC_ADDR0                 (REGW(MAC_BASE+0X040))        /* MAC��ַ�Ĵ��� */
#define MAC_ADDR1                 (REGW(MAC_BASE+0X044))        /* MAC��ַ�Ĵ��� */
#define MAC_HASH0                 (REGW(MAC_BASE+0X048))        /* MAC HASH�Ĵ��� */
#define MAC_HASH1                 (REGW(MAC_BASE+0X04C))        /* MAC HASH�Ĵ��� */
#define MAC_TXPAUSE              (REGW(MAC_BASE+0X050))        /* MAC����֡�Ĵ��� */
#define MAC_TX_BD                (MAC_BASE+0x400)      /*����������*/
#define MAC_RX_BD                (MAC_BASE+0x600)       /*����������*/

/*
*  GPIOģ��
*  ��ַ: 0x1000B000
 */

#define GPIO_DBCLK_DIV            (REGW(GPIO_BASE+0X000))       /* ȥë�̲���ʱ�ӷ�Ƶ�����üĴ��� */
#define GPIO_PORTA_DIR            (REGW(GPIO_BASE+0X004))       /* A��˿���������������üĴ��� */
#define GPIO_PORTA_SEL            (REGW(GPIO_BASE+0X008))       /* A��˿�ͨ����;ѡ�����üĴ��� */
#define GPIO_PORTA_INCTL          (REGW(GPIO_BASE+0X00C))       /* A��˿�ͨ����;����ʱ�������üĴ��� */
#define GPIO_PORTA_INTRCTL        (REGW(GPIO_BASE+0X010))       /* A��˿��жϴ����������üĴ��� */
#define GPIO_PORTA_INTRCLR        (REGW(GPIO_BASE+0X014))       /* A��˿�ͨ����;�ж�������üĴ��� */
#define GPIO_PORTA_DATA           (REGW(GPIO_BASE+0X018))       /* A��˿�ͨ����;�������üĴ��� */
#define GPIO_PORTB_DIR            (REGW(GPIO_BASE+0X01C))       /* B��˿���������������üĴ��� */
#define GPIO_PORTB_SEL            (REGW(GPIO_BASE+0X020))       /* B��˿�ͨ����;ѡ�����üĴ��� */
#define GPIO_PORTB_DATA           (REGW(GPIO_BASE+0X024))       /* B��˿�ͨ����;�������üĴ��� */
#define GPIO_PORTC_DIR            (REGW(GPIO_BASE+0X028))       /* C��˿���������������üĴ��� */
#define GPIO_PORTC_SEL            (REGW(GPIO_BASE+0X02C))       /* C��˿�ͨ����;ѡ�����üĴ��� */
#define GPIO_PORTC_DATA           (REGW(GPIO_BASE+0X030))       /* C��˿�ͨ����;�������üĴ��� */
#define GPIO_PORTD_DIR            (REGW(GPIO_BASE+0X034))       /* D��˿���������������üĴ��� */
#define GPIO_PORTD_SEL            (REGW(GPIO_BASE+0X038))       /* D��˿�ͨ����;ѡ�����üĴ��� */
#define GPIO_PORTD_SPECII         (REGW(GPIO_BASE+0X03C))       /* D��˿�ר����;2ѡ�����üĴ��� */
#define GPIO_PORTD_DATA           (REGW(GPIO_BASE+0X040))       /* D��˿�ͨ����;�������üĴ��� */
#define GPIO_PORTE_DIR            (REGW(GPIO_BASE+0X044))       /* E��˿���������������üĴ��� */
#define GPIO_PORTE_SEL            (REGW(GPIO_BASE+0X048))       /* E��˿�ͨ����;ѡ�����üĴ��� */
#define GPIO_PORTE_DATA           (REGW(GPIO_BASE+0X04C))       /* E��˿�ͨ����;�������üĴ��� */
#define GPIO_PORTF_DIR            (REGW(GPIO_BASE+0X050))       /* F��˿���������������üĴ��� */
#define GPIO_PORTF_SEL            (REGW(GPIO_BASE+0X054))       /* F��˿�ͨ����;ѡ�����üĴ��� */
#define GPIO_PORTF_INCTL          (REGW(GPIO_BASE+0X058))       /* F��˿�ͨ����;����ʱ�������üĴ��� */
#define GPIO_PORTF_INTRCTL        (REGW(GPIO_BASE+0X05C))       /* F��˿��жϴ����������üĴ��� */
#define GPIO_PORTF_INTRCLR        (REGW(GPIO_BASE+0X060))       /* F��˿�ͨ����;�ж�������üĴ��� */
#define GPIO_PORTF_DATA           (REGW(GPIO_BASE+0X064))       /* F��˿�ͨ����;�������üĴ��� */
#define GPIO_PORTG_DIR            (REGW(GPIO_BASE+0X068))       /* G��˿���������������üĴ��� */
#define GPIO_PORTG_SEL            (REGW(GPIO_BASE+0X06C))       /* G��˿�ͨ����;ѡ�����üĴ��� */
#define GPIO_PORTG_DATA           (REGW(GPIO_BASE+0X070))       /* G��˿�ͨ����;�������üĴ��� */
#define GPIO_PORTH_DIR            (REGW(GPIO_BASE+0X074))       /* H��˿���������������üĴ��� */
#define GPIO_PORTH_SEL            (REGW(GPIO_BASE+0X078))       /* H��˿�ͨ����;ѡ�����üĴ��� */
#define GPIO_PORTH_DATA           (REGW(GPIO_BASE+0X07C))       /* H��˿�ͨ����;�������üĴ��� */
#define GPIO_PORTI_DIR            (REGW(GPIO_BASE+0X080))       /* I��˿���������������üĴ��� */
#define GPIO_PORTI_SEL            (REGW(GPIO_BASE+0X084))       /* I��˿�ͨ����;ѡ�����üĴ��� */
#define GPIO_PORTI_DATA           (REGW(GPIO_BASE+0X088))       /* I��˿�ͨ����;�������üĴ��� */



#define UTXH0		(*(volatile unsigned char *)(0x10005000))
#define NAND_READ_ID	9
#define NAND_READ_STATUS 10
#endif
