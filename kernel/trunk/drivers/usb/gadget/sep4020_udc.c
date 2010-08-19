/* linux/drivers/char/sep4020_char/sep4020_udc.c
*
* Copyright (c) 2009 prochip company
*	http://www.prochip.com.cn
*	http://www.prochip.com.cn
*
* sep4020 usb device driver.
*
* Changelog:
*	22-March-2009 ztt&fp	 Initial version
*	26 -May-2009  ztt     fixed	a lot
*
* 
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*/

#include <linux/config.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/delay.h>
#include <linux/ioport.h>
#include <linux/sched.h>
#include <linux/slab.h>
#include <linux/smp_lock.h>
#include <linux/errno.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/list.h>
#include <linux/interrupt.h>
#include <linux/version.h>
#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/dma-mapping.h>
#include <linux/usb.h>
#include <linux/usb_gadget.h>
#include <linux/completion.h>

#include <asm/types.h>
#include <asm/byteorder.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/system.h>
#include <asm/unaligned.h>
#include <asm/arch/irqs.h>
#include <asm/arch/hardware.h>
#include <asm/arch/dma.h>

#include "sep4020_udc.h"

#define DRIVER_VERSION  "28 feb 2009"
#define DRIVER_AUTHOR	"ztt"

#define CHAN3 3
#define RESSIZE(ressource) (((ressource)->end - (ressource)->start)+1)

static const char gadget_name[] = "sep4020_udc";

static struct sep4020_udc *the_controller;

static unsigned char  *vaddr;
static unsigned char  *phyaddr;


/*
    SEP4020 WRITE/READ REG
*/
int usb_write_reg(u32 reg, u32 val)
{
    u32 temp;
    u8  i = 0;

    *(volatile unsigned long*)reg = val;
    temp = *(volatile unsigned long*)reg;

    while((!((temp &= WRITE_SUCCESS) == WRITE_SUCCESS))&&((++i) < 100))
    { 
        temp = *(volatile unsigned long*)reg; 
    }

    return(0);	
}

int usb_read_reg(u32 reg)
{
    u32 temp,t;
    u8 i = 0;

    *(volatile unsigned long*)USBD_READFLAG_V = 0x1;

    temp = *(volatile unsigned long*)reg;

	do{ 
        temp = *(volatile unsigned long*)reg;
        t = temp; 
    }while((!(temp &= READ_SUCCESS)) && ((++i)< 100));

    t &= 0x0fffffff;

    return t;
}

u32 usb_write_read_reg(u32 reg, u32 val)
{
    u32 data;

    usb_write_reg(reg, val);
    data = usb_read_reg(reg);
	
    return data;
}

u8 usb_chang_reg32_to_reg8(u32 reg)
{
    u8 reg8;

    reg8 = (u8)(usb_read_reg(reg));

    return reg8;
}


u16 usb_change_reg32_to_reg16(u32 reg)
{
    u16 reg16;

    reg16 = (u16)(usb_read_reg(reg));

    return reg16;
}


u8 usb_get_regbit15_to_8(u32 reg)
{
    u8 reg8;

    reg8 = (u8)(usb_read_reg(reg)>>8);

    return reg8;
}

int usb_RMW_reg(u32 reg, u32 val)
{
    u32 tmp;
	
    *(volatile unsigned long*)USBD_APPLOCK_V = 0X01; 
    tmp = usb_read_reg(reg);			
    tmp |= val;					
    usb_write_reg(reg,tmp);			
    *(volatile unsigned long*)USBD_APPLOCK_V = 0X00;
	
    return 0;
}

int usb_RMW_clrreg(u32 reg, u32 val)
{
    u32 tmp;
	
    *(volatile unsigned long*)USBD_APPLOCK_V = 0X01; 
    tmp = usb_read_reg(reg);			
    tmp &= val;					
    usb_write_reg(reg,tmp);			
    *(volatile unsigned long*)USBD_APPLOCK_V = 0X00;
	
    return 0;
}

int usb_lock_readreg(u32 reg)
{
    u32 tmp;
	
    *(volatile unsigned long*)USBD_APPLOCK_V = 0X01; 
    tmp = usb_read_reg(reg);			
    *(volatile unsigned long*)USBD_APPLOCK_V = 0X00;
	
    return tmp;
}



/*************************** DEBUG FUNCTION ***************************/
#define DEBUG_NORMAL	1
#define DEBUG_VERBOSE	2

#ifdef CONFIG_USB_SEP4020_DEBUG
#define USB_SEP4020_DEBUG_LEVEL 1
uint32_t        sep4020_ticks = 0;
static int dprintk(int level, const char *fmt, ...)
{
    static char     printk_buf[1024];
    static long     prevticks;
    static int      invocation;
    va_list         args;
    int             len;

    if (level > USB_SEP4020_DEBUG_LEVEL)
        return 0;

    if (sep4020_ticks != prevticks)
    {
        prevticks = sep4020_ticks;
        invocation = 0;
    }

    len = scnprintf(printk_buf, sizeof(printk_buf), "%1lu.%02d USB: ", prevticks, invocation++);

    va_start(args, fmt);
    len = vscnprintf(printk_buf + len, sizeof(printk_buf) - len, fmt, args);
    va_end(args);

    return printk("%s", printk_buf);
}

#else
static int dprintk(int level, const char *fmt, ...)
{
    return 0;
}
#endif

/*------------------------- I/O ----------------------------------*/
/*
 * 	nuke
 */
static void nuke(struct sep4020_udc *udc, struct sep4020_ep *ep)
{
    if (&ep->queue != NULL)     /* Sanity check */
        while (!list_empty(&ep->queue))
        {
            struct sep4020_request *req;
            req = list_entry(ep->queue.next, struct sep4020_request, queue);

            list_del_init(&req->queue);
            req->req.status = -ESHUTDOWN;
            req->req.complete(&ep->ep, &req->req);
        }
}

/*
 * 	done
 */
static void done(struct sep4020_ep *ep, struct sep4020_request *req, int status)
{
    list_del_init(&req->queue);

    if (likely(req->req.status == -EINPROGRESS))
        req->req.status = status;
    else
        status = req->req.status;

    req->req.complete(&ep->ep, &req->req);
}

/* 
 *  clear_ep_state  
 *  hardware SET_{CONFIGURATION,INTERFACE} automagic resets endpoint fifos, 
 *  and pending transactions mustn't be continued in any case.     
*/
static inline void clear_ep_state(struct sep4020_udc *dev)
{
    unsigned  i; 
 
    for (i = 1; i < SEP4020_ENDPOINTS; i++)
        nuke(dev, &dev->ep[i]);
}


