/**
 * @file	s1r72v05_udc-regs.h
 * @brief	S1R72V05 USB Contoroller Driver(USB Device)
 * @date	2007/02/27
 * Copyright (C)SEIKO EPSON CORPORATION 2003-2007. All rights reserved.
 */
#ifndef S1R72V05_USBC_H		/* include guard */
#define S1R72V05_USBC_H

/*-------------------/
/ USB Device         /
/-------------------*/
/**
 * @name S1R72_EP0_XXX
 * @brief	definitions of FIFO address.
/*@{*/
#ifdef S1R72_MASS_STORAGE_ONLY 
#define S1R72_EP0_MAX_PKT		64		/** max packet size of endpoint 0 */
#define S1R72_EPA_MAX_PKT		2048	/** max packet size of endpoint A */
#define S1R72_EPB_MAX_PKT		2048	/** max packet size of endpoint B */
#define S1R72_EPC_MAX_PKT		8		/** max packet size of endpoint C */
#define S1R72_EP0_START_ADRS	0x0000	/** EP0 FIFO start address */
#define S1R72_EPA_START_ADRS	0x01C0	/** EPA FIFO start address */
#else
#define S1R72_EP0_MAX_PKT		64		/** max packet size of endpoint 0 */
#define S1R72_EPA_MAX_PKT		1024	/** max packet size of endpoint A */
#define S1R72_EPB_MAX_PKT		1024	/** max packet size of endpoint B */
#define S1R72_EPC_MAX_PKT		1024	/** max packet size of endpoint C */
#define S1R72_EP0_START_ADRS	0x0000	/** EP0 FIFO start address */
#define S1R72_EPA_START_ADRS	0x0600	/** EPA FIFO start address */
#endif
/** EPB FIFO start address */
#define S1R72_EPB_START_ADRS	S1R72_EPA_START_ADRS + S1R72_EPA_MAX_PKT
/** EPC FIFO start address */
#define S1R72_EPC_START_ADRS	S1R72_EPB_START_ADRS + S1R72_EPB_MAX_PKT
/** EPC FIFO end address */
#define S1R72_EPC_END_ADRS		S1R72_EPC_START_ADRS + S1R72_EPC_MAX_PKT
/*@}*/

/**
 * @name rcD_S1R72_XXX
 * @brief	definitions of register address and bits.
/*@{*/
#define S1R72_WAKEUP_TIM_2MSEC		0xBBCC	/* = BBCC = 48,076 x 41.67ns = 2msec */
/*@}*/

/**
 * @name S1R72_EPXXX
 * @brief	definitions of endpoint settings macro.
/*@{*/
#define S1R72_EPFIFO_CLR(x,y)\
if(y > 0){\
	rcD_RegWrite8(x, rcD_S1R72_EPrFIFO_Clr, (1 << ((y) - 1)));\
} else { \
	rcD_RegWrite8(x, rcD_S1R72_EPnControl, S1R72_EP0FIFO_Clr);}
#define S1R72_EP0FIFO_CLR(x)\
	rcD_RegWrite8(x, rcD_S1R72_EPnControl, S1R72_EP0FIFO_Clr);
/** FIFO Join clear macro */
#define S1R72_EPJOINCPU_CLR(x)  \
	        rcD_RegWrite8(x, rcD_S1R72_ClrAllEPnJoin , \
	                (S1R72_ClrJoinCPU_Rd | S1R72_ClrJoinCPU_Wr))
/** epx AREAxJoin_1 enabling macro */
#define S1R72_AREAxJOIN_ENB(x,y)
/** epx AREAxJoin_1 disable macro */
#define S1R72_AREAxJOIN_DIS(x,y)
/** ep offset address */
#define S1R72_EPxOFFSET16(x)     \
	        ((x - 1) * 0x10)
#define S1R72_EPxOFFSET8(x)     \
	        ((x - 1) * 0x08)
#define S1R72_EPxOFFSET4(x)     \
	        ((x - 1) * 0x04)
#define S1R72_EPxOFFSET2(x)     \
	        ((x - 1) * 0x02)
#define S1R72_EPINT_ENB(x)	(1 << ((x) - 1))
/*@}*/

/**
 * @name rcD_S1R72_XXX
 * @brief	definitions of register address and bits.
/*@{*/
	#define S1R72_VBUS				BIT(7)
	#define S1R72_FSxHS				BIT(6)
	#define S1R72_LineState			(0x03)

