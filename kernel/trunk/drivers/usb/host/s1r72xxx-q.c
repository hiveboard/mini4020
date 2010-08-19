/**
 * @file	s1r72xxx-q.c
 * @brief	Process of Submit Data
 * @date	2007/03/15
 * @author	Luxun9 project team
 * Copyright (C)SEIKO EPSON CORPORATION. 2006-2007 All rights reserved.
 * This file is licenced under the GPLv2.
 *
 */

#define BUSMONITOR		0
#if BUSMONITOR
#define LIMITMONITOR		1
#define LIMITMONITORNO		64
#endif

/******************************************
 * Declarations of function prototype
 ******************************************/
static int s1r72xxx_hcd_enqueue(struct usb_hcd *hcd,
	struct usb_host_endpoint *ep, struct urb *urb, unsigned mflag);
static int s1r72xxx_hcd_dequeue(struct usb_hcd *hcd, struct urb *urb);
	
static void update_toggle(S_R_HC_CH *, unsigned int *);
static unsigned char cnv_toggle(int, unsigned int);
static unsigned char cnv_speed(unsigned char);
static unsigned short cnv_interval(unsigned short, struct urb *);
#if BUSMONITOR
static void s1r72xxx_bus_monitor(unsigned, int, int, int, int,
	unsigned char *);
#endif

static char get_devaddr(struct urb* urb);
static int get_ch_connected_to(struct hcd_priv* priv, unsigned char dev_addr,
	struct usb_host_endpoint* ep);
static int alloc_ch_resource(struct usb_hcd *hcd, struct hcd_priv* priv,
	unsigned char dev_addr, struct usb_host_endpoint* ep, struct urb* urb);
static void get_hubaddr_portnum(struct usb_hcd *hcd, struct urb* urb,
	unsigned char *dev_addr, unsigned char *port_num);
static void free_ch_resource(struct usb_hcd *hcd, S_R_HC_CH *ch_info,
	int status);

static int submit_control(struct usb_hcd *hcd,
	struct usb_host_endpoint *ep, unsigned mflag);
static int submit_bulk_intr(int ch, struct usb_hcd *hcd,
	struct usb_host_endpoint *ep, struct urb *urb, unsigned mflag);
static inline int check_fifo_remain(void);
static int set_setup_tran_data(struct usb_hcd *hcd, S_R_HC_CH *ch_info,
	S_R_HCD_QUEUE *hcd_queue, struct urb* urb);
static int set_in_tran_data(unsigned char ch, S_R_HC_CH *ch_info,
	S_R_HCD_QUEUE *hcd_queue, struct urb* urb);
static int set_out_tran_data(unsigned char ch, S_R_HC_CH *ch_info,
	S_R_HCD_QUEUE *hcd_queue, struct urb* urb);
static void complete_urb(struct usb_hcd *hcd, S_R_HCD_QUEUE *hcd_queue,
	S_R_HC_CH *ch_info, struct urb *urb, int status_flag);
static void check_device_request(struct urb *urb);

#ifdef DEBUG_PROC
extern void s1r72xxx_queue_log(unsigned char func_name, unsigned int data1,
	unsigned int data2);

#else
#define s1r72xxx_queue_log(x,y,z)
#endif

unsigned char	tran_type_id_tbl[USB_ENDPOINT_XFER_INT + 1] = {
	S_R_CH_TR_CONTROL, S_R_CH_TR_ISOC, S_R_CH_TR_BULK, S_R_CH_TR_INT
};


/**
 * @brief	- Enqueue process for URB.
 * @param	*hcd;	struct of usb_hcd
 * @param	*ep;	struct of usb_host_endpoint
 * @param	*urb;	struct of urb
 * @param	mflag;	flag
 * @retval	return 0;	complete
 *				if error return error code
 */
static int s1r72xxx_hcd_enqueue(struct usb_hcd *hcd, 
	struct usb_host_endpoint *ep, struct urb *urb, unsigned mflag)
{
	struct hcd_priv *priv = (struct hcd_priv *)hcd->hcd_priv[0];
	int ch;
	int allocated;
	int ret = 0;
	unsigned char	dev_addr;	/* device address */
	unsigned char	list_is_empty = 0;/* urb_list empty */
	S_R_HCD_QUEUE	*hcd_queue;
	unsigned long	lock_flags;

	DPRINT(DBG_API,"%s:enter\n",__FUNCTION__);

	/**
	 * - 1. Check URB:
	 *  - if error then return -EINVAL
	 */
	if (urb == (struct urb *)NULL) {
		DPRINT(DBG_API,"%s: urb is NULL \n", __FUNCTION__);
		s1r72xxx_queue_log(S_R_DEBUG_QUEUE, 0xFF, 0xFF);
		return -EINVAL;
	}

	if ( urb->status != -EINPROGRESS){
		return  urb->status;
	}

	if (ep == (struct usb_host_endpoint *)NULL) {
		DPRINT(DBG_API,"%s: ep is NULL \n", __FUNCTION__);
		s1r72xxx_queue_log(S_R_DEBUG_QUEUE, 0xFF, 0xFF);
		return -EINVAL;
	}
		
	/**
	 * - 2. Get memory area to save URB informations:
	 *  - if error then return -ENOMEM
	 */
	hcd_queue = (S_R_HCD_QUEUE *)kmalloc(sizeof(S_R_HCD_QUEUE), GFP_ATOMIC);
	if (hcd_queue == NULL){
		return -ENOMEM;
	}
	if (priv->called_state == S_R_CALLED_FROM_API){
		spin_lock_irqsave(&priv->lock, lock_flags);
	}
	INIT_LIST_HEAD(&hcd_queue->urb_list);
	hcd_queue->urb			= urb;
	hcd_queue->data_length	= urb->transfer_buffer_length;
	hcd_queue->ctrl_status	= CTRL_IDLE;
	D_QUEUE_INC(hcd_queue);

	/**
	 * - 3. Allocate CH:
	 *  - Check allocated CH to a same EP. If allocated CH is not exist, 
	 *    allocate new CH.:
	 */
	dev_addr	= get_devaddr(urb);
	allocated	= get_ch_connected_to(priv, dev_addr, ep);
	ch		= allocated;

	DPRINT(DBG_API,"%s: get ch connected %d\n",__FUNCTION__, allocated);
	s1r72xxx_queue_log(S_R_DEBUG_QUEUE, allocated, hcd_queue->count);
	s1r72xxx_queue_log(S_R_DEBUG_QUEUE, urb->transfer_buffer_length
		,urb->actual_length);

	if (allocated < 0){
		/* if this ep is not asigned early enqueue, allocate new CH. */
		ch = alloc_ch_resource(hcd, priv, dev_addr, ep, urb);
		DPRINT(DBG_API,"%s: alloc ch resource %d\n",__FUNCTION__,ch);
	}

	/**
	 * - 4. Add urb to queue list:
	 */
	if ((ch >= CH_0) && (ch < CH_MAX)){
		list_is_empty = list_empty(&priv->ch_info[ch].urb_list);
		list_add_tail(&hcd_queue->urb_list, &priv->ch_info[ch].urb_list);
		if (ch == CH_0){
			hcd_queue->ep = ep;
		}
		DPRINT(DBG_API,"%s: list add tail\n",__FUNCTION__);
	} else {
		/* CH full */
		printk("%s-usb:all channel have allocated\n",CHIP_NAME);
		s1r72xxx_queue_log(S_R_DEBUG_QUEUE, ch, 0xFF);

		kfree(hcd_queue);
		spin_unlock_irqrestore(&priv->lock, lock_flags);

		return -EINVAL;
	}

	if ( ((allocated >= 0) && (!list_is_empty))
		|| (!(priv->rh.port_status.wPortStatus & USB_PORT_STAT_CONNECTION)) ){

		DPRINT(DBG_API,"%s:already queued\n", __FUNCTION__);
		s1r72xxx_queue_log(S_R_DEBUG_QUEUE, ch, allocated);
		
		if (priv->called_state == S_R_CALLED_FROM_API){
			spin_unlock_irqrestore(&priv->lock, lock_flags);
		}
		return ret;
	}

