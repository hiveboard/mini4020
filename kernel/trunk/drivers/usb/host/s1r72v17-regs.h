/*
 * @file        s1r72v17-regs.h
 * @brief       Define of S1R72V17 Registers
 * @date        2007/03/14
 * @author      Luxun9 project team
 * Copyright (C)SEIKO EPSON CORPORATION. 2006-2007 All rights reserved.
 */

#ifndef __S1R72V17_REGS_H__
#define	__S1R72V17_REGS_H__

#define CHIP_NAME	"S1R72V17"
#define BIT(x)		(1 << (x))

/**
 * @name	rcC_
 * @brief	Register 8bit Access Define:
 */
/*@{*/
#define rcC_MainIntStat			(0x00)		/* Main Interrupt Status */
	#define HostIntStat				BIT(6)
	#define FinishedPM				BIT(0)
	#define ValidMainIntStat	(HostIntStat | FinishedPM)
#define rcC_DeviceIntStat		(0x01)		/* EPr Interrupt Status */
#define rcC_HostIntStat			(0x02)		/* SIE Interrupt Status */
	#define H_VBUS_Err				BIT(7)
	#define H_SIE_IntStat_1			BIT(5)
	#define H_SIE_IntStat_0			BIT(4)
	#define H_FrameIntStat			BIT(3)
	#define H_CH0IntStat			BIT(1)
	#define H_CHrIntStat			BIT(0)
	#define ValidHostIntStat	(H_VBUS_Err | H_SIE_IntStat_1\
		| H_SIE_IntStat_0 | H_FrameIntStat | H_CH0IntStat | H_CHrIntStat)
#define rcC_CPU_IntStat			(0x03)		/* CPU Interrupt Status */
#define rcC_FIFO_IntStat		(0x04)		/* FIFO Interrupt Status */

#define rcC_MainIntEnb			(0x08)		/* Main Interrupt Enable */
	#define EnHostIntStat			BIT(6)
	#define EnFinishedPM			BIT(0)
	#define EnValidMainIntStat	(HostIntStat | FinishedPM)
#define rcC_DeviceIntEnb		(0x09)		/* EPr Interrupt Enable */
#define rcC_HostIntEnb			(0x0A)		/* SIE Interrupt Enable */
	#define	EnH_VBUS_Err			BIT(7)
	#define	EnH_SIE_IntStat_1		BIT(5)
	#define	EnH_SIE_IntStat_0		BIT(4)
	#define EnH_FrameIntStat		BIT(3)
	#define	EnH_CH0IntStat			BIT(1)
	#define	EnH_CHrIntStat			BIT(0)
	#define EnValidHostEnb		(EnH_VBUS_Err | EnH_SIE_IntStat_1\
		| EnH_SIE_IntStat_0 | EnH_FrameIntStat | EnH_CH0IntStat\
		| EnH_CHrIntStat)
#define rcC_CPU_IntEnb			(0x0B)		/* CPU Interrupt Enable */
#define rcC_MediaFIFO_IntEnb	(0x0C)		/* FIFO Interrupt Enable */

#define rcC_RevisionNum			(0x10)		/* Revision Number */
	#define RevisionNum			(0x10)
#define rcC_ChipReset			(0x11)		/* Chip Reset */
	#define MTMReset			BIT(7)
	#define AllReset			BIT(0)
#define rcC_PM_Control			(0x12)		/* Power Management Control 0 */
	#define GoSleep				BIT(7)
	#define GoActive			BIT(6)
	#define GoCPU_Cut			BIT(5)
	#define PM_StateMask			(0x03)
	#define SLEEP				(0x00)
	#define SNOOZE				(0x01)
	#define ACTIVE				(0x03)
#define rsC_WakeupTim_HL		(0x14)		/* Wake Up Timer High-Low */
	#define WakeupTim_2msec			(0xBBCC)
#define rcC_WakeupTim_H			(0x14)		/* Wake Up Timer Low */
#define rcC_WakeupTim_L			(0x15)		/* Wake Up Timer High */
#define rcC_H_USB_Control		(0x16)		/* Host USB Control */
#define rcC_H_XcvrControl		(0x17)		/* Host Xcvr Control */
#define rcC_D_USB_Status		(0x18)		/* Device USB Status */
#define rcC_H_USB_Status		(0x19)		/* Host USB Status */
	#define VBUS_State              BIT(7)
	#define VBUS_State_High         (0x00)
	#define VBUS_State_Low          (0x80)
	
#define rcC_MTM_Config			(0x1B)		/* Multi Transceiver Macro Config */

#define rcC_HostDeviceSel		(0x1F)		/* Host Device Select */
	#define HOSTxDEVICE			BIT(0)
	#define HostMode			(0x01)