#define rcD_S1R72_SIE_IntStat		(0xE0)	/* Device SIE Interrupt Status		*/
	#define S1R72_NonJ				BIT(6)
	#define S1R72_RcvSOF			BIT(5)
	#define S1R72_DetectRESET		BIT(4)
	#define S1R72_DetectSUSPEND		BIT(3)
	#define S1R72_ChirpCmp			BIT(2)
	#define S1R72_RestoreCmp		BIT(1)
	#define S1R72_SetAddressCmp		BIT(0)

#define rcD_S1R72_FIFO_IntStat		(0xE2)	/* Device FIFO Interrupt Status		*/
	#define S1R72_DescriptorCmp		BIT(7)
	#define S1R72_FIFO_IDE_Cmp		BIT(6)
	#define S1R72_FIFO1_Cmp			BIT(5)
	#define S1R72_FIFO0_Cmp			BIT(4)

#define rcD_S1R72_BulkIntStat		(0xE3)	/* Device Bulk Interrupt Status		*/
	#define S1R72_CBW_Cmp			BIT(7)
	#define S1R72_CBW_LengthErr		BIT(6)
	#define S1R72_CBW_Err			BIT(5)
	#define S1R72_CSW_Cmp			BIT(3)
	#define S1R72_CSW_Err			BIT(2)

#define rcD_S1R72_EPrIntStat		(0xE4)	/* Device EPr Interrupt Status		*/
	#define S1R72_EPcIntStat		BIT(2)
	#define S1R72_EPbIntStat		BIT(1)
	#define S1R72_EPaIntStat		BIT(0)
	#define S1R72_EPxIntStat_NONE	(0x00)

#define rcD_S1R72_EP0IntStat		(0xE5)	/* Device EP0 Interrupt Status		*/
#define rcD_S1R72_EPaIntStat		(0xE6)	/* Device EPa Interrupt Status		*/
#define rcD_S1R72_EPbIntStat		(0xE7)	/* Device EPb Interrupt Status		*/
#define rcD_S1R72_EPcIntStat		(0xE8)	/* Device EPc Interrupt Status		*/
	#define S1R72_DescriptorCmp		BIT(7)
	#define S1R72_OUT_ShortACK		BIT(6)
	#define S1R72_IN_TranACK		BIT(5)
	#define S1R72_OUT_TranACK		BIT(4)
	#define S1R72_IN_TranNAK		BIT(3)
	#define S1R72_OUT_TranNAK		BIT(2)
	#define S1R72_IN_TranErr		BIT(1)
	#define S1R72_OUT_TranErr		BIT(0)
        #define S1R72_EP_ALL_INT                ( S1R72_DescriptorCmp\
			                        | S1R72_OUT_ShortACK    | S1R72_IN_TranACK\
			                        | S1R72_OUT_TranACK             | S1R72_IN_TranNAK\
			                        | S1R72_OUT_TranNAK             | S1R72_IN_TranErr\
			                        | S1R72_OUT_TranErr)

#define rcD_S1R72_SIE_IntEnb		(0xF0)	/* Device SIE Interrupt Enable		*/
	#define S1R72_EnNonJ			BIT(6)
	#define S1R72_EnRcvSOF			BIT(5)
	#define S1R72_EnDetectRESET		BIT(4)
	#define S1R72_EnDetectSUSPEND	BIT(3)
	#define S1R72_EnChirpCmp		BIT(2)
	#define S1R72_EnRestoreCmp		BIT(1)
	#define S1R72_EnSetAddressCmp	BIT(0)

#define rcD_S1R72_FIFO_IntEnb		(0xF2)	/* Device FIFO Interrupt Enable		*/
	#define S1R72_EnDescriptorCmp	BIT(7)
	#define S1R72_EnFIFO_IDE_Cmp	BIT(6)
	#define S1R72_EnFIFO1_Cmp		BIT(5)
	#define S1R72_EnFIFO0_Cmp		BIT(4)
	#define S1R72_EnFIFO_NotEmpty	BIT(2)
	#define S1R72_EnFIFO_Full		BIT(1)
	#define S1R72_EnFIFO_Empty		BIT(0)

