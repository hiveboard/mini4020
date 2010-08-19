/**
 * @file	s1r72v05_udc.c
 * @brief	S1R72V05 USB Contoroller Driver(USB Device)
 * @date	2007/03/20
 * Copyright (C)SEIKO EPSON CORPORATION 2003-2006. All rights reserved.
 * This file is licenced under the GPLv2
 */

#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/ioport.h>
#include <linux/types.h>
#include <linux/version.h>
#include <linux/errno.h>
#include <linux/delay.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/list.h>
#include <linux/interrupt.h>
#include <linux/moduleparam.h>
#include <linux/proc_fs.h>
#include <linux/mm.h>
#include <linux/device.h>

#include <asm/byteorder.h>
#include <asm/dma.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/system.h>
#include <asm/mach-types.h>
#include <asm/unaligned.h>
#include <asm/hardware.h>


#include <linux/usb_ch9.h>
#include <linux/usb_gadget.h>

#include "s1r72v05_udc-regs.h"
#include "s1r72v05.h"
#include "s1r72xxx_udc.h"

#define DRIVER_DESC     "S1R72V05 USB Device Controller driver"

#define S1R72_DEVICE_NAME	"s1r72v05"

#define	USBC_INIT	s1r72v05_usbc_module_init
#define	USBC_EXIT	s1r72v05_usbc_module_exit

#include "s1r72xxx_udc.c"

int usbc_lsi_init(S1R72XXX_USBC_DEV *dev);
void usbc_ep_init(S1R72XXX_USBC_DEV *dev);

static const unsigned char epname[S1R72_MAX_ENDPOINT][S1R72_EP_NAME_LEN]
        = {"ep0","ep1","ep2","ep3"};

