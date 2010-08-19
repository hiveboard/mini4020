/**
 * @file	s1r72xxx-hub.c
 * @brief	Process of Hub Control
 * @date	2007/04/10
 * @author	Luxun9 project team
 * Copyright (C)SEIKO EPSON CORPORATION. 2006-2007 All rights reserved.
 * This file is licensed under GPLv2.
 */

#define HUBMONITOR 0
static int hub_test_mode(struct usb_hcd *hcd, unsigned short wIndex);

/* Virtual Root Hub */
static __u8 root_hub_des[] = {
	13,		/* bDescLength */
	USB_DT_HUB,	/* bDescriptorType */
	1,		/* bNbrPorts */
	0x0A,		/* wHubCharacteristics */
	0x00,		
	0x50,		/* bPwrOn2PwrGood */
	0x00,		/* bHubContrCurrent */
	0xFC,		/* DeviceRmovable */
	0xFF,		
	0xFF,		
	0xFF,		/* PortPwrCtrlMask */
	0xFF,
	0xFF,
};

/* TEST Packet data */
static const unsigned char test_mode_packet[TEST_MODE_PACKET_SIZE + 1] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA, 0xAA,
	0xAA, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE, 0xEE,
	0xEE, 0xFE, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
	0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0x7F, 0xBF, 0xDF,
	0xEF, 0xF7, 0xFB, 0xFD, 0xFC, 0x7E, 0xBF, 0xDF,
	0xEF, 0xF7, 0xFB, 0xFD, 0x7E, 0x00
};

/**
 * @brief	- Checking Hub Status
 * @param	*hcd;	struct of usb_hcd
 * @param	*buf;	buffer of data
 * @retval	return;	length of wrote data
 */

static int s1r72xxx_hcd_hub_status(struct usb_hcd *hcd, char *buf)
{
	struct hcd_priv *priv = (struct hcd_priv *)hcd->hcd_priv[0];
	unsigned long	lock_flags;
	unsigned short	*short_buf = (unsigned short*)buf;
	int len = sizeof(unsigned short);

	/* DPRINT(DBG_HUB_STATUS,"%s:enter\n", __FUNCTION__); */
	spin_lock_irqsave(&priv->lock, lock_flags);

	/**
	 * - 1. get change status:
	 */
	if (priv->rh.wPortTrigger) {
		priv->rh.wPortTrigger = RH_PORT_STATUS_STAYED;
		*short_buf = RH_CHANGED_PORT_NO(ROOT_HUB_PORT_NO);
	} else {
		*short_buf = RH_CHANGED_PORT_NOTHING;
	}
	spin_unlock_irqrestore(&priv->lock, lock_flags);
	/* DPRINT(DBG_HUB_STATUS,"%s:exit 0x%04x\n", __FUNCTION__, *short_buf); */

	return len;
}

/**
 * @brief	- Operation for Hub Control
 * @param	*hcd;	struct of usb_hcd
 * @param	treq;	request type
 * @param	value;	value of request
 * @param	index;	index of request
 * @param	*buf;	buffer of data
 * @param	len;	Length of buffer
 * @retval	return 0;	complete
 *		if error return error code;
 */
