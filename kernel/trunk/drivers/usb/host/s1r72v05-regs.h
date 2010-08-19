/**
 * @file	s1r72v05-regs.h
 * @brief	Define of S1R72V05 Registers
 * @date	2007/03/14
 * @author	Luxun9 project team
 * Copyright (C)SEIKO EPSON CORPORATION. 2006-2007 All rights reserved.
 */

#ifndef __S1R72V05_REGS_H__
#define __S1R72V05_REGS_H__

#define CHIP_NAME	"S1R72V05"
#define BIT(x)		(1 << (x))

/**
 * @name	rcC_
 * @brief	Register 8bit Access Define:
 */
/*@{*/
#define IDE_RdReg		0x40
#define IDE_WrReg		0x80

#define rcC_MainIntStat			(0x00)		/* Main Interrupt Status */
	#define HostIntStat			BIT(6)
	#define FinishedPM			BIT(0)
	#define ValidMainIntStat	(HostIntStat | FinishedPM)
#define rcC_DeviceIntStat		(0x01)		/* EPr Interrupt Status */
#define rcC_HostIntStat			(0x02)		/* SIE Interrupt Status */
	#define H_VBUS_Err			BIT(7)
	#define H_SIE_IntStat_1			BIT(5)
	#define H_SIE_IntStat_0			BIT(4)
	#define H_FrameIntStat			BIT(3)
	#define H_CH0IntStat			BIT(1)
	#define H_CHrIntStat			BIT(0)
	#define ValidHostIntStat	(H_VBUS_Err | H_SIE_IntStat_1\
		| H_SIE_IntStat_0 | H_FrameIntStat | H_CH0IntStat | H_CHrIntStat)
#define rcC_CPU_IntStat			(0x03)		/* CPU Interrupt Status */
#define rcC_IDE_IntStat			(0x04)		/* IDE Interrupt Status */
#define rcC_MediaFIFO_IntStat	(0x05)		/* Media FIFO Interrupt Status */

#define rcC_MainIntEnb			(0x10)		/* Main Interrupt Enable */
	#define EnHostIntStat			BIT(6)
	#define EnFinishedPM			BIT(0)
	#define EnValidMainIntStat	(HostIntStat | FinishedPM)
#define rcC_DeviceIntEnb		(0x11)		/* EPr Interrupt Enable */
#define rcC_HostIntEnb			(0x12)		/* SIE Interrupt Enable */
	#define	EnH_VBUS_Err			BIT(7)
	#define	EnH_SIE_IntStat_1		BIT(5)
	#define	EnH_SIE_IntStat_0		BIT(4)
	#define EnH_FrameIntStat		BIT(3)
	#define	EnH_CH0IntStat			BIT(1)
	#define	EnH_CHrIntStat			BIT(0)
	#define EnValidHostEnb		(EnH_VBUS_Err | EnH_SIE_IntStat_1\
		| EnH_SIE_IntStat_0 | EnH_FrameIntStat | EnH_CH0IntStat\
		| EnH_CHrIntStat)
#define rcC_CPU_IntEnb			(0x13)		/* CPU Interrupt Enable */
#define rcC_IDE_IntEnb			(0x14)		/* IDE Interrupt Enable */
#define rcC_MediaFIFO_IntEnb	(0x15)		/* Media FIFO Interrupt Enable 	*/

#define rcC_RevisionNum			(0x20)		/* Revision Number */
	#define RevisionNum			(0x50)
#define rcC_ChipReset			(0x21)		/* Chip Reset */
	#define AllReset			BIT(0)
#define rcC_PM_Control_0		(0x22)		/* Power Management Control 0 */
	#define GoSleep				BIT(7)
	#define GoSnooze			BIT(6)
	#define GoActive60			BIT(5)
	#define GoActDevice			BIT(4)
	#define GoActHost			BIT(3)
#define rcC_PM_Control_1		(0x23)		/* Power Management Control 1 */
	#define PM_StateMask			(0x0F)
	#define SLEEP				(0x00)
	#define SNOOZE				(0x01)
	#define ACTIVE60			(0x03)
	#define ACT_DEVICE			(0x07)
	#define ACT_HOST			(0x0B)
#define rsC_WakeupTim_HL		(0x24)		/* Wake Up Timer High-Low */
	#define WakeupTim_2msec			(0xBBCC)
#define rcC_WakeupTim_H			(0x24)		/* Wake Up Timer Low */
#define rcC_WakeupTim_L			(0x25)		/* Wake Up Timer High */
#define rcC_H_USB_Control		(0x26)		/* Host USB Control */
#define rcC_H_XcvrControl		(0x27)		/* Host Xcvr Control */
#define rcC_D_USB_Status		(0x28)		/* Device USB Status */
#define rcC_H_USB_Status		(0x29)		/* Host USB Status */
	#define VBUS_State				BIT(7)
	#define VBUS_State_High			(0x00)
	#define VBUS_State_Low			(0x80)
#define rsC_FIFO_Rd				(0x30)		/* FIFO Read */
#define rcC_FIFO_Rd_0			(0x30)		/* FIFO Read High */
#define rcC_FIFO_Rd_1			(0x31)		/* FIFO Read Low */
#define rsC_FIFO_Wr				(0x32)		/* FIFO Write High */
#define rcC_FIFO_Wr_0			(0x32)		/* FIFO Write High */
#define rcC_FIFO_Wr_1			(0x33)		/* FIFO Write Low */
#define rsC_FIFO_RdRemain_HL	(0x34)		/* FIFO Read Remain High-Low */
#define rcC_FIFO_RdRemain_H		(0x34)		/* FIFO Read Remain Low */
#define rcC_FIFO_RdRemain_L		(0x35)		/* FIFO Read Remain High */
#define rsC_FIFO_WrRemain_HL	(0x36)		/* FIFO Write Remain High-Low */
	#define RdRemainValid			BIT(15)
	#define RdRemain			(0x1FFF)