S1R72XXX_USBC_DRV_STATE_TABLE_ELEMENT \
	s1r72xxx_drv_state_tbl[S1R72_GD_MAX_DRV_STATE][S1R72_GD_MAX_DRV_EVENT] =
{
	/* INIT */
	{	/* next driver */	/* next_power	*/
		{S1R72_GD_REGD		,S1R72_GD_POWER_DONT_CHG	},/* API_PROBE		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* API_REGISTER	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* API_UNREGISTER	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* API_REMOVE		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* API_SUSPEND	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* API_RESUME		*/
		{S1R72_GD_DONT_CHG	,GoSleep					},/* API_SHUTDOWN	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_NONJ		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_RESET		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_RESUME		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_SUSPEND	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_CHIRP		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_ADDRESS	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_RESTORE	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_VBUS_H		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_VBUS_L		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_SLEEP		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_SNOOZE		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_ACTIVE60	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_ACT_DEVICE	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	} /* ERR_CHIRP_FAIL	*/
	},
	/* REGD */
	{	/* next driver */	/* next_power	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* API_PROBE		*/
		{S1R72_GD_BOUND		,S1R72_GD_POWER_DONT_CHG	},/* API_REGISTER	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* API_UNREGISTER	*/
		{S1R72_GD_INIT		,S1R72_GD_POWER_DONT_CHG	},/* API_REMOVE		*/
		{S1R72_GD_SYSSUSPD	,S1R72_GD_POWER_DONT_CHG	},/* API_SUSPEND	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* API_RESUME		*/
		{S1R72_GD_INIT		,GoSleep					},/* API_SHUTDOWN	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_NONJ		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_RESET		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_RESUME		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_SUSPEND	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_CHIRP		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_ADDRESS	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_RESTORE	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_VBUS_H		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_VBUS_L		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_SLEEP		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_SNOOZE		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_ACTIVE60	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_ACT_DEVICE	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	} /* ERR_CHIRP_FAIL	*/
	},
	/* BOUND */
	{	/* next driver */	/* next_power	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* API_PROBE		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* API_REGISTER	*/
		{S1R72_GD_REGD		,GoSleep					},/* API_UNREGISTER	*/
		{S1R72_GD_INIT		,GoSleep					},/* API_REMOVE		*/
		{S1R72_GD_SYSSUSPD	,S1R72_GD_POWER_DONT_CHG	},/* API_SUSPEND	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* API_RESUME		*/
		{S1R72_GD_INIT		,GoSleep					},/* API_SHUTDOWN	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_NONJ		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_RESET		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_RESUME		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_SUSPEND	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_CHIRP		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_ADDRESS	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_RESTORE	*/
		{S1R72_GD_DONT_CHG	,GoActDevice				},/* H_W_VBUS_H		*/
		{S1R72_GD_DONT_CHG	,GoSleep					},/* H_W_VBUS_L		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_SLEEP		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_SNOOZE		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_ACTIVE60	*/
		{S1R72_GD_ATTACH	,S1R72_GD_POWER_DONT_CHG	},/* H_W_ACT_DEVICE	*/
		{S1R72_GD_DONT_CHG	,GoSleep					} /* ERR_CHIRP_FAIL	*/
	},
	/* ATTACH */
	{	/* next driver */	/* next_power	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* API_PROBE		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* API_REGISTER	*/
		{S1R72_GD_BOUND		,GoSleep					},/* API_UNREGISTER	*/
		{S1R72_GD_INIT		,GoSleep					},/* API_REMOVE		*/
		{S1R72_GD_SYSSUSPD	,GoSleep					},/* API_SUSPEND	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* API_RESUME		*/
		{S1R72_GD_INIT		,GoSleep					},/* API_SHUTDOWN	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_NONJ		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_RESET		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_RESUME		*/
		{S1R72_GD_DONT_CHG	,GoSleep					},/* H_W_SUSPEND	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_CHIRP		*/
		{S1R72_GD_RUN		,S1R72_GD_POWER_DONT_CHG	},/* H_W_ADDRESS	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_RESTORE	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_VBUS_H		*/
		{S1R72_GD_BOUND		,GoSleep					},/* H_W_VBUS_L		*/
		{S1R72_GD_USBSUSPD	,S1R72_GD_POWER_DONT_CHG	},/* H_W_SLEEP		*/
		{S1R72_GD_USBSUSPD	,S1R72_GD_POWER_DONT_CHG	},/* H_W_SNOOZE		*/
		{S1R72_GD_USBSUSPD	,S1R72_GD_POWER_DONT_CHG	},/* H_W_ACTIVE60	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_ACT_DEVICE	*/
		{S1R72_GD_BOUND		,GoSleep					} /* ERR_CHIRP_FAIL	*/
	},
	/* RUN */
	{	/* next driver */	/* next_power	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* API_PROBE		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* API_REGISTER	*/
		{S1R72_GD_BOUND		,GoSleep					},/* API_UNREGISTER	*/
		{S1R72_GD_INIT		,GoSleep					},/* API_REMOVE		*/
		{S1R72_GD_SYSSUSPD	,GoSleep					},/* API_SUSPEND	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* API_RESUME		*/
		{S1R72_GD_INIT		,GoSleep					},/* API_SHUTDOWN	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_NONJ		*/
		{S1R72_GD_ATTACH	,S1R72_GD_POWER_DONT_CHG	},/* H_W_RESET		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_RESUME		*/
		{S1R72_GD_DONT_CHG	,GoSleep,					},/* H_W_SUSPEND	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_CHIRP		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_ADDRESS	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_RESTORE	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_VBUS_H		*/
		{S1R72_GD_BOUND		,GoSleep					},/* H_W_VBUS_L		*/
		{S1R72_GD_USBSUSPD	,S1R72_GD_POWER_DONT_CHG	},/* H_W_SLEEP		*/
		{S1R72_GD_USBSUSPD	,S1R72_GD_POWER_DONT_CHG	},/* H_W_SNOOZE		*/
		{S1R72_GD_USBSUSPD	,S1R72_GD_POWER_DONT_CHG	},/* H_W_ACTIVE60	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_ACT_DEVICE	*/
		{S1R72_GD_BOUND		,GoSleep					} /* ERR_CHIRP_FAIL	*/
	},
	/* USBSUSPD */
	{	/* next driver */	/* next_power	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* API_PROBE		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* API_REGISTER	*/
		{S1R72_GD_REGD		,GoSleep					},/* API_UNREGISTER	*/
		{S1R72_GD_INIT		,GoSleep					},/* API_REMOVE		*/
		{S1R72_GD_SYSSUSPD	,GoSleep					},/* API_SUSPEND	*/
		{S1R72_GD_DONT_CHG	,GoActDevice				},/* API_RESUME		*/
		{S1R72_GD_INIT		,GoSleep					},/* API_SHUTDOWN	*/
		{S1R72_GD_DONT_CHG	,GoActDevice				},/* H_W_NONJ		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_RESET		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_RESUME		*/
		{S1R72_GD_DONT_CHG	,GoSleep					},/* H_W_SUSPEND	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_CHIRP		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_ADDRESS	*/
		{S1R72_GD_ATTACH	,S1R72_GD_POWER_DONT_CHG	},/* H_W_RESTORE	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_VBUS_H		*/
		{S1R72_GD_BOUND		,S1R72_GD_POWER_DONT_CHG	},/* H_W_VBUS_L		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_SLEEP		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_SNOOZE		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_ACTIVE60	*/
		{S1R72_GD_ATTACH	,S1R72_GD_POWER_DONT_CHG	},/* H_W_ACT_DEVICE	*/
		{S1R72_GD_BOUND		,S1R72_GD_POWER_DONT_CHG	} /* ERR_CHIRP_FAIL	*/
	},
	/* SYSSUSPD */
	{	/* next driver */	/* next_power	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* API_PROBE		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* API_REGISTER	*/
		{S1R72_GD_REGD		,S1R72_GD_POWER_DONT_CHG	},/* API_UNREGISTER	*/
		{S1R72_GD_INIT		,S1R72_GD_POWER_DONT_CHG	},/* API_REMOVE		*/
		{S1R72_GD_DONT_CHG	,GoSleep					},/* API_SUSPEND	*/
		{S1R72_GD_BOUND		,S1R72_GD_POWER_DONT_CHG	},/* API_RESUME		*/
		{S1R72_GD_INIT		,S1R72_GD_POWER_DONT_CHG	},/* API_SHUTDOWN	*/
		{S1R72_GD_USBSUSPD	,GoActDevice				},/* H_W_NONJ		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_RESET		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_RESUME		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_SUSPEND	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_CHIRP		*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_ADDRESS	*/
		{S1R72_GD_DONT_CHG	,S1R72_GD_POWER_DONT_CHG	},/* H_W_RESTORE	*/
		{S1R72_GD_BOUND		,GoActDevice				},/* H_W_VBUS_H		*/
		{S1R72_GD_DONT_CHG	,GoSleep					},/* H_W_VBUS_L		*/
		{S1R72_GD_BOUND		,S1R72_GD_POWER_DONT_CHG	},/* H_W_SLEEP		*/
		{S1R72_GD_BOUND		,S1R72_GD_POWER_DONT_CHG	},/* H_W_SNOOZE		*/
		{S1R72_GD_BOUND		,S1R72_GD_POWER_DONT_CHG	},/* H_W_ACTIVE60	*/
		{S1R72_GD_BOUND		,S1R72_GD_POWER_DONT_CHG	},/* H_W_ACT_DEVICE	*/
		{S1R72_GD_BOUND		,S1R72_GD_POWER_DONT_CHG	} /* ERR_CHIRP_FAIL	*/
	}
};