#define rsC_FIFO_Rd				(0x20)		/* FIFO Read High */
#define rcC_FIFO_Rd_0			(0x20)		/* FIFO Read High */
#define rcC_FIFO_Rd_1			(0x21)		/* FIFO Read Low */
#define rsC_FIFO_Wr				(0x22)		/* FIFO Write High */
#define rcC_FIFO_Wr_0			(0x22)		/* FIFO Write High */
#define rcC_FIFO_Wr_1			(0x23)		/* FIFO Write Low */
#define rsC_FIFO_RdRemain_HL	(0x24)		/* FIFO Read Remain High-Low */
#define rcC_FIFO_RdRemain_H		(0x24)		/* FIFO Read Remain Low */
#define rcC_FIFO_RdRemain_L		(0x25)		/* FIFO Read Remain High */
#define rsC_FIFO_WrRemain_HL	(0x26)		/* FIFO Write Remain High-Low */
#define rcC_FIFO_WrRemain_H		(0x26)		/* FIFO Write Remain Low */
#define rcC_FIFO_WrRemain_L		(0x27)		/* FIFO Write Remain High */
#define rcC_FIFO_ByteRd			(0x28)		/* FIFO Byte Read */
	#define RdRemainValid			BIT(15)
	#define RdRemain			(0x1FFF)
#define rsC_RAM_RdAdrs_HL		(0x30)		/* RAM Read Address High-Low */
#define rcC_RAM_RdAdrs_L		(0x31)		/* RAM Read Address Low */
#define rcC_RAM_RdAdrs_H		(0x30)		/* RAM Read Address High */
#define rcC_RAM_RdControl		(0x32)		/* RAM Read Control */
#define rcC_RAM_RdCount			(0x35)		/* RAM Read Counter */
#define rsC_RAM_WrAdrs_HL		(0x38)		/* RAM Write Address High-Low */
#define rcC_RAM_WrAdrs_L		(0x38)		/* RAM Write Address Low */
#define rcC_RAM_WrAdrs_H		(0x39)		/* RAM Write Address High */
#define rcC_RAM_WrDoor_0		(0x3A)		/* RAM Write Door 0 */
#define rcC_RAM_WrDoor_1		(0x3B)		/* RAM Write Door 1 */


#define rcC_RAM_Rd_00			(0x40)		/* RAM Read 00 */
#define rcC_RAM_Rd_01			(0x41)		/* RAM Read 01 */
#define rcC_RAM_Rd_02			(0x42)		/* RAM Read 02 */
#define rcC_RAM_Rd_03			(0x43)		/* RAM Read 03 */
#define rcC_RAM_Rd_04			(0x44)		/* RAM Read 04 */
#define rcC_RAM_Rd_05			(0x45)		/* RAM Read 05 */
#define rcC_RAM_Rd_06			(0x46)		/* RAM Read 06 */
#define rcC_RAM_Rd_07			(0x47)		/* RAM Read 07 */
#define rcC_RAM_Rd_08			(0x48)		/* RAM Read 08 */
#define rcC_RAM_Rd_09			(0x49)		/* RAM Read 09 */
#define rcC_RAM_Rd_0A			(0x4A)		/* RAM Read 0A */
#define rcC_RAM_Rd_0B			(0x4B)		/* RAM Read 0B */
#define rcC_RAM_Rd_0C			(0x4C)		/* RAM Read 0C */
#define rcC_RAM_Rd_0D			(0x4D)		/* RAM Read 0D */
#define rcC_RAM_Rd_0E			(0x4E)		/* RAM Read 0E */
#define rcC_RAM_Rd_0F			(0x4F)		/* RAM Read 0F */


#define rcC_RAM_Rd_10			(0x50)		/* RAM Read 10 */
#define rcC_RAM_Rd_11			(0x51)		/* RAM Read 11 */
#define rcC_RAM_Rd_12			(0x52)		/* RAM Read 12 */
#define rcC_RAM_Rd_13			(0x53)		/* RAM Read 13 */
#define rcC_RAM_Rd_14			(0x54)		/* RAM Read 14 */
#define rcC_RAM_Rd_15			(0x55)		/* RAM Read 15 */
#define rcC_RAM_Rd_16			(0x56)		/* RAM Read 16 */
#define rcC_RAM_Rd_17			(0x57)		/* RAM Read 17 */
#define rcC_RAM_Rd_18			(0x58)		/* RAM Read 18 */
#define rcC_RAM_Rd_19			(0x59)		/* RAM Read 19 */
#define rcC_RAM_Rd_1A			(0x5A)		/* RAM Read 1A */
#define rcC_RAM_Rd_1B			(0x5B)		/* RAM Read 1B */
#define rcC_RAM_Rd_1C			(0x5C)		/* RAM Read 1C */
#define rcC_RAM_Rd_1D			(0x5D)		/* RAM Read 1D */
#define rcC_RAM_Rd_1E			(0x5E)		/* RAM Read 1E */
#define rcC_RAM_Rd_1F			(0x5F)		/* RAM Read 1F */