	/**
	 * - 5. Submit data
	 */
	switch (usb_pipetype(urb->pipe)) {
	case PIPE_CONTROL:
		if (ch == CH_0) {
			/**
			 * - 5.1. submit control transaction:
			 */
			ret = submit_control(hcd, ep, mflag);
			DPRINT(DBG_API, "%s:submit control = %d \n",__FUNCTION__, ret);
		} else {
			DPRINT(DBG_API, "%s:bad CH%d\n",__FUNCTION__, ch);
			ret = -EINVAL;
		}
		break;
	case PIPE_BULK:
	case PIPE_INTERRUPT:
		/**
		 * - 5.2. pipe type is bulk or interrupt:
		 */
		ret = submit_bulk_intr(ch, hcd, ep, urb, mflag);
		break;
	case PIPE_ISOCHRONOUS:
	default:
		DPRINT(DBG_API,"%s:unknown transfer type\n",__FUNCTION__);
		ret = -EINVAL;
		break;
	}
	
	if(ret < 0){
		list_del_init(&hcd_queue->urb_list);
		kfree(hcd_queue);
	}

	DPRINT(DBG_API,"%s:exit\n",__FUNCTION__);
	s1r72xxx_queue_log(S_R_DEBUG_QUEUE,ch ,0xFF );

	if (priv->called_state == S_R_CALLED_FROM_API){
		spin_unlock_irqrestore(&priv->lock, lock_flags);
	}

	return ret;
}

/**
 * @brief	get device address.
 * @param	*urb	urb structure
 * @retval	device address.
 */
static char get_devaddr(struct urb* urb)
{
	/**
	 * - 1. Return device address:
	 */
	return (urb->dev->devnum & S_R_USB_ADDR_MASK) ;
}

/**
 * @brief	get channel number matches to queued data.
 * @param	*priv;		HCD private structure
 * @param	*dev_addr;	USB device address
 * @param	*ep;		endpoint structure
 * @retval	return CH number.
 */
static int get_ch_connected_to(struct hcd_priv* priv, unsigned char dev_addr,
	struct usb_host_endpoint* ep)
{
	unsigned char	ch_ct;		/* channel counter */
	unsigned char	ep_no;		/* endpoint number */
	
	ep_no	= ep->desc.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK;

	/**
	 * - 1. Serch from all CH informations that matches ep:
	 */
	/* endpoint 0 is shared by many devices */
	if ((priv->ch_info[CH_0].status == S_R_CH_ST_ACTIVE) && (ep_no == 0)){
		return CH_0;
	}
	for (ch_ct = CH_A ; ch_ct < CH_MAX ; ch_ct++){
		/* find connected CH */
		if ( (priv->ch_info[ch_ct].status == S_R_CH_ST_ACTIVE)
				&& (priv->ch_info[ch_ct].dev_addr == dev_addr)
				&& (priv->ch_info[ch_ct].ep_no == ep_no) )
		{
		/*modified by kyon @ 090626 SDK 3.2*/
				if ((((ep->desc.bEndpointAddress) & USB_ENDPOINT_DIR_MASK)
					== USB_DIR_IN && priv->ch_info[ch_ct].dir == S_R_CH_DIR_IN) 
					&& (((ep->desc.bEndpointAddress) & USB_ENDPOINT_DIR_MASK)
						== USB_DIR_OUT && priv->ch_info[ch_ct].dir == S_R_CH_DIR_OUT)	)		
					{					
						DPRINT(DBG_API,"%s:match CH%d addr=%d, ep=%d\n", __FUNCTION__,
							ch_ct, dev_addr, ep_no);
						return ch_ct;
					}
		}
	}

	DPRINT(DBG_API,"%s:CH not matched\n", __FUNCTION__);

	return -ENODEV;
}

/**
 * @brief	allocate CH.
 * @param	*hcd;		usb_hcd structure
 * @param	*priv;		HCD private structure
 * @param	dev_addr;	USB Device address will be connected to
 * @param	*ep;		endpoint structre
 * @param	*urb;		urb structure
 * @retval	return CH number
 */
static int alloc_ch_resource(struct usb_hcd *hcd, struct hcd_priv* priv,
	unsigned char dev_addr, struct usb_host_endpoint* ep, struct urb* urb)
{
	unsigned char	ch_ct;				/* channel counter */
	unsigned char	ep_no;				/* endpoint number */
	unsigned char	tran_type;			/* transfer type */
	unsigned char	config1_tmp = 0;	/* register temp */
	
	ep_no	= ep->desc.bEndpointAddress & USB_ENDPOINT_NUMBER_MASK;
	tran_type
		= tran_type_id_tbl[ep->desc.bmAttributes & USB_ENDPOINT_XFERTYPE_MASK];
	
	DPRINT(DBG_API,"%s:requested CH with maxpacket=%d, tran_type=%d\n",
		__FUNCTION__, ep->desc.wMaxPacketSize , tran_type);

	/**
	 * - 1. serch all CH informations
	 */
	for (ch_ct = CH_0 ; ch_ct < CH_MAX ; ch_ct++){
		/**
		 * - 1.1. if status is IDLE, check FIFO size and supported
		 *   transfer types 
		 */
		if (priv->ch_info[ch_ct].status == S_R_CH_ST_IDLE){
			/**
			 * - 1.1.1. Check FIFO size.
			 */
			if (priv->ch_info[ch_ct].fifo_size
				< le16_to_cpu(ep->desc.wMaxPacketSize)){
				/* FIFO size is small */
				DPRINT(DBG_API,"%s:CH%d packet size ng %d\n",
					__FUNCTION__, ch_ct, priv->ch_info[ch_ct].fifo_size);
				continue;
			}

			/**
			 * - 1.1.2. Check transfer type.
			 */
			if ( (priv->ch_info[ch_ct].tran_type & tran_type)
					!= tran_type){
				/* this transfer type is not supported */
				DPRINT(DBG_API,"%s:CH%d tran type ng\n", __FUNCTION__, ch_ct);
				continue;
			}

			/**
			 * - 1.1.3. available CH is found. initialize stuff.
			 */
			priv->ch_info[ch_ct].status		= S_R_CH_ST_ACTIVE;
			priv->ch_info[ch_ct].ep			= ep;
			if (urb->dev->state == USB_STATE_DEFAULT){
				priv->ch_info[ch_ct].dev_addr	= 0;
			} else {
				priv->ch_info[ch_ct].dev_addr	= dev_addr;
			}
			priv->ch_info[ch_ct].ep_no		= ep_no;

			get_hubaddr_portnum(hcd, urb,
				&priv->ch_info[ch_ct].hub_addr, &priv->ch_info[ch_ct].port_no);

			priv->ch_info[ch_ct].actual_size
				= priv->ch_info[ch_ct].fifo_size
					- ((priv->ch_info[ch_ct].fifo_size)
						% (ep->desc.wMaxPacketSize));
			priv->ch_info[ch_ct].is_short	= S_R_CH_LAST_ISNOT_SHORT;

			INIT_LIST_HEAD(&priv->ch_info[ch_ct].urb_list);

			/**
			 * - 1.1.4. Set hardware.
			 */
			S_R_REGS16(priv->ch_info[ch_ct].CHxMaxPktSize)
				= ep->desc.wMaxPacketSize;
			S_R_REGS8(priv->ch_info[ch_ct].CHxHubAdrs)
				= (S_R_HUB_ADDR_OFFSET(priv->ch_info[ch_ct].hub_addr)
					| priv->ch_info[ch_ct].port_no);
			S_R_REGS8(priv->ch_info[ch_ct].CHxFuncAdrs)
				= (S_R_FUNC_ADDR_OFFSET(priv->ch_info[ch_ct].dev_addr)
					| priv->ch_info[ch_ct].ep_no);

			switch (ch_ct){
			case CH_0:
				break;
			default:
				if (((ep->desc.bEndpointAddress) & USB_ENDPOINT_DIR_MASK)
					== USB_DIR_IN){
					S_R_REGS8(priv->ch_info[ch_ct].CHxConfig_0)
						= ( (cnv_speed(urb->dev->speed) & SpeedMode_MASK)
							| cnv_toggle(ep_no,
								urb->dev->toggle[S_R_CH_TGL_IN]) );
					DPRINT(DBG_API,"%s:toggle =%d\n",__FUNCTION__,
						cnv_toggle(ep_no,
							urb->dev->toggle[S_R_CH_TGL_IN]));
					priv->ch_info[ch_ct].dir	= S_R_CH_DIR_IN;
					config1_tmp = CHxTID_IN;
				} else {
					S_R_REGS8(priv->ch_info[ch_ct].CHxConfig_0)
						= ( (cnv_speed(urb->dev->speed) & SpeedMode_MASK)
							| cnv_toggle(ep_no,
								urb->dev->toggle[S_R_CH_TGL_OUT]) );
					DPRINT(DBG_API,"%s:toggle =%d\n",__FUNCTION__,
						cnv_toggle(ep_no,
							urb->dev->toggle[S_R_CH_TGL_OUT]));
					priv->ch_info[ch_ct].dir	= S_R_CH_DIR_OUT;
					config1_tmp = CHxTID_OUT;
				}
			}

			switch (ep->desc.bmAttributes & USB_ENDPOINT_XFERTYPE_MASK){
			case USB_ENDPOINT_XFER_CONTROL:
				priv->ch_info[ch_ct].cur_tran_type = S_R_CH_TR_CONTROL;
				break;
			case USB_ENDPOINT_XFER_ISOC:
				priv->ch_info[ch_ct].cur_tran_type = S_R_CH_TR_ISOC;
				config1_tmp |= CHxTranType_ISOC;
				break;
			case USB_ENDPOINT_XFER_BULK:
				priv->ch_info[ch_ct].cur_tran_type = S_R_CH_TR_BULK;
				config1_tmp |= CHxTranType_BULK;
				break;
			case USB_ENDPOINT_XFER_INT:
				priv->ch_info[ch_ct].cur_tran_type = S_R_CH_TR_INT;
				config1_tmp |= CHxTranType_INT;
				S_R_REGS16(priv->ch_info[ch_ct].CHxInterval)
					= cnv_interval(urb->interval, urb);
				break;
			default:
				break;
			}

			S_R_REGS8(priv->ch_info[ch_ct].CHxConfig_1)
				= config1_tmp;

			/* enable interrupt */
			switch (ch_ct){
			case CH_0:
				break;
			default:
				S_R_REGS8(priv->ch_info[ch_ct].CHxIntStat)
					= CHx_IntStat_AllBit;
				S_R_REGS8(priv->ch_info[ch_ct].CHxIntEnb)
					= TotalSizeCmp | ChangeCondition;
				S_R_REGS8(rcH_CHr_IntEnb) |= SET_CHR_NUM(ch_ct);
				break;
			}
			DPRINT(DBG_API,"%s:alloc CH%d to addr=%d, ep=%d\n", __FUNCTION__,
				ch_ct, dev_addr, ep_no);

			return ch_ct;
		}
	}
	return -ENODEV;
}