unsigned short start_adrs_tbl[S1R72_MAX_ENDPOINT] = 
{
	S1R72_EP0_START_ADRS,
	S1R72_EPA_START_ADRS,
	S1R72_EPB_START_ADRS,
	S1R72_EPC_START_ADRS
};

unsigned short end_adrs_tbl[S1R72_MAX_ENDPOINT] = 
{
	S1R72_EPA_START_ADRS,	/* EP0_END_ADRS = EPA_START_ADRS */
	S1R72_EPB_START_ADRS,	/* EPA_END_ADRS = EPB_START_ADRS */
	S1R72_EPC_START_ADRS,	/* EPB_END_ADRS = EPC_START_ADRS */
	S1R72_EPC_END_ADRS
};

unsigned short epx_max_packet_tbl[S1R72_MAX_ENDPOINT] =
{
	S1R72_EP0_MAX_PKT,	/* EP0 */
	S1R72_EPA_MAX_PKT,	/* EPa */
	S1R72_EPB_MAX_PKT,	/* EPb */
	S1R72_EPC_MAX_PKT	/* EPc */
};

S1R72XXX_USBC_EP_IRQ_TBL usbc_ep_irq_tbl[] =
{
	{S1R72_EPxIntStat_NONE, S1R72_GD_EP0},
	{S1R72_EPaIntStat, S1R72_GD_EPA},
	{S1R72_EPbIntStat, S1R72_GD_EPB},
	{S1R72_EPcIntStat, S1R72_GD_EPC},
	{S1R72_EPxIntStat_NONE, S1R72_MAX_ENDPOINT}
};

