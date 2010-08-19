/**
 * @file	s1r72v05-hcd.c
 * @brief	S1R72V05 HCD (Host Controller Driver) for USB
 * @date	2007/04/17
 * @author	Luxun9 project team
 * Copyright (C)SEIKO EPSON CORPORATION. 2006-2007 All rights reserved.
 * This file is licenced under GPLv2.
 */

#include <linux/module.h>
#ifdef	CONFIG_PCI
#include <linux/pci.h>
#endif
#include <linux/time.h>
#include <linux/proc_fs.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/usb.h>
#include <linux/usb_ch9.h>
#include <asm/io.h>
#include <asm/irq.h>
#include "../core/hcd.h"

static const char	hcd_name[] = "S1R72V05-hcd";
#define	DRV_NAME	"s1r72v05"

#undef	CONFIG_USB_V17_HCD
#undef	CONFIG_USB_V17_HCD_MODULE

#define	HC_INIT	v05_hc_init
#define	HC_EXIT	v05_hc_exit


#include "s1r72v05-regs.h"
#include "s1r72xxx-hcd.h"
#include "s1r72xxx-hcd.c"
#include "s1r72xxx-q.c"
#include "s1r72xxx-hub.c"

S_R_CH_INFTBL	ch_info_tbl[CH_MAX] = {
	{S_R_CH_TR_CONTROL, HOST_FIFO_CH0_LEN},					/* CH0 */
	{S_R_CH_TR_BULK, HOST_FIFO_CHA_LEN},					/* CHa */
	{S_R_CH_TR_BULK | S_R_CH_TR_INT, HOST_FIFO_CHB_LEN},	/* CHb */
	{S_R_CH_TR_BULK | S_R_CH_TR_INT, HOST_FIFO_CHC_LEN},	/* CHc */
	{S_R_CH_TR_BULK | S_R_CH_TR_INT, HOST_FIFO_CHD_LEN},	/* CHd */
	{S_R_CH_TR_BULK | S_R_CH_TR_INT, HOST_FIFO_CHE_LEN}		/* CHe */
};

unsigned char   automode_cmp_tbl[GoRESUMEtoOP + 2] =
{
	AUTO_NOT_USED,		/* GoCANCEL				(0x00) */
	AUTO_NOT_USED,		/* GoIDLE				(0x01) */
	AUTO_NOT_USED,		/* GoWAIT_CONNECT		(0x02) */
	AUTO_DISABLED_CMP,	/* GoDISABLED			(0x03) */
	AUTO_NOT_USED,		/* GoRESET				(0x04) */
	AUTO_NOT_USED,		/* GoOPERATIONAL		(0x05) */
	AUTO_SUSPEND_CMP,	/* GoSUSPEND			(0x06) */
	AUTO_NOT_USED,		/* GoRESUME				(0x07) */
	AUTO_NOT_USED,		/* reserve				(0x08) */
	AUTO_DISABLED_CMP,	/* GoWAIT_CONNECTtoDIS	(0x09) */
	AUTO_NOT_USED,		/* GoWAIT_CONNECTtoOP	(0x0A) */
	AUTO_NOT_USED,		/* reserve				(0x0B) */
	AUTO_RESET_CMP,		/* GoRESETtoOP			(0x0C) */
	AUTO_NOT_USED,		/* reserve				(0x0D) */
	AUTO_NOT_USED,		/* GoSUSPENDtoOP		(0x0E) */
	AUTO_RESUME_CMP,	/* GoRESUMEtoOP			(0x0F) */
	AUTO_NOT_USED,		/* NONE					(0xFF) */
};

/**
 * @brief       - initialize CH information.
 * @param       *hcd;   struct of usb_hcd
 * @retval      return 0;       complete
 *                              if error return error code
 */