/* 
 *  dma_wait_complete
*/
int dma_wait_complete(void)   /* 查询dma完成状态*/
{
    int waitnum=0;
    while(1)
    {
        if(((*(volatile unsigned long*)DMAC_INTTCSTATUS_V) & 0x8 ) == 0x8) 
        {                     
            break;
        }
        udelay(10);

        waitnum++;
        if(waitnum==1000000) 
        {
            printk("still in write DMAC_INTTCSTATUS_V while\n");

            if(((*(volatile unsigned long*)DMAC_INTERRORSTATUS_V ) & 0x8 ) == 0x8) 
                printk("DMAC is transtaning error!!!!!!!!!!\n");

            //waitnum=0;					
            return -1;          //等待超时
        }
    }

    writel(0x8,DMAC_INTTCCLEAR_V);    // 清除DMA完成中断
    writel(0x0,DMAC_INTTCCLEAR_V);

    return 0;
}

u32 transctl(struct sep4020_ep *ep, struct sep4020_request *req)
{
    int        is_last;
    unsigned char *urb;
    int i,tnum, tcount,mod;
	 int wait=0;
    struct sep4020dma_param txdma;

		/*IN 传输*/
        tcount = req->req.length - req->req.actual;
        urb = req->req.buf + req->req.actual;

        if(tcount < 8)
            *(volatile unsigned long*)(USBD_TXLENGTH_V) = tcount;/*包长度不超过64 byte*/
        else
            *(volatile unsigned long*)(USBD_TXLENGTH_V) = 8;

        tnum = tcount/8;
        mod = tcount%8;

        //set the dma param
        txdma.srcadd = (u32)phyaddr;
        txdma.destadd = USBD_TXFIFO;
        txdma.channelnum = CHAN3;
        //control reg
        txdma.destinc = 0;
        txdma.srcinc = 1;
        txdma.destsize = SIZEWORD;
        txdma.srcsize = SIZEWORD;
        txdma.destburst = BURST16;
        txdma.srcburst = BURST16;
        //configuration reg
        txdma.destnum = DMAUSB;
        txdma.srcnum = DMAMEM;
        txdma.controller = DMACONTROL;
        txdma.compintmask = 0; //1:mask 0:unmask
        txdma.errintmask = 1; //1:mask 0:unmask
        txdma.transtype = MENTOPERIPH;
	
        if(tcount < 8)   //trans a packet less than 64 byte
		{
            memcpy(vaddr,urb,tcount);            
            req->req.actual +=tcount;

            txdma.transize = (64)>>2;
            sep4020_dma_set(&txdma);       
		}
        else 
		{
            for(i = 0; i < tnum; i++)          
                memcpy(vaddr+i*64,urb+i*8,0x8);                      

            req->req.actual +=tnum*8;

            txdma.transize = tnum*0x10;
            sep4020_dma_set(&txdma);
		}

            /* 查询dma完成状态*/
        if(dma_wait_complete())
            goto responed_result;

        if(req->req.actual == req->req.length )   //if end or not
        {
            //send an empty packet
            mdelay(1);
            usb_RMW_reg(USBD_EP0OUTSTAT_V,0x2a); //设置端点0状态寄存器bit5，发送一个空包
            ep->dev->ep0state = EP0_IDLE;
            is_last = 1;
        }
        else  
            is_last = 0;

        // dprintk(DEBUG_NORMAL, "Written ep0 .%d of %d b [last %d,z %d]\n", req->req.actual, req->req.length, is_last, req->req.zero);
        if( !is_last )
        {
                /* 查询usb txfifo*/
            while(!(usb_read_reg(USBD_RECEIVETYPE_V)&0x2))
            {
                udelay(10);                             
                wait++;
                if(wait==1000000) 
                {
                    printk("wait for USBD_RECEIVETYPE_V\n");
                    wait=0;
                    goto responed_result; 
                }
            }

            tcount = req->req.length - req->req.actual;
            *(volatile unsigned long*)(USBD_TXLENGTH_V) = tcount; 

            urb = req->req.buf + req->req.actual;
           
            memcpy(vaddr,urb,tcount);  
            txdma.transize = (64)>>2;
            sep4020_dma_set(&txdma);          
		
                /* 查询dma完成状态*/
            if(dma_wait_complete())
                goto responed_result;

            req->req.actual += tcount ;
            if(req->req.actual == req->req.length )
            {
                ep->dev->ep0state = EP0_IDLE;
                is_last = 1;
            }
            else  
            {
                is_last = 0;
            }
		
        }
      
        if( is_last ) 	/*if end ,submit to uplayer*/
            done(ep, req, 0); 	
        else       		
            printk("is_last is still not 1\n");          		  	

    return is_last;

responed_result:
    return -1;
}