/* ========================================================================= */
/**
 * @brief	- initialize hadeware.
 * @par		usage:
 *				internal use only.
 * @param	*usbc_dev;	device informations structure.
 * @retval	return 0 operation completed successfully.
 *			in case of error, return error code.
 */
/* ========================================================================= */
int usbc_lsi_init(S1R72XXX_USBC_DEV *usbc_dev)
{
	unsigned char		chg_endian;		/* chg_endian dummy read */

	DEBUG_MSG("%s, enter\n", __FUNCTION__);
	
	/**
	 * - 1. Initialize device:
	 *  - initialize device register.
	 *  - LSI reset, change endian, set clock frequency, USB mode,
	 *     PLL timer, DTM reset, interrupt enable.
	 */
	if ( rcD_RegRead8(usbc_dev, rcRevisionNum) != RevisionNum ) {
		rcD_RegWrite8(usbc_dev, rcModeProtect - 1, unProtect);
		rcD_RegWrite8(usbc_dev, rcChipConfig - 1, BUS_SWAP);
		chg_endian = rsD_RegRead16(usbc_dev, rcChgEndian);
	}
	if ( rcD_RegRead8(usbc_dev, rcRevisionNum) != RevisionNum ) {
		rcD_RegWrite8(usbc_dev, rcChipConfig - 1, BUS_SWAP);
		chg_endian = rsD_RegRead16(usbc_dev, rcChgEndian - 1);
		rcD_RegWrite8(usbc_dev, rcModeProtect, Protect);
		printk("The USB device controller is not found\n");
		return -ENODEV;
	}
	if ( (rcD_RegRead8(usbc_dev, rcIDE_Config_1) & ActiveIDE) != ActiveIDE ) {
		rcD_RegWrite8(usbc_dev, rcChipReset, AllReset);	/* LSI reset */
		chg_endian = rsD_RegRead16(usbc_dev, rcChgEndian - 1);
		if ( rcD_RegRead8(usbc_dev, rcRevisionNum) != RevisionNum ) {
			rcD_RegWrite8(usbc_dev, rcModeProtect - 1, unProtect);
			rcD_RegWrite8(usbc_dev, rcChipConfig - 1, BUS_SWAP);
			chg_endian = rsD_RegRead16(usbc_dev, rcChgEndian);
		}
	}

	rcD_RegWrite8(usbc_dev, rcClkSelect, CLK_24); /* input clock is 24MHz */
	rcD_RegWrite8(usbc_dev, rcModeProtect, Protect); /* register protect */

	/* Set to USB device mode */
	rcD_RegClear8(usbc_dev, rcHostDeviceSel, HOST_MODE);

	/* 2msec timer */
	rsD_RegWrite16(usbc_dev, rsWakeupTim, S1R72_WAKEUP_TIM_2MSEC);

	/* DTM Reset */
	rcD_RegSet8(usbc_dev, rcD_S1R72_Reset, S1R72_ResetDTM);

	/* Enable PowerManagement, Device interrupt */
	rcD_RegSet8(usbc_dev, rcMainIntEnb, (FinishedPM | DevInt));

	/* Enable VBUS, SIE interrupt */
	rcD_IntClr8(usbc_dev, rcDeviceIntStat, (D_VBUS_Changed | D_SIE_IntStat));
	rcD_RegWrite8(usbc_dev, rcDeviceIntEnb, (D_VBUS_Changed | D_SIE_IntStat));

	/* Clear DTM Reset */
	rcD_RegClear8(usbc_dev, rcD_S1R72_Reset, S1R72_ResetDTM);

	return 0;
}