/**
 * @brief	get device address and port number.
 * @param	*hcd;	struct of usb_hcd
 * @param	*urb;		urb structure
 * @param	*dev_addr;	USB device address
 * @param	*port_num;	Port number
 * @retval	return.
 */
static void get_hubaddr_portnum(struct usb_hcd *hcd, struct urb* urb,
	unsigned char *dev_addr, unsigned char *port_num)
{
	/**
	 * - 1. Returns hub address and port number:
	 */
	if (urb->dev->tt) {
		*dev_addr = urb->dev->tt->hub->devnum;
		*port_num = urb->dev->ttport & 0x0F;
	} else {
		*dev_addr = hcd->self.root_hub->devnum;
		*port_num = hcd->self.busnum & 0x0F;
	}
	DPRINT(DBG_DATA,"%s:hub addr=%d, port num=%d\n",__FUNCTION__,
		*dev_addr, *port_num);
	return;
}

/**
 * @brief	free CH.
 * @param	*hcd;		struct of usb_hcd
 * @param	*ch_info;	CH informations
 * @param	status;		status
 * @retval	return CH number
 */
static void free_ch_resource(struct usb_hcd *hcd, 
	S_R_HC_CH *ch_info, int status)
{
	S_R_HCD_QUEUE	*hcd_queue = NULL;
	/**
	 * 1. serch all CH informations:
	 */
	while (!list_empty(&ch_info->urb_list)) {
		hcd_queue = list_entry(ch_info->urb_list.next, 
			S_R_HCD_QUEUE, urb_list);
		if (hcd_queue){
			/**
			 * 1.1. Complete urbs:
			 */
			hcd_queue->urb->status = status;
			list_del_init(&hcd_queue->urb_list);
			usb_hcd_giveback_urb(hcd, hcd_queue->urb, (struct pt_regs*)NULL);
			kfree(hcd_queue);
		}
	}

	/**
	 * 2. Initialize informations:
	 */
	ch_info->dir			= S_R_CH_DIR_UNKNOWN;
	ch_info->status			= S_R_CH_ST_IDLE;
	ch_info->ep			= NULL;
	ch_info->dev_addr		= 0;
	ch_info->ep_no			= 0;
	ch_info->hub_addr		= 0;
	ch_info->port_no		= 0;
	ch_info->actual_size		= 0;
	ch_info->is_short		= S_R_CH_LAST_ISNOT_SHORT;
	ch_info->cur_tran_type 		= S_R_CH_TR_NONE;
	INIT_LIST_HEAD(&ch_info->urb_list);

	/**
	 * 3. Clear all register:
	 */
	S_R_REGS8(ch_info->CHxConfig_0)		= S_R_REG_CLEAR;
	S_R_REGS16(ch_info->CHxMaxPktSize)	= S_R_REG_CLEAR;
	S_R_REGS8(ch_info->CHxHubAdrs)		= S_R_REG_CLEAR;
	S_R_REGS8(ch_info->CHxFuncAdrs)		= S_R_REG_CLEAR;
	S_R_REGS16(ch_info->CHxInterval)	= S_R_REG_CLEAR;
	S_R_REGS8(ch_info->CHxConfig_1)		= S_R_REG_CLEAR;
	S_R_REGS8(ch_info->CHxIntEnb)		= S_R_REG_CLEAR;
	S_R_REGS8(ch_info->CHxIntStat)		= CHx_IntStat_AllBit;
	if (ch_info->ch_no == CH_0){
		S_R_REGS8(ch_info->CTL_SUP_CONT)= S_R_REG_CLEAR;
	} else {
		S_R_REGS8(rcH_CHr_IntEnb)		&= ~(SET_CHR_NUM(ch_info->ch_no));
	}
	S_R_CH_FIFO_CLR(ch_info->ch_no);

	return;
}

/**
 * @brief	- Submit Control transfer
 * @param	*hcd;	struct of usb_hcd
 * @param	*ep;	struct of usb_host_endpoint
 * @param	mflag;	flag
 * @retval	return 0;	complete
 *				if error then return error code
 */