#define rcC_DMA0_Config			(0x61)		/* DMA0 Configuration */
#define rcC_DMA0_Control		(0x62)		/* DMA0 Control */

#define rsC_DMA0_Remain_HL		(0x64)		/* DMA0 FIFO Remain High-Low */
#define rcC_DMA0_Remain_L		(0x64)		/* DMA0 FIFO Remain Low */
#define rcC_DMA0_Remain_H		(0x65)		/* DMA0 FIFO Remain High */

#define rsC_DMA0_Count_HHL		(0x68)		/* DMA0 Transfer Byte Counter High/High-Low */
#define rcC_DMA0_Count_HL		(0x68)		/* DMA0 Transfer Byte Counter High/Low */
#define rcC_DMA0_Count_HH		(0x69)		/* DMA0 Transfer Byte Counter High/High */
#define rsC_DMA0_Count_LHL		(0x6A)		/* DMA0 Transfer Byte Counter Low/High-Low */
#define rcC_DMA0_Count_LL		(0x6A)		/* DMA0 Transfer Byte Counter Low/Low */
#define rcC_DMA0_Count_LH		(0x6B)		/* DMA0 Transfer Byte Counter Low/High */
#define rcC_DMA0_RdData_0		(0x6C)		/* DMA0 Read Data High */
#define rcC_DMA0_RdData_1		(0x6D)		/* DMA0 Read Data Low */
#define rcC_DMA0_WrData_0		(0x6E)		/* DMA0 Write Data High */
#define rcC_DMA0_WrData_1		(0x6F)		/* DMA0 Write Data Low */

#define rcC_ModeProtect			(0x71)		/* Mode Protection */
	#define ModeProtectSet			(0x00)
	#define ModeProtectClear		(0x56)
#define rcC_ClkSelect			(0x73)		/* Clock Select */
	#define ClkSource			BIT(7)
	#define ClkSource_CLKIN			(0x80)
	#define ClkFreq_Mask			(0x03)
	#define CLKFreq_24M			(0x01)
	#define CLKFreq_12M			(0x00) //add by kyon
#define rcC_CPU_Config			(0x75)		/* CPU Configuration */
	#define CPU_Endian			BIT(2)
	#define BE_MODE			(0x02)		//add by kyon
#define rcC_CPU_ChgEndian		(0x77)		/* CPU Change Endian */

#define rsC_AREA0_StartAdrs_HL	(0x80)		/* AREA 0 Start Address High-Low */
#define rcC_AREA0_StartAdrs_H	(0x80)		/* AREA 0 Start Address Low */
#define rcC_AREA0_StartAdrs_L	(0x81)		/* AREA 0 Start Address High */
#define rsC_AREA0_EndAdrs_HL	(0x82)		/* AREA 0 End Address High-Low */	
#define rcC_AREA0_EndAdrs_H		(0x82)		/* AREA 0 End Address Low */	
#define rcC_AREA0_EndAdrs_L		(0x83)		/* AREA 0 End Address High */
#define rsC_AREA1_StartAdrs_HL	(0x84)		/* AREA 1 Start Address High-Low */
#define rcC_AREA1_StartAdrs_H	(0x84)		/* AREA 1 Start Address Low */
#define rcC_AREA1_StartAdrs_L	(0x85)		/* AREA 1 Start Address High */
#define rsC_AREA1_EndAdrs_HL	(0x86)		/* AREA 1 End Address High-Low */
#define rcC_AREA1_EndAdrs_H		(0x86)		/* AREA 1 End Address Low */
#define rcC_AREA1_EndAdrs_L		(0x87)		/* AREA 1 End Address High */
#define rsC_AREA2_StartAdrs_HL	(0x88)		/* AREA 2 Start Address High-Low */
#define rcC_AREA2_StartAdrs_H	(0x88)		/* AREA 2 Start Address Low */
#define rcC_AREA2_StartAdrs_L	(0x89)		/* AREA 2 Start Address High */
#define rsC_AREA2_EndAdrs_HL	(0x8A)		/* AREA 2 End Address High-Low */
#define rcC_AREA2_EndAdrs_H		(0x8A)		/* AREA 2 End Address Low */
#define rcC_AREA2_EndAdrs_L		(0x8B)		/* AREA 2 End Address High */
#define rsC_AREA3_StartAdrs_HL	(0x8C)		/* AREA 3 Start Address High-Low */
#define rcC_AREA3_StartAdrs_H	(0x8C)		/* AREA 3 Start Address Low */
#define rcC_AREA3_StartAdrs_L	(0x8D)		/* AREA 3 Start Address High */
#define rsC_AREA3_EndAdrs_HL	(0x8E)		/* AREA 3 End Address High-Low */
#define rcC_AREA3_EndAdrs_H		(0x8E)		/* AREA 3 End Address Low */
#define rcC_AREA3_EndAdrs_L		(0x8F)		/* AREA 3 End Address High */
#define rsC_AREA4_StartAdrs_HL	(0x90)		/* AREA 4 Start Address High-Low */
#define rcC_AREA4_StartAdrs_H	(0x90)		/* AREA 4 Start Address Low */
#define rcC_AREA4_StartAdrs_L	(0x91)		/* AREA 4 Start Address High */
#define rsC_AREA4_EndAdrs_HL	(0x92)		/* AREA 4 End Address High-Low */
#define rcC_AREA4_EndAdrs_H		(0x92)		/* AREA 4 End Address Low */
#define rcC_AREA4_EndAdrs_L		(0x93)		/* AREA 4 End Address High */
#define rsC_AREA5_StartAdrs_HL	(0x94)		/* AREA 5 Start Address High-Low */
#define rcC_AREA5_StartAdrs_H	(0x94)		/* AREA 5 Start Address Low */
#define rcC_AREA5_StartAdrs_L	(0x95)		/* AREA 5 Start Address High */
#define rsC_AREA5_EndAdrs_HL	(0x96)		/* AREA 5 End Address High-Low */
#define rcC_AREA5_EndAdrs_H		(0x96)		/* AREA 5 End Address Low */
#define rcC_AREA5_EndAdrs_L		(0x97)		/* AREA 5 End Address High */