#define rcC_FIFO_WrRemain_H		(0x36)		/* FIFO Write Remain Low */
#define rcC_FIFO_WrRemain_L		(0x37)		/* FIFO Write Remain High */
#define rcC_FIFO_ByteRd			(0x38)		/* FIFO Byte Read */


#define rsC_RAM_RdAdrs_HL		(0x40)		/* RAM Read Address High-Low */
#define rcC_RAM_RdAdrs_H		(0x40)		/* RAM Read Address Low */
#define rcC_RAM_RdAdrs_L		(0x41)		/* RAM Read Address High */
#define rcC_RAM_RdControl		(0x42)		/* RAM Read Control */
#define rcC_RAM_RdCount			(0x43)		/* RAM Read Counter */
#define rsC_RAM_WrAdrs_HL		(0x44)		/* RAM Write Address High-Low */
#define rcC_RAM_WrAdrs_H		(0x44)		/* RAM Write Address Low */
#define rcC_RAM_WrAdrs_L		(0x45)		/* RAM Write Address High */
#define rsC_RAM_WrDoor_01		(0x46)		/* RAM Write Door 0-1 */
#define rcC_RAM_WrDoor_0		(0x46)		/* RAM Write Door 0 */
#define rcC_RAM_WrDoor_1		(0x47)		/* RAM Write Door 1 */
#define rcC_MediaFIFO_Control		(0x48)	/* Media FIFO Control */
#define rcC_ClrAllMediaFIFO_Join	(0x49)	/* Clear All Media FIFO Join */
#define rcC_MediaFIFO_Join		(0x4A)		/* Media FIFO Join */


#define rcC_RAM_Rd_00			(0x50)		/* RAM Read 00 */
#define rcC_RAM_Rd_01			(0x51)		/* RAM Read 01 */
#define rcC_RAM_Rd_02			(0x52)		/* RAM Read 02 */
#define rcC_RAM_Rd_03			(0x53)		/* RAM Read 03 */
#define rcC_RAM_Rd_04			(0x54)		/* RAM Read 04 */
#define rcC_RAM_Rd_05			(0x55)		/* RAM Read 05 */
#define rcC_RAM_Rd_06			(0x56)		/* RAM Read 06 */
#define rcC_RAM_Rd_07			(0x57)		/* RAM Read 07 */
#define rcC_RAM_Rd_08			(0x58)		/* RAM Read 08 */
#define rcC_RAM_Rd_09			(0x59)		/* RAM Read 09 */
#define rcC_RAM_Rd_0A			(0x5A)		/* RAM Read 0A */
#define rcC_RAM_Rd_0B			(0x5B)		/* RAM Read 0B */
#define rcC_RAM_Rd_0C			(0x5C)		/* RAM Read 0C */
#define rcC_RAM_Rd_0D			(0x5D)		/* RAM Read 0D */
#define rcC_RAM_Rd_0E			(0x5E)		/* RAM Read 0E */
#define rcC_RAM_Rd_0F			(0x5F)		/* RAM Read 0F */


#define rcC_RAM_Rd_10			(0x60)		/* RAM Read 10 */
#define rcC_RAM_Rd_11			(0x61)		/* RAM Read 11 */
#define rcC_RAM_Rd_12			(0x62)		/* RAM Read 12 */
#define rcC_RAM_Rd_13			(0x63)		/* RAM Read 13 */
#define rcC_RAM_Rd_14			(0x64)		/* RAM Read 14 */
#define rcC_RAM_Rd_15			(0x65)		/* RAM Read 15 */
#define rcC_RAM_Rd_16			(0x66)		/* RAM Read 16 */
#define rcC_RAM_Rd_17			(0x67)		/* RAM Read 17 */
#define rcC_RAM_Rd_18			(0x68)		/* RAM Read 18 */
#define rcC_RAM_Rd_19			(0x69)		/* RAM Read 19 */
#define rcC_RAM_Rd_1A			(0x6A)		/* RAM Read 1A */
#define rcC_RAM_Rd_1B			(0x6B)		/* RAM Read 1B */
#define rcC_RAM_Rd_1C			(0x6C)		/* RAM Read 1C */
#define rcC_RAM_Rd_1D			(0x6D)		/* RAM Read 1D */
#define rcC_RAM_Rd_1E			(0x6E)		/* RAM Read 1E */
#define rcC_RAM_Rd_1F			(0x6F)		/* RAM Read 1F */


#define rcC_DMA0_Config			(0x71)		/* DMA0 Configuration */
#define rcC_DMA0_Control		(0x72)		/* DMA0 Control */

#define rsC_DMA0_Remain_HL		(0x74)		/* DMA0 FIFO Remain High-Low */
#define rcC_DMA0_Remain_H		(0x74)		/* DMA0 FIFO Remain Low */
#define rcC_DMA0_Remain_L		(0x75)		/* DMA0 FIFO Remain High */