static int submit_control(struct usb_hcd *hcd, struct usb_host_endpoint *ep,
	unsigned mflag)
{
	struct hcd_priv	*priv = (struct hcd_priv *)hcd->hcd_priv[0];
	S_R_HCD_QUEUE		*hcd_queue = NULL;	/* handled urb */
	struct urb		*urb_queued = NULL;	/* handled urb */
	S_R_HC_CH		*ch_info;		/* CH informations */
	unsigned char		ch;			/* CH number */
	struct usb_ctrlrequest	*ctrlreq;
	int			retval = 0;

	ch = CH_0;
	ch_info = &priv->ch_info[ch];

	s1r72xxx_queue_log(S_R_DEBUG_SUBMIT, ch, ch_info->ep_no);

	/**
	 * - 1. Get urb from queue:
	 */
	if (list_empty(&ch_info->urb_list)) {
		/* empty */
		DPRINT(DBG_DATA,"%s:queue list is empty\n",__FUNCTION__);
		urb_queued = NULL;
	} else {
		DPRINT(DBG_DATA,"%s:already queued\n",__FUNCTION__);
		hcd_queue = list_entry(ch_info->urb_list.next, S_R_HCD_QUEUE,
			urb_list);
		urb_queued = hcd_queue->urb;
	}
	if (urb_queued == NULL){
		/* empty */
		return -EINVAL;
	}

	ctrlreq = (struct usb_ctrlrequest*)(urb_queued->setup_packet);

	/**
	 * - 2. Set data to hardware:
	 */
	switch (hcd_queue->ctrl_status){
	case CTRL_IDLE:
		/**
		 * - 2.1. SETUP stage:
		 */
		s1r72xxx_queue_log(S_R_DEBUG_SUBMIT, urb_queued->setup_packet[0],
			urb_queued->setup_packet[1]);
		retval = set_setup_tran_data(hcd, ch_info, hcd_queue, urb_queued);
		if (retval == 0){
			hcd_queue->ctrl_status = CTRL_SETUP;
		}
		break;
	
	case CTRL_SETUP:
		/**
		 * - 2.1.1. change to DATA stage:
		 *  - wLength is not zero, set data and change status to DATA IN/OUT. 
		 *    if wLength is zero, change to STATUS IN/OUT.
		 */
		/* IN Transaction */
		if ((urb_queued->setup_packet[0]) & USB_DIR_IN) {
			/* IN direction */
			if (le16_to_cpu(ctrlreq->wLength)){
				S_R_REGS8(priv->ch_info[ch].CHxConfig_1) = CHxTID_IN;

				hcd_queue->data_length = le16_to_cpu(ctrlreq->wLength);
				hcd_queue->ctrl_status = CTRL_DATAIN;

				retval = set_in_tran_data(ch, ch_info, hcd_queue, urb_queued);

			} else {
				S_R_REGS8(priv->ch_info[ch].CHxConfig_1) = CHxTID_OUT;

				hcd_queue->ctrl_status = CTRL_STATUSOUT;

				retval = set_out_tran_data(ch, ch_info, hcd_queue, urb_queued);
			}
		/* OUT Transaction */
		} else {
			if (le16_to_cpu(ctrlreq->wLength)){
				S_R_REGS8(priv->ch_info[ch].CHxConfig_1) = CHxTID_OUT;

				hcd_queue->data_length = le16_to_cpu(ctrlreq->wLength);
				hcd_queue->ctrl_status = CTRL_DATAOUT;

				retval = set_out_tran_data(ch, ch_info, hcd_queue, urb_queued);

			} else {
				S_R_REGS8(priv->ch_info[ch].CHxConfig_1) = CHxTID_IN;

				hcd_queue->ctrl_status = CTRL_STATUSIN;

				retval = set_in_tran_data(ch, ch_info, hcd_queue, urb_queued);
			}
		}
		check_device_request(urb_queued);
		break;

	case CTRL_DATAIN:
		/**
		 * - 2.2. DATA stage(IN):
		 */
		retval = set_in_tran_data(ch, ch_info, hcd_queue, urb_queued);

		if (retval == S_R_CH_ALLDATA_CMP){
			S_R_REGS8(priv->ch_info[ch].CHxConfig_0) |= CHxConfig_Toggle;
			S_R_REGS8(priv->ch_info[ch].CHxConfig_1) = CHxTID_OUT;
			hcd_queue->ctrl_status = CTRL_STATUSOUT;

			retval = set_out_tran_data(ch, ch_info, hcd_queue, urb_queued);
		}
		break;

	case CTRL_DATAOUT:
		/**
		 * - 2.3. DATA stage(OUT):
		 */
		retval = set_out_tran_data(ch, ch_info, hcd_queue, urb_queued);

		if (retval == S_R_CH_ALLDATA_CMP){
			S_R_REGS8(priv->ch_info[ch].CHxConfig_0) |= CHxConfig_Toggle;
			S_R_REGS8(priv->ch_info[ch].CHxConfig_1) = CHxTID_IN;

			hcd_queue->ctrl_status = CTRL_STATUSIN;

			retval = set_in_tran_data(ch, ch_info, hcd_queue, urb_queued);
		}
		break;

	case CTRL_STATUSIN:
	case CTRL_STATUSOUT:
		/**
		 * - 2.4. STATUS stage(IN/OUT):
		 */
		DPRINT(DBG_DATA,"%s:all urb data processed\n",__FUNCTION__);
		complete_urb(hcd, hcd_queue, ch_info, urb_queued, 0);
		break;
	default:
		retval = -EINVAL;
		break;
	}

	DPRINT(DBG_API,"%s:submit control completed\n",__FUNCTION__);

	return retval;
}

/**
 * @brief	- submit bulk or interrupt transfer
 * @param	ch;	channel number
 * @param	*hcd;	struct of usb_hcd
 * @param	*ep;	struct of usb_host_endpoint
 * @param	*urb;	struct of urb
 * @param	mflag;	flag
 * @retval	return 0;	complete
 *				if error then error code
 */
static int submit_bulk_intr(int ch, struct usb_hcd *hcd,
	struct usb_host_endpoint *ep, struct urb *urb, unsigned mflag)
{
	struct hcd_priv *priv = (struct hcd_priv *)hcd->hcd_priv[0];

	S_R_HCD_QUEUE	*hcd_queue = NULL;	/* handled urb */
	struct urb	*urb_queued = NULL;	/* handled urb */
	S_R_HC_CH	*ch_info;		/* CH informations */
	int		retval;

	ch_info = &priv->ch_info[ch];

	s1r72xxx_queue_log(S_R_DEBUG_SUBMIT, ch, ch_info->ep_no);
	
	/**
	 * - 1. Get urb from queue:
	 */
	if (list_empty(&ch_info->urb_list)) {
		/* empty */
		DPRINT(DBG_DATA,"%s:queue list is empty\n",__FUNCTION__);
		urb_queued = NULL;
	} else {
		DPRINT(DBG_DATA,"%s:already queued\n",__FUNCTION__);
		hcd_queue = list_entry(ch_info->urb_list.next, S_R_HCD_QUEUE,
			urb_list);
		urb_queued = hcd_queue->urb;
	}
	if (urb_queued == NULL){
		/* empty */
		return -EINVAL;
	}

	/**
	 * - 2. Set data to hardware:
	 */
	/* IN Transaction */
	if (usb_pipein(urb_queued->pipe)) {
		/* set join register*/
		retval = set_in_tran_data(ch, ch_info, hcd_queue, urb_queued);
	/* OUT Transaction */
	} else {
		/* set join register*/
		retval = set_out_tran_data(ch, ch_info, hcd_queue, urb_queued);
	}
	s1r72xxx_queue_log(S_R_DEBUG_SUBMIT, ch, retval);

	/**
	 * - 3. Complete urb:
	 */
	/* if length = actual, complete this transaction */
	if(retval == S_R_CH_ALLDATA_CMP){
		DPRINT(DBG_DATA,"%s:all urb data processed\n",__FUNCTION__);
		complete_urb(hcd, hcd_queue, ch_info, urb_queued, 0);
	} else if (retval < 0) {
		DPRINT(DBG_DATA,"%s:error occured\n",__FUNCTION__);
		complete_urb(hcd, hcd_queue, ch_info, urb_queued, -ENODEV);
	}

#if BUSMONITOR
	s1r72xxx_bus_monitor(PIPE_BULK,
		(ep->desc.bEndpointAddress & USB_DIR_IN) ? 1 : 0,
		ch, epno, trano, urb->transfer_buffer);
#endif
	DPRINT(DBG_API,"%s:submit bulk completed\n",__FUNCTION__);
	return 0;
}

