#ifndef _SEP4020_UDC_H
#define _SEP4020_UDC_H

struct sep4020_ep 
{
    struct list_head		queue;
    unsigned long			last_io;	/* jiffies timestamp */
    struct usb_gadget		*gadget;
    struct sep4020_udc		*dev;
    const struct usb_endpoint_descriptor *desc;
    struct usb_ep			ep;
    u8				num;

    unsigned short			fifo_size;
    u8				bEndpointAddress;
    u8				bmAttributes;

    unsigned			halted : 1;
    unsigned			already_seen : 1;
    unsigned			setup_stage : 1;
};

/* Warning : ep0 has a maxsize of 8 bytes */
/* Don't try to set 32 or 64            */
#define EP0_MAXPK_SIZE		   8     
#define EP_FIFO_SIZE		      64    

static const char ep0name [] = "ep0-control";

static const char *const ep_name[] = {
    ep0name,                                /* everyone has ep0 */
	/* sep4020 two bidirectional bulk endpoints */
    "ep1-bulk",  "ep2-bulk", 
};

#define SEP4020_ENDPOINTS       ARRAY_SIZE(ep_name)

struct sep4020_request {
    struct list_head		queue;		/* ep's requests */
    struct usb_request		req;
};

enum ep0_state 
{
    EP0_IDLE,
    EP0_IN_DATA_PHASE,
    EP0_OUT_DATA_PHASE,
    EP0_END_XFER,
    EP0_STALL,
};

static const char *ep0states[]= { //for print
    "EP0_IDLE",
    "EP0_IN_DATA_PHASE",
    "EP0_OUT_DATA_PHASE",
    "EP0_END_XFER",
    "EP0_STALL",
};

struct sep4020_udc {
    spinlock_t			lock;

    struct sep4020_ep		ep[SEP4020_ENDPOINTS];
    int				address;
    struct usb_gadget		gadget;
    struct usb_gadget_driver	*driver;
    struct sep4020_request		fifo_req;
    struct resource		*mem;

    u8				fifo_buf[EP_FIFO_SIZE];
    u16				devstatus;
    u32				port_status;
    int 	    	   ep0state;

    unsigned			got_irq : 1;

    unsigned			req_std : 1;
    unsigned			req_config : 1;
    unsigned			req_pending : 1;
	
};

#define WRITE_SUCCESS	0x80000000
#define READ_SUCCESS	   0x40000000

#define SEP4020_UDC_EP_CTLCOMP		0x8   
#define SEP4020_UDC_EP_PROSTALL		0x4   

#define   INTSETUP        0x01  
#define   INTIN           0x02  
#define   INTOUT          0x04 
#define   INTPING         0x08
#define   INTTRAERR       0x10  
#define   INTACK          0x20  
#define   INTSTCOV        0x40  
#define   INTCTRAERR      0x80  
#define   INTRESET        0x200
#define   INTSUSPEND      0x100

#endif