/* 
 *  handle bulk transfer
*/
u32 transbulk(struct sep4020_ep *ep, struct sep4020_request *req)
{
    int             is_last;
    unsigned        len;
    struct sep4020_udc *dev;
    unsigned char *urb;
    int tnum, tcount,t_remain;
	 u32 wait=0;
    struct sep4020dma_param txdma;
    struct sep4020dma_param rxdma; 
//printk("transbulk\n");	
	dev = ep->dev;
	spin_lock_irq(&dev->lock);
    if((ep->bEndpointAddress & USB_DIR_IN) != 0)    //bulkin write data to fifo
    {
		/*IN 传输*/
        tcount = req->req.length - req->req.actual;
        urb = req->req.buf + req->req.actual;

        if(tcount < 64)
            *(volatile unsigned long*)(USBD_TXLENGTH_V) = tcount;/*包长度不超过64 byte*/
        else
            *(volatile unsigned long*)(USBD_TXLENGTH_V) = 64;

        tnum = tcount/64;

        //set the dma param
        txdma.srcadd = (u32)phyaddr;
        txdma.destadd = USBD_TXFIFO;
        txdma.channelnum = CHAN3;
        //control reg
        txdma.destinc = 0;
        txdma.srcinc = 1;
        txdma.destsize = SIZEWORD;
        txdma.srcsize = SIZEWORD;
        txdma.destburst = BURST16;
        txdma.srcburst = BURST16;
        //configuration reg
        txdma.destnum = DMAUSB;
        txdma.srcnum = DMAMEM;
        txdma.controller = DMACONTROL;
        txdma.compintmask = 0; //1:mask 0:unmask
        txdma.errintmask = 1; //1:mask 0:unmask
        txdma.transtype = MENTOPERIPH;
	
        if(tcount < 64)   //trans a packet less than 64 byte
		{
            memcpy(vaddr,urb,tcount);            
            req->req.actual +=tcount;

            txdma.transize = (64)>>2;
            sep4020_dma_set(&txdma);       
		}
        else 
		{
            memcpy(vaddr,urb,tnum*0x40);               
            req->req.actual +=tnum*0x40;

            txdma.transize = tnum*0x10;
            sep4020_dma_set(&txdma);
		}

            /* 查询dma完成状态*/
        if(dma_wait_complete())
            goto responed_result;

        if(req->req.actual == req->req.length )   //if end or not
            is_last = 1;
        else  
            is_last = 0;

        //dprintk(DEBUG_NORMAL, "Written ep%d .%d of %d b [last %d,z %d]\n", 1,  req->req.actual, req->req.length, is_last, req->req.zero);
        if( !is_last )
        {
            wait=0;
                /* 查询usb txfifo*/
            while(!(usb_read_reg(USBD_RECEIVETYPE_V)&0x2))
            {
                udelay(10);                             
                wait++;
                if(wait==1000000) 
                {
                    printk("wait for USBD_RECEIVETYPE_V\n");
                    wait=0;
                    goto responed_result; 
                }
            }

            tcount = req->req.length - req->req.actual;
            *(volatile unsigned long*)(USBD_TXLENGTH_V) = tcount; 

            urb = req->req.buf + req->req.actual;
           
            memcpy(vaddr,urb,tcount);  
            txdma.transize = (64)>>2;
            sep4020_dma_set(&txdma);          
		
                /* 查询dma完成状态*/
            if(dma_wait_complete())
                goto responed_result;

            req->req.actual += tcount ;
            if(req->req.actual == req->req.length )
                is_last = 1;
            else  
            {
                is_last = 0;
                printk("the remain data num no 64,tcount is 0x%x\n",tcount);
            }
		
        }
      
        if( is_last ) 	/*if end ,submit to uplayer*/
            done(ep, req, 0); 	
        else       		
            printk("is_last is still not 1\n");          		  	
    }

    else   //OUT传输   bulkout  receive data from the fifo
    {	
        tcount = req->req.length - req->req.actual;      

        urb = req->req.buf + req->req.actual;

        tnum=tcount/64;    
        t_remain = tcount%64; //the last short packet is t_remain
        if(t_remain != 0 ) 
            tnum += 1;  

        //set dma param
        rxdma.destadd = (u32)phyaddr;
        rxdma.srcadd = USBD_RXFIFO;
        rxdma.channelnum = CHAN3;
        //control reg
        rxdma.destinc = 1;
        rxdma.srcinc = 0;
        rxdma.destsize = SIZEWORD;
        rxdma.srcsize = SIZEWORD;
        rxdma.destburst = BURST16;
        rxdma.srcburst = BURST16;
        //configuration reg
        rxdma.srcnum = DMAUSB;
        rxdma.destnum = DMAMEM;
        rxdma.controller = DMACONTROL;
        rxdma.compintmask = 0; //1:mask 0:unmask
        rxdma.errintmask = 1; //1:mask 0:unmask
        rxdma.transtype = PERIPHTOMEM;

        if(tcount <= 64)
        {
            len = usb_read_reg(USBD_RXLENGTH_V);

            rxdma.transize = (64)>>2; //read 64byte
            sep4020_dma_set(&rxdma);

                /* 查询dma完成状态*/
            if(dma_wait_complete())
                goto responed_result;           

            req->req.actual += len;
            memcpy(urb,vaddr,len);
            is_last = 1;
        }
        else
		{
            len = tcount;             

	         rxdma.transize = tnum*0x10; //the total is tnum*64byte
            sep4020_dma_set(&rxdma);	

                /* 查询dma完成状态*/
            if(dma_wait_complete())
                goto responed_result;  

            req->req.actual += len;
            memcpy(urb,vaddr,len);

            if(req->req.actual <  req->req.length)
                is_last = 0;
            else 
                is_last = 1;
        }
		
        if(is_last ==1 )
        {		
           // usb_write_reg(USBD_INTRMASK_V,usb_read_reg(USBD_INTRMASK_V)|0x4); //mask out int  FP
            done(ep, req, 0);
        }
        else
        {
            printk("read fifo is not completed\n");
				//usb_write_reg(USBD_INTRMASK_V,usb_read_reg(USBD_INTRMASK_V)&(~0x4)); //unmask out int
        }   		
		
    }
	spin_unlock_irq(&dev->lock);
    return is_last;

responed_result:
	spin_unlock_irq(&dev->lock);
    return -1;
}


static inline int read_packet(int fifo, u8 * buf, struct sep4020_request *req, unsigned avail)
{
    unsigned   len;    
   
    //set dma param
    struct sep4020dma_param rxdma = 
    {
        .destadd = (u32)phyaddr,
        .srcadd = USBD_RXFIFO,
        .channelnum = CHAN3,
        //control reg
        .transize = (64)>>2,
        .destinc = 1,
        .srcinc = 0,
        .destsize = SIZEWORD,
        .srcsize = SIZEWORD,
        .destburst = BURST16,
        .srcburst = BURST16,
        //configuration reg
        .srcnum = DMAUSB,
        .destnum = DMAMEM,
        .controller = DMACONTROL,
        .compintmask = 0, //1:mask 0:unmask
        .errintmask = 1, //1:mask 0:unmask
        .transtype = PERIPHTOMEM,
    };

    len = min(req->req.length - req->req.actual, avail);

    memset(vaddr,0,64);

    sep4020_dma_set(&rxdma);  //receive the data from rxfifo

    if(dma_wait_complete())   // 查询dma完成状态
        goto responed_result;	

    req->req.actual += len;

    memcpy(buf,vaddr,len);

    return len;

responed_result:
     return -1;
}

// return:  0 = still running, 1 = queue empty, negative = errno
static int read_fifo(struct sep4020_ep *ep, struct sep4020_request *req)
{
    u8             *buf;
    unsigned        bufferspace;
    int             is_last = 1;
    unsigned        avail;
    int             fifo_count = 0;
    u32             idx;
    int             fifo_reg;     

    idx = ep->bEndpointAddress & 0x7F;
    fifo_reg = USBD_RXFIFO;	//sep4020----------shared fifo

    buf = req->req.buf + req->req.actual;
    bufferspace = req->req.length - req->req.actual;
    if (!bufferspace)
    {
        dprintk(DEBUG_NORMAL, "read_fifo: Buffer full !!\n");
        return -1;
    }

    fifo_count = usb_read_reg(USBD_RXLENGTH_V);

    if (fifo_count > ep->ep.maxpacket)
       avail = ep->ep.maxpacket;
    else
       avail = fifo_count;
   
    fifo_count = read_packet(fifo_reg, buf, req, avail);

    if (fifo_count < ep->ep.maxpacket)
    {
        is_last = 1;      
        // overflowed this request? flush extra data     
    }
    else 
    {
        if(req->req.length == req->req.actual) 
            is_last = 1;
        else
            is_last = 0;
    }

    dprintk(DEBUG_VERBOSE, "read ep%d %d.%d of %d b [last %d]\n", idx, fifo_count, req->req.actual, req->req.length, is_last);
    

    if (is_last) //if it is the last transfer, submit to uplayer
    {
        ep->dev->ep0state = EP0_IDLE; //set the ep0 state IDLE

        done(ep, req, 0); //submit to uplayer

        if (!list_empty(&ep->queue))
        {
            is_last = 0;
            req = container_of(ep->queue.next, struct sep4020_request, queue);
        }
        else
            is_last = 1;
    }
    
    return is_last;

}