static int s1r72xxx_hcd_hub_control(struct usb_hcd* hcd, u16 treq, u16 value,
	u16 index, char *buf, u16 len)
{
	struct hcd_priv *priv = (struct hcd_priv *)hcd->hcd_priv[0];
	unsigned long lock_flags;
	unsigned char host_state;
	char* bdata_buf = (char *)buf;
	short* wdata_buf = (short *)buf;
	int ret = 0;

	DPRINT(DBG_API,"%s:enter\n", __FUNCTION__);
	s1r72xxx_queue_log(S_R_DEBUG_HUB_CONT, treq, value);
	
	spin_lock_irqsave(&priv->lock, lock_flags);
	/**
	 * - 1. switch by request type:
	 */
	switch (treq) {
	case ClearHubFeature:
		/**
		 * - 2. in case Clear Hub Feature:
		 * - no operation. because this hardware do not have this feature.
		 */
		DPRINT(DBG_API,"%s:ClearHubFeature 0x%04x\n", __FUNCTION__, value);
		switch (value) {
		case C_HUB_OVER_CURRENT:
			priv->rh.hub_status.wHubChange &= ~HUB_CHANGE_OVERCURRENT;
			ret = 0;
			break;
		case C_HUB_LOCAL_POWER:
		default:
			goto error;
		}
		break;

	case ClearPortFeature:
		/**
		 * - 3. in case Clear Port Feature:
		 * - operation into bit
		 */
		DPRINT(DBG_API,"%s:ClearPortFeature 0x%04x\n", __FUNCTION__, value);
		switch (value) {
		case USB_PORT_FEAT_ENABLE:
			DPRINT(DBG_API,"%s:enable clear\n", __FUNCTION__);
			/** if value is USB_PORT_FEAT_ENABLE, disable port */
			if(priv->rh.port_status.wPortStatus == USB_PORT_STAT_CONNECTION){
				priv->rh.port_status.wPortStatus &= ~USB_PORT_STAT_ENABLE;
				port_control(hcd, S_R_PORT_CNT_REG_WRITE, GoDISABLED);
			}
			break;
		case USB_PORT_FEAT_SUSPEND:
			/** if value is USB_PORT_FEAT_SUSPEND, resume USB bus */
			hc_resume(hcd);
			break;
		case USB_PORT_FEAT_POWER:
			DPRINT(DBG_API,"%s:power off\n", __FUNCTION__);
			/** set hardware to IDLE */
			priv->rh.port_status.wPortStatus &= ~USB_PORT_STAT_POWER;
			port_control(hcd, S_R_PORT_CNT_POW_OFF, GoIDLE);
			break;
		case USB_PORT_FEAT_INDICATOR:
			/** no operation */
			break;
		case USB_PORT_FEAT_C_CONNECTION:
			/** clear C_PORT_CONNECT */
			priv->rh.port_status.wPortChange &= ~USB_PORT_STAT_C_CONNECTION;
			break;
		case USB_PORT_FEAT_C_ENABLE:
			/** clear C_PORT_ENABLE */
			priv->rh.port_status.wPortChange &= ~USB_PORT_STAT_C_ENABLE;
			break;
		case USB_PORT_FEAT_C_SUSPEND:
			/** clear C_PORT_SUSPEND */
			priv->rh.port_status.wPortChange &= ~USB_PORT_STAT_C_SUSPEND;
			break;
		case USB_PORT_FEAT_C_OVER_CURRENT:
			/** clear C_PORT_OVER_CURRENT */
			priv->rh.port_status.wPortChange &= ~USB_PORT_STAT_C_OVERCURRENT;
			break;
		case USB_PORT_FEAT_C_RESET:
			/** clear C_PORT_RESET */
			priv->rh.port_status.wPortChange &= ~USB_PORT_STAT_C_RESET;
			break;
		default:
			goto error;
		}
		break;
	case GetHubDescriptor:
		/**
		 * - 4. in case Get Hub Descriptor:
		 * - copy root_hub_des
		 */
		DPRINT(DBG_API,"%s:GetHubDescriptor 0x%04x\n", __FUNCTION__, value);
		if (len) {
			bdata_buf = root_hub_des;
		} else {
			ret = -EPIPE;
		}
		break;

	case GetHubStatus:
		/**
		 * - 5. in case Get Hub Status:
		 */
		DPRINT(DBG_API,"%s:GetHubStatus 0x%04x\n", __FUNCTION__, value);
		if (len == sizeof(struct usb_hub_status)) {
			/* Hub Status */
			*wdata_buf	= cpu_to_le16(priv->rh.hub_status.wHubStatus);
			wdata_buf++;
			*wdata_buf	= cpu_to_le16(priv->rh.hub_status.wHubChange);
#if HUBMONITOR
			printk("%s-usb:GetHubStatus %04X %04X\n",
				CHIP_NAME, cpu_to_le16(priv->rh.hub_status.wHubStatus),
				cpu_to_le16(priv->rh.hub_status.wHubChange));
#endif
		} else {
			ret = -EPIPE;
		}
		break;

	case GetPortStatus:
		/**
		 * - 6. in case Get Port Status:
		 */
		priv->rh.port_status.wPortStatus
			&= ~(USB_PORT_STAT_HIGH_SPEED | USB_PORT_STAT_LOW_SPEED);
		switch (S_R_REGS8(rcH_NegoControl_1) & PortSpeedMask) {
		case PortSpeed_High:
			priv->rh.port_status.wPortStatus |= USB_PORT_STAT_HIGH_SPEED;
			break;
		case PortSpeed_Low:
			priv->rh.port_status.wPortStatus |= USB_PORT_STAT_LOW_SPEED;
			break;
		case PortSpeed_Full:
		default:
			break;
		}
		
		DPRINT(DBG_API,"%s:GetPortStatus 0x%04x\n", __FUNCTION__, value);
		if (len == sizeof(struct usb_port_status)) {
			/* Port Status */
			*wdata_buf	= cpu_to_le16(priv->rh.port_status.wPortStatus);
			wdata_buf++;
			*wdata_buf	= cpu_to_le16(priv->rh.port_status.wPortChange);

#if HUBMONITOR
			printk("%s-usb:GetPortStatus %04X %04X\n",
				CHIP_NAME, cpu_to_le16(priv->rh.port_status.wPortStatus),
				cpu_to_le16(priv->rh.port_status.wPortChange));
#endif
		} else {
			ret = -EPIPE;
		}
		break;

	case SetHubFeature:
		/**
		 * - 7. in case Set Hub Feature:
		 */
		DPRINT(DBG_API,"%s:SetHubFeature 0x%04x\n", __FUNCTION__, value);
		switch (value) {
		case C_HUB_OVER_CURRENT:
			ret = 0;
			break;
		case C_HUB_LOCAL_POWER:
		default:
			goto error;
		}
		break;

	case SetPortFeature:
		/**
		 * - 8. in case Set Port Feature:
		 */
		DPRINT(DBG_API,"%s:SetPortFeature 0x%04x\n", __FUNCTION__, value);
		switch (value) {
		case USB_PORT_FEAT_RESET:
			host_state = S_R_REGS8(rcH_NegoControl_0) & HostStateMask;	
			if( (host_state != HostState_IDLE) &&
				(host_state != HostState_WAIT_CONNECT) ){
				priv->rh.port_status.wPortStatus |= USB_PORT_STAT_RESET;
				priv->rh.port_status.wPortStatus &= ~(USB_PORT_STAT_ENABLE);
				port_control(hcd, S_R_PORT_CNT_REG_WRITE, GoRESETtoOP);
			} else {
				priv->rh.port_status.wPortStatus &= ~(USB_PORT_STAT_RESET);
				priv->rh.port_status.wPortStatus |= USB_PORT_STAT_ENABLE;
				priv->rh.port_status.wPortChange |= USB_PORT_STAT_C_RESET;
				priv->rh.wPortTrigger = RH_PORT_STATUS_CHANGED;
			}
			break;

		case USB_PORT_FEAT_SUSPEND:
			/** suspend */
			hc_suspend(hcd);
			break;
		case USB_PORT_FEAT_POWER:
			if(!(priv->rh.port_status.wPortStatus & USB_PORT_STAT_POWER)) {
				host_start(hcd);
			}
			break;
		case USB_PORT_FEAT_TEST:
			ret = hub_test_mode(hcd, index);
			break;
		case USB_PORT_FEAT_INDICATOR:
		case USB_PORT_FEAT_C_CONNECTION:
		case USB_PORT_FEAT_C_ENABLE:
		case USB_PORT_FEAT_C_SUSPEND:
		case USB_PORT_FEAT_C_OVER_CURRENT:
			/** no operation */
			break;
		default:
			goto error;
		}
		break;
	default:
		DPRINT(DBG_API,"%s:non supported\n", __FUNCTION__);
error:
		ret = -EPIPE;
	}

	/**
	 * - 9. set data to buffer:
	 */
	if (buf && bdata_buf && len) {
		memcpy(buf, bdata_buf, len);
	}

	spin_unlock_irqrestore(&priv->lock, lock_flags);

	DPRINT(DBG_API,"%s:exit %d\n", __FUNCTION__, ret);
	s1r72xxx_queue_log(S_R_DEBUG_HUB_CONT,
		priv->rh.port_status.wPortStatus,
		priv->rh.port_status.wPortChange);

	return ret;
}