#define rcD_S1R72_BulkIntEnb		(0xF3)	/* Device Bulk Interrupt Enable		*/
	#define S1R72_EnCBW_Cmp			BIT(7)
	#define S1R72_EnCBW_LengthErr	BIT(6)
	#define S1R72_EnCBW_Err			BIT(5)
	#define S1R72_EnCSW_Cmp			BIT(3)
	#define S1R72_EnCSW_Err			BIT(2)

#define rcD_S1R72_EPrIntEnb			(0xF4)	/* Device EPr Interrupt Enable		*/
	#define S1R72_EnEPcIntStat		BIT(2)
	#define S1R72_EnEPbIntStat		BIT(1)
	#define S1R72_EnEPaIntStat		BIT(0)

#define rcD_S1R72_EP0IntEnb			(0xF5)	/* Device EP0 Interrupt Enable		*/
#define rcD_S1R72_EPaIntEnb			(0xF6)	/* Device EPa Interrupt Enable		*/
#define rcD_S1R72_EPbIntEnb			(0xF7)	/* Device EPb Interrupt Enable		*/
#define rcD_S1R72_EPcIntEnb			(0xF8)	/* Device EPc Interrupt Enable		*/
	#define S1R72_EnOUT_ShortACK	BIT(6)
	#define S1R72_EnIN_TranACK		BIT(5)
	#define S1R72_EnOUT_TranACK		BIT(4)
	#define S1R72_EnIN_TranNAK		BIT(3)
	#define S1R72_EnOUT_TranNAK		BIT(2)
	#define S1R72_EnIN_TranErr		BIT(1)
	#define S1R72_EnOUT_TranErr		BIT(0)

#define rcD_S1R72_Reset				(0x100)	/* Device Reset						*/
	#define S1R72_ResetDTM			BIT(0)

#define rcD_S1R72_NegoControl		(0x102)	/* Device Negotiation Control		*/
	#define S1R72_DisBusDetect		BIT(7)
	#define S1R72_EnAutoNego		BIT(6)
	#define S1R72_InSUSPEND			BIT(5)
	#define S1R72_DisableHS			BIT(4)
	#define S1R72_SendWakeup		BIT(3)
	#define S1R72_RestoreUSB		BIT(2)
	#define S1R72_GoChirp			BIT(1)
	#define S1R72_ActiveUSB			BIT(0)

#define rcD_S1R72_ClrAllEPnJoin		(0x104)	/* Device Clear All EPn Join		*/
	#define S1R72_ClrJoinIDE		BIT(7)
	#define S1R72_ClrJoinFIFO_Stat	BIT(6)
	#define S1R72_ClrJoinDMA1		BIT(3)
	#define S1R72_ClrJoinDMA0		BIT(2)
	#define S1R72_ClrJoinCPU_Rd		BIT(1)
	#define S1R72_ClrJoinCPU_Wr		BIT(0)

#define rcD_S1R72_XcvrControl		(0x105)	/* Device Xcvr Control				*/
	#define S1R72_TermSelect				BIT(7)
	#define S1R72_XcvrSelect				BIT(6)
	#define S1R72_OpMode				(0x03)
	#define S1R72_OpMode_PowerDown			(0x03)
	#define S1R72_OpMode_DisableBitStuffing	(0x02)
	#define S1R72_OpMode_NonDriving			(0x01)
	#define S1R72_OpMode_Normal				(0x00)

#define rcD_S1R72_USB_Test			(0x106)	/* Device USB Test					*/
	#define S1R72_EnHS_Test			BIT(7)
	#define S1R72_Test_SE0_NAK		BIT(3)
	#define S1R72_Test_J			BIT(2)
	#define S1R72_Test_K			BIT(1)
	#define S1R72_Test_Packet		BIT(0)

#define rcD_S1R72_EPrFIFO_Clr		(0x109)	/* Device EPr FIFO Clear			*/
	#define S1R72_EPcFIFO_Clr		BIT(2)
	#define S1R72_EPbFIFO_Clr		BIT(1)
	#define S1R72_EPaFIFO_Clr		BIT(0)

#define rcD_S1R72_EPnControl		(0x108)	/* Device EPn Control				*/
	#define S1R72_AllForceNAK		BIT(7)
	#define S1R72_EPrForceSTALL		BIT(6)
	#define S1R72_AllFIFO_Clr		BIT(5)
	#define S1R72_EP0FIFO_Clr		BIT(0)

#define rcD_S1R72_BulkOnlyControl	(0x10A)	/* Device BulkOnly Control			*/
	#define S1R72_AutoForceNAK_CBW	BIT(7)
	#define S1R72_GoCBW_Mode		BIT(2)
	#define S1R72_GoCSW_Mode		BIT(1)