/*------------------------- usb state machine -------------------------------*/
static void handle_ep0(struct sep4020_udc *dev)
{
    u32    ep0csr;
    u32    transtat;
    struct sep4020_ep *ep = &dev->ep[0];
    struct sep4020_request *req;
    struct usb_ctrlrequest crlq;
    int    ret;
    int    tmp;

    if (list_empty(&ep->queue))
        req = 0;
    else
        req = list_entry(ep->queue.next, struct sep4020_request, queue);
         
         
    transtat = usb_read_reg(USBD_RECEIVETYPE_V); 
    ep0csr = usb_lock_readreg(USBD_EP0OUTSTAT_V);
   
    dprintk(DEBUG_NORMAL, "EP0OUTSTAT %x,transtat is 0x%08x\n", ep0csr,transtat);
	
    /* clear stall status */
    if (ep0csr & SEP4020_UDC_EP_PROSTALL)
    {
        nuke(dev, ep);
        dprintk(DEBUG_NORMAL, "... clear SENT_STALL ...\n");

        usb_RMW_clrreg(USBD_EP0OUTSTAT_V, 0X7FFFFFFb);   
        dev->ep0state = EP0_IDLE;

        return;
    }

    if (!(ep0csr & SEP4020_UDC_EP_CTLCOMP))
    {
        //dprintk(DEBUG_NORMAL, "... begain or SETUP_END ...\n");
        nuke(dev, ep);	  
        dev->ep0state = EP0_IDLE;
    }
    


    switch (dev->ep0state)
    {          
        case EP0_IDLE:
            /* start control request????????????? */   
            nuke(dev, ep);	  

            /*get the data of setup state*/
            crlq.bRequestType = usb_chang_reg32_to_reg8(USBD_EP0OUTBMREQTYPE_V);
            crlq.bRequest = usb_chang_reg32_to_reg8(USBD_EP0OUTBREQUEST_V);
            crlq.wValue = usb_change_reg32_to_reg16(USBD_EP0OUTWVALUE_V);
            crlq.wIndex = usb_change_reg32_to_reg16(USBD_EP0OUTWINDEX_V);
            crlq.wLength = usb_change_reg32_to_reg16(USBD_EP0OUTWLENGTH_V);
			
            dprintk(DEBUG_NORMAL, "bRequestType = %d bRequest = %d wLength = %d\n", crlq.bRequestType, crlq.bRequest, crlq.wLength);
          
            /* cope with automagic for some standard requests. */          
            dev->req_std = (crlq.bRequestType & USB_TYPE_MASK) == USB_TYPE_STANDARD;
            dev->req_config = 0;
            dev->req_pending = 1;

            usb_RMW_reg(USBD_EP0OUTSTAT_V, 0xa);//set the control complete bit 1
		
            switch (crlq.bRequest)
            {		
                 /* hardware restricts gadget drivers here! */
                case USB_REQ_SET_CONFIGURATION:
                    dprintk(DEBUG_NORMAL, "USB_REQ_SET_CONFIGURATION ... \n");
 
                    if (crlq.bRequestType == USB_RECIP_DEVICE)
                        {                
                        dev->req_config = 1;
                        clear_ep_state(dev);		
                        usb_write_reg(USBD_BCONFIGURATIONVALUE_V, crlq.wValue);					              
                        }

                    break;
                
                case USB_REQ_SET_INTERFACE:
                    dprintk(DEBUG_NORMAL, "USB_REQ_SET_INTERFACE ... \n");

                    if (crlq.bRequestType == USB_RECIP_INTERFACE)
                        {
                        dev->req_config = 1;
                        clear_ep_state(dev);	
                        }

                    break;

                /* hardware was supposed to hide this */
                case USB_REQ_SET_ADDRESS:
                    dprintk(DEBUG_NORMAL, "USB_REQ_SET_ADDRESS ... \n");

                    if (crlq.bRequestType == USB_RECIP_DEVICE)
                        {
                        tmp = crlq.wValue & 0x7F;
                        dev->address = tmp;             
                        return;
                        }

                    break;

                default:          
                    break;
            }

            /*set the next state*/
            if (crlq.bRequestType & USB_DIR_IN)
                dev->ep0state = EP0_IN_DATA_PHASE;
            else
                dev->ep0state = EP0_OUT_DATA_PHASE;
		
            ret = dev->driver->setup(&dev->gadget, &crlq);//submit to uplayer here		  
            if (ret < 0) //handle the error
            {
                if (dev->req_config)
                {
                    dprintk(DEBUG_NORMAL, "config change %02x fail %d?\n", crlq.bRequest, ret);
                    return;
                }

                if (ret == -EOPNOTSUPP)
                    dprintk(DEBUG_NORMAL, "Operation not supported\n");
                else
                    dprintk(DEBUG_NORMAL, "dev->driver->setup failed. (%d)\n", ret);

                //set the ep0 stall and set the state IDLE
                usb_RMW_reg(USBD_EP0OUTSTAT_V, SEP4020_UDC_EP_PROSTALL|usb_lock_readreg(USBD_EP0OUTSTAT_V));
                dev->ep0state = EP0_IDLE;               
            }
            else if (dev->req_pending)
            {
                dprintk(DEBUG_VERBOSE, "dev->req_pending... what now?\n");
                dev->req_pending = 0;
            }
            dprintk(DEBUG_NORMAL, "ep0state %s\n", ep0states[dev->ep0state]);
              
            break;
		 
        case EP0_IN_DATA_PHASE:    /* GET_DESCRIPTOR etc */
            dprintk(DEBUG_NORMAL, "EP0_IN_DATA_PHASE ... what now?\n");
		
            if ((transtat & 0x2) && req) //if tx-fifo is ready, write data to the fifo
            {          
                transctl(ep, req);
            }
            break;

        case EP0_OUT_DATA_PHASE:   /* SET_DESCRIPTOR etc */
            dprintk(DEBUG_NORMAL, "EP0_OUT_DATA_PHASE ... what now?\n");
      
            if ((transtat & 0x4) && req) //if rx-fifo have data to read out, read it
            {        
		   	    read_fifo(ep, req);
            }
            break;

        case EP0_END_XFER:
            dprintk(DEBUG_NORMAL, "EP0_END_XFER ... what now?\n");
            dev->ep0state = EP0_IDLE;
            break;

        case EP0_STALL:
            dev->ep0state = EP0_IDLE;
            break;
    }
}

/*
 * 	handle_ep - Manage I/O endpoints
 */