#define rsC_DMA0_Count_HHL		(0x78)		/* DMA0 Transfer Byte Counter High/High-Low */
#define rcC_DMA0_Count_HH		(0x78)		/* DMA0 Transfer Byte Counter High/Low */
#define rcC_DMA0_Count_HL		(0x79)		/* DMA0 Transfer Byte Counter High/High */
#define rsC_DMA0_Count_LHL		(0x7A)		/* DMA0 Transfer Byte Counter Low/High-Low */
#define rcC_DMA0_Count_LH		(0x7A)		/* DMA0 Transfer Byte Counter Low/Low */
#define rcC_DMA0_Count_LL		(0x7B)		/* DMA0 Transfer Byte Counter Low/High */
#define rcC_DMA0_RdData_0		(0x7C)		/* DMA0 Read Data 0 */
#define rcC_DMA0_RdData_1		(0x7D)		/* DMA0 Read Data 1 */
#define rcC_DMA0_WrData_0		(0x7E)		/* DMA0 Write Data 0 */
#define rcC_DMA0_WrData_1		(0x7F)		/* DMA0 Write Data 1 */


#define rcC_DMA1_Config			(0x81)		/* DMA1 Configuration */
#define rcC_DMA1_Control		(0x82)		/* DMA1 Control */

#define rsC_DMA1_Remain_HL		(0x84)		/* DMA1 FIFO Remain High-Low */
#define rcC_DMA1_Remain_H		(0x84)		/* DMA1 FIFO Remain Low */
#define rcC_DMA1_Remain_L		(0x85)		/* DMA1 FIFO Remain High */

#define rsC_DMA1_Count_HHL		(0x88)		/* DMA1 Transfer Byte Counter High/High-Low */
#define rcC_DMA1_Count_HH		(0x88)		/* DMA1 Transfer Byte Counter High/Low */
#define rcC_DMA1_Count_HL		(0x89)		/* DMA1 Transfer Byte Counter High/High */
#define rsC_DMA1_Count_LHL		(0x8A)		/* DMA1 Transfer Byte Counter Low/High-Low */
#define rcC_DMA1_Count_LH		(0x8A)		/* DMA1 Transfer Byte Counter Low/Low */
#define rcC_DMA1_Count_LL		(0x8B)		/* DMA1 Transfer Byte Counter Low/High */
#define rcC_DMA1_RdData_0		(0x8C)		/* DMA1 Read Data 0 */
#define rcC_DMA1_RdData_1		(0x8D)		/* DMA1 Read Data 1 */
#define rcC_DMA1_WrData_0		(0x8E)		/* DMA1 Write Data 0 */
#define rcC_DMA1_WrData_1		(0x8F)		/* DMA1 Write Data 1 */


#define rcC_IDE_Status			(0x90)		/* IDE Status */
#define rcC_IDE_Control			(0x91)		/* IDE Control */
#define rcC_IDE_Config_0		(0x92)		/* IDE Configuration 0 */
#define rcC_IDE_Config_1		(0x93)		/* IDE Configuration 1 */
	#define ActiveIDE				BIT(7)
#define rcC_IDE_Rmod			(0x94)		/* IDE Register Mode */
#define rcC_IDE_Tmod			(0x95)		/* IDE Transfer Mode */
#define rcC_IDE_Umod			(0x96)		/* IDE Ultra-DMA Transfer Mode */

#define rsC_IDE_CRC_HL			(0x9A)		/* IDE CRC High-Low */
#define rcC_IDE_CRC_H			(0x9A)		/* IDE CRC Low */
#define rcC_IDE_CRC_L			(0x9B)		/* IDE CRC High */

#define rsC_IDE_Count_H			(0x9D)		/* IDE Transfer Byte Counter High(16bit) */
#define rcC_IDE_Count_H			(0x9D)		/* IDE Transfer Byte Counter High */
#define rsC_IDE_Count_ML		(0x9E)		/* IDE Transfer Byte Counter Middle-Low */
#define rcC_IDE_Count_L			(0x9F)		/* IDE Transfer Byte Counter Low */
#define rcC_IDE_Count_M			(0x9E)		/* IDE Transfer Byte Counter Middle */


#define rcC_IDE_RegAdrs			(0xA0)		/* IDE Register Address */

#define rcC_IDE_RdRegValue_0	(0xA2)		/* IDE Register Read Value 0 */
#define rcC_IDE_RdRegValue_1	(0xA3)		/* IDE Register Read Value 1 */
#define rcC_IDE_WrRegValue_0	(0xA4)		/* IDE Register Write Value 0 */
#define rcC_IDE_WrRegValue_1	(0xA5)		/* IDE Register Write Value 1 */
#define rcC_IDE_SeqWrRegControl	(0xA6)		/* IDE Sequential Register Write Control */
#define rcC_IDE_SeqWrRegCnt		(0xA7)		/* IDE Sequential Register Write Counter */
#define rcC_IDE_SeqWrRegAdrs	(0xA8)		/* IDE Sequential Register Write Address FIFO */
#define rcC_IDE_SeqWrRegValue	(0xA9)		/* IDE Sequential Register Write Value FIFO */

#define rcC_IDE_RegConfig		(0xAC)		/* IDE Register Configuration */

#define rcC_HostDeviceSel		(0xB1)		/* Host Device Select */
	#define HOSTxDEVICE				BIT(0)
	#define HostMode				(0x01)
#define rcC_ModeProtect			(0xB3)		/* Mode Protection */
	#define ModeProtectSet			(0x00)
	#define ModeProtectClear		(0x56)