/**
 * @brief	- check FIFORdRemain register
 * @retval	return 0;	complete
 *			if error then error code
 */
static inline int check_fifo_remain(void)
{
	unsigned char	timeout_ct = 0;	/* timeout counter */
	
	/**
	 * - 1. Check RdRemainValid bit to confirm validation of RdRemain:
	 */
	while(!(S_R_REGS16(rsC_FIFO_RdRemain_HL) & RdRemainValid)){
		timeout_ct ++;
		if (timeout_ct > S_R_RD_REMAIN_TIMEOUT){
			DPRINT(DBG_API,"%s:time out\n",__FUNCTION__);
			return -EBUSY;
		}
	}
	return 0;
}


/**
 * @brief	- set CH0
 * @param	*hcd;		struct of usb_hcd
 * @param	*ch_info;	CH informations
 * @param	*hcd_queue;	struct of S_R_HCD_QUEUE
 * @param	*urb;		struct of urb
 * @retval	return 0;	complete
 *				if error then error code
 */
void set_ch0_registers(struct usb_hcd *hcd, S_R_HC_CH *ch_info,
	S_R_HCD_QUEUE *hcd_queue, struct urb* urb)
{
	unsigned char	dev_addr;		/* USB device address */
	unsigned char	config1_tmp;		/* Config_1 register value */
	struct usb_ctrlrequest*		ctrlreq;/* device request */

	ch_info->cur_tran_type = S_R_CH_TR_CONTROL;

	dev_addr	= get_devaddr(urb);
	ch_info->status	= S_R_CH_ST_ACTIVE;
	if (urb->dev->state == USB_STATE_DEFAULT){
		ch_info->dev_addr	= 0;
	} else {
		ch_info->dev_addr	= dev_addr;
	}
	ch_info->ep_no	= 0;

	get_hubaddr_portnum(hcd, urb,
		&ch_info->hub_addr, &ch_info->port_no);

	ch_info->actual_size
		= ch_info->fifo_size
			- (unsigned int)(ch_info->fifo_size % ch_info->ep->desc.wMaxPacketSize);
	ch_info->is_short	= S_R_CH_LAST_ISNOT_SHORT;

	S_R_REGS16(ch_info->CHxMaxPktSize)
				= hcd_queue->ep->desc.wMaxPacketSize;
	S_R_REGS8(ch_info->CHxHubAdrs)
		= (S_R_HUB_ADDR_OFFSET(ch_info->hub_addr)
			| ch_info->port_no);
	S_R_REGS8(ch_info->CHxFuncAdrs)
		= (S_R_FUNC_ADDR_OFFSET(ch_info->dev_addr)
			| ch_info->ep_no);
	
	S_R_REGS8(ch_info->CHxConfig_0)
		= (cnv_speed(urb->dev->speed) & SpeedMode_MASK);
	ctrlreq = (struct usb_ctrlrequest*)(urb->setup_packet);
	if (urb->setup_packet[ctrlreq->bRequestType] & USB_DIR_IN) {
		config1_tmp = CHxTID_IN;
	} else {
		config1_tmp = CHxTID_OUT;
	}
	config1_tmp |= CHxTID_SETUP;
	S_R_REGS8(ch_info->CHxConfig_1)
		= config1_tmp;
	ch_info->cur_tran_type = S_R_CH_TR_CONTROL;

	/* enable interrupt */
	S_R_REGS8(ch_info->CHxIntStat)
		= CH0_IntStat_AllBit;
	S_R_REGS8(ch_info->CHxIntEnb)
		= TotalSizeCmp |ChangeCondition;
	S_R_REGS8(rcC_HostIntEnb) |= EnH_CH0IntStat;

	return;
}

/**
 * @brief	- set SETUP transaction data to FIFO
 * @param	*hcd;		struct of usb_hcd
 * @param	*ch_info;	CH informations
 * @param	*hcd_queue;	struct of usb_queue
 * @param	*urb;		struct of urb
 * @retval	return 0;	complete
 *				if error then error code
 */
static int set_setup_tran_data(struct usb_hcd *hcd, S_R_HC_CH *ch_info,
	S_R_HCD_QUEUE *hcd_queue, struct urb* urb)
{
	unsigned char	setup_ct;		/* fifo pointer for 8 bit access */
	unsigned char	config0_tmp;		/* register value */
	DPRINT(DBG_OUT,"%s:enter\n",__FUNCTION__);
	
	if(urb->setup_packet == NULL) {
		return -EINVAL;
	}

	/*
	 * - 1. set hub address and function address:
	 */
	set_ch0_registers(hcd, ch_info, hcd_queue, urb);

	/**
	 * - 2. write SETUP packet:
	 */
	ch_info->queue_set = S_R_QUEUE_IS_SET;
	S_R_REGS8(ch_info->CHxConfig_1) = CHxTID_SETUP;
	for (setup_ct = 0; setup_ct < S_R_SETUP_PACKET_LEN ; setup_ct++){
		S_R_REGS8(rcH_CH0SETUP_0 + setup_ct)
			= urb->setup_packet[setup_ct];
		DPRINT(DBG_OUT,"0x%02x ", urb->setup_packet[setup_ct]);
	}
	DPRINT(DBG_OUT,"\n");
	
	config0_tmp = S_R_REGS8(ch_info->CHxConfig_0) & (~CHxConfig_Toggle);
	S_R_REGS8(ch_info->CHxConfig_0) = config0_tmp | CHxConfig_TranGo;
	DPRINT(DBG_OUT,"%s:exit\n",__FUNCTION__);

	return 0;
}

/**
 * @brief	- set IN transaction data to FIFO
 * @param	ch		channel number
 * @param	*ch_info;	CH informations
 * @param	*hcd_queue	struct of S_R_HCD_QUEUE
 * @param	*urb;		struct of urb
 * @retval	return 0;	complete
 *				if error then error code
 */