static void handle_ep(struct sep4020_ep *ep)
{
    struct sep4020_request *req;
    int             is_in = ep->bEndpointAddress & USB_DIR_IN;
    u32             transtat,epstat;
    u32             idx;

    if (likely(!list_empty(&ep->queue)))
        req = list_entry(ep->queue.next, struct sep4020_request, queue);
    else
        req = 0;

    idx = (u32) (ep->bEndpointAddress & 0x7F);
 
    transtat = usb_read_reg(USBD_RECEIVETYPE_V);
	
    switch(idx)
    { 
        case 1:          	 	
            if (is_in) //bulk in
            {         	                              
                if ((transtat & 0x2) && req)
                {
                    transbulk(ep, req);
                }                
            }
            else  //bulk out
            {
                epstat =usb_lock_readreg(USBD_EP1OUTSTAT_V); 
         
                if ((transtat & 0x4) && req)
                {
                    transbulk(ep, req);
                }
            }             	
            break;

        case 2:  
         	 	
            if (is_in)//bulk in
            {         	                   
                epstat =usb_lock_readreg(USBD_EP2INSTAT_V); 
    
                if ((transtat & 0x2) && req)
                {
                    transbulk(ep, req);
                }	
            }
            else
            {
                epstat =usb_lock_readreg(USBD_EP2OUTSTAT_V); 
                      
                if ((transtat & 0x4) && req)
                {
                    transbulk(ep, req);			
                }
            }             	
            break;
  
        default: 
            break;
    }
 
}

/*
 *      sep4020_udc_irq - interrupt handler
 */
static irqreturn_t sep4020_udc_irq(int irq, void *_dev, struct pt_regs *r)
{
    struct sep4020_udc *dev = _dev;
    int epstat;
    u32 evnt,intr,imask,epnumb;
    unsigned long   flags;
	
       /* Driver connected ? */
    if (!dev->driver)
    {	   
        usb_RMW_reg(USBD_PROTOCOLINTR_V, 0); // Clear interrupts 
        printk("the driver is not exist!\n");
    }

    spin_lock_irqsave(&dev->lock, flags);
			  
    intr  =  usb_lock_readreg(USBD_PROTOCOLINTR_V);
    imask =  usb_read_reg(USBD_INTRMASK_V);
    epstat = usb_lock_readreg(USBD_EPINFO_V);
    epnumb = (epstat&0x1e)>>1;


    if(((epstat &= 0x1f)== 0) || ((epstat &= 0x1f)== 1))
    {    
        dprintk(DEBUG_VERBOSE, "USB ep0 irq\n");
						
        evnt = intr & (INTSETUP|INTIN|INTOUT|INTPING|INTTRAERR|INTACK|INTSTCOV|INTCTRAERR|INTRESET) & (~imask);		
				
        if(evnt & INTRESET)
        {
            printk("Go into INTRESET!\n");	

            usb_write_reg((u32)USBD_EP0OUTSTAT_V, 0x02);		   
            usb_write_reg((u32)USBD_EP1INSTAT_V, 0x02);			
            usb_write_reg((u32)USBD_EP2OUTSTAT_V, 0x02);
            usb_RMW_clrreg((u32)USBD_PROTOCOLINTR_V, 0x0); 
            usb_write_reg((u32)USBD_INTRMASK_V, 0x172);
            usb_RMW_clrreg(USBD_PROTOCOLINTR_V,0x7ffffdff);					   
        }	
					
        if(evnt & INTOUT)
        {
            //printk("Go into INTOUT!,USBD_PROTOCOLINTR_V is 0x%08x\n",usb_read_reg(USBD_PROTOCOLINTR_V));					
            usb_RMW_clrreg(USBD_PROTOCOLINTR_V,0x7ffffffb);
						
            *(volatile unsigned long*)USBD_RECEIVETYPE_V &= 0x07;																	
        }

        if(evnt & INTIN) //IN is mask
        {						 
            usb_RMW_clrreg(USBD_PROTOCOLINTR_V,0x7ffffffd);
            handle_ep0(dev);		
        }
					
        if( evnt & INTSETUP)
        {
            //printk("Go into INTSETUP!\n");				
            usb_RMW_clrreg(USBD_PROTOCOLINTR_V,0x7ffffffe);
            handle_ep0(dev);
        }	      
        
        if(evnt & INTCTRAERR)
        {
            //	printk("Go into INTCTRAERR!\n");
            usb_RMW_clrreg(USBD_PROTOCOLINTR_V,0x7fffff7f);
        }
				
    }
    else
    {		
        evnt = intr & (INTSETUP|INTIN|INTOUT|INTPING|INTTRAERR|INTACK|INTSTCOV|INTCTRAERR|INTRESET) & (~imask);

        if(evnt & INTOUT)
        {
            //printk("go into bulk out\n");
            usb_RMW_clrreg(USBD_PROTOCOLINTR_V,0x7ffffffb);			 					
            handle_ep(&dev->ep[epnumb]);										 
        }

        if(evnt & INTIN)
        {
            //printk("go into bulkin\n");			 			 
            usb_RMW_clrreg(USBD_PROTOCOLINTR_V,0x7ffffffd);
 				handle_ep(&dev->ep[epnumb]);
        }
		 
        if(evnt & INTRESET)
        {
            usb_RMW_clrreg(USBD_PROTOCOLINTR_V,0x7ffffdff);   
        }	
       
    }

    spin_unlock_irqrestore(&dev->lock, flags);
	       
    return IRQ_HANDLED;
}

/*------------------------- sep4020_ep_ops ----------------------------------*/

/*
 * 	sep4020_ep_enable
 */
static int sep4020_ep_enable(struct usb_ep *_ep, const struct usb_endpoint_descriptor *desc)
{
    struct sep4020_udc *dev;
    struct sep4020_ep  *ep;
    u32             max;
    u32             tmp;
    unsigned long   flags;

	//printk("4020endpoint enable\n");


    ep = container_of(_ep, struct sep4020_ep, ep);

    if (!_ep || !desc || ep->desc || _ep->name == ep0name || desc->bDescriptorType != USB_DT_ENDPOINT)
        return -EINVAL;

    dev = ep->dev;
    if (!dev->driver || dev->gadget.speed == USB_SPEED_UNKNOWN)
        return -ESHUTDOWN;

    max = le16_to_cpu(desc->wMaxPacketSize) & 0x1fff;

    spin_lock_irqsave(&dev->lock, flags);

    _ep->maxpacket = max & 0x7ff;
    ep->desc = desc;
    ep->bEndpointAddress = desc->bEndpointAddress;

    if(ep->bEndpointAddress& USB_DIR_IN)
    {
        switch(ep->num)
        {	
            case 1:
                usb_write_reg((u32)USBD_EP1INMAXPKTSIZE_V, max);
                usb_write_reg((u32)USBD_EP1INBMATTR_V, 0x02);
                usb_RMW_reg((u32)USBD_EP1INSTAT_V, 0x02);
                usb_write_reg((u32)USBD_EP1INADDR_V, 0x81);
                break;
            case 2:
                usb_write_reg((u32)USBD_EP2INMAXPKTSIZE_V, max);
                usb_write_reg((u32)USBD_EP2INBMATTR_V, 0x02);
                usb_RMW_reg((u32)USBD_EP2INSTAT_V, 0x02);
                usb_write_reg((u32)USBD_EP2INADDR_V, 0x82);
                break;
            default:
                break;
        }	
    }
    else
    {
        switch(ep->num)
        {	
            case 1:
                usb_write_reg((u32)USBD_EP1OUTMAXPKTSIZE_V, max);
                usb_write_reg((u32)USBD_EP1OUTBMATTR_V, 0x02);
                usb_RMW_reg((u32)USBD_EP1OUTSTAT_V, 0x02);
                usb_write_reg((u32)USBD_EP1OUTADDR_V, 0x01);
                break;
            case 2:
                usb_write_reg((u32)USBD_EP2OUTMAXPKTSIZE_V, max);
                usb_write_reg((u32)USBD_EP2OUTBMATTR_V, 0x02);
                usb_RMW_reg((u32)USBD_EP2OUTSTAT_V, 0x02);
                usb_write_reg((u32)USBD_EP2OUTADDR_V, 0x02);
                break;
            default:
                break;
        }	
    }

    usb_write_reg((u32)USBD_INTRMASK_V, 0x17a); 

    /*
    * print some debug message 
     */

    tmp = desc->bEndpointAddress;
    dprintk(DEBUG_NORMAL, "enable %s(%d) ep%x%s-blk max %02x\n", _ep->name, ep->num, tmp, desc->bEndpointAddress & USB_DIR_IN ? "in" : "out", max);

    spin_unlock_irqrestore(&dev->lock, flags);

    return 0;
}