#define rcC_ClkSelect			(0xB5)		/* Clock Select */
	#define CLK_24					BIT(0)
#define rcC_ChipConfig			(0xB7)		/* Chip Configuration */
	#define CPU_Endian				BIT(2)
#define rcC_CPU_ChgEndian		(0xB9)		/* CPU Change Endian */
/*@}*/

/**
 * @name	rcH_
 * @brief	define for host:
 */
/*@{*/
#define rcH_SIE_IntStat_0		(0xE0)		/* Host SIE Interrupt Status */
	#define DetectCon			BIT(4)
	#define DetectDiscon			BIT(3)
	#define DetectRmtWkup			BIT(2)
	#define DetectChirpOK			BIT(1)
	#define DetectChirpNG			BIT(0)
	#define SIE_IntStat_0_AllBit	(DetectCon | DetectDiscon)
	#define SIE_IntEnb_0_AllBit	(DetectCon | DetectDiscon)
#define rcH_SIE_IntStat_1		(0xE1)
	#define DisabledCmp			BIT(3)
	#define ResumeCmp			BIT(2)
	#define SuspendCmp			BIT(1)
	#define ResetCmp			BIT(0)
	#define SIE_IntStat_1_AllBit	(DisabledCmp | ResumeCmp | SuspendCmp | ResetCmp)
	#define SIE_IntEnb_1_AllBit		(DisabledCmp | ResumeCmp | SuspendCmp | ResetCmp)
#define rcH_FIFO_IntStat		(0xE2)		/* Host FIFO Interrupt Status */
#define rcH_Frame_IntStat		(0xE3)		/* Host Frame Interrupt Status */
	#define PortErr				BIT(2)
#define rcH_CHr_IntStat			(0xE4)		/* Host CHr Interrupt Status */
#define rcH_CH0_IntStat			(0xE5)
	#define TotalSizeCmp			BIT(7)
	#define TranACK				BIT(6)
	#define TranErr				BIT(5)
	#define ChangeCondition			BIT(4)
	#define CTL_SupportCmp			BIT(1)
	#define CTL_SupportStop			BIT(0)
	#define CHx_IntStat_AllBit		(TotalSizeCmp | TranACK\
		| TranErr | ChangeCondition)
	#define CH0_IntStat_AllBit		(TotalSizeCmp | TranACK\
		| TranErr | ChangeCondition)

#define rcH_CHa_IntStat			(0xE6)
#define rcH_CHb_IntStat			(0xE7)
#define rcH_CHc_IntStat			(0xE8)
#define rcH_CHd_IntStat			(0xE9)
#define rcH_CHe_IntStat			(0xEA)
#define rcH_SIE_IntEnb_0		(0xF0)		/* Host SIE Interrupt Enable */
#define rcH_SIE_IntEnb_1		(0xF1)
#define rcH_FIFO_IntEnb			(0xF2)		/* Host FIFO Interrupt Enable */
#define rcH_Frame_IntEnb		(0xF3)		/* Host FIFO Interrupt Enable */

#define rcH_CHr_IntEnb			(0xF4)		/* Host CHr Interrupt Enable */
#define rcH_CH0_IntEnb			(0xF5)		/* Host CH0 Interrupt Enable */
#define rcH_CHa_IntEnb			(0xF6)
#define rcH_CHb_IntEnb			(0xF7)
#define rcH_CHc_IntEnb			(0xF8)
#define rcH_CHd_IntEnb			(0xF9)
#define rcH_CHe_IntEnb			(0xFA)

#define rcH_Reset			(0x100)		/* Host Reset */
	#define ResetHTM				BIT(0)
#define rcH_NegoControl_0		(0x102)		/* Host NegoControl */
	#define AutoModeCancel			BIT(7)
	#define HostStateMask			(0x70)
	#define HostState_IDLE			(0x10)
	#define HostState_WAIT_CONNECT	(0x20)
	#define HostState_DISABLE		(0x30)
	#define HostState_USB_OPERATION	(0x50)
	#define HostState_USB_SUSPEND	(0x60)
	#define HostState_USB_RESUME	(0x70)
	#define AutoModeMask			(0x0F)
	#define GoCANCEL				(0x00)
	#define GoIDLE					(0x01)
	#define GoWAIT_CONNECT			(0x02)
	#define GoDISABLED				(0x03)
	#define GoRESET					(0x04)
	#define GoOPERATIONAL			(0x05)
	#define GoSUSPEND				(0x06)
	#define GoRESUME				(0x07)
	#define GoWAIT_CONNECTtoDIS		(0x09)
	#define GoWAIT_CONNECTtoOP		(0x0A)
	#define GoRESETtoOP				(0x0C)
	#define GoSUSPENDtoOP			(0x0E)
	#define GoRESUMEtoOP			(0x0F)
#define rcH_NegoControl_1		(0x104)
	#define PortSpeedMask			(0x30)
	#define PortSpeed_High			(0x00)
	#define PortSpeed_Full			(0x10)
	#define PortSpeed_Low			(0x30)
	#define DisChirpFinish			BIT(1)
	#define RmtWkupDetEnb			BIT(0)
#define rcH_USB_Test			(0x106)
	#define EnHS_Test			BIT(7)
	#define Test_Force_Enable		BIT(4)
	#define Test_SE0_NAK			BIT(3)
	#define TEST_J				BIT(2)
	#define TEST_K				BIT(1)
	#define Test_Packet			BIT(0)
