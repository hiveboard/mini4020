/**
 * @file	s1r72v05.h
 * @brief	S1R72V05 Driver
 * @date	2007/04/11
 * Copyright (C)SEIKO EPSON CORPORATION 2003-2006. All rights reserved.
 */
/*
 *  rcXXX = Register 8Bit Access
 *  rsXXX = Register 16Bit Access
 *
 */


#ifndef __S1R72V05_USB_H__
#define	__S1R72V05_USB_H__

#define BIT(x)		(1 << x)

/*==========================================================================================================/
/ Register Access Define   /
/==========================================================================================================*/

#define rcMainIntStat				(0x00)	/* Main Interrupt Status */
#define rcMainIntEnb				(0x10)	/* Main Interrupt Enable */
	#define DevInt			BIT(7)
	#define HostInt			BIT(6)
	#define CPUInt			BIT(5)
	#define IDEInt			BIT(4)
	#define MediaInt		BIT(3)
	#define FinishedPM		BIT(0)

#define rcDeviceIntStat				(0x01)	/* EPr Interrupt Status */
	#define D_VBUS_Changed	BIT(7)
	#define D_SIE_IntStat	BIT(5)
	#define D_BulkIntStat	BIT(4)
	#define D_RcvEP0SETUP	BIT(3)
	#define D_FIFO_IntStat	BIT(2)
	#define D_EP0IntStat	BIT(1)
	#define D_EPrIntStat	BIT(0)

 #define rcDeviceIntEnb				(0x11)	/* EPr Interrupt Enable */

#define rcHostIntStat				(0x02)	/* SIE Interrupt Status */
#define rcHostIntEnb				(0x12)	/* SIE Interrupt Enable */
	#define VBUSErr			BIT(7)
	#define LineChg			BIT(6)
	#define H_SIE_Int1		BIT(5)
	#define H_SIE_Int0		BIT(4)
	#define H_FrameInt		BIT(3)
	#define H_FIFO_Int		BIT(2)
	#define H_CH0Int		BIT(1)
	#define H_CHrInt		BIT(0)

#define rcCPU_IntStat				(0x03)	/* CPU Interrupt Status */
#define rcCPU_IntEnb				(0x13)	/* CPU Interrupt Enable */

#define rcIDE_IntStat				(0x04)	/* IDE Interrupt Status */
#define rcIDE_IntEnb				(0x14)	/* IDE Interrupt Enable */

#define rcMediaFIFO_IntStat			(0x05)	/* Media FIFO Interrupt Status */
#define rcMediaFIFO_IntEnb			(0x15)	/* Media FIFO Interrupt Enable */

#define rcRevisionNum				(0x20)	/* Revision Number */
	#define RevisionNum		0x50
#define rcChipReset					(0x21)	/* Chip Reset */
	#define AllReset		BIT(0)

#define rcPM_Control_0				(0x22)	/* Power Management Control 0 */
	#define GoSleep			BIT(7)
	#define GoSnooze		BIT(6)
	#define GoActive60		BIT(5)
	#define GoActDevice		BIT(4)
	#define GoActHost		BIT(3)

	#define	GoActive	GoActDevice

#define rcPM_Control_1				(0x23)	/* Power Management Control 1 */
	#define	PM_State_Mask			(0x0F)
	#define S1R72_PM_State_SLEEP	(0x00)	/* PM_State SLEEP */
	#define S1R72_PM_State_SNOOZE	(0x01)	/* PM_State SNOOZE */
	#define S1R72_PM_State_ACTIVE60	(0x03)	/* PM_State ACTIVE60 */
	#define S1R72_PM_State_ACT_DEVICE	(0x07)	/* PM_State ACT_DEVICE */
	#define S1R72_PM_State_ACT_HOST	(0x0B)	/* PM_State ACT_HOST */

#define rsWakeupTim					(0x24)	/* Wake Up Timer */
#define rcWakeupTim_H				(0x24)	/* Wake Up Timer High */
#define rcWakeupTim_L				(0x25)	/* Wake Up Timer Low */
#define rcH_USB_Control				(0x26)	/* Host USB Control */
	#define VBUS_Enb		BIT(7)

#define rcH_XcvrControl				(0x27)	/* Host Xcvr Control */
#define rcD_USB_Status				(0x28)	/* Device USB Status */
        #define S1R72_VBUS                      BIT(7)
        #define S1R72_FSxHS                     BIT(6)
        #define S1R72_Status_FS                 (0x40)
        #define S1R72_Status_HS                 (0x00)
	#define S1R72_LineState			(0x03)
	#define S1R72_LineState_SE0			(0x00)
	#define S1R72_LineState_J			(0x01)
	#define S1R72_LineState_K			(0x02)
	#define S1R72_LineState_SE1			(0x03)
#define rcH_USB_Status				(0x29)	/* Host USB Status */

#define rsFIFO_Rd					(0x30)	/* FIFO Read */
#define rcFIFO_Rd_0					(0x30)	/* FIFO Read High */
#define rcFIFO_Rd_1					(0x31)	/* FIFO Read Low */