#define rcD_S1R72_BulkOnlyConfig	(0x10B)	/* Device BulkOnly Config			*/

#define rcD_S1R72_EP0SETUP_0		(0x110)	/* Device EP0 SETUP 0				*/
#define rcD_S1R72_EP0SETUP_1		(0x111)	/* Device EP0 SETUP 1 				*/
#define rcD_S1R72_EP0SETUP_2		(0x112)	/* Device EP0 SETUP 2 				*/
#define rcD_S1R72_EP0SETUP_3		(0x113)	/* Device EP0 SETUP 3 				*/
#define rcD_S1R72_EP0SETUP_4		(0x114)	/* Device EP0 SETUP 4 				*/
#define rcD_S1R72_EP0SETUP_5		(0x115)	/* Device EP0 SETUP 5 				*/
#define rcD_S1R72_EP0SETUP_6		(0x116)	/* Device EP0 SETUP 6 				*/
#define rcD_S1R72_EP0SETUP_7		(0x117)	/* Device EP0 SETUP 7 				*/
#define rcD_S1R72_USB_Address		(0x118)	/* Device USB Address		 		*/

#define rcD_S1R72_SETUP_Control		(0x11A)	/* Device SETUP Control				*/
	#define S1R72_ProtectEP0		BIT(0)

#define rsD_S1R72_FrameNumber		(0x11E)	/* Device FrameNumber				*/
#define rcD_S1R72_FrameNumber_H		(0x11E)	/* Device FrameNumber High			*/
#define rcD_S1R72_FrameNumber_L		(0x11F)	/* Device FrameNumber Low 			*/
	#define S1R72_FrameNumber		(0x03FF)
	#define S1R72_FrameNumber_H		(0x03)
	#define S1R72_FrameNumber_L		(0xFF)

#define rcD_S1R72_EP0MaxSize		(0x120)	/* Device EP0 Max Packet Size		*/

#define rcD_S1R72_EP0Control		(0x121)	/* Device EP0 Control		 		*/
	#define S1R72_EP0INxOUT			BIT(7)
        #define S1R72_EP0INxOUT_IN                      (0x80)
        #define S1R72_EP0INxOUT_OUT             (0x00)
	#define S1R72_ReplyDescriptor	BIT(0)

#define rcD_S1R72_EP0ControlIN		(0x122)	/* Device EP0 Control IN 			*/
	#define S1R72_EnShortPkt		BIT(6)
	#define S1R72_ToggleStat		BIT(4)
	#define S1R72_ToggleSet			BIT(3)
	#define S1R72_ToggleClr			BIT(2)
	#define S1R72_ForceNAK			BIT(1)
	#define S1R72_ForceSTALL		BIT(0)

#define rcD_S1R72_EP0ControlOUT		(0x123)	/* Device EP0 Control OUT 			*/
	#define S1R72_AutoForceNAK		BIT(7)
	#define S1R72_ToggleStat		BIT(4)
	#define S1R72_ToggleSet			BIT(3)
	#define S1R72_ToggleClr			BIT(2)
	#define S1R72_ForceNAK			BIT(1)
	#define S1R72_ForceSTALL		BIT(0)

#define rcD_S1R72_EP0Join			(0x125)	/* Device EP0 Join		 			*/
	#define S1R72_JoinFIFO_Stat		BIT(6)
	#define S1R72_JoinDMA1			BIT(3)
	#define S1R72_JoinDMA0			BIT(2)
	#define S1R72_JoinCPU_Rd		BIT(1)
	#define S1R72_JoinCPU_Wr		BIT(0)

#define rsD_S1R72_EPaMaxSize		(0x130)	/* Device EPa Max Packet Size Low	*/
#define rcD_S1R72_EPaMaxSize_H		(0x130)	/* Device EPa Max Packet Size Low	*/
#define rcD_S1R72_EPaMaxSize_L		(0x131)	/* Device EPa Max Packet Size Low	*/

#define rcD_S1R72_EPaConfig_0		(0x132)	/* Device EPa Configuration 0		*/
	#define S1R72_INxOUT			BIT(7)
	#define S1R72_IntEP_Mode		BIT(6)
	#define S1R72_EnEndpoint		BIT(5)
	#define S1R72_EndpointNumber	(0x0F)
        #define S1R72_DIR_IN            (0x80)