#define rcH_CHn_Control			(0x108)		/* CHn Control */
	#define CH0FIFO_Clr			BIT(0)
#define rcH_CHr_FIFO_Clr		(0x109)		/* CHr FIFO Clear */
#define rcH_ClrAllCHnJoin		(0x10A)		/* Host Clear All CHn Join */

#define rcH_CH0SETUP_0			(0x110)		/* Setup */
#define rcH_CH0SETUP_1			(0x111)
#define rcH_CH0SETUP_2			(0x112)
#define rcH_CH0SETUP_3			(0x113)
#define rcH_CH0SETUP_4			(0x114)
#define rcH_CH0SETUP_5			(0x115)
#define rcH_CH0SETUP_6			(0x116)
#define rcH_CH0SETUP_7			(0x117)

#define rsH_FrameNumber_HL		(0x11E)
#define rcH_FrameNumber_H		(0x11E)
#define rcH_FrameNumber_L		(0x11F)
	#define FrameNumberMask		(0x07FF)

#define rcH_CH0_Config_0		(0x120)		/* Host Channel 0 Configuration */
	#define ACK_Cnt_MASK			(0xF0)
	#define SpeedMode_MASK			(0x0C)
	#define SpeedMode_HS			(0x00)
	#define SpeedMode_FS			(0x04)
	#define SpeedMode_LS			(0x0C)
	#define CHxConfig_Toggle_MASK	(0x02)
	#define CHxConfig_Toggle_0		(0x00)
	#define CHxConfig_Toggle_1		(0x02)
	#define CHxConfig_Toggle		BIT(1)
	#define CHxConfig_TranGo		BIT(0)

#define rcH_CH0_Config_1		(0x121)
	#define CHxTID_MASK			(0xC0)
	#define CHxTID_SETUP			(0x00)
	#define CHxTID_IN			(0x80)
	#define CHxTID_OUT			(0x40)
	#define CHxTranType_ISOC		(0x00)
	#define CHxTranType_BULK		(0x20)
	#define CHxTranType_INT			(0x30)
	#define AutoZerolen			BIT(3)

#define rsH_CH0_MaxPktSize		(0x122)		/* Host Channel 0 Max Packet Size(16bit) */
#define rcH_CH0_MaxPktSize		(0x122)		/* Host Channel 0 Max Packet Size */
#define rsH_CH0_TotalSize_HL	(0x126)		/* Host Channel 0 Total Size(16bit) */
#define rcH_CH0_TotalSize_H		(0x126)		/* Host Channel 0 Total Size */
#define rcH_CH0_TotalSize_L		(0x127)
#define rcH_CH0_HubAdrs			(0x128)		/* Host Channel 0 Hub Address */
	#define S_R_HUB_ADDR_OFFSET(x)	((x) << 4)/* hub register bit offset */
#define rcH_CH0_FuncAdrs		(0x129)	/* Host Channel 0 Function Address */
	#define FuncAdrs_MASK			(0xF0)
	#define EP_Number_MASK			(0x0F)
	#define S_R_FUNC_ADDR_OFFSET(x)	(x << 4)/* FuncAdrs egister bit offset */

#define rcH_CTL_SupportControl		(0x12B)		/* Host ControlTransfer Support */
#define rcH_CH0_ConditionCode		(0x12E)
	#define ConditionCode_MASK		(0x70)
	#define CCode_NoError			(0)
	#define CCode_Stall				((1) << 4)
	#define CCode_OverRun			((2) << 4)
	#define CCode_UnderRun			((3) << 4)
	#define CCode_RetryError		((4) << 4)

#define rcH_CH0_Join			(0x12F)
	#define S_R_JoinCPU_Rd			BIT(1)
	#define S_R_JoinCPU_Wr			BIT(0)

#define rcH_CHa_Config_0		(0x130)		/* Host Channel Configuration */
#define rcH_CHa_Config_1		(0x131)
#define rsH_CHa_MaxPktSize_HL	(0x132)		/* Host Channel a Max Packet Size High-Low */
#define rcH_CHa_MaxPktSize_H	(0x132)
#define rcH_CHa_MaxPktSize_L	(0x133)
#define rsH_CHa_TotalSize_HHL	(0x134)		/* Host Channel a Total Size High/High-Low */
#define rcH_CHa_TotalSize_HH	(0x134)
#define rcH_CHa_TotalSize_HL	(0x135)
#define rsH_CHa_TotalSize_LHL	(0x136)		/* Host Channel a Total Size Low/High-Low */
#define rcH_CHa_TotalSize_LH	(0x136)
#define rcH_CHa_TotalSize_LL	(0x137)
#define rcH_CHa_HubAdrs			(0x138)
#define rcH_CHa_FuncAdrs		(0x139)
#define rcH_BO_SupportControl	(0x13A)		/* Host Bulk Only Transfer Support */
#define rcH_CSW_RcvDataSize		(0x13B)
#define rcH_OUT_EP_Control		(0x13C)
#define rcH_IN_EP_Control		(0x13D)
#define rcH_CHa_ConditionCode	(0x13E)
#define rcH_CHa_Join			(0x13F)