static int set_in_tran_data(unsigned char ch, S_R_HC_CH *ch_info,
	S_R_HCD_QUEUE *hcd_queue, struct urb* urb)
{
	int		wait_join;		/* return value from sub function */
	int		retval;			/* return value */
	unsigned short	fifo_remain;		/* data in FIFO */
	unsigned int	read_ct;		/* read data counter */
	unsigned int	buf_remain;		/* data still in buffer */
	unsigned short	*fifo_16;		/* fifo pointer for 16 bit access */
	unsigned char	*fifo_8;		/* fifo pointer for 8 bit access */

	retval = S_R_CH_DATA_REMAIN;

	/**
	 * - 1. Get number of data in FIFO:
	 */
	/* set join register*/
	S_R_REGS8(ch_info->CHxJoin) = S_R_JoinCPU_Rd;

	/* FIFO check */
	wait_join = check_fifo_remain();
	if (wait_join < 0){
		DPRINT(DBG_IN,"%s:RdRemainValid error\n",__FUNCTION__);
		/* clear join register*/
		S_R_REGS8(ch_info->CHxJoin) = S_R_REG_CLEAR;
		return retval;
	}

	/* if FIFO is empty, setup CH to recieve data */
	fifo_remain = S_R_REGS16(rsC_FIFO_RdRemain_HL) & RdRemain;
	if (hcd_queue->ctrl_status == CTRL_STATUSIN){
		buf_remain = 0;
	} else {
		buf_remain = hcd_queue->data_length - urb->actual_length;
	}
	ch_info->queue_set = S_R_QUEUE_IS_SET;
	
	DPRINT(DBG_IN,"%s:buf len = %d, actual = %d\n",__FUNCTION__,
		urb->transfer_buffer_length, urb->actual_length);
	DPRINT(DBG_IN,"%s:FIFO data = %d\n",__FUNCTION__, fifo_remain);
	s1r72xxx_queue_log(S_R_DEBUG_IN, urb->transfer_buffer_length,
		urb->actual_length);
	s1r72xxx_queue_log(S_R_DEBUG_IN, fifo_remain, buf_remain);

	/**
	 * - 2. Set TranGo when FIFO is empty:
	 */
	if ((fifo_remain < 1) && (ch_info->is_short != S_R_CH_LAST_IS_SHORT)){
		/**
		 * - 2.1. Clear FIFO:
		 */
		DPRINT(DBG_IN,"%s:CH%d FIFO clear\n",__FUNCTION__, ch);
		S_R_CH_FIFO_CLR(ch);

		/**
		 * - 2.2. Start data recieve:
		 */
		/* set total size */
		read_ct = min(ch_info->actual_size, buf_remain);
		S_R_REGS16(ch_info->CHxTotalSize_H) = S_R_CH_TOTALSIZE_H(read_ct);
		S_R_REGS16(ch_info->CHxTotalSize_L) = S_R_CH_TOTALSIZE_L(read_ct);
		S_R_REGS8(ch_info->CHxConfig_0) |= CHxConfig_TranGo;

	/**
	 * - 3. Read data when FIFO is empty:
	 */
	} else {
		/**
		 * - 3.1. Read data:
		 */
		/* set total size */
		read_ct = min((unsigned int)fifo_remain, buf_remain);
		fifo_16 = (unsigned short*)(urb->transfer_buffer + urb->actual_length);
		urb->actual_length += read_ct;

		if(urb->transfer_buffer == NULL){
			printk("%s:transfer_buffer is NULL. transfer_buffer_length=%d\n",
				__FUNCTION__, urb->transfer_buffer_length);
			S_R_REGS8(ch_info->CHxJoin) = S_R_REG_CLEAR;
			return -EINVAL;
		}
		DPRINT(DBG_IN,"in :\n");

		while ( read_ct > 1 ) {
			*fifo_16 = S_R_REGS16(rsC_FIFO_Rd);
			DPRINT(DBG_DUMP,"0x%04x ", *fifo_16);
			fifo_16++;
			read_ct -= 2;
		}

		if (read_ct == 1){
			fifo_8 = (unsigned char*)(urb->transfer_buffer
				+ urb->actual_length - 1);
			*fifo_8 = S_R_REGS8(rcC_FIFO_ByteRd);
			DPRINT(DBG_DUMP,"0x%02x ", *fifo_8);
		}
		DPRINT(DBG_IN,"\n");

		/**
		 * - 3.2. Check urb is full or not:
		 */
		if ((hcd_queue->data_length == urb->actual_length)
			|| (ch_info->is_short == S_R_CH_LAST_IS_SHORT)){
			DPRINT(DBG_IN,"%s:all data read complete or short packet\n",
				__FUNCTION__);
			ch_info->is_short = S_R_CH_LAST_ISNOT_SHORT;
			retval = S_R_CH_ALLDATA_CMP;
		} else {
			buf_remain = hcd_queue->data_length - urb->actual_length;
			DPRINT(DBG_IN,"%s:continue %04x\n",__FUNCTION__, buf_remain);

			/* set total size */
			read_ct = min(ch_info->actual_size, buf_remain);
			S_R_REGS16(ch_info->CHxTotalSize_H)
				= S_R_CH_TOTALSIZE_H(read_ct);
			S_R_REGS16(ch_info->CHxTotalSize_L)
				= S_R_CH_TOTALSIZE_L(read_ct);
			S_R_REGS8(ch_info->CHxConfig_0) |= CHxConfig_TranGo;
		}
		s1r72xxx_queue_log(S_R_DEBUG_IN, urb->transfer_buffer_length,
			urb->actual_length);

	}

	/* clear join register*/
	S_R_REGS8(ch_info->CHxJoin) = S_R_REG_CLEAR;

	return retval;
}

/**
 * @brief	- set OUT transaction data to FIFO
 * @param	ch;		channel number
 * @param	*ch_info;	CH informations
 * @param	*hcd_queue;	struct of S_R_HCD_QUEUE
 * @param	*urb;		struct of urb
 * @retval	return 0;	complete
 *				if error then error code
 */
static int set_out_tran_data(unsigned char ch, S_R_HC_CH *ch_info,
	S_R_HCD_QUEUE *hcd_queue, struct urb* urb)
{
	unsigned short	fifo_remain;		/* data in FIFO */
	unsigned int	write_ct;		/* write data counter */
	unsigned int	buf_remain;		/* data still in buffer */
	unsigned short	*fifo_16;		/* fifo pointer for 16 bit access */
	unsigned char	*fifo_8;		/* fifo pointer for 8 bit access */
	int		retval;

	retval = S_R_CH_DATA_REMAIN;

	s1r72xxx_queue_log(S_R_DEBUG_OUT, urb->transfer_buffer_length,
		urb->actual_length);

	/**
	 * - 1. Data length check:
	 *  - if all data sending is completed. retrun all_data_cmp flag.
	 *  - when state is control STATUS, must set zero length packet.
	 */
	if ( !((usb_pipetype(urb->pipe) == PIPE_CONTROL)
		&& (hcd_queue->ctrl_status == CTRL_STATUSOUT)) ) {
		if ( hcd_queue->data_length == urb->actual_length ){
			DPRINT(DBG_OUT,"%s:finished\n",__FUNCTION__);
			return S_R_CH_ALLDATA_CMP;
		}
	}

	/**
	 * - 2. Get number of data in FIFO:
	 */
	/* set join register*/
	S_R_REGS8(ch_info->CHxJoin) = S_R_JoinCPU_Wr;

	/* FIFO check */
	fifo_remain = S_R_REGS16(rsC_FIFO_WrRemain_HL);

	DPRINT(DBG_OUT,"%s:FIFO space = %d,%d\n",__FUNCTION__,
		fifo_remain, ch_info->fifo_size);
	s1r72xxx_queue_log(S_R_DEBUG_OUT, fifo_remain, ch_info->fifo_size);

	if (fifo_remain < ch_info->fifo_size){
		DPRINT(DBG_OUT,"%s:FIFO has data = %d\n",__FUNCTION__, fifo_remain);
		/* clear join register*/
		S_R_REGS8(ch_info->CHxJoin) = S_R_REG_CLEAR;
		/* FIFO is not empty */
		return retval;
	}
	ch_info->queue_set = S_R_QUEUE_IS_SET;

	/* FIFO clear */
	S_R_CH_FIFO_CLR(ch);

	/* set total size */
	DPRINT(DBG_OUT,"%s:buf = %d, act = %d\n",__FUNCTION__,
		urb->transfer_buffer_length, urb->actual_length);

	if (hcd_queue->ctrl_status == CTRL_STATUSOUT){
		buf_remain = 0;
	} else {
		buf_remain = hcd_queue->data_length - urb->actual_length;
	}
	write_ct = min(ch_info->actual_size, buf_remain);
	S_R_REGS16(ch_info->CHxTotalSize_H) = S_R_CH_TOTALSIZE_H(write_ct);
	S_R_REGS16(ch_info->CHxTotalSize_L) = S_R_CH_TOTALSIZE_L(write_ct);

	/* write data to FIFO */
	fifo_16 = (unsigned short*)(urb->transfer_buffer + urb->actual_length);
	urb->actual_length += write_ct;

	/**
	 * - 3. Write data:
	 */
	DPRINT(DBG_OUT,"out :\n");
	s1r72xxx_queue_log(S_R_DEBUG_OUT, write_ct, fifo_remain);
	while ( write_ct > 1 ) {
		DPRINT(DBG_DUMP,"0x%04x ", *fifo_16);
		S_R_REGS16(rsC_FIFO_Wr) = *fifo_16;
		fifo_16++;
		write_ct -= 2;
	}
	
	if (write_ct == 1){
		fifo_8 = (unsigned char*)(urb->transfer_buffer
			+ (urb->actual_length) - 1);
		DPRINT(DBG_DUMP,"0x%02x ", *fifo_8);
		S_R_REGS8(rcC_FIFO_Wr_0) = *fifo_8;
	}
	DPRINT(DBG_OUT,"\n");
	
	DPRINT(DBG_OUT,"%s:FIFO space = %d\n",__FUNCTION__,
		S_R_REGS16(rsC_FIFO_WrRemain_HL));
	s1r72xxx_queue_log(S_R_DEBUG_OUT, urb->actual_length,
		S_R_REGS16(rsC_FIFO_WrRemain_HL));

	/* clear join register*/
	S_R_REGS8(ch_info->CHxJoin) = S_R_REG_CLEAR;

	/**
	 * - 4. Start data send:
	 */
	/* submit */
	if (urb->transfer_flags & URB_ZERO_PACKET){
		S_R_REGS8(ch_info->CHxConfig_1) |= AutoZerolen;
	}
	
	S_R_REGS8(ch_info->CHxConfig_0) |= CHxConfig_TranGo;
	DPRINT(DBG_OUT,"%s:CHxConfig_0 = 0x%02x\n",__FUNCTION__,
		S_R_REGS8(ch_info->CHxConfig_0));
	s1r72xxx_queue_log(S_R_DEBUG_OUT, ch, S_R_REGS8(ch_info->CHxConfig_0));

	return retval;
}