#define rcC_AREAn_FIFO_clr		(0x9F)

#define rcC_AREA0_Join_0		(0xA0)
	#define S_R_JoinCPU_Rd			BIT(1)
	#define S_R_JoinCPU_Wr			BIT(0)
#define rcC_AREA0_Join_1		(0xA1)
#define rcC_AREA1_Join_0		(0xA2)
#define rcC_AREA1_Join_1		(0xA3)
#define rcC_AREA2_Join_0		(0xA4)
#define rcC_AREA2_Join_1		(0xA5)
#define rcC_AREA3_Join_0		(0xA6)
#define rcC_AREA3_Join_1		(0xA7)
#define rcC_AREA4_Join_0		(0xA8)
#define rcC_AREA4_Join_1		(0xA9)
#define rcC_AREA5_Join_0		(0xAA)
#define rcC_AREA5_Join_1		(0xAB)

#define AREAxJoin0Offset		((rcC_AREA5_Join_0 - rcC_AREA0_Join_0) / CH_E)

#define rcC_ClrAREAn_Join_0		(0xAE)
#define rcC_ClrAREAn_Join_1		(0xAF)
/*@}*/

/**
 * @name	rcH_
 * @brief	 Define for Host:
 */
/*@{*/
#define rcH_SIE_IntStat_0		(0x140)		/* Host SIE Interrupt Status */
	#define DetectCon			BIT(4)
	#define DetectDiscon			BIT(3)
	#define DetectRmtWkup			BIT(2)
	#define DetectChirpOK			BIT(1)
	#define DetectChirpNG			BIT(0)
	#define SIE_IntStat_0_AllBit	(DetectCon | DetectDiscon)
	#define SIE_IntEnb_0_AllBit		(DetectCon | DetectDiscon)
#define rcH_SIE_IntStat_1		(0x141)
	#define DisabledCmp				BIT(3)
	#define ResumeCmp				BIT(2)
	#define SuspendCmp				BIT(1)
	#define ResetCmp				BIT(0)
	#define SIE_IntStat_1_AllBit	(DisabledCmp | ResumeCmp | SuspendCmp | ResetCmp)
	#define SIE_IntEnb_1_AllBit		(DisabledCmp | ResumeCmp | SuspendCmp | ResetCmp)
#define rcH_Frame_IntStat		(0x143)		/* Host Frame Interrupt Status */
	#define PortErr					BIT(2)
#define rcH_CHr_IntStat			(0x144)		/* Host CHr Interrupt Status */
#define rcH_CH0_IntStat			(0x145)
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

#define rcH_CHa_IntStat			(0x146)
#define rcH_CHb_IntStat			(0x147)
#define rcH_CHc_IntStat			(0x148)
#define rcH_CHd_IntStat			(0x149)
#define rcH_CHe_IntStat			(0x14A)


#define rcH_SIE_IntEnb_0		(0x150)		/* Host SIE Interrupt Enable */
#define rcH_SIE_IntEnb_1		(0x151)

#define rcH_Frame_IntEnb		(0x153)
#define rcH_CHr_IntEnb			(0x154)		/* Host CHr Interrupt Enable */
#define rcH_CH0_IntEnb			(0x155)		/* Host CH0 Interrupt Enable */
#define rcH_CHa_IntEnb			(0x156)
#define rcH_CHb_IntEnb			(0x157)
#define rcH_CHc_IntEnb			(0x158)
#define rcH_CHd_IntEnb			(0x159)
#define rcH_CHe_IntEnb			(0x15A)