#define rcH_CHb_Config_0		(0x140)
#define rcH_CHb_Config_1		(0x141)
#define rsH_CHb_MaxPktSize_HL	(0x142)		/* Host Channel b Max Packet Size High/Low */
#define rcH_CHb_MaxPktSize_H	(0x142)
#define rcH_CHb_MaxPktSize_L	(0x143)
#define rsH_CHb_TotalSize_HHL	(0x144)		/* Host Channel b Total Size High/High-Low */
#define rcH_CHb_TotalSize_HH	(0x144)
#define rcH_CHb_TotalSize_HL	(0x145)
#define rsH_CHb_TotalSize_LHL	(0x146)		/* Host Channel a Total Size Low/High-Low */
#define rcH_CHb_TotalSize_LH	(0x146)
#define rcH_CHb_TotalSize_LL	(0x147)
#define rcH_CHb_HubAdrs			(0x148)
#define rcH_CHb_FuncAdrs		(0x149)
#define rsH_CHb_Interval_HL		(0x14A)		/* Host Channel b Interval High/Low */
#define rcH_CHb_Interval_H		(0x14A)
#define rcH_CHb_Interval_L		(0x14B)
#define rcH_CHb_ConditionCode	(0x14E)
#define rcH_CHb_Join			(0x14F)

#define rcH_CHc_Config_0		(0x150)
#define rcH_CHc_Config_1		(0x151)
#define rsH_CHc_MaxPktSize_HL	(0x152)		/* Host Channel c Max Packet Size High/Low */
#define rcH_CHc_MaxPktSize_H	(0x152)
#define rcH_CHc_MaxPktSize_L	(0x153)
#define rsH_CHc_TotalSize_HHL	(0x154)		/* Host Channel c Total Size High/High-Low */
#define rcH_CHc_TotalSize_HH	(0x154)
#define rcH_CHc_TotalSize_HL	(0x155)
#define rsH_CHc_TotalSize_LHL	(0x156)		/* Host Channel c Total Size Low/High-Low */
#define rcH_CHc_TotalSize_LH	(0x156)
#define rcH_CHc_TotalSize_LL	(0x157)
#define rcH_CHc_HubAdrs			(0x158)
#define rcH_CHc_FuncAdrs		(0x159)
#define rsH_CHc_Interval_HL		(0x15A)		/* Host Channel c Interval High/Low */
#define rcH_CHc_Interval_H		(0x15A)
#define rcH_CHc_Interval_L		(0x15B)
#define rcH_CHc_ConditionCode	(0x15E)
#define rcH_CHc_Join			(0x15F)

#define rcH_CHd_Config_0		(0x160)
#define rcH_CHd_Config_1		(0x161)
#define rsH_CHd_MaxPktSize_HL	(0x162)		/* Host Channel d Max Packet Size High/Low */
#define rcH_CHd_MaxPktSize_H	(0x162)
#define rcH_CHd_MaxPktSize_L	(0x163)
#define rsH_CHd_TotalSize_HHL	(0x164)		/* Host Channel d Total Size High/High-Low */
#define rcH_CHd_TotalSize_HH	(0x164)
#define rcH_CHd_TotalSize_HL	(0x165)
#define rsH_CHd_TotalSize_LHL	(0x166)		/* Host Channel d Total Size Low/High-Low */
#define rcH_CHd_TotalSize_LH	(0x166)
#define rcH_CHd_TotalSize_LL	(0x167)
#define rcH_CHd_HubAdrs			(0x168)
#define rcH_CHd_FuncAdrs		(0x169)
#define rsH_CHd_Interval_HL		(0x16A)		/* Host Channel d Interval High/Low */
#define rcH_CHd_Interval_H		(0x16A)
#define rcH_CHd_Interval_L		(0x16B)
#define rcH_CHd_ConditionCode	(0x16E)
#define rcH_CHd_Join			(0x16F)

#define rcH_CHe_Config_0		(0x170)
#define rcH_CHe_Config_1		(0x171)
#define rsH_CHe_MaxPktSize_HL	(0x172)		/* Host Channel e Max Packet Size High/Low */
#define rcH_CHe_MaxPktSize_H	(0x172)
#define rcH_CHe_MaxPktSize_L	(0x173)
#define rsH_CHe_TotalSize_HHL	(0x174)		/* Host Channel e Total Size High/High-Low */
#define rcH_CHe_TotalSize_HH	(0x174)
#define rcH_CHe_TotalSize_HL	(0x175)
#define rsH_CHe_TotalSize_LHL	(0x176)		/* Host Channel e Total Size Low/High-Low */
#define rcH_CHe_TotalSize_LH	(0x176)
#define rcH_CHe_TotalSize_LL	(0x177)
#define rcH_CHe_HubAdrs			(0x178)
#define rcH_CHe_FuncAdrs		(0x179)
#define rsH_CHe_Interval_HL		(0x17A)		/* Host Channel e Interval High/Low */
#define rcH_CHe_Interval_H		(0x17A)
#define rcH_CHe_Interval_L		(0x17B)
#define rcH_CHe_ConditionCode	(0x17E)
#define rcH_CHe_Join			(0x17F)

#define CHxConfig_0Offset		((rcH_CHe_Config_0 - rcH_CH0_Config_0) / CH_E)
#define CHxJoinOffset			((rcH_CHe_Join - rcH_CH0_Join) / CH_E)

#define rsH_CH0_StartAdrs_HL	(0x180)		/* Host Channel 0 Start Address High/Low */
#define rcH_CH0_StartAdrs_L		(0x180)		/* FIFO for CH0	*/
#define rcH_CH0_StartAdrs_H		(0x181)
#define rsH_CH0_EndAdrs_HL		(0x182)		/* Host Channel 0 End Address High/Low */
#define rcH_CH0_EndAdrs_L		(0x182)
#define rcH_CH0_EndAdrs_H		(0x183)