#define rcD_S1R72_EPaControl		(0x134)	/* Device EPa Control				*/
	#define S1R72_DisAF_NAK_Short	BIT(5)
	#define S1R72_ForceSTALL		BIT(0)

#define rcD_S1R72_EPaJoin			(0x135)	/* Device EPa Join					*/
	#define S1R72_JoinIDE			BIT(7)
	#define S1R72_JoinCPU_Rd		BIT(1)
	#define S1R72_JoinCPU_Wr		BIT(0)

#define rsD_S1R72_EPbMaxSize		(0x140)	/* Device EPb Max Packet Size High	*/
#define rcD_S1R72_EPbMaxSize_H		(0x140)	/* Device EPb Max Packet Size High	*/
#define rcD_S1R72_EPbMaxSize_L		(0x141)	/* Device EPb Max Packet Size Low	*/
#define rcD_S1R72_EPbConfig_0		(0x142)	/* Device EPb Configuration 0		*/
#define rcD_S1R72_EPbControl		(0x144)	/* Device EPb Control	 			*/
#define rcD_S1R72_EPbJoin			(0x145)	/* Device EPb Join		 			*/

#define rsD_S1R72_EPcMaxSize		(0x150)	/* Device EPc Max Packet Size High	*/
#define rcD_S1R72_EPcMaxSize_H		(0x150)	/* Device EPc Max Packet Size High	*/
#define rcD_S1R72_EPcMaxSize_L		(0x151)	/* Device EPc Max Packet Size Low	*/
#define rcD_S1R72_EPcConfig_0		(0x152)	/* Device EPc Configuration 0		*/
#define rcD_S1R72_EPcControl		(0x154)	/* Device EPc Control	 			*/
#define rcD_S1R72_EPcJoin			(0x155)	/* Device EPc Join					*/

#define rcD_S1R72_DescAdrs_H		(0x160)	/* Device Descriptor Address High	*/
#define rcD_S1R72_DescAdrs_L		(0x161)	/* Device Descriptor Address Low	*/
#define rcD_S1R72_DescSize_H		(0x162)	/* Device Descriptor Size High		*/
#define rcD_S1R72_DescSize_L		(0x163)	/* Device Descriptor Size Low		*/
#define rcD_S1R72_DMA0_FIFO_Control	(0x170)	/* Device DMA0 FIFO Control			*/
#define rcD_S1R72_DMA1_FIFO_Control	(0x172)	/* Device DMA1 FIFO Control			*/
#define rsD_S1R72_EPaStartAdrs		(0x184)	/* Device EPa Start Address High	*/
#define rcD_S1R72_EPaStartAdrs_H	(0x184)	/* Device EPa Start Address High	*/
#define rcD_S1R72_EPaStartAdrs_L	(0x185)	/* Device EPa Start Address Low		*/
#define rsD_S1R72_EPbStartAdrs		(0x188)	/* Device EPb Start Address High	*/
#define rcD_S1R72_EPbStartAdrs_H	(0x188)	/* Device EPb Start Address High	*/
#define rcD_S1R72_EPbStartAdrs_L	(0x189)	/* Device EPb Start Address Low		*/
#define rsD_S1R72_EPcStartAdrs		(0x18C)	/* Device EPc Start Address High	*/
#define rcD_S1R72_EPcStartAdrs_H	(0x18C)	/* Device EPc Start Address High	*/
#define rcD_S1R72_EPcStartAdrs_L	(0x18D)	/* Device EPc Start Address Low		*/
#define rsD_S1R72_EPcEndAdrs		(0x18E)	/* Device EPc End Address High		*/
#define rcD_S1R72_EPcEndAdrs_H		(0x18E)	/* Device EPc End Address High		*/
#define rcD_S1R72_EPcEndAdrs_L		(0x18F)	/* Device EPc End Address Low		*/
/*@}*/

/**
 * @name S1R72_GD_EP_NAME
 * @brief	endpoint name.
 */
/*****************************************
 * definitions of "enum"
 *****************************************/
enum S1R72_GD_EP_NAME { 
	S1R72_GD_EP0	= 0,
	S1R72_GD_EPA,
	S1R72_GD_EPB,
	S1R72_GD_EPC,
	S1R72_MAX_ENDPOINT
};

#endif	/* S1R72V05_USBC_H */