#define rcH_NegoControl_0		(0x160)		/* Host NegoControl */
	#define AutoModeCancel			BIT(7)
	#define HostStateMask			(0x70)
	#define HostState_IDLE			(0x10)
	#define HostState_WAIT_CONNECT		(0x20)
	#define HostState_DISABLE		(0x30)
	#define HostState_USB_OPERATION		(0x50)
	#define HostState_USB_SUSPEND		(0x60)
	#define HostState_USB_RESUME		(0x70)
	#define AutoModeMask			(0x0F)
	#define GoCANCEL			(0x00)
	#define GoIDLE				(0x01)
	#define GoWAIT_CONNECT			(0x02)
	#define GoDISABLED			(0x03)
	#define GoRESET				(0x04)
	#define GoOPERATIONAL			(0x05)
	#define GoSUSPEND			(0x06)
	#define GoRESUME			(0x07)
	#define GoWAIT_CONNECTtoDIS		(0x09)
	#define GoWAIT_CONNECTtoOP		(0x0A)
	#define GoRESETtoOP			(0x0C)
	#define GoSUSPENDtoOP			(0x0E)
	#define GoRESUMEtoOP			(0x0F)
#define rcH_NegoControl_1		(0x162)
	#define PortSpeedMask			(0x30)
	#define PortSpeed_High			(0x00)
	#define PortSpeed_Full			(0x10)
	#define PortSpeed_Low			(0x30)
	#define DisChirpFinish			BIT(1)
	#define RmtWkupDetEnb			BIT(0)
#define rcH_USB_Test			(0x164)
	#define EnHS_Test			BIT(7)
	#define Test_Force_Enable		BIT(4)
	#define Test_SE0_NAK			BIT(3)
	#define TEST_J				BIT(2)
	#define TEST_K				BIT(1)
	#define Test_Packet			BIT(0)

#define rcH_CH0SETUP_0			(0x170)		/* Setup */
#define rcH_CH0SETUP_1			(0x171)
#define rcH_CH0SETUP_2			(0x172)
#define rcH_CH0SETUP_3			(0x173)
#define rcH_CH0SETUP_4			(0x174)
#define rcH_CH0SETUP_5			(0x175)
#define rcH_CH0SETUP_6			(0x176)
#define rcH_CH0SETUP_7			(0x177)

#define rsH_FrameNumber_HL		(0x17E)
#define rcH_FrameNumber_H		(0x17E)
#define rcH_FrameNumber_L		(0x17F)
	#define FrameNumberMask		(0x07FF)

#define rcH_CH0_Config_0		(0x180)		/* Host Channel 0 Configuration */
	#define ACK_Cnt_MASK			(0xF0)
	#define SpeedMode_MASK			(0x0C)
	#define SpeedMode_HS			(0x00)
	#define SpeedMode_FS			(0x04)
	#define SpeedMode_LS			(0x0C)
	#define CHxConfig_Toggle_MASK		(0x02)
	#define CHxConfig_Toggle_0		(0x00)
	#define CHxConfig_Toggle_1		(0x02)
	#define CHxConfig_Toggle		BIT(1)
	#define CHxConfig_TranGo		BIT(0)

#define rcH_CH0_Config_1		(0x181)
	#define CHxTID_MASK			(0xC0)
	#define CHxTID_SETUP			(0x00)
	#define CHxTID_IN			(0x80)
	#define CHxTID_OUT			(0x40)
	#define CHxTranType_ISOC		(0x00)
	#define CHxTranType_BULK		(0x20)
	#define CHxTranType_INT			(0x30)
	#define AutoZerolen				BIT(3)

#define rsH_CH0_MaxPktSize		(0x182)		/* Host Channel 0 Max Packet Size(16bit) */
#define rcH_CH0_MaxPktSize		(0x182)		/* Host Channel 0 Max Packet Size */

#define rsH_CH0_TotalSize_HL	(0x186)		/* Host Channel 0 Total Size(16bit) */
#define rcH_CH0_TotalSize_H		(0x186)		/* Host Channel 0 Total Size */
#define rcH_CH0_TotalSize_L		(0x187)
#define rcH_CH0_HubAdrs			(0x188)		/* Host Channel 0 Hub Address */
	#define S_R_HUB_ADDR_OFFSET(x)	((x) << 4)/* hub register bit offset */
#define rcH_CH0_FuncAdrs		(0x189)		/* Host Channel 0 Function Address */
	#define FuncAdrs_MASK			(0xF0)
	#define EP_Number_MASK			(0x0F)
	#define S_R_FUNC_ADDR_OFFSET(x)	(x << 4)/* FuncAdrs egister bit offset */