/* ========================================================================= */
/**
 * @brief	- initialize endpoint structure.
 * @par		usage:
 *				internal use only.
 * @param	*dev;	device informations structure.
 * @retval	none;
 */
/* ========================================================================= */
void usbc_ep_init(S1R72XXX_USBC_DEV *dev)
{
        unsigned char   ep_ct;  /* endpoint counter */

	ep_ct = S1R72_GD_EP0;

	/**
	 * - 1. Initialize s1r72xxx_usbc_ep endpoint structures:
	 *  - initialize member of endpoint structures depends on hardware.
	 *  - ep_subname, ep_state, queue, reg.ep0.xxx or reg.epx.xxx
	 */
	dev->remote_wakeup_enb = S1R72_RT_WAKEUP_DIS;

	/* endpoint 0 */
	dev->usbc_ep[S1R72_GD_EP0].reg.ep0.EP0IntStat	= rcD_S1R72_EP0IntStat;
	dev->usbc_ep[S1R72_GD_EP0].reg.ep0.EP0IntEnb	= rcD_S1R72_EP0IntEnb;
	dev->usbc_ep[S1R72_GD_EP0].reg.ep0.EPnControl	= rcD_S1R72_EPnControl;
	dev->usbc_ep[S1R72_GD_EP0].reg.ep0.EP0SETUP_0	= rcD_S1R72_EP0SETUP_0;
	dev->usbc_ep[S1R72_GD_EP0].reg.ep0.SETUP_Control= rcD_S1R72_SETUP_Control;
	dev->usbc_ep[S1R72_GD_EP0].reg.ep0.EP0MaxSize	= rcD_S1R72_EP0MaxSize;
	dev->usbc_ep[S1R72_GD_EP0].reg.ep0.EP0Control	= rcD_S1R72_EP0Control;
	dev->usbc_ep[S1R72_GD_EP0].reg.ep0.EP0ControlIN	= rcD_S1R72_EP0ControlIN;
	dev->usbc_ep[S1R72_GD_EP0].reg.ep0.EP0ControlOUT= rcD_S1R72_EP0ControlOUT;
	dev->usbc_ep[S1R72_GD_EP0].reg.ep0.EP0Join		= rcD_S1R72_EP0Join;

	/* endpoint A-C */
	for (ep_ct = S1R72_GD_EPA ; ep_ct < S1R72_MAX_ENDPOINT ; ep_ct ++) {
		dev->usbc_ep[ep_ct].reg.epx.EPxIntStat
			= rcD_S1R72_EPaIntStat + ep_ct - 1;
		dev->usbc_ep[ep_ct].reg.epx.EPxIntEnb
			= rcD_S1R72_EPaIntEnb + ep_ct -1;
		dev->usbc_ep[ep_ct].reg.epx.EPxFIFOClr
			= rcD_S1R72_EPrFIFO_Clr;
		dev->usbc_ep[ep_ct].reg.epx.EPxMaxSize
			= rsD_S1R72_EPaMaxSize + S1R72_EPxOFFSET16(ep_ct);
		dev->usbc_ep[ep_ct].reg.epx.EPxConfig_0
			= rcD_S1R72_EPaConfig_0 + S1R72_EPxOFFSET16(ep_ct);
		dev->usbc_ep[ep_ct].reg.epx.EPxControl
			= rcD_S1R72_EPaControl + S1R72_EPxOFFSET16(ep_ct);
		dev->usbc_ep[ep_ct].reg.epx.EPxJoin
			= rcD_S1R72_EPaJoin + S1R72_EPxOFFSET16(ep_ct);
		dev->usbc_ep[ep_ct].reg.epx.EPxStartAdrs
			= rsD_S1R72_EPaStartAdrs + S1R72_EPxOFFSET4(ep_ct);
		dev->usbc_ep[ep_ct].reg.epx.EPxEndAdrs
			= S1R72_REG_ALL_CLR;
	}
	dev->usbc_ep[S1R72_MAX_ENDPOINT - 1].reg.epx.EPxEndAdrs	= rsD_S1R72_EPcEndAdrs;

	/**
	 * - 2. Initialize usb_ep endpoint structures:
	 *  - initialize member of endpoint structures independs on hardware.
	 *  - usb_ep.name, usb_ep.ops, usb_ep.maxpacket
	 */
	for ( ep_ct = S1R72_GD_EP0; ep_ct < S1R72_MAX_ENDPOINT ; ++ep_ct ){
		dev->usbc_ep[ep_ct].ep.name	= epname[ep_ct];
		dev->usbc_ep[ep_ct].ep.ops			= &s1r72xxx_ep_ops;
		dev->usbc_ep[ep_ct].ep.maxpacket	= epx_max_packet_tbl[ep_ct];
		dev->usbc_ep[ep_ct].ep_subname	= ep_ct;
		dev->usbc_ep[ep_ct].fifo_size	= epx_max_packet_tbl[ep_ct];
		dev->usbc_ep[ep_ct].fifo_data	= 0;
		dev->usbc_ep[ep_ct].dev	= dev;
		dev->usbc_ep[ep_ct].intenb	= S1R72_IRQ_NOT_OCCURED;
		dev->usbc_ep[ep_ct].last_is_short = S1R72_IRQ_SHORT;
		dev->usbc_ep[ep_ct].is_stopped = S1R72_EP_STALL;
		change_epx_state(&dev->usbc_ep[ep_ct], S1R72_GD_EP_INIT);
	}

}