/**
 * @brief	- complete urb
 * @param	*hcd;		struct of usb_hcd
 * @param	*hcd_queue;	struct of S_R_HCD_QUEUE
 * @param	*ch_info;	CH informations
 * @param	*urb;		struct of urb
 * @param	status_flag;	urb status return to core driver
 * @retval	return 0;	complete
 *				if error then error code
 */
static void complete_urb(struct usb_hcd *hcd, S_R_HCD_QUEUE *hcd_queue,
	S_R_HC_CH *ch_info, struct urb *urb, int status_flag)
{
	S_R_HCD_QUEUE	*hcd_queue_tmp = NULL;	/* handled urb */
	struct urb	*urb_queued_tmp = NULL;	/* handled urb */
	int		retval = 0;

	s1r72xxx_queue_log(S_R_DEBUG_COMP, ch_info->ch_no, ch_info->ep_no);
	s1r72xxx_queue_log(S_R_DEBUG_COMP, hcd_queue->count, status_flag);

	/**
	 * - 1. Set status flag to urb:
	 */
	urb->status = status_flag;

	list_del_init(&hcd_queue->urb_list);

	if (usb_pipein(urb->pipe)) {
		update_toggle(ch_info, &urb->dev->toggle[S_R_CH_TGL_IN]);
	} else {
		update_toggle(ch_info, &urb->dev->toggle[S_R_CH_TGL_OUT]);
	}

	/**
	 * - 2. Give back urb to core driver:
	 */
	ch_info->queue_set = S_R_QUEUE_ISNOT_SET;
	usb_hcd_giveback_urb(hcd, urb, (struct pt_regs*)NULL);

	kfree(hcd_queue);

	/**
	 * - 3. Next data check:
	 */
	while((!(list_empty(&ch_info->urb_list))
		&& (ch_info->queue_set == S_R_QUEUE_ISNOT_SET))){
		DPRINT(DBG_DATA,"%s:list is not empty\n",__FUNCTION__);
		/* Get Transaction */
		hcd_queue_tmp = list_entry(ch_info->urb_list.next,
			S_R_HCD_QUEUE, urb_list);
		/* prepare for next next transaction */
		urb_queued_tmp = hcd_queue_tmp->urb;
		if (usb_pipetype(urb_queued_tmp->pipe) == PIPE_CONTROL){
			retval = set_setup_tran_data(hcd, ch_info, hcd_queue_tmp,
				urb_queued_tmp);
			if (retval == 0){
				hcd_queue_tmp->ctrl_status = CTRL_SETUP;
			}
		} else if (usb_pipein(urb_queued_tmp->pipe)) {
			retval = set_in_tran_data(ch_info->ch_no, ch_info, hcd_queue_tmp,
				urb_queued_tmp);
		} else {
			retval = set_out_tran_data(ch_info->ch_no, ch_info,
				hcd_queue_tmp, urb_queued_tmp);
		}
		if (retval < 0) {
			DPRINT(DBG_DATA,"%s:error\n",__FUNCTION__);
			list_del_init(&hcd_queue_tmp->urb_list);
			ch_info->queue_set = S_R_QUEUE_ISNOT_SET;
			usb_hcd_giveback_urb(hcd, urb_queued_tmp, (struct pt_regs*)NULL);
			kfree(hcd_queue_tmp);
		} else {
			DPRINT(DBG_DATA,"%s:new urb is set\n",__FUNCTION__);
			break;
		}
	}

	return;
}

/**
 * @brief	- change drive state by device requests
 * @param	*urb;	struct of urb
 * @retval	none
 * cf. USB2.0 chapter9
 */
static void check_device_request(struct urb *urb)
{
	/**
	 * - 1. update device state:
	 * - by setup data in control transaction
	 */
	switch (urb->setup_packet[1]) {
	/**
	 * - 1.1. Request is USB_REQ_SET_ADDRESS:
	 * - by setup data in control transaction
	 */
	case USB_REQ_SET_ADDRESS:
		switch (urb->dev->state) {
		/**
		 * - 1.1.1. USB_STATE_DEFAULT:
		 */
		case USB_STATE_DEFAULT:
			if ( (urb->setup_packet[2] != 0)
				|| (urb->setup_packet[3] != 0)) {
				urb->dev->state = USB_STATE_ADDRESS;
			}
			break;
		/**
		 * - 1.1.2. USB_STATE_ADDRESS:
		 */
		case USB_STATE_ADDRESS:
			if (urb->setup_packet[2] == 0 
				&& urb->setup_packet[3] == 0) {
				urb->dev->state = USB_STATE_DEFAULT;
			}
			break;
		/**
		 * - 1.1.3. USB_STATE_CONFIGURED:
		 */
		case USB_STATE_CONFIGURED:
		default:
			break;
		}
		break;
	/**
	 * - 1.2. Request is USB_REQ_SET_CONFIGURATION:
	 * - by setup data in control transaction
	 */
	case USB_REQ_SET_CONFIGURATION:
		switch (urb->dev->state) {
		/**
		 * - 1.2.1. USB_STATE_DEFAULT:
		 */
		case USB_STATE_DEFAULT:
				break;
		/**
		 * - 1.2.2. USB_STATE_ADDRESS:
		 */
		case USB_STATE_ADDRESS:
				if (urb->setup_packet[2] != 0
					|| urb->setup_packet[3] != 0) {
					urb->dev->state = USB_STATE_CONFIGURED;
				}
				break;
		/**
		 * - 1.2.3. USB_STATE_CONFIGURED:
		 */
		case USB_STATE_CONFIGURED:
				if (urb->setup_packet[2] == 0
					&& urb->setup_packet[3] == 0) {
					//urb->dev->state = USB_STATE_ADDRESS;  //moved by kyon @ 090626
				}
				break;
		default:
				break;
		}
		break;
	default:
		break;
	}

	return;
}

/**
 * @brief	- process of dequeue
 * @param	*hcd;	struct of usb_hcd
 * @param	*urb;	struct of urb
 * @retval	return 0;	complete
 */