/*
 * sep4020_ep_disable
 */
static int sep4020_ep_disable(struct usb_ep *_ep)
{
	
    struct sep4020_ep *ep = container_of(_ep, struct sep4020_ep, ep);
    unsigned long   flags;

    dprintk(DEBUG_NORMAL, "%s not enabled\n", _ep ? ep->ep.name : NULL);
    if (!_ep || !ep->desc)
    {
        dprintk(DEBUG_NORMAL, "%s not enabled\n", _ep ? ep->ep.name : NULL);
        return -EINVAL;
    }

    spin_lock_irqsave(&ep->dev->lock, flags);
    ep->desc = 0;

    nuke(ep->dev, ep);
  
	spin_unlock_irqrestore(&ep->dev->lock, flags);
        
    return 0;
}

/*
 * sep4020_alloc_request
 */
static struct usb_request *sep4020_alloc_request(struct usb_ep *_ep, unsigned int mem_flags)
{
    struct sep4020_ep *ep;
    struct sep4020_request *req;

    //dprintk(DEBUG_NORMAL, "sep4020_alloc_request(ep=%p,flags=%d)\n", _ep, mem_flags);

    ep = container_of(_ep, struct sep4020_ep, ep);

    if (!_ep)
        return 0;

    req = kmalloc(sizeof *req, mem_flags);
    if (!req)
        return 0;
    memset(req, 0, sizeof *req);
    INIT_LIST_HEAD(&req->queue);
    return &req->req;

}

/*
 * sep4020_free_request
 */
static void sep4020_free_request(struct usb_ep *_ep, struct usb_request *_req)
{
    struct sep4020_ep *ep;
    struct sep4020_request *req;

    

    ep = container_of(_ep, struct sep4020_ep, ep);

    if (!ep || !_req || (!ep->desc && _ep->name != ep0name))
        return;
        
	//dprintk(DEBUG_NORMAL, "sep4020_free_request(ep=%p,req=%p)\n", _ep, _req);
    req = container_of(_req, struct sep4020_request, req);
    WARN_ON(!list_empty(&req->queue));
    kfree(req);
    
}

/*
 * 	sep4020_alloc_buffer
 */
static void    *sep4020_alloc_buffer(struct usb_ep *_ep, unsigned bytes, dma_addr_t * dma, unsigned int mem_flags)
{
    char           *retval;

    //dprintk(DEBUG_NORMAL, "sep4020_alloc_buffer()\n");

    if (!the_controller->driver)
        return 0;
    retval = kmalloc(bytes, mem_flags);
    *dma = (dma_addr_t) retval;
    return retval;
}

/*
 * sep4020_free_buffer
 */
static void sep4020_free_buffer(struct usb_ep *_ep, void *buf, dma_addr_t dma, unsigned bytes)
{
   // dprintk(DEBUG_NORMAL, "sep4020_free_buffer()\n");

   if (bytes)
        kfree(buf);
}

/*
 * 	sep4020
 */
static int sep4020_queue(struct usb_ep *_ep, struct usb_request *_req, unsigned int gfp_flags)
{
    struct sep4020_request *req;
    struct sep4020_ep *ep;
    struct sep4020_udc *dev;
    u32             tt=0;
    unsigned long   flags,tempval;

    //dprintk(DEBUG_NORMAL, "go into sep4020_queue\n");

    ep = container_of(_ep, struct sep4020_ep, ep);

    if (unlikely(!_ep || (!ep->desc && ep->ep.name != ep0name)))
    {
        dprintk(DEBUG_NORMAL, "sep4020_queue: inval 2\n");
        return -EINVAL;
    }

    dev = ep->dev;
    if (unlikely(!dev->driver || dev->gadget.speed == USB_SPEED_UNKNOWN))
    {
        return -ESHUTDOWN;
    }

    //spin_lock_irqsave(&dev->lock, flags);

    req = container_of(_req, struct sep4020_request, req);
    if (unlikely(!_req || !_req->complete || !_req->buf || !list_empty(&req->queue)))
    {
        if (!_req)
            dprintk(DEBUG_NORMAL, "sep4020_queue: 1 X X X\n");
        else
            dprintk(DEBUG_NORMAL, "sep4020_queue: 0 %01d %01d %01d\n", !_req->complete, !_req->buf, !list_empty(&req->queue));

        //spin_unlock_irqrestore(&dev->lock, flags);
        return -EINVAL;
    }

    _req->status = -EINPROGRESS;
    _req->actual = 0;
    
    /*
     * kickstart this i/o queue? 
     */
    if (list_empty(&ep->queue))
    {
        if (ep->bEndpointAddress == 0 /* ep0 */ )
        {
            dprintk(DEBUG_NORMAL, "going into ep0 equeue!,ep0state 0x%x\n",dev->ep0state);
            switch (dev->ep0state)
            {
                case EP0_IN_DATA_PHASE:
                    if (transctl(ep, req))
                        req = 0;
                    break;

                case EP0_OUT_DATA_PHASE:
                    dev->ep0state = EP0_IDLE;
                    break;

                default:
                printk("goto ep0 queue default\n");
                //    spin_unlock_irqrestore(&dev->lock, flags);
                    return -EL2HLT;
            }
        }
   
        else if ((ep->bEndpointAddress & USB_DIR_IN) != 0) //bulk-in
        {
            if(usb_read_reg(USBD_RECEIVETYPE_V)&0x2)  
            {
                if(transbulk(ep, req))
                {
                    req=0;
                }
            }             

        }
    
		else if ((ep->bEndpointAddress & USB_DIR_IN) == 0) //bulk-out
        {				 
				tempval = *(volatile unsigned long*)USBD_RECEIVETYPE_V & 0x4;
				
				while(!tempval)
            {
                tempval = *(volatile unsigned long*)USBD_RECEIVETYPE_V & 0x4;
                udelay(10);
                         
                tt++;
                if(tt==100000) 
                {
					printk("ENTER ep2 wait for USBD_RECEIVETYPE_V\n");
					tt=0;
					break;
				}
            }  

            if((*(volatile unsigned long*)USBD_RECEIVETYPE_V & 0x4) && transbulk(ep, req)) //judge again to avoid break from the wait
                req = 0;
        }

    }

    if (likely(req != 0))
        list_add_tail(&req->queue, &ep->queue);

    //spin_unlock_irqrestore(&dev->lock, flags);
    dprintk(DEBUG_VERBOSE, "sep4020_queue normal end\n");

    return 0;
}