#define rcH_CTL_SupportControl	(0x18B)		/* Host ControlTransfer Support */
#define rcH_CH0_ConditionCode	(0x18E)
	#define ConditionCode_MASK		(0x70)
	#define CCode_NoError			(0)
	#define CCode_Stall			((1) << 4)
	#define CCode_OverRun			((2) << 4)
	#define CCode_UnderRun			((3) << 4)
	#define CCode_RetryError		((4) << 4)

#define rcH_CHa_Config_0		(0x190)		/* Host Channel Configuration */
#define rcH_CHa_Config_1		(0x191)
#define rsH_CHa_MaxPktSize_HL	(0x192)		/* Host Channel b Max Packet Size High/Low */
#define rcH_CHa_MaxPktSize_H	(0x192)
#define rcH_CHa_MaxPktSize_L	(0x193)
#define rsH_CHa_TotalSize_HHL	(0x194)		/* Host Channel b Total Size High/High-Low */
#define rcH_CHa_TotalSize_HH	(0x194)
#define rcH_CHa_TotalSize_HL	(0x195)
#define rsH_CHa_TotalSize_LHL	(0x196)		/* Host Channel a Total Size Low/High-Low */
#define rcH_CHa_TotalSize_LH	(0x196)
#define rcH_CHa_TotalSize_LL	(0x197)
#define rcH_CHa_HubAdrs			(0x198)
#define rcH_CHa_FuncAdrs		(0x199)
#define rcH_BO_SupportControl	(0x19A)		/* Host Bulk Only Transfer Support */
#define rcH_CSW_RcvDataSize		(0x19B)
#define rcH_OUT_EP_Control		(0x19C)
#define rcH_IN_EP_Control		(0x19D)
#define rcH_CHa_ConditionCode	(0x19E)		

#define rcH_CHb_Config_0		(0x1A0)
#define rcH_CHb_Config_1		(0x1A1)
#define rsH_CHb_MaxPktSize_HL	(0x1A2)		/* Host Channel b Max Packet Size High/Low */
#define rcH_CHb_MaxPktSize_H	(0x1A2)
#define rcH_CHb_MaxPktSize_L	(0x1A3)
#define rsH_CHb_TotalSize_HHL	(0x1A4)		/* Host Channel b Total Size High/High-Low */
#define rcH_CHb_TotalSize_HH	(0x1A4)
#define rcH_CHb_TotalSize_HL	(0x1A5)
#define rsH_CHb_TotalSize_LHL	(0x1A6)		/* Host Channel a Total Size Low/High-Low */
#define rcH_CHb_TotalSize_LH	(0x1A6)
#define rcH_CHb_TotalSize_LL	(0x1A7)
#define rcH_CHb_HubAdrs			(0x1A8)
#define rcH_CHb_FuncAdrs		(0x1A9)
#define rsH_CHb_Interval_HL		(0x1AA)		/* Host Channel b Interval High/Low */
#define rcH_CHb_Interval_H		(0x1AA)
#define rcH_CHb_Interval_L		(0x1AB)

#define rcH_CHb_ConditionCode	(0x1AE)

#define rcH_CHc_Config_0		(0x1B0)
#define rcH_CHc_Config_1		(0x1B1)
#define rsH_CHc_MaxPktSize_HL		(0x1B2)		/* Host Channel c Max Packet Size High/Low */
#define rcH_CHc_MaxPktSize_H		(0x1B2)
#define rcH_CHc_MaxPktSize_L		(0x1B3)
#define rsH_CHc_TotalSize_HHL		(0x1B4)		/* Host Channel c Total Size High/High-Low */
#define rcH_CHc_TotalSize_HH		(0x1B4)
#define rcH_CHc_TotalSize_HL		(0x1B5)
#define rsH_CHc_TotalSize_LHL		(0x1B6)		/* Host Channel c Total Size Low/High-Low */
#define rcH_CHc_TotalSize_LH		(0x1B6)
#define rcH_CHc_TotalSize_LL		(0x1B7)
#define rcH_CHc_HubAdrs			(0x1B8)
#define rcH_CHc_FuncAdrs		(0x1B9)
#define rsH_CHc_Interval_HL		(0x1BA)		/* Host Channel c Interval High/Low */
#define rcH_CHc_Interval_H		(0x1BA)
#define rcH_CHc_Interval_L		(0x1BB)

#define rcH_CHc_ConditionCode		(0x1BE)

#define rcH_CHd_Config_0		(0x1C0)
#define rcH_CHd_Config_1		(0x1C1)
#define rsH_CHd_MaxPktSize_HL		(0x1C2)		/* Host Channel d Max Packet Size High/Low */
#define rcH_CHd_MaxPktSize_H		(0x1C2)
#define rcH_CHd_MaxPktSize_L		(0x1C3)
#define rsH_CHd_TotalSize_HHL		(0x1C4)		/* Host Channel d Total Size High/High-Low */
#define rcH_CHd_TotalSize_HH		(0x1C4)
#define rcH_CHd_TotalSize_HL		(0x1C5)
#define rsH_CHd_TotalSize_LHL		(0x1C6)		/* Host Channel d Total Size Low/High-Low */
#define rcH_CHd_TotalSize_LH		(0x1C6)
#define rcH_CHd_TotalSize_LL		(0x1C7)
#define rcH_CHd_HubAdrs			(0x1C8)
#define rcH_CHd_FuncAdrs		(0x1C9)
#define rsH_CHd_Interval_HL		(0x1CA)		/* Host Channel d Interval High/Low */
#define rcH_CHd_Interval_H		(0x1CA)
#define rcH_CHd_Interval_L		(0x1CB)