/**
 * @brief	- TEST mode
 * @param	*hcd;	struct of usb_hcd
 * @param	wIndex;	select mode
 * @retval	return 0;	complete
 */
static int hub_test_mode(struct usb_hcd *hcd, unsigned short wIndex)
{
	struct hcd_priv* priv
		= (struct hcd_priv*)hcd->hcd_priv[0];	/* private data */
	unsigned char		ch;
	unsigned char		fifo_ct = 0;
	unsigned short		*fifo_16	= NULL;
	unsigned char		regval = 0;
	int			retval = -EINVAL;

	DPRINT(DBG_API,"%s:\n", __FUNCTION__);

	/**
	 * - 1. call function that makes USB suspend:
	 */
	priv->rh.port_status.wPortStatus |= USB_PORT_STAT_TEST;
	init_ch_info(priv);
	for (ch = CH_0 ; ch < CH_MAX ; ch ++) {
		free_ch_resource(hcd, &priv->ch_info[ch], -ECONNRESET);
	}
	/* stop Host auto mode */
	port_control(hcd, S_R_PORT_CNT_AUTO_CANCEL, 0);
					
	switch(wIndex){
	case S_R_TEST_MODE_J:
		DPRINT(DBG_API,"%s:TEST_MODE_J\n", __FUNCTION__);
		regval = TEST_J;
		break;
	case S_R_TEST_MODE_K:
		DPRINT(DBG_API,"%s:TEST_MODE_K\n", __FUNCTION__);
		regval = TEST_K;
		break;
	case S_R_TEST_MODE_SE0_NAK:
		DPRINT(DBG_API,"%s:TEST_MODE_SE0_NAK\n", __FUNCTION__);
		regval = Test_SE0_NAK;
		break;
	case S_R_TEST_MODE_PACKET:
		DPRINT(DBG_API,"%s:TEST_MODE_PACKET\n", __FUNCTION__);
		S_R_CH0_FIFO_CLR(CH_0);
		S_R_REGS8(priv->ch_info[CH_0].CHxJoin) = S_R_JoinCPU_Wr;
		fifo_16 = (unsigned short *)test_mode_packet;
		while ( fifo_ct <  TEST_MODE_PACKET_SIZE - 1){
			S_R_REGS16(rsC_FIFO_Wr) = *fifo_16;
			fifo_16++;
			fifo_ct += 2;
		}
		S_R_REGS8(rcC_FIFO_Wr_0) = test_mode_packet[TEST_MODE_PACKET_SIZE - 1];
		S_R_REGS8(priv->ch_info[CH_0].CHxJoin) = S_R_REG_CLEAR;
		
		regval = Test_Packet;
		break;
	case S_R_TEST_MODE_FORCE_ENABLE:
		DPRINT(DBG_API,"%s:TEST_MODE_FORCE_ENABLE\n", __FUNCTION__);
		regval = Test_Force_Enable;
		break;
	default:
		retval = -EPIPE;
		break;
	}
	if(regval != 0){
		S_R_REGS8(rcH_USB_Test) = regval;
		S_R_REGS8(rcH_USB_Test) |= EnHS_Test;
		retval = 0;
	}

	DPRINT(DBG_API,"%s:\n", __FUNCTION__);

	return retval;
}