#define rsH_CHa_StartAdrs_HL	(0x184)		/* Host Channel a Start Address High/Low */
#define rcH_CHa_StartAdrs_L		(0x184)		/* FIFO for CHa	*/
#define rcH_CHa_StartAdrs_H		(0x185)
#define rsH_CHa_EndAdrs_HL		(0x186)		/* Host Channel a End Address High/Low */
#define rcH_CHa_EndAdrs_L		(0x186)
#define rcH_CHa_EndAdrs_H		(0x187)
#define rsH_CHb_StartAdrs_HL	(0x188)		/* Host Channel b Start Address High/Low */
#define rcH_CHb_StartAdrs_L		(0x188)		/* FIFO for CHb	*/
#define rcH_CHb_StartAdrs_H		(0x189)
#define rsH_CHb_EndAdrs_HL		(0x18A)		/* Host Channel b End Address High/Low */
#define rcH_CHb_EndAdrs_L		(0x18A)
#define rcH_CHb_EndAdrs_H		(0x18B)
#define rsH_CHc_StartAdrs_HL	(0x18C)		/* Host Channel c Start Address High/Low */
#define rcH_CHc_StartAdrs_L		(0x18C)		/* FIFO for CHc */
#define rcH_CHc_StartAdrs_H		(0x18D)
#define rsH_CHc_EndAdrs_HL		(0x18E)		/* Host Channel c End Address High/Low */
#define rcH_CHc_EndAdrs_L		(0x18E)
#define rcH_CHc_EndAdrs_H		(0x18F)
#define rsH_CHd_StartAdrs_HL	(0x190)		/* Host Channel d Start Address High/Low */
#define rcH_CHd_StartAdrs_L		(0x190)		/* FIFO for CHd */
#define rcH_CHd_StartAdrs_H		(0x191)
#define rsH_CHd_EndAdrs_HL		(0x192)		/* Host Channel d End Address High/Low */
#define rcH_CHd_EndAdrs_L		(0x192)
#define rcH_CHd_EndAdrs_H		(0x193)
#define rsH_CHe_StartAdrs_HL	(0x194)		/* Host Channel e Start Address High/Low */
#define rcH_CHe_StartAdrs_L		(0x194)		/* FIFO for CHe */
#define rcH_CHe_StartAdrs_H		(0x195)
#define rsH_CHe_EndAdrs_HL		(0x196)		/* Host Channel e End Address High/Low */
#define rcH_CHe_EndAdrs_L		(0x196)
#define rcH_CHe_EndAdrs_H		(0x197)
/*@}*/

/**
 * @name	_FIFO_Clr
 * @brief	bit assign:
 */
/*@{*/
/* rcH_CHn_Control (0x108)  Host CHn Control */
#define S_R_AllFIFO_Clr			0x20

/* rcH_CHr_FIFO_Clr (0x109)  Host CHr FIFO Clear */
#define S_R_CHa_FIFO_Clr		0x01
#define S_R_CHb_FIFO_Clr		0x02
#define S_R_CHc_FIFO_Clr		0x04
#define S_R_CHd_FIFO_Clr		0x08
#define S_R_CHe_FIFO_Clr		0x10
/*@}*/

/**
 * @name	rc_
 * @brief	redefine:
 */
/*@{*/
#define rc_Reset				rcH_Reset
#define rc_PM_Control			rcC_PM_Control_0
#define rc_PM_Control_State		rcC_PM_Control_1
#define rc_ChipConfig			rcC_ChipConfig

#define rsH_BUF0_StartAdrs_HL	rsH_CH0_StartAdrs_HL
#define rcH_BUF0_StartAdrs_L	rcH_CH0_StartAdrs_L
#define rcH_BUF0_StartAdrs_H	rcH_CH0_StartAdrs_H
#define rsH_BUF0_EndAdrs_HL		rsH_CH0_EndAdrs_HL
#define rcH_BUF0_EndAdrs_L		rcH_CH0_EndAdrs_L
#define rcH_BUF0_EndAdrs_H		rcH_CH0_EndAdrs_H
#define rsH_BUFa_StartAdrs_HL	rsH_CHa_StartAdrs_HL
#define rcH_BUFa_StartAdrs_L	rcH_CHa_StartAdrs_L
#define rcH_BUFa_StartAdrs_H	rcH_CHa_StartAdrs_H
#define rsH_BUFa_EndAdrs_HL		rsH_CHa_EndAdrs_HL
#define rcH_BUFa_EndAdrs_L		rcH_CHa_EndAdrs_L
#define rcH_BUFa_EndAdrs_H		rcH_CHa_EndAdrs_H
#define rsH_BUFb_StartAdrs_HL	rsH_CHb_StartAdrs_HL
#define rcH_BUFb_StartAdrs_L	rcH_CHb_StartAdrs_L
#define rcH_BUFb_StartAdrs_H	rcH_CHb_StartAdrs_H
#define rsH_BUFb_EndAdrs_HL		rsH_CHb_EndAdrs_HL
#define rcH_BUFb_EndAdrs_L		rcH_CHb_EndAdrs_L
#define rcH_BUFb_EndAdrs_H		rcH_CHb_EndAdrs_H
#define rsH_BUFc_StartAdrs_HL	rsH_CHc_StartAdrs_HL
#define rcH_BUFc_StartAdrs_L	rcH_CHc_StartAdrs_L
#define rcH_BUFc_StartAdrs_H	rcH_CHc_StartAdrs_H
#define rsH_BUFc_EndAdrs_HL		rsH_CHc_EndAdrs_HL
#define rcH_BUFc_EndAdrs_L		rcH_CHc_EndAdrs_L
#define rcH_BUFc_EndAdrs_H		rcH_CHc_EndAdrs_H
#define rsH_BUFd_StartAdrs_HL	rsH_CHd_StartAdrs_HL
#define rcH_BUFd_StartAdrs_L	rcH_CHd_StartAdrs_L
#define rcH_BUFd_StartAdrs_H	rcH_CHd_StartAdrs_H
#define rsH_BUFd_EndAdrs_HL		rsH_CHd_EndAdrs_HL
#define rcH_BUFd_EndAdrs_L		rcH_CHd_EndAdrs_L
#define rcH_BUFd_EndAdrs_H		rcH_CHd_EndAdrs_H
#define rsH_BUFe_StartAdrs_HL	rsH_CHe_StartAdrs_HL
#define rcH_BUFe_StartAdrs_L	rcH_CHe_StartAdrs_L
#define rcH_BUFe_StartAdrs_H	rcH_CHe_StartAdrs_H
#define rsH_BUFe_EndAdrs_HL		rsH_CHe_EndAdrs_HL
#define rcH_BUFe_EndAdrs_L		rcH_CHe_EndAdrs_L
#define rcH_BUFe_EndAdrs_H		rcH_CHe_EndAdrs_H