/* ========================================================================= */
/**
 * @brief       - initialize endpoint fifo.
 * @par         usage:
 *                              internal use only.
 * @param       *dev;   device informations structure.
 * @retval      none;
 */
/* ========================================================================= */
void usbc_fifo_init(S1R72XXX_USBC_DEV *dev)
{
	unsigned char   ep_ct;  /** endpoint number */

	/**
	 * - 1. Set EPx FIFO:
	 *  - set EPx FIFO address, clear FIFO data, set Join.
	*/
	for(ep_ct = S1R72_GD_EPA ; ep_ct < S1R72_MAX_ENDPOINT ; ep_ct++){
		/* endpoint a-c */
		rsD_RegWrite16(dev, dev->usbc_ep[ep_ct].reg.epx.EPxStartAdrs,
			start_adrs_tbl[ep_ct]);
		rsD_RegWrite16(dev, dev->usbc_ep[ep_ct].reg.epx.EPxEndAdrs,
			end_adrs_tbl[ep_ct]);

		/* fifo clear */
		S1R72_EPFIFO_CLR(dev, ep_ct);
	}
	ep_ct = S1R72_MAX_ENDPOINT - 1;
	rsD_RegWrite16(dev, dev->usbc_ep[ep_ct].reg.epx.EPxEndAdrs,
		end_adrs_tbl[ep_ct]);
}

module_init(s1r72v05_usbc_module_init);
module_exit(s1r72v05_usbc_module_exit);

MODULE_AUTHOR("Luxun9 Project Team.");
MODULE_DESCRIPTION(DRIVER_DESC);
MODULE_LICENSE("GPL");