#define rcH_CHd_ConditionCode		(0x1CE)

#define rcH_CHe_Config_0		(0x1D0)
#define rcH_CHe_Config_1		(0x1D1)
#define rsH_CHe_MaxPktSize_HL		(0x1D2)		/* Host Channel e Max Packet Size High/Low */
#define rcH_CHe_MaxPktSize_L		(0x1D2)
#define rcH_CHe_MaxPktSize_H		(0x1D3)
#define rsH_CHe_TotalSize_HHL		(0x1D4)		/* Host Channel e Total Size High/High-Low */
#define rcH_CHe_TotalSize_HL		(0x1D4)
#define rcH_CHe_TotalSize_HH		(0x1D5)
#define rsH_CHe_TotalSize_LHL		(0x1D6)		/* Host Channel e Total Size Low/High-Low */
#define rcH_CHe_TotalSize_LL		(0x1D6)
#define rcH_CHe_TotalSize_LH		(0x1D7)
#define rcH_CHe_HubAdrs			(0x1D8)
#define rcH_CHe_FuncAdrs		(0x1D9)
#define rsH_CHe_Interval_HL		(0x1DA)		/* Host Channel e Interval High/Low */
#define rcH_CHe_Interval_L		(0x1DA)
#define rcH_CHe_Interval_H		(0x1DB)

#define rcH_CHe_ConditionCode		(0x1DE)

#define CHxConfig_0Offset		((rcH_CHe_Config_0 - rcH_CH0_Config_0) / CH_E)
/*@}*/

/**
 * @name	_FIFO_Clr
 * @brief	bit assign:
 */
/*@{*/
/* rcC_AREAn_FIFO_clr (0x9F)  AREA n FIFO Clear */
#define S_R_AREA0_FIFO_Clr		0x01
#define S_R_AREA1_FIFO_Clr		0x02
#define S_R_AREA2_FIFO_Clr		0x04
#define S_R_AREA3_FIFO_Clr		0x08
#define S_R_AREA4_FIFO_Clr		0x10
#define S_R_AREA5_FIFO_Clr		0x20
#define S_R_AllFIFO_Clr			(S_R_AREA1_FIFO_Clr | S_R_AREA2_FIFO_Clr | S_R_AREA3_FIFO_Clr | \
					 S_R_AREA4_FIFO_Clr | S_R_AREA5_FIFO_Clr)

/* rcC_AREAx_Join_1 (0xA1, 0xA3, 0xA5, 0xA7, 0xA9, 0xAB)  AREA x Join 1 */
#define S_R_JoinEP0_CH0			0x01
#define S_R_JoinEPa_CHa			0x02
#define S_R_JoinEPb_CHb			0x04
#define S_R_JoinEPc_CHc			0x08
#define S_R_JoinEPd_CHd			0x10
#define S_R_JoinEPe_CHe			0x20
/*@}*/

/**
 * @name	rc_
 * @brief	redefine:
 */
/*@{*/
#define rc_Reset			rcC_ChipReset
#define rc_PM_Control			rcC_PM_Control
#define rc_PM_Control_State		rcC_PM_Control
#define ACT_HOST			ACTIVE
#define GoActHost			GoActive
#define ResetHTM			MTMReset
#define rc_ChipConfig			rcC_CPU_Config