#define rcH_BUFn_Control		rcH_CHn_Control
#define rcH_BUFr_FIFO_Clr		rcH_CHr_FIFO_Clr
#define rcH_BUF0_Join			rcH_CH0_Join
#define rcH_BUFa_Join			rcH_CHa_Join
#define rcH_BUFb_Join			rcH_CHb_Join
#define rcH_BUFc_Join			rcH_CHc_Join
#define rcH_BUFd_Join			rcH_CHd_Join
#define rcH_BUFe_Join			rcH_CHe_Join

#define S_R_BUFx_Config_0_Offset	CHxConfig_0Offset
#define S_R_BUFx_Join_Offset		CHxJoinOffset

#define rcH_ClrAllBUFn_Join		rcH_ClrAllCHnJoin


/* rcH_CHn_Control (0x108)  Host CHn Control */
#define S_R_FIFO_ALL_CLR		S_R_AllFIFO_Clr

/* rcH_CHr_FIFO_Clr (0x109)  Host CHr FIFO Clear */
#define S_R_BUFa_FIFO_Clr		S_R_CHa_FIFO_Clr
#define S_R_BUFb_FIFO_Clr		S_R_CHb_FIFO_Clr
#define S_R_BUFc_FIFO_Clr		S_R_CHc_FIFO_Clr
#define S_R_BUFd_FIFO_Clr		S_R_CHd_FIFO_Clr
#define S_R_BUFe_FIFO_Clr		S_R_CHe_FIFO_Clr

#define S_R_CH_FIFO_CLR(x)\
	if (x > 0) {\
		S_R_REGS8(rcH_BUFr_FIFO_Clr) = (1 << ((x) - 1));\
	} else {\
		S_R_REGS8(rcH_BUFn_Control) = CH0FIFO_Clr;\
	}

#define S_R_CH0_FIFO_CLR(x)\
	S_R_REGS8(rcH_BUFn_Control) = CH0FIFO_Clr;
/*@}*/

/**
 * @name	HOST_FIFO_
 * @brief	Define for Host FIFO:
 */
/*@{*/
#define HOST_FIFO_CH0_LEN	64
#define HOST_FIFO_CHA_LEN	1024
#define HOST_FIFO_CHB_LEN	512
#define HOST_FIFO_CHC_LEN	512
#define HOST_FIFO_CHD_LEN	1024
#define HOST_FIFO_CHE_LEN	1024

#define HOST_FIFO_CH0_START_ADRS	0x0040
#define HOST_FIFO_CH0_END_ADRS		HOST_FIFO_CH0_START_ADRS+HOST_FIFO_CH0_LEN
#define HOST_FIFO_CHA_START_ADRS	HOST_FIFO_CH0_END_ADRS
#define HOST_FIFO_CHA_END_ADRS		HOST_FIFO_CHA_START_ADRS+HOST_FIFO_CHA_LEN
#define HOST_FIFO_CHB_START_ADRS	HOST_FIFO_CHA_END_ADRS
#define HOST_FIFO_CHB_END_ADRS		HOST_FIFO_CHB_START_ADRS + HOST_FIFO_CHB_LEN
#define HOST_FIFO_CHC_START_ADRS	HOST_FIFO_CHB_END_ADRS
#define HOST_FIFO_CHC_END_ADRS		HOST_FIFO_CHC_START_ADRS + HOST_FIFO_CHC_LEN
#define HOST_FIFO_CHD_START_ADRS	HOST_FIFO_CHC_END_ADRS
#define HOST_FIFO_CHD_END_ADRS		HOST_FIFO_CHD_START_ADRS + HOST_FIFO_CHD_LEN
#define HOST_FIFO_CHE_START_ADRS	HOST_FIFO_CHD_END_ADRS
#define HOST_FIFO_CHE_END_ADRS		HOST_FIFO_CHE_START_ADRS + HOST_FIFO_CHE_LEN
/*@}*/

#endif