static int s1r72xxx_hcd_dequeue(struct usb_hcd *hcd, struct urb *urb)
{
	struct hcd_priv *priv = (struct hcd_priv *)hcd->hcd_priv[0];
	int ch;
	S_R_HCD_QUEUE	*hcd_queue;
	unsigned long	lock_flags;
	DPRINT(DBG_API,"%s:enter\n", __FUNCTION__);
	s1r72xxx_queue_log(S_R_DEBUG_DEQUEUE,0 , urb->transfer_buffer_length);

	/**
	 * - 1. Serch a urb that matches parameter urb:
	 */
	spin_lock_irqsave(&priv->lock, lock_flags);
	if (urb == (struct urb*)NULL) {
		DPRINT(DBG_API,"%s: urb is NULL \n", __FUNCTION__);
		s1r72xxx_queue_log(S_R_DEBUG_DEQUEUE, 0xFF, 0xFF);
		return -EINVAL;
	}
	
	for (ch = CH_0 ; ch < CH_MAX ; ch++){
		list_for_each_entry(hcd_queue, &priv->ch_info[ch].urb_list, urb_list) {
			if (hcd_queue->urb == urb){
				break;
			}
		}
		if (hcd_queue->urb == urb){
			break;
		}
	}
	if (ch == CH_MAX){
		DPRINT(DBG_API,"%s: ch error \n", __FUNCTION__);
		s1r72xxx_queue_log(S_R_DEBUG_DEQUEUE, 0xFF, 0xFF);
		return -EINVAL;
	}
	
	/**
	 * - 2. Complete urb:
	 */
	priv->called_state = S_R_CALLED_FROM_OTH;
	if (hcd_queue->urb == urb) {
		DPRINT(DBG_API,"%s:dequeue CH%d urb\n", __FUNCTION__, ch);
		if (hcd_queue->urb->status == -EINPROGRESS){
			complete_urb(hcd, hcd_queue, &priv->ch_info[ch], hcd_queue->urb,
				-ECONNRESET);
		} else {
			complete_urb(hcd, hcd_queue, &priv->ch_info[ch], hcd_queue->urb,
				hcd_queue->urb->status);
		}
		if (list_empty(&priv->ch_info[ch].urb_list)) {
			/* empty */
			DPRINT(DBG_API,"%s:free CH%d urb\n", __FUNCTION__, ch);
			free_ch_resource(hcd, &priv->ch_info[ch], 0);
		}
	}
	s1r72xxx_queue_log(S_R_DEBUG_DEQUEUE,0xFF , 0xFF);

	priv->called_state = S_R_CALLED_FROM_API;
	spin_unlock_irqrestore(&priv->lock, lock_flags);

	DPRINT(DBG_API,"%s:exit\n", __FUNCTION__);
	return 0;
}

/**
 * @brief	- Update toggle infomation
 * @param	*ch_info;	S_R_HC_CH structure
 * @param	*ptog;		image of toggle
 * @retval	none
 */
static void update_toggle(S_R_HC_CH *ch_info, unsigned int *ptog)
{
	unsigned char stat;

	if (ch_info->ep_no < 16) {
		if ((ch_info->ch_no >= CH_0) && (ch_info->ch_no < CH_MAX)) {
			stat = S_R_REGS8(ch_info->CHxConfig_0);
		} else {
			return;		/* noop */
		}
		if (stat & CHxConfig_Toggle_MASK) {
			/* bit 0 is toggle state at EP0, bit 1 is toggle state at EP1.. */
			*ptog |= 0x00000001 << ch_info->ep_no;
		} else {
			*ptog &= ~(0x00000001 << ch_info->ep_no);
		}
	}
}

/**
 * @brief	- Convert code from toggle image to register
 * @param	epno;	endpoint number
 * @param	tog;	image of toggle
 * @retval	converted
 */
static unsigned char cnv_toggle(int epno, unsigned int tog)
{
	unsigned char cnv = 0;

	if (epno < 16) {
		cnv = ((tog & (0x00000001 << epno)) != 0)
			? CHxConfig_Toggle_1 : CHxConfig_Toggle_0;
	}
	return cnv;
}


/**
 * @brief	- Convert code from speed to register
 * @param	num;	speed
 * @retval	converted
 */
static unsigned char cnv_speed(unsigned char num)
{
	unsigned char cnv = 0;

	switch (num) {
	case USB_SPEED_LOW:
		DPRINT(DBG_DATA, "%s:low speed\n", __FUNCTION__);
		cnv = SpeedMode_LS;
		break;
	case USB_SPEED_FULL:
		DPRINT(DBG_DATA, "%s:full speed\n", __FUNCTION__);
		cnv = SpeedMode_FS;
		break;
	case USB_SPEED_HIGH:
		DPRINT(DBG_DATA,"%s:high speed\n", __FUNCTION__);
		cnv = SpeedMode_HS;
		break;
	case USB_SPEED_UNKNOWN:
	default:
		DPRINT(DBG_DATA, "%s:unknown speed\n", __FUNCTION__);
		cnv = SpeedMode_FS;
	}
	return cnv;
}

/**
 * @brief	- Convert code for interval
 * @param	interval;	raw data
 * @param	*urb;	struct of urb
 * @retval	converted
 */
static unsigned short cnv_interval(unsigned short interval,
	struct urb *urb)
{
	unsigned short cnvinterval;

	if (urb->dev->speed == USB_SPEED_HIGH) {
		if (interval < 0x8) {
			cnvinterval = interval;
		} else if (interval < 0x800) {
			cnvinterval = interval & 0x7F8;
		} else {
			cnvinterval = 0x7F8;
		}
	} else {
		/**
		 * - 1. convert msec to frame number:
		 */
		if (interval < 0x100) {
			cnvinterval = interval << 3;
		} else {
			cnvinterval = 0x7F8;
		}
	}

	return cnvinterval;
}

#if BUSMONITOR
/**
 * @brief	- USB bus monitor
 * @param	type;	pipe type
 * @param	dir;	direction
 * @param	ch;	channel number
 * @param	endp;	end point number
 * @param	len;	monitor length
 * @param	*buf;	data buffer
 * @retval	none
 */
static void s1r72xxx_bus_monitor(unsigned type, int dir, int ch, int endp,
	int len, unsigned char *buf)
{
	int i;
	char	cbuf[128], *p, *c;

	switch (type) {
	case PIPE_CONTROL:	DPRINT(DBG_API,"CTL ");		break;
	case PIPE_BULK:		DPRINT(DBG_API,"BLK ");		break;
	case PIPE_INTERRUPT:	DPRINT(DBG_API,"INT ");		break;
	default:		DPRINT(DBG_API,"DUMP ");
	}
	switch (dir) {
	case 0:			DPRINT(DBG_API,"OUT ");		break;
	case 1:			DPRINT(DBG_API,"IN ");		break;
	default:		;
	}
	DPRINT(DBG_API,"%d %d %d \n", ch, endp, len);

#if LIMITMONITOR
	len = (len > LIMITMONITORNO)? LIMITMONITORNO : len;
#endif
	(void)memset(cbuf, ' ', 128);
	cbuf[0] = (char)NULL;
	p = c = cbuf;
 
	for (i = 0; i < len; i++) {
		if ((i & 0xF) == 0) {
			*p = '\t';
			*c = (char)NULL;
			(void)DPRINT(DBG_API,"\t%s\n", cbuf);
			p = cbuf;
			c = cbuf + 49;
		}

		*p++ = "0123456789ABCDEF"[*buf >> 4];
		*p++ = "0123456789ABCDEF"[*buf & 0xF];
		*p++ = ' ';
		if (*buf < 0x20 || *buf > 0x7E) {
			*c++ = '.';
		} else {
			*c++ = *buf;
		}
		buf++;
	}
 
	if ((i & 0xf) != 0) {
		(void)memset(p, ' ', (cbuf + 48) - p);
		*(cbuf + 48) = '\t';
		*c = (char)NULL;
	}	   
	(void)DPRINT(DBG_API,"\t%s\n", cbuf);
	return;
}
#endif