#define rsH_BUF0_StartAdrs_HL		rsC_AREA0_StartAdrs_HL
#define rcH_BUF0_StartAdrs_L		rcC_AREA0_StartAdrs_L
#define rcH_BUF0_StartAdrs_H		rcC_AREA0_StartAdrs_H
#define rsH_BUF0_EndAdrs_HL		rsC_AREA0_EndAdrs_HL
#define rcH_BUF0_EndAdrs_L		rcC_AREA0_EndAdrs_L
#define rcH_BUF0_EndAdrs_H		rcC_AREA0_EndAdrs_H
#define rsH_BUFa_StartAdrs_HL		rsC_AREA1_StartAdrs_HL
#define rcH_BUFa_StartAdrs_L		rcC_AREA1_StartAdrs_L
#define rcH_BUFa_StartAdrs_H		rcC_AREA1_StartAdrs_H
#define rsH_BUFa_EndAdrs_HL		rsC_AREA1_EndAdrs_HL
#define rcH_BUFa_EndAdrs_L		rcC_AREA1_EndAdrs_L
#define rcH_BUFa_EndAdrs_H		rcC_AREA1_EndAdrs_H
#define rsH_BUFb_StartAdrs_HL		rsC_AREA2_StartAdrs_HL
#define rcH_BUFb_StartAdrs_L		rcC_AREA2_StartAdrs_L
#define rcH_BUFb_StartAdrs_H		rcC_AREA2_StartAdrs_H
#define rsH_BUFb_EndAdrs_HL		rsC_AREA2_EndAdrs_HL
#define rcH_BUFb_EndAdrs_L		rcC_AREA2_EndAdrs_L
#define rcH_BUFb_EndAdrs_H		rcC_AREA2_EndAdrs_H
#define rsH_BUFc_StartAdrs_HL		rsC_AREA3_StartAdrs_HL
#define rcH_BUFc_StartAdrs_L		rcC_AREA3_StartAdrs_L
#define rcH_BUFc_StartAdrs_H		rcC_AREA3_StartAdrs_H
#define rsH_BUFc_EndAdrs_HL		rsC_AREA3_EndAdrs_HL
#define rcH_BUFc_EndAdrs_L		rcC_AREA3_EndAdrs_L
#define rcH_BUFc_EndAdrs_H		rcC_AREA3_EndAdrs_H
#define rsH_BUFd_StartAdrs_HL		rsC_AREA4_StartAdrs_HL
#define rcH_BUFd_StartAdrs_L		rcC_AREA4_StartAdrs_L
#define rcH_BUFd_StartAdrs_H		rcC_AREA4_StartAdrs_H
#define rsH_BUFd_EndAdrs_HL		rsC_AREA4_EndAdrs_HL
#define rcH_BUFd_EndAdrs_L		rcC_AREA4_EndAdrs_L
#define rcH_BUFd_EndAdrs_H		rcC_AREA4_EndAdrs_H
#define rsH_BUFe_StartAdrs_HL		rsC_AREA5_StartAdrs_HL
#define rcH_BUFe_StartAdrs_L		rcC_AREA5_StartAdrs_L
#define rcH_BUFe_StartAdrs_H		rcC_AREA5_StartAdrs_H
#define rsH_BUFe_EndAdrs_HL		rsC_AREA5_EndAdrs_HL
#define rcH_BUFe_EndAdrs_L		rcC_AREA5_EndAdrs_L
#define rcH_BUFe_EndAdrs_H		rcC_AREA5_EndAdrs_H

#define rcH_BUFn_Control		rcC_AREAn_FIFO_clr
#define rcH_BUFr_FIFO_Clr		rcC_AREAn_FIFO_clr
#define rcH_BUF0_Join			rcC_AREA0_Join_0
#define rcH_BUFa_Join			rcC_AREA1_Join_0
#define rcH_BUFb_Join			rcC_AREA2_Join_0
#define rcH_BUFc_Join			rcC_AREA3_Join_0
#define rcH_BUFd_Join			rcC_AREA4_Join_0
#define rcH_BUFe_Join			rcC_AREA5_Join_0

#define S_R_BUFx_Config_0_Offset	CHxConfig_0Offset
#define S_R_BUFx_Join_Offset		AREAxJoin0Offset

#define rcH_ClrAllBUFn_Join		rcC_ClrAREAn_Join_0


/* rcC_AREAn_FIFO_clr (0x9F)  AREA n FIFO Clear */
#define S_R_BUFa_FIFO_Clr		S_R_AREA1_FIFO_Clr
#define S_R_BUFb_FIFO_Clr		S_R_AREA2_FIFO_Clr
#define S_R_BUFc_FIFO_Clr		S_R_AREA3_FIFO_Clr
#define S_R_BUFd_FIFO_Clr		S_R_AREA4_FIFO_Clr
#define S_R_BUFe_FIFO_Clr		S_R_AREA5_FIFO_Clr
#define S_R_FIFO_ALL_CLR		S_R_AllFIFO_Clr

#define S_R_CH_FIFO_CLR(x)\
	S_R_REGS8(rcH_BUFr_FIFO_Clr) = (1 << (x));\

#define S_R_CH0_FIFO_CLR(x) S_R_CH_FIFO_CLR(x)

#define S_R_CH_JOIN_OFFSET(x)	((x) << 1)	/* CH register offset */
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

#define HOST_FIFO_CH0_START_ADRS	0x0030
#define HOST_FIFO_CH0_END_ADRS		HOST_FIFO_CH0_START_ADRS + HOST_FIFO_CH0_LEN
#define HOST_FIFO_CHA_START_ADRS	HOST_FIFO_CH0_END_ADRS
#define HOST_FIFO_CHA_END_ADRS		HOST_FIFO_CHA_START_ADRS + HOST_FIFO_CHA_LEN
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