/**
 * @brief	- process for suspend
 * @param	*hcd;	struct of usb_hcd
 * @retval	return 0;	complete
 */
static int s1r72xxx_hcd_hub_suspend(struct usb_hcd *hcd)
{
	struct hcd_priv* priv
		= (struct hcd_priv*)hcd->hcd_priv[0];/* private data */
	unsigned long		lock_flags;		/* spin lock flags */

	DPRINT(DBG_API,"%s:\n", __FUNCTION__);
	
	spin_lock_irqsave(&priv->lock, lock_flags);

	/**
	 * - 1. call function that makes USB suspend:
	 */
	hc_suspend(hcd);

	spin_unlock_irqrestore(&priv->lock, lock_flags);

	DPRINT(DBG_API,"%s:\n", __FUNCTION__);

	return 0;
}


/**
 * @brief	- process for resume
 * @param	*hcd;	struct of usb_hcd
 * @retval	return 0;	complete
 */
static int s1r72xxx_hcd_hub_resume(struct usb_hcd *hcd)
{
	struct hcd_priv* priv
		= (struct hcd_priv*)hcd->hcd_priv[0];/* private data */
	unsigned long	lock_flags;	/* spin lock flags */
	DPRINT(DBG_API,"%s:\n", __FUNCTION__);
	
	spin_lock_irqsave(&priv->lock, lock_flags);

	/**
	 * - 1. call function that makes USB resume:
	 */
	hc_resume(hcd);
	
	spin_unlock_irqrestore(&priv->lock, lock_flags);

	DPRINT(DBG_API,"%s:\n", __FUNCTION__);

	return 0;
}


/**
 * @brief	- Initialize Virtual Root Hub
 * @param	priv;	private area for HCD
 * @retval	none;
 */
static void s1r72xxx_init_virtual_root_hub(struct hcd_priv *priv)
{
	/**
	 * - 1. fill root hub information:
	 * - in private area
	 */
	priv->rh.devnum				= 0;
	priv->rh.send				= 0;
	priv->rh.interval			= 0;
	priv->rh.numports			= 1;
	priv->rh.port_status.wPortChange	= 0;
	priv->rh.port_status.wPortStatus	= 0;
	priv->rh.wPortTrigger			= 0;
	priv->rh.wPortTriggerAccepted		= 0;
	priv->rh.hub_status.wHubChange		= 0;
	priv->rh.hub_status.wHubStatus		= 0;
}