#define rsFIFO_Wr					(0x32)	/* FIFO Write */
#define rcFIFO_Wr_0					(0x32)	/* FIFO Write High */
#define rcFIFO_Wr_1					(0x33)	/* FIFO Write Low */

#define rsFIFO_RdRemain				(0x34)	/* FIFO Read Remain High */
#define rcFIFO_RdRemain_H			(0x34)	/* FIFO Read Remain High */
#define rcFIFO_RdRemain_L			(0x35)	/* FIFO Read Remain Low */
	#define FIFO_RdRemain			(0x1FFF)	/* FIFO Read Remain mask */
	#define RdRemainValid	BIT(15)			/* FIFO Read Remain valid */
#define rsFIFO_WrRemain				(0x36)	/* FIFO Write Remain High */
#define rcFIFO_WrRemain_H			(0x36)	/* FIFO Write Remain High */
#define rcFIFO_WrRemain_L			(0x37)	/* FIFO Write Remain Low */
#define rcFIFO_ByteRd				(0x38)	/* FIFO Byte Read */


#define rcRAM_RdAdrs_H				(0x40)	/* RAM Read Address High */
#define rcRAM_RdAdrs_L				(0x41)	/* RAM Read Address Low */
#define rcRAM_RdControl				(0x42)	/* RAM Read Control */
#define rcRAM_RdCount				(0x43)	/* RAM Read Counter */
#define rcRAM_WrAdrs_H				(0x44)	/* RAM Write Address High */
#define rcRAM_WrAdrs_L				(0x45)	/* RAM Write Address Low */
#define rcRAM_WrDoor_0				(0x46)	/* RAM Write Door High */
#define rcRAM_WrDoor_1				(0x47)	/* RAM Write Door Low */
#define rcMediaFIFO_Control			(0x48)	/* Media FIFO Clear */
#define rcClrAllMediaFIFO_Join		(0x49)	/* Clear All Media FIFO Join */
#define rcMediaFIFO_Join			(0x4A)	/* Media FIFO Join */

#define rcRAM_Rd_00					(0x50)	/* RAM Read 00 */
#define rcRAM_Rd_01					(0x51)	/* RAM Read 01 */
#define rcRAM_Rd_02					(0x52)	/* RAM Read 02 */
#define rcRAM_Rd_03					(0x53)	/* RAM Read 03 */
#define rcRAM_Rd_04					(0x54)	/* RAM Read 04 */
#define rcRAM_Rd_05					(0x55)	/* RAM Read 05 */
#define rcRAM_Rd_06					(0x56)	/* RAM Read 06 */
#define rcRAM_Rd_07					(0x57)	/* RAM Read 07 */
#define rcRAM_Rd_08					(0x58)	/* RAM Read 08 */
#define rcRAM_Rd_09					(0x59)	/* RAM Read 09 */
#define rcRAM_Rd_0A					(0x5A)	/* RAM Read 0A */
#define rcRAM_Rd_0B					(0x5B)	/* RAM Read 0B */
#define rcRAM_Rd_0C					(0x5C)	/* RAM Read 0C */
#define rcRAM_Rd_0D					(0x5D)	/* RAM Read 0D */
#define rcRAM_Rd_0E					(0x5E)	/* RAM Read 0E */
#define rcRAM_Rd_0F					(0x5F)	/* RAM Read 0F */

#define rcRAM_Rd_10				(0x60)	/* RAM Read 10 */
#define rcRAM_Rd_11				(0x61)	/* RAM Read 11 */
#define rcRAM_Rd_12				(0x62)	/* RAM Read 12 */
#define rcRAM_Rd_13				(0x63)	/* RAM Read 13 */
#define rcRAM_Rd_14				(0x64)	/* RAM Read 14 */
#define rcRAM_Rd_15				(0x65)	/* RAM Read 15 */
#define rcRAM_Rd_16				(0x66)	/* RAM Read 16 */
#define rcRAM_Rd_17				(0x67)	/* RAM Read 17 */
#define rcRAM_Rd_18				(0x68)	/* RAM Read 18 */
#define rcRAM_Rd_19				(0x69)	/* RAM Read 19 */
#define rcRAM_Rd_1A				(0x6A)	/* RAM Read 1A */
#define rcRAM_Rd_1B				(0x6B)	/* RAM Read 1B */
#define rcRAM_Rd_1C				(0x6C)	/* RAM Read 1C */
#define rcRAM_Rd_1D				(0x6D)	/* RAM Read 1D */
#define rcRAM_Rd_1E				(0x6E)	/* RAM Read 1E */
#define rcRAM_Rd_1F				(0x6F)	/* RAM Read 1F */


#define rcDMA0_Config			(0x71)	/* DMA0 Configuration */
#define rcDMA0_Control			(0x72)	/* DMA0 Control */

#define rcDMA0_Remain_H			(0x74)	/* DMA0 FIFO Remain High */
#define rcDMA0_Remain_L			(0x75)	/* DMA0 FIFO Remain Low */