/*
 * 	sep4020_dequeue
 */
static int sep4020_dequeue(struct usb_ep *_ep, struct usb_request *_req)
{
    struct sep4020_ep *ep;
    struct sep4020_udc *udc;
    int             retval = -EINVAL;
    unsigned long   flags;
    struct sep4020_request *req = 0;

    dprintk(DEBUG_VERBOSE, "sep4020_dequeue(ep=%p,req=%p)\n", _ep, _req);

    if (!the_controller->driver)
        return -ESHUTDOWN;

    if (!_ep || !_req)
        return retval;
    ep = container_of(_ep, struct sep4020_ep, ep);
    udc = container_of(ep->gadget, struct sep4020_udc, gadget);

    spin_lock_irqsave(&udc->lock, flags);
    list_for_each_entry(req, &ep->queue, queue)
    {
        if (&req->req == _req)
        {
            list_del_init(&req->queue);
            _req->status = -ECONNRESET;
            retval = 0;
            break;
        }
    }
    spin_unlock_irqrestore(&udc->lock, flags);

    if (retval == 0)
    {
        dprintk(DEBUG_VERBOSE, "dequeued req %p from %s, len %d buf %p\n", req, _ep->name, _req->length, _req->buf);

        _req->complete(_ep, _req);
        done(ep, req, -ECONNRESET);
    }
    return retval;
}

/*
 * sep4020_set_halt
 */
static int sep4020_set_halt(struct usb_ep *_ep, int value)
{
    return 0;
}


static const struct usb_ep_ops sep4020_ep_ops = {
    .enable = sep4020_ep_enable,
    .disable = sep4020_ep_disable,

    .alloc_request = sep4020_alloc_request,
    .free_request = sep4020_free_request,

    .alloc_buffer = sep4020_alloc_buffer,
    .free_buffer = sep4020_free_buffer,

    .queue = sep4020_queue,
    .dequeue = sep4020_dequeue,

    .set_halt = sep4020_set_halt,
};

/*------------------------- usb_gadget_ops ----------------------------------*/

/*
 * 	sep4020_g_get_frame
 */
static int sep4020_g_get_frame(struct usb_gadget *_gadget)
{
   int             tmp;

    dprintk(DEBUG_VERBOSE, "sep4020_g_get_frame()\n");

    tmp = usb_read_reg(USBD_FRAMENUMBER_V);
    
    return tmp & 0xfff;
    
}

/*
 * 	sep4020_wakeup
 */
static int sep4020_wakeup(struct usb_gadget *_gadget)
{
    dprintk(DEBUG_NORMAL, "sep4020_wakeup()\n");

    return 0;
}

/*
 * 	sep4020_set_selfpowered
 */
static int sep4020_set_selfpowered(struct usb_gadget *_gadget, int value)
{
    struct sep4020_udc *udc;

    //dprintk(DEBUG_NORMAL, "sep4020_set_selfpowered()\n");

    udc = container_of(_gadget, struct sep4020_udc, gadget);

    if (value)
        udc->devstatus |= (1 << USB_DEVICE_SELF_POWERED);
    else
        udc->devstatus &= ~(1 << USB_DEVICE_SELF_POWERED);

    return 0;
}



static const struct usb_gadget_ops sep4020_ops = 
{
    .get_frame = sep4020_g_get_frame,
    .wakeup = sep4020_wakeup,
    .set_selfpowered = sep4020_set_selfpowered,
};


/*------------------------- gadget driver handling---------------------------*/
/*
 * udc_disable
 */
static void udc_disable(struct sep4020_udc *dev)
{
    dprintk(DEBUG_NORMAL, "udc_disable called\n");

    /* Disable all interrupts */
    INT_DISABLE(INTSRC_USB);
   
    /* Clear the interrupt registers */
    usb_write_reg((u32)USBD_INTRMASK_V, 0xFFFFFFFF);
    usb_write_reg((u32)USBD_PROTOCOLINTR_V,0);

    /* Set speed to unknown */
    dev->gadget.speed = USB_SPEED_UNKNOWN;
}

/*
 * udc_reinit
 */
static void udc_reinit(struct sep4020_udc *dev)
{
    u32  i;

    /* device/ep0 records init */
    INIT_LIST_HEAD(&dev->gadget.ep_list);
    INIT_LIST_HEAD(&dev->gadget.ep0->ep_list);

    dev->ep0state = EP0_IDLE;//set the initial stat IDLE

    for (i = 0; i < SEP4020_ENDPOINTS; i++)
    {
        struct sep4020_ep *ep = &dev->ep[i];

        if (i != 0)
            list_add_tail(&ep->ep.ep_list, &dev->gadget.ep_list);

        ep->dev = dev;
        ep->desc = 0;
        INIT_LIST_HEAD(&ep->queue);
    }
}

/*
 * udc_enable
 */
static void udc_enable(struct sep4020_udc *dev)
{
    dprintk(DEBUG_NORMAL, "udc_enable called\n");

    dev->gadget.speed = USB_SPEED_FULL;

    /*set the hardware reg
    here we just config ep0, we'll config ep1 and ep2 later in sep4020_ep_enable*/

    usb_write_reg((u32)USBD_EP0OUTMAXPKTSIZE_V, 0x8);   //ep0 maxpacket 8 byte
    usb_write_reg((u32)USBD_BMATTRIBUTES_V, 0xe0);     //remote awake,self-powered//
    usb_RMW_reg((u32)USBD_EP0OUTSTAT_V, 0x02);         //active ep//   

    usb_write_reg((u32)USBD_RECEIVETYPE_V, 0x01);     //dma write/read fifo

    usb_write_reg((u32)USBD_PROTOCOLINTR_V,0);        //clear int reg//
    usb_write_reg((u32)USBD_INTRMASK_V, 0x172);     // SETUP/OUT/PING/RESET/CTRL_ERR UNMASK
    
    /*enable USB IRQ*/
    INT_ENABLE(INTSRC_USB);
}

/*
 * 	nop_release
 */
static void nop_release(struct device *dev)
{
    dprintk(DEBUG_NORMAL, "%s %s\n", __FUNCTION__, dev->bus_id);
}

/*
 *	usb_gadget_register_driver
 */