void init_ch_info(struct hcd_priv *hcd)
{
	unsigned char	ch_ct;		/* CH counter */

	DPRINT(DBG_INIT,"%s:enter\n",__FUNCTION__);

	/**
	 * 1. Init all CH informations.
	 */
	for (ch_ct = CH_0; ch_ct < CH_MAX ; ch_ct ++){
		/* set initial values */
		hcd->ch_info[ch_ct].ch_no = ch_ct;
		hcd->ch_info[ch_ct].tran_type = ch_info_tbl[ch_ct].tran_type;
		hcd->ch_info[ch_ct].dir = S_R_CH_DIR_UNKNOWN;
		hcd->ch_info[ch_ct].fifo_size = ch_info_tbl[ch_ct].fifo_size;
		hcd->ch_info[ch_ct].status = S_R_CH_ST_IDLE;
		INIT_LIST_HEAD(&hcd->ch_info[ch_ct].urb_list);
		hcd->ch_info[ch_ct].ep = NULL;
		hcd->ch_info[ch_ct].dev_addr = 0;
		hcd->ch_info[ch_ct].ep_no = 0;
		hcd->ch_info[ch_ct].hub_addr = 0;
		hcd->ch_info[ch_ct].port_no = 0;
		hcd->ch_info[ch_ct].is_short = S_R_CH_LAST_ISNOT_SHORT;
		hcd->ch_info[ch_ct].cur_tran_type = S_R_CH_TR_NONE;
		/* set register address */
		hcd->ch_info[ch_ct].CHxIntStat		=rcH_CH0_IntStat + ch_ct;
		hcd->ch_info[ch_ct].CHxIntEnb		=rcH_CH0_IntEnb + ch_ct;
		hcd->ch_info[ch_ct].CHxConfig_0
			=rcH_CH0_Config_0 + S_R_CH_OFFSET(ch_ct);
		hcd->ch_info[ch_ct].CHxConfig_1
			=rcH_CH0_Config_1 + S_R_CH_OFFSET(ch_ct);
		hcd->ch_info[ch_ct].CHxMaxPktSize
			=rsH_CH0_MaxPktSize + S_R_CH_OFFSET(ch_ct);
		if (ch_ct == CH_0){
			hcd->ch_info[ch_ct].CHxTotalSize_H	= 0;
		} else {
			hcd->ch_info[ch_ct].CHxTotalSize_H
				=rsH_CH0_TotalSize_HL - 2 + S_R_CH_OFFSET(ch_ct);
		}
		hcd->ch_info[ch_ct].CHxTotalSize_L
			=rsH_CH0_TotalSize_HL + S_R_CH_OFFSET(ch_ct);
		hcd->ch_info[ch_ct].CHxHubAdrs
			=rcH_CH0_HubAdrs + S_R_CH_OFFSET(ch_ct);
		hcd->ch_info[ch_ct].CHxFuncAdrs
			=rcH_CH0_FuncAdrs + S_R_CH_OFFSET(ch_ct);

		if (ch_ct == CH_0){ 
			hcd->ch_info[ch_ct].CTL_SUP_CONT	= rcH_CTL_SupportControl;
		} else {
			hcd->ch_info[ch_ct].CTL_SUP_CONT	= 0;
		}
		
		if (ch_ct == CH_A){
			hcd->ch_info[ch_ct].BO_SUP_CONT
				= rcH_BO_SupportControl + S_R_CH_OFFSET(ch_ct);
			hcd->ch_info[ch_ct].CSW_RCV_DAT_SIZE
				= rcH_CSW_RcvDataSize + S_R_CH_OFFSET(ch_ct);
			hcd->ch_info[ch_ct].OUT_EP_CONT
				= rcH_OUT_EP_Control + S_R_CH_OFFSET(ch_ct);
			hcd->ch_info[ch_ct].IN_EP_CONT
				= rcH_IN_EP_Control + S_R_CH_OFFSET(ch_ct);
		} else {
			hcd->ch_info[ch_ct].BO_SUP_CONT			= 0;
			hcd->ch_info[ch_ct].CSW_RCV_DAT_SIZE	= 0;
			hcd->ch_info[ch_ct].OUT_EP_CONT			= 0;
			hcd->ch_info[ch_ct].IN_EP_CONT			= 0;
		}
		
		if ( (ch_ct == CH_0) || (ch_ct == CH_A) ){
			hcd->ch_info[ch_ct].CHxInterval		= 0;
		} else { 
			hcd->ch_info[ch_ct].CHxInterval
				= rsH_CHb_Interval_HL + S_R_CH_OFFSET(ch_ct - 2);
		}
		
		hcd->ch_info[ch_ct].CHxCondCode
			= rcH_CH0_ConditionCode + S_R_CH_OFFSET(ch_ct);
		hcd->ch_info[ch_ct].CHxJoin		= rcH_CH0_Join + S_R_CH_OFFSET(ch_ct);
	}

	DPRINT(DBG_INIT,"%s:exit\n",__FUNCTION__);
	return;
}

/**
 * @brief       - initialize hardware.
 * @retval      return 0;       complete
 *                              if error return error code
 */