/* DMA0 Transfer Byte Counter High/High */
#define rcDMA0_Count_HH			(0x78)
#define rcDMA0_Count_HL			(0x79)
#define rcDMA0_Count_LH			(0x7A)
#define rcDMA0_Count_LL			(0x7B)

#define rcDMA0_RdData_0			(0x7C)	/* DMA0 Read Data High */
#define rcDMA0_RdData_1			(0x7D)	/* DMA0 Read Data Low */
#define rcDMA0_WrData_0			(0x7E)	/* DMA0 Write Data High */
#define rcDMA0_WrData_1			(0x7F)	/* DMA0 Write Data Low */

#define rcDMA1_Config			(0x81)	/* DMA0 Configuration */
#define rcDMA1_Control			(0x82)	/* DMA0 Control */

#define rcDMA1_Remain_H			(0x84)	/* DMA0 FIFO Remain High */
#define rcDMA1_Remain_L			(0x85)	/* DMA0 FIFO Remain Low */

/* DMA0 Transfer Byte Counter High/High */
#define rcDMA1_Count_HH			(0x88)
#define rcDMA1_Count_HL			(0x89)
#define rcDMA1_Count_LH			(0x8A)
#define rcDMA1_Count_LL			(0x8B)
#define rcDMA1_RdData_0			(0x8C)	/* DMA0 Read Data High */
#define rcDMA1_RdData_1			(0x8D)	/* DMA0 Read Data Low */
#define rcDMA1_WrData_0			(0x8E)	/* DMA0 Write Data High */
#define rcDMA1_WrData_1			(0x8F)	/* DMA0 Write Data Low */

#define rcIDE_Status			(0x90)	/* IDE Status */
#define rcIDE_Control			(0x91)	/* IDE Control */
#define rcIDE_Config_0			(0x92)	/* IDE Configuration 0 */
#define rcIDE_Config_1			(0x93)	/* IDE Configuration 1 */
	#define ActiveIDE	BIT(7)
#define rcIDE_Rmod				(0x94)	/* IDE Register Mode */
#define rcIDE_Tmod				(0x95)	/* IDE Transfer Mode */
#define rcIDE_Umod				(0x96)	/* IDE Ultra-DMA Transfer Mode */

#define rcIDE_CRC_H				(0x9A)	/* IDE CRC High */
#define rcIDE_CRC_L				(0x9B)	/* IDE CRC Low */

#define rcIDE_Count_H			(0x9D)	/* IDE Transfer Byte Counter High */
#define rcIDE_Count_M			(0x9E)	/* IDE Transfer Byte Counter Middle */
#define rcIDE_Count_L			(0x9F)	/* IDE Transfer Byte Counter Low */

#define rcIDE_RegAdrs			(0xA0)	/* IDE Register Address */
	#define IDE_WrReg		BIT(7)
	#define IDE_RdReg		BIT(6)
#define rcIDE_RdRegValue		(0xA2)	/* IDE Register Read Value High */
#define rcIDE_RdRegValue_0		(0xA2)	/* IDE Register Read Value High */
#define rcIDE_RdRegValue_1		(0xA3)	/* IDE Register Read Value Low */
#define rcIDE_WrRegValue		(0xA4)	/* IDE Register Write Value High */
#define rcIDE_WrRegValue_0		(0xA4)	/* IDE Register Write Value High */
#define rcIDE_WrRegValue_1		(0xA5)	/* IDE Register Write Value Low */
/* IDE Sequential Register Write Control */
#define rcIDE_SeqWrRegControl	(0xA6)
/* IDE Sequential Register Write Counter */
#define rcIDE_SeqWrRegCnt		(0xA7)
/* IDE Sequential Register Write Address FIFO */
#define rcIDE_SeqWrRegAdrs		(0xA8)
/* IDE Sequential Register Write Value FIFO */
#define rcIDE_SeqWrRegValue		(0xA9)
#define rcIDE_RegConfig			(0xAC)	/* IDE Register Configuration */

#define rcHostDeviceSel			(0xB1)	/* Host Device Select  */
	#define HOST_MODE		BIT(0)

#define rcModeProtect			(0xB3)	/* Mode Protection  	*/
#define Protect					0x00
#define unProtect				0x56

#define rcClkSelect				(0xB5)	/* Clock Select */
	#define IDETERM_OFF		BIT(7)
	#define IDEDD_TERM_OFF	BIT(6)
	#define PORT_1			BIT(1)
	#define CLK_24			BIT(0)

#define rcChipConfig			(0xB7)	/* Chip Configuration */
	#define LEV_HIGH		BIT(7)
	#define MODE_Hiz		BIT(6)
	#define DREQ_HIGH		BIT(5)
	#define DACK_HIGH		BIT(4)
	#define MODE_CS			BIT(3)
	#define BUS_SWAP		BIT(2)
	#define XBEH			BIT(1)
	#define MODE_8BIT		BIT(0)

#define rcChgEndian				(0xB9)	/* Chip Configuration */

#endif