int usb_gadget_register_driver(struct usb_gadget_driver *driver)
{
    struct sep4020_udc *udc = the_controller;
    int             retval;
   
    dprintk(DEBUG_NORMAL, "usb_gadget_register_driver() '%s'\n", driver->driver.name);

    /* Sanity checks */
    if (!udc)
        return -ENODEV;
    if (udc->driver)
        return -EBUSY;
    if (!driver->bind || !driver->unbind  || (driver->speed == USB_SPEED_UNKNOWN)||!(driver->setup))  //
        return -EINVAL;

    /* Hook the driver */
    udc->driver = driver;
    udc->gadget.dev.driver = &driver->driver;

    /* Bind the driver */
    device_add(&udc->gadget.dev);
    dprintk(DEBUG_NORMAL, "binding gadget driver '%s'\n", driver->driver.name);
    if ((retval = driver->bind(&udc->gadget)) != 0)
    {
        device_del(&udc->gadget.dev);
        udc->driver = 0;
        udc->gadget.dev.driver = 0;
        return retval;
    }
    
    /* Enable udc */
    udc_enable(udc);

    return 0;
}


/*
 * 	usb_gadget_unregister_driver
 */
int usb_gadget_unregister_driver(struct usb_gadget_driver *driver)
{
    struct sep4020_udc *udc = the_controller;

    if (!udc)
        return -ENODEV;
    if (!driver || driver != udc->driver)
        return -EINVAL;

    dprintk(DEBUG_NORMAL, "usb_gadget_unregister_driver() '%s'\n", driver->driver.name);

    //device_release_driver(&udc->gadget.dev);
    //driver_unregister(&driver->driver);
    driver->unbind(&udc->gadget);
    device_del(&udc->gadget.dev);
    udc->driver = NULL;
	udc->gadget.dev.driver = 0;
    

    /* Disable udc */
    udc_disable(udc);

    return 0;
}

/*---------------------------------------------------------------------------*/
static struct sep4020_udc memory = 
{
    .gadget = {
               .ops = &sep4020_ops,
               .ep0 = &memory.ep[0].ep,
               .name = gadget_name,
               .dev = {
                       .bus_id = "gadget",
                       .release = nop_release,
                            },
                    },

    /* control endpoint */
    .ep[0] = {
              .num = 0,
              .ep = {
                     .name = ep0name,
                     .ops = &sep4020_ep_ops,
                     .maxpacket = EP0_MAXPK_SIZE,
                        },
              .dev = &memory,
              },

    /* first group of endpoints */
    .ep[1] = {
              .num = 1,
              .ep = {
                     .name = "ep1-bulk",
                     .ops = &sep4020_ep_ops,
                     .maxpacket = EP_FIFO_SIZE,
                        },
              .dev = &memory,
              .fifo_size = EP_FIFO_SIZE,
              .bEndpointAddress = 1,
              .bmAttributes = USB_ENDPOINT_XFER_BULK,
                },
    .ep[2] = {
              .num = 2,
              .ep = {
                     .name = "ep2-bulk",
                     .ops = &sep4020_ep_ops,
                     .maxpacket = EP_FIFO_SIZE,
                        },
              .dev = &memory,
              .fifo_size = EP_FIFO_SIZE,
              .bEndpointAddress = 2,
              .bmAttributes = USB_ENDPOINT_XFER_BULK,
              }
};


/*
 *	probe - binds to the platform device
 */
static int sep4020_udc_probe(struct device *_dev)
{
    struct platform_device	*pdev = to_platform_device(_dev);
    struct sep4020_udc *udc = &memory;
    int             retval;
  
    dprintk(DEBUG_NORMAL, "sep4020_udc_probe\n");

    spin_lock_init(&udc->lock);
  
    udc->mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!udc->mem) 
    {
        dprintk(DEBUG_VERBOSE,"failed to get io memory region resouce.\n");
        return -ENOENT;
    }

    if (!request_mem_region(udc->mem->start,RESSIZE(udc->mem), gadget_name))
        return -ENOENT;

    device_initialize(&udc->gadget.dev);

    udc->gadget.dev.parent = _dev;
    udc->gadget.dev.dma_mask = _dev->dma_mask;

    the_controller = udc;
    dev_set_drvdata(_dev, udc);

    udc_disable(udc);
    udc_reinit(udc);

    /* request dma mem */	   
    vaddr = (unsigned char *) dma_alloc_coherent(NULL,0x2008, (dma_addr_t *) &phyaddr, GFP_KERNEL);
    if(!vaddr)
    {
        dprintk(DEBUG_VERBOSE,"fail to allocate dma mem!\n");
        retval = -ENOMEM;
        goto err_mem;
    }

    /* irq setup after old hardware state is cleaned up */
    retval = request_irq(INTSRC_USB, sep4020_udc_irq, SA_INTERRUPT, gadget_name, udc);
    if (retval != 0)
    {
        printk(KERN_ERR "%s: can't get irq %i, err %d\n", gadget_name, INTSRC_USB, retval);
        retval = -EBUSY;
        goto probe_irq_req_fail;
    }
    dprintk(DEBUG_VERBOSE, "%s: got irq %i\n", gadget_name, INTSRC_USB);


    return 0;

probe_irq_req_fail:
    dma_free_coherent(NULL, 0x2008, vaddr, (dma_addr_t )phyaddr);

err_mem:   
    release_mem_region(udc->mem->start, RESSIZE(udc->mem));
    return retval;
}


/*
 * 	sep4020_udc_remove
 */
static int sep4020_udc_remove(struct device *_dev)
{
    struct sep4020_udc *udc = _dev->driver_data;

    dprintk(DEBUG_NORMAL, "sep4020_udc_remove\n");
    usb_gadget_unregister_driver(udc->driver);

    if (udc->got_irq)
    {
        free_irq(INTSRC_USB, udc);
        udc->got_irq = 0;
    }

    release_mem_region(udc->mem->start, RESSIZE(udc->mem));

    dma_free_coherent(NULL, 0x2008, vaddr, (dma_addr_t )phyaddr);

    dev_set_drvdata(_dev, 0);
    kfree(udc);

    return 0;
}

static struct device_driver udc_driver = {
    .name = "sep4020-usbgadget",
    .bus = &platform_bus_type,
    .probe = sep4020_udc_probe,
    .remove = sep4020_udc_remove,
};


static int __init udc_init(void)
{
    printk("%s: version %s\n", gadget_name, DRIVER_VERSION);

    *(volatile unsigned int *)PMU_PUCR_V=0xc;
    *(volatile unsigned int *)PMU_PUCR_V=0x800c;

    return driver_register(&udc_driver);
}

static void __exit udc_exit(void)
{
    driver_unregister(&udc_driver);
}


EXPORT_SYMBOL(usb_gadget_unregister_driver);
EXPORT_SYMBOL(usb_gadget_register_driver);

module_init(udc_init);
module_exit(udc_exit);

MODULE_AUTHOR(DRIVER_AUTHOR);
MODULE_DESCRIPTION("sep4020 USB Device Controller Gadget");
MODULE_VERSION(DRIVER_VERSION);
MODULE_LICENSE("GPL");