void init_lsi(void)
{
	unsigned char	chgendian;	/* dummy read value */

	DPRINT(DBG_INIT,"%s:enter\n", __FUNCTION__);

	/**
	 * 1. Init endian settings informations.
	 */
	if (S_R_REGS8(rcC_RevisionNum) != RevisionNum) {
		/* protect mode clear */
		S_R_REGS8(rcC_ModeProtect) = ModeProtectClear;
		/* change endian */
		S_R_REGS8(rc_ChipConfig - 1) = CPU_Endian;
		/* dummy read */
		chgendian = S_R_REGS8(rcC_CPU_ChgEndian);
		/* protect mode clear */
		S_R_REGS8(rcC_ModeProtect) = ModeProtectSet;
		
		/* check revision number */
		if (S_R_REGS8(rcC_RevisionNum) != RevisionNum) {
			DPRINT(DBG_INIT,"%s Chip Revision No is 0x%x (0x%x)\n", DRV_NAME,
				RevisionNum, S_R_REGS8(rcC_RevisionNum));
			DPRINT(DBG_INIT,"USB Host controller is not found\n");
			return;
		}
	}
	
	/**
	 * 2. Reset hardware.
	 */
	/* Check IDE driver is available */
	if (!(S_R_REGS8(rcC_IDE_Config_1) & ActiveIDE)){
		/* reset hardware */
		S_R_REGS8(rcC_ChipReset) = AllReset;
		/* dummy read */
		chgendian = S_R_REGS8(rcC_CPU_ChgEndian);
		
		if (S_R_REGS8(rcC_RevisionNum) != RevisionNum) {
			/* protect mode clear */
			S_R_REGS8(rcC_ModeProtect) = ModeProtectClear;
			/* change endian */
			S_R_REGS8(rc_ChipConfig - 1) = CPU_Endian;
			/* set clock */
			S_R_REGS8(rcC_ClkSelect) = CLK_24;
			/* dummy read */
			chgendian = S_R_REGS8(rcC_CPU_ChgEndian);
			/* protect mode clear */
			S_R_REGS8(rcC_ModeProtect) = ModeProtectSet;
		
		}
	}
	/* set wakeup timer */
	S_R_REGS16(rsC_WakeupTim_HL) = WakeupTim_2msec;

	/* reset HTM */
	S_R_REGS8(rcC_HostDeviceSel) = HostMode;
	S_R_REGS8(rcH_Reset) = ResetHTM;
	S_R_REGS8(rcH_Reset) = S_R_REG_CLEAR;

	DPRINT(DBG_INIT,"%s:exit\n", __FUNCTION__);

	return;
}

/**
 * @brief       - initialize FIFO registers.
 * @retval      return 0;       complete
 *                              if error return error code
 */
void init_fifo(void)
{
	/**
	 * - 1. Set FIFO address:
	 */
	DPRINT(DBG_INIT,"%s:enter\n", __FUNCTION__);
	S_R_REGS16(rsH_BUF0_StartAdrs_HL) = HOST_FIFO_CH0_START_ADRS;
	S_R_REGS16(rsH_BUF0_EndAdrs_HL)   = HOST_FIFO_CH0_END_ADRS;

	S_R_REGS16(rsH_BUFa_StartAdrs_HL) = HOST_FIFO_CHA_START_ADRS;
	S_R_REGS16(rsH_BUFa_EndAdrs_HL)   = HOST_FIFO_CHA_END_ADRS;

	S_R_REGS16(rsH_BUFb_StartAdrs_HL) = HOST_FIFO_CHB_START_ADRS;
	S_R_REGS16(rsH_BUFb_EndAdrs_HL)   = HOST_FIFO_CHB_END_ADRS;
	S_R_REGS16(rsH_BUFc_StartAdrs_HL) = HOST_FIFO_CHC_START_ADRS;
	S_R_REGS16(rsH_BUFc_EndAdrs_HL)   = HOST_FIFO_CHC_END_ADRS;
	S_R_REGS16(rsH_BUFd_StartAdrs_HL) = HOST_FIFO_CHD_START_ADRS;
	S_R_REGS16(rsH_BUFd_EndAdrs_HL)   = HOST_FIFO_CHD_END_ADRS;
	S_R_REGS16(rsH_BUFe_StartAdrs_HL) = HOST_FIFO_CHE_START_ADRS;
	S_R_REGS16(rsH_BUFe_EndAdrs_HL)   = HOST_FIFO_CHE_END_ADRS;

	S_R_REGS8(rcH_BUFn_Control) = S_R_FIFO_ALL_CLR;
	DPRINT(DBG_INIT,"%s:exit\n", __FUNCTION__);
}

module_init(v05_hc_init);
module_exit(v05_hc_exit);

MODULE_AUTHOR("Luxun9 Project Team.");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("SEIKO EPSON S1R72V05 USB Host Control Driver");

