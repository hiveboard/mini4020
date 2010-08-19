/*
 *  linux/drivers/mmc/host/sep_mci.c - ASIC MCI Driver
 *
 *  Copyright (C) 2005 Cougar Creek Computing Devices Ltd, All Rights Reserved
 *
 *  Copyright (C) 2006 Malcolm Noyes
 *  Copyright (C) 2008 FP kyon prochip
 *	 Url: http://www.prochip.com.cn

 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

/*
   This is the SEP4020 MCI driver that has been tested with both MMC cards
   and SD-cards.  Boards that support write protect are now supported.
   The MBT1.5 board does not support SD cards.

   The three entry points are sep_mci_request, sep_mci_set_ios
   and sep_mci_get_ro.

   SET IOS
     This configures the device to put it into the correct mode and clock speed
     required.

   MCI REQUEST
     MCI request processes the commands sent in the mmc_request structure. This
     can consist of a processing command and a stop command in the case of
     multiple block transfers.

     There are three main types of request, commands, reads and writes.

     Commands are straight forward. The command is submitted to the controller and
     the request function returns. When the controller generates an interrupt to indicate
     the command is finished, the response to the command are read and the mmc_request_done
     function called to end the request.

     Reads and writes work in a similar manner to normal commands but involve the PDC (DMA)
     controller to manage the transfers.

     A read is done from the controller directly to the scatterlist passed in from the request.
     Due to a bug in the AT91RM9200 controller, when a read is completed, all the words are byte
     swapped in the scatterlist buffers.  AT91SAM926x are not affected by this bug.

     The sequence of read interrupts is: ENDRX, RXBUFF, CMDRDY

     A write is slightly different in that the bytes to write are read from the scatterlist
     into a dma memory buffer (this is in case the source buffer should be read only). The
     entire write buffer is then done from this single dma memory buffer.

     The sequence of write interrupts is: ENDTX, TXBUFE, NOTBUSY, CMDRDY

   GET RO
     Gets the status of the write protect pin, if available.
   File Name:  sep_eth.c
 *
 *	File Description: SEP4020的SD驱动程序
 *
 *    Version         Date            Author           
 *------------------------------------------------------------------------------------    
 *                3.0.0                         	2009.2.15                           方攀 程杰         
 *   	          3.0.1			                	2009.5.10			            	方攀
*/
 
#include <linux/config.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/ioport.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/interrupt.h>
#include <linux/dma-mapping.h>
#include <linux/mmc/host.h>
#include <linux/mmc/protocol.h> 
#include <linux/pnp.h>	
#include <linux/highmem.h>
#include <linux/clk.h>

#include <asm/scatterlist.h>
#include <asm/dma.h>
#include <asm/io.h>
#include <asm/irq.h>
#include <asm/mach/mmc.h>
#include <asm/arch/board.h>
#include <asm/arch/hardware.h>
#include <asm/arch/dma.h>
#include <asm/arch/irqs.h>

#undef CONFIG_SDI_SEP4020_DEBUG
#define DRIVER_NAME "sep4020_mci"
#define DRV_VERSION     "1.0"
#define PFX DRIVER_NAME ": "  
#define RESSIZE(ressource) (((ressource)->end - (ressource)->start)+1)

/*DMA 的通道号*/
#define sep4020SDI_DMA 2

/*DMA通道的类型*/
typedef enum 
{ 
    DMAP_READ,
    DMAP_WRITE,
} SEP_DMA_PURPOSE_T;
  
typedef enum //sd卡的传输状态类型
{
    COMPLETION_NONE,
    COMPLETION_CMDSENT,
    COMPLETION_RSPFIN,
    COMPLETION_XFERFINISH,
    COMPLETION_XFERFINISH_RSPFIN,
} SEP_WAIT_STA;

struct sep4020sdi_host {
    struct mmc_host		*mmc;

    struct resource		*mem;
    struct clk		*clk;
    void __iomem	*base;
    int			irq;
    int			irq_cd;
    int			dma;

    struct scatterlist *cur_sg;		/* Current SG entry */
    unsigned int		num_sg;		/* Number of entries left */
    void				*mapped_sg;	/* vaddr of mapped sg */

    unsigned int		offset;		/* Offset into current entry */
    unsigned int		remain;		/* Data left in curren entry */

    unsigned int		size;		/* Total size of transfer */

    struct mmc_request	*mrq;
    unsigned char		bus_width;	/* Current bus width */
  
	spinlock_t		complete_lock;
    struct completion	complete_request;
	
    SEP_WAIT_STA	complete_what;  //SD卡的传输状态

    unsigned int sd_respond[4];
    unsigned int sd_respond_last[4];
    unsigned int clkdiv;  //SD卡的时钟分频因子
    
        /***申请的DMA空间的虚拟地址和物理地址*/
    unsigned char  * vaddr;
    unsigned char  * vphyaddr;
};

/*************************** DEBUG FUNCTION ***************************/
#define DEBUG_NORMAL	1
#define DEBUG_VERBOSE	2

#ifdef CONFIG_SDI_SEP4020_DEBUG
#define SEP4020_SDI_DEBUG_LEVEL 1
unsigned int  sep4020_ticks = 0;
static int dprintk(int level, const char *fmt, ...)
{
    static char     printk_buf[1024];
    static long     prevticks;
    static int      invocation;
    va_list         args;
    int             len;

    if (level > SEP4020_SDI_DEBUG_LEVEL)
        return 0;

    if (sep4020_ticks != prevticks)
            {
        prevticks = sep4020_ticks;
        invocation = 0;
             }

    len = scnprintf(printk_buf, sizeof(printk_buf), "%1lu.%02d SEP4020: ", prevticks, invocation++);

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


/*
*sep4020 SD卡的中断处理主入口函数
*/
static irqreturn_t sep4020sdi_irq(int irq, void *dev_id, struct pt_regs *regs)
{
    struct sep4020sdi_host *host;
    unsigned long iflags;
    u32 sdi_csta,sdi_cclear,sdi_csta_mask;
	
    dprintk(DEBUG_VERBOSE,"go to sep4020sdi_irq.......................!\n");
  	
    host = (struct sep4020sdi_host *)dev_id;
    sdi_cclear	= 0;
    
    /*Check for things not supposed to happen*/
    if(!host) return IRQ_HANDLED;
	
    sdi_csta = readl(MMC_INTERRUPT_STATUS_V);
    sdi_csta_mask = readl(MMC_INTERRUPT_STATUS_MASK_V);		
    spin_lock_irqsave(&host->complete_lock, iflags);
	
    if(host->complete_what==COMPLETION_NONE)  //在空闲状态直接退出
	      {
        sdi_cclear	= 0xffff;
        goto clear_imask;
	      }

    if(!host->mrq) 
          {
        sdi_cclear	= 0xffff;
        goto clear_imask;
	      }
	
    if(sdi_csta & 0x1)  //命令响应超时中断
    	  {
        host->mrq->cmd->error = MMC_ERR_TIMEOUT;
        sdi_cclear |= 0x1;
        goto transfer_closed;
	        }

    if(sdi_csta & 0x80) //命令发送完毕中断
           {
        if(host->complete_what == COMPLETION_CMDSENT) 
		             {
			        host->mrq->cmd->error = MMC_ERR_NONE;
			        goto transfer_closed;
		              }
		
		     sdi_cclear |= 0x80;
	        }

    if(sdi_csta & 0x1000)   //命令响应中断
	       {
		    if(host->complete_what == COMPLETION_RSPFIN) //如果是命令响应状态则可以正常推出了
		           { 
			        host->mrq->cmd->error = MMC_ERR_NONE;
			        goto transfer_closed;
		            }

		    if(host->complete_what == COMPLETION_XFERFINISH_RSPFIN) //如果是数据加命令响应状态则可以转到数据传输状态
		           { 
			        dprintk(DEBUG_VERBOSE,"COMPLETION_XFERFINISH_RSPFIN\n");
			        host->mrq->cmd->error = MMC_ERR_NONE;
			        host->complete_what = COMPLETION_XFERFINISH;
		            }

		    sdi_cclear |= 0x1000;
	        }

    if(sdi_csta & 0x18)   //接收或发送FIFO
	       {
        host->mrq->cmd->error = MMC_ERR_NONE;
        host->mrq->data->error = MMC_ERR_FIFO;
		
		     sdi_cclear |= 0x18;
		     goto transfer_closed;
	       }

    if(sdi_csta & 0x2) //写数据 CRC校验出错
           {
        host->mrq->cmd->error = MMC_ERR_NONE;
        host->mrq->data->error = MMC_ERR_BADCRC;
		
        sdi_cclear |= 0x2;
        goto transfer_closed;
	        }

    if(sdi_csta & 0x4) //读数据 CRC校验出错
           {
        host->mrq->cmd->error = MMC_ERR_NONE;
        host->mrq->data->error = MMC_ERR_BADCRC;
		
        sdi_cclear |= 0x4;
        goto transfer_closed;
           }

    if(sdi_csta & 0x20) //读超时错误
    	  {
        host->mrq->cmd->error = MMC_ERR_NONE;
        host->mrq->data->error = MMC_ERR_TIMEOUT;
		
        sdi_cclear |= 0x20;
        goto transfer_closed;
	       }

    if(sdi_csta & 0x40) //数据传输完毕
          {
        if(host->complete_what == COMPLETION_XFERFINISH) //如果是数据传输状态就可以退出了
                     {
            host->mrq->cmd->error = MMC_ERR_NONE;
            host->mrq->data->error = MMC_ERR_NONE;
            dprintk(DEBUG_VERBOSE,"SD data transfer>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");
            goto transfer_closed;
    	           	 }

        if(host->complete_what == COMPLETION_XFERFINISH_RSPFIN) //如果是命令加数据状态则可以转到命令响应阶段
                    {
            host->mrq->data->error = MMC_ERR_NONE;
            host->complete_what = COMPLETION_RSPFIN;
    	           	 }

        sdi_cclear |= 0x40;
	        }

	
    writel(sdi_cclear|sdi_csta_mask, MMC_INTERRUPT_STATUS_MASK_V);
    spin_unlock_irqrestore(&host->complete_lock, iflags);
       
    return IRQ_HANDLED;


transfer_closed:                              //正常结束时的返回位置
    host->complete_what = COMPLETION_NONE;
    complete(&host->complete_request);
    writel(sdi_cclear|sdi_csta_mask, MMC_INTERRUPT_STATUS_MASK_V);
    spin_unlock_irqrestore( &host->complete_lock, iflags);
    dprintk(DEBUG_VERBOSE,KERN_INFO PFX "IRQ transfer closed.\n");
    return IRQ_HANDLED;
	
clear_imask:                //异常结束时的返回位置
    writel(sdi_cclear|sdi_csta_mask, MMC_INTERRUPT_STATUS_MASK_V);
    spin_unlock_irqrestore( &host->complete_lock, iflags);
    dprintk(DEBUG_NORMAL,"IRQ clear imask.\n");
    return IRQ_HANDLED;

}


/*
 * ISR for the CardDetect Pin
*/
/*
static irqreturn_t sep4020sdi_irq_cd(int irq, void *dev_id, struct pt_regs *regs)
{
    struct sep4020sdi_host *host = (struct sep4020sdi_host *)dev_id;
    mmc_detect_change(host->mmc,0);
    return IRQ_HANDLED;
}
*/

/*
 * 大小印地安转换函数
*/
inline unsigned long le32_to_sep(unsigned long x)
{
    return (((x & 0x000000ffU) << 24) |
    ((x & 0x0000ff00U) <<  8) |
    ((x & 0x00ff0000U) >>  8) |
    ((x & 0xff000000U) >> 24));
}

/*
 * dma设置函数
*/
static void sep4020sdi_dma_setup(struct sep4020sdi_host *host, SEP_DMA_PURPOSE_T purpose,struct mmc_request *mrq) 
{	
    dprintk(DEBUG_VERBOSE,"sep4020sdi_dma_setup...,mrq->data->blocks is 0x%x,mrq->data->blksz_bits is 0x%x\n",mrq->data->blocks,mrq->data->blksz_bits);
	
    switch(purpose) 
          {
        case DMAP_READ:
                    /***********************配置DMA2进行读*************************/
            *(volatile unsigned long*)DMAC_C2SRCADDR_V  = MMC_READ_BUFER_ACCESS;
            *(volatile unsigned long*)DMAC_C2DESTADDR_V = sg_dma_address(&mrq->data->sg[0]);//host->dma_addr;
            *(volatile unsigned long*)DMAC_C2CONTROL_V  = (0x1L<<13)|(2<<9)|(2<<6)|(3<<3)|3;     // burst=16 32bit  目的增加
            *(volatile unsigned long*)DMAC_C2CONTROL_V |= (((mrq->data->blocks << mrq->data->blksz_bits)>>2)<<14);    //dma传送的大小                          			
            *(volatile unsigned long*)DMAC_C2CONFIGURATION_V = (0<<11)|(7<<7)|(0<<6)|(0<<4)|(1<<3)|(2<<1)|1;          //0x38d;			
        break;
	
        case DMAP_WRITE:
                     /***********************配置DMA2进行写*************************/
            *(volatile unsigned long*)DMAC_C2SRCADDR_V  = (unsigned long)host->vphyaddr;
            *(volatile unsigned long*)DMAC_C2DESTADDR_V = MMC_WRITE_BUFER_ACCESS;
            *(volatile unsigned long*)DMAC_C2CONTROL_V  = (0x1L<<12)|(2<<9)|(2<<6)|(3<<3)|3;  //  burst=16   32bit 源增加
            *(volatile unsigned long*)DMAC_C2CONTROL_V |= (((mrq->data->blocks << mrq->data->blksz_bits)>>2)<<14);   //dma传送的大小             
            *(volatile unsigned long*)DMAC_C2CONFIGURATION_V = (7<<11)|(0<<7)|(0<<6)|(0<<4)|(1<<3)|(1<<1)|1;   				 	    
        break;
			
        default:
        break;
          }

}

/*
 * 设置sd卡时钟的函数
*/
static void setclk(u32 timeval,u32 control)
{
    u32 sdi_con;
	
    if(control == 0)
	      {
        sdi_con = timeval<<8;
        writel(sdi_con, MMC_CLOCK_CONTROL_V);
           }
    else
          {
        sdi_con = timeval<<8;
        sdi_con |= 0x4;
        writel(sdi_con, MMC_CLOCK_CONTROL_V);
          }
}
/*
 * sd卡命令请求函数
*/
static void sep4020sdi_request(struct mmc_host *mmc, struct mmc_request *mrq) 
{
    struct sep4020sdi_host *host = mmc_priv(mmc);
    struct device *dev = mmc_dev(host->mmc);
    struct platform_device *pdev = to_platform_device(dev);
    unsigned long iflags;
    u32 sdi_carg, sdi_ccon, sdi_timer,sdi_block;
    u32 sdi_bsize, sdi_transfer_mode, sdi_imsk,dma_len;
    u32 exchange,i=0;
    u32 sgdmaaddress=0;
    u32 dma_wait_count=0;
    
    dprintk(DEBUG_NORMAL,KERN_INFO PFX "request: [CMD] opcode:%d arg:0x%08x flags:%x retries:%u\n",mrq->cmd->opcode, mrq->cmd->arg, mrq->cmd->flags, mrq->cmd->retries);
    
    sdi_bsize =0;
    sdi_block =0;
    sdi_ccon = mrq->cmd->opcode <<5;
    sdi_ccon|= 1<<3;  //命令发送使能
    sdi_carg = mrq->cmd->arg;

    sdi_timer= 0xffffffff; //读超时寄存器值
    sdi_transfer_mode = 0;
	
               	/*使能命令发送和接收响应...中断位*/
    sdi_imsk = (~0x1080);

    host->clkdiv = 0x80;                     //时间分频因子  0x80;258分频 
    host->complete_what = COMPLETION_CMDSENT;  //默认的传输状态

    if (mrq->cmd->flags & (MMC_RSP_PRESENT)) 
          {                //根据协议层的标志来改变SD卡的传输状态
        host->complete_what = COMPLETION_RSPFIN;
        dprintk(DEBUG_VERBOSE,"Respond is MMC_RSP_MASK\n");
		
        sdi_ccon &= ~0x3;//清除低两位
        sdi_ccon |= 0x2;//对低两位赋值
        sdi_imsk = (~0x1001);
	      } 
    else 
          {
		//We need the CMDSENT-Interrupt only if we want are not waiting for a response
        sdi_imsk = (~0x80);
          }

    if(mrq->cmd->flags & MMC_RSP_136)  //长反馈
          {
        dprintk(DEBUG_VERBOSE,"Respond is MMC_RSP_LONG\n");
		
        sdi_ccon &= ~0x3;//清除低两位
        sdi_ccon |= 0x1;//对低两位赋值
           }
	
    if(mrq->cmd->flags & MMC_RSP_BUSY) //短反馈且加busy判断
          {
        dprintk(DEBUG_VERBOSE,"Respond is MMC_RSP_BUSY\n");
        sdi_ccon &= ~0x3;//清除低两位
        sdi_ccon |= 0x3;//对低两位赋值
           }
	

    if (mrq->data) //数据传输请求
            {
        dprintk(DEBUG_VERBOSE,"mmc/sd will transmit data!!!!\n");
        host->clkdiv = 0x2;  //主频6分频
        host->complete_what = COMPLETION_XFERFINISH_RSPFIN;
        sdi_ccon |= 0x4;
        sdi_bsize = (1 << mrq->data->blksz_bits);
        sdi_block  = (mrq->data->blocks & 0xfff);
	
        sdi_imsk = (~0x7e);

        if (host->bus_width == MMC_BUS_WIDTH_4)
            sdi_transfer_mode |= 0x1;

        dma_len = dma_map_sg(&pdev->dev, mrq->data->sg,	mrq->data->sg_len, mrq->data->flags & MMC_DATA_READ ? DMA_FROM_DEVICE : DMA_TO_DEVICE);//建立DMA到文件碎片链表SG的映射关系
        host->size = (mrq->data->blocks << mrq->data->blksz_bits);
        dprintk(DEBUG_VERBOSE,PFX "dma_len: 0x%08x\n",dma_len);
        sgdmaaddress = sg_dma_address(&mrq->data->sg[0]);//为后面大小印第安转化准备

        if(mrq->data->flags & MMC_DATA_WRITE) //写SD卡
                      {
            sdi_transfer_mode |= 0x2;

            spin_lock_irqsave( &host->complete_lock, iflags);
			
            for(i = 0; i < host->size; i += 4)//小印第安转化为大印第安
                                {
                exchange = le32_to_sep(*(unsigned long*)(sgdmaaddress+0x90000000+i));
                *(unsigned long*)(host->vaddr+i) = exchange;		
                                }

            spin_unlock_irqrestore( &host->complete_lock, iflags);

            sep4020sdi_dma_setup(host, DMAP_WRITE,mrq);
                       }

        if(mrq->data->flags & MMC_DATA_READ) //读SD卡
                     {
            sdi_transfer_mode &= (~0x2);
			
            sep4020sdi_dma_setup(host, DMAP_READ,mrq);
                     }
		
           }

    setclk(host->clkdiv,0);//关闭SD控制器的时钟开关
		
    host->mrq = mrq;	
    init_completion(&host->complete_request);	
  		
    // Setup SDI controller
    writel(sdi_ccon, MMC_COMMAND_V);
    writel(sdi_bsize,MMC_BLOCK_SIZE_V);
    writel(sdi_block,MMC_BLOCK_COUNT_V);
    writel(sdi_timer,MMC_READ_TIMEOUT_CONTROL_V);
    writel(sdi_imsk,MMC_INTERRUPT_STATUS_MASK_V);
    // Setup SDI command argument and data control
    writel(sdi_carg, MMC_ARGUMENT_V);
    writel(sdi_transfer_mode, MMC_TRANSFER_MODE_V);
	
    setclk(host->clkdiv,1);  //使能SD控制器的时钟开关
 
    if(mrq->data) 
          {
        dprintk(DEBUG_VERBOSE,"wait for dma!!!!!!!!!!!!!!!!!");
	 
        while(1)    //查询dam是否完成
                     {
			
            if(((*(unsigned long*)DMAC_INTTCSTATUS_V) & 0x4 ) == 0x4) //等待DMA传输完成             
            break;
				
            udelay(10);
            dma_wait_count++;
			
                                /*********若发生SD卡不响应的错误时，进入dma错误处理************/	
            if(dma_wait_count==10000) 
                                {
                dprintk(DEBUG_VERBOSE,"still in DMAC_INTTCSTATUS_V while\n");
                dma_wait_count=0;
                goto responed_result;          //如果有错误直接返回，交给上层来处理
		                    	}
            	   	}
		
    if(((*(unsigned long*)DMAC_INTERRORSTATUS_V ) & 0x4) == 0x4) 
        dprintk(DEBUG_VERBOSE,"DMAC is transtaning error!!!!!!!!!!\n");///如果dma出错的话则打印出错信息

    if(mrq->data->flags & MMC_DATA_READ) 
		  {
        spin_lock_irqsave( &host->complete_lock, iflags);
 			//大印第安转化成小印第安
        for(i=0;i<host->size;i +=4)
                    {
            exchange = le32_to_sep(*(unsigned long*)(sgdmaaddress+0x90000000+i));
            *(unsigned long*)(sgdmaaddress+0x90000000+i) = exchange;
			        }
			
        spin_unlock_irqrestore( &host->complete_lock, iflags);
	     }
	     
    writel(0x4,DMAC_INTTCCLEAR_V);
    writel(0x0,DMAC_INTTCCLEAR_V); //清除DMA通道相应位

    dprintk(DEBUG_VERBOSE,KERN_INFO PFX"[DAT] DMA complete.\n");
          }
	
    wait_for_completion(&host->complete_request); //等待SD完成事件的到来，此时任务挂起
	
    dprintk(DEBUG_VERBOSE,KERN_INFO PFX"[CMD] request complete.\n");
	
responed_result:	
	// 读返回寄存器
    host->sd_respond[0] = readl(MMC_RESPONSE0_V);
    host->sd_respond[1] = readl(MMC_RESPONSE1_V);
    host->sd_respond[2] = readl(MMC_RESPONSE2_V);
    host->sd_respond[3] = readl(MMC_RESPONSE3_V);
	
    if(mrq->cmd->flags & MMC_RSP_136)
          {
        mrq->cmd->resp[0] = (host->sd_respond[3]<<8) | (host->sd_respond[2]>>24);
        mrq->cmd->resp[1] = (host->sd_respond[2]<<8) | (host->sd_respond[1]>>24);
        mrq->cmd->resp[2] = (host->sd_respond[1]<<8) | (host->sd_respond[0]>>24);
        mrq->cmd->resp[3] = host->sd_respond[0]<<8;
           }
    else if (mrq->cmd->flags & MMC_RSP_R1)
   	      {
        mrq->cmd->resp[0] = host->sd_respond[0];
        mrq->cmd->resp[1] = host->sd_respond_last[1];
        mrq->cmd->resp[2] = host->sd_respond_last[2];
        mrq->cmd->resp[3] = host->sd_respond_last[3];
   	      }
    else
         {
        mrq->cmd->resp[0] = host->sd_respond_last[0];
        mrq->cmd->resp[1] = host->sd_respond_last[1];
        mrq->cmd->resp[2] = host->sd_respond_last[2];
        mrq->cmd->resp[3] = host->sd_respond_last[3];
	      }
  
    host->mrq = NULL;
  
    host->sd_respond_last[0] = mrq->cmd->resp[0];
    host->sd_respond_last[1] = mrq->cmd->resp[1];
    host->sd_respond_last[2] = mrq->cmd->resp[2];
    host->sd_respond_last[3] = mrq->cmd->resp[3];

    // If we have no data transfer we are finished here
    if (!mrq->data) 
        goto request_done;
	             //取消dma到文件碎片的映射
    dma_unmap_sg(&pdev->dev,mrq->data->sg,dma_len, mrq->data->flags & MMC_DATA_READ ? DMA_FROM_DEVICE : DMA_TO_DEVICE);
	
    if(mrq->data->error == MMC_ERR_NONE) 
          {
        mrq->data->bytes_xfered = (mrq->data->blocks << mrq->data->blksz_bits);
           }
    else 
          {
        mrq->data->bytes_xfered = 0;
        dprintk(DEBUG_NORMAL,"mrq->data->error!!!!!!!!!!!!!!!!!!!\n");
          }

    // Issue stop command
    if(mrq->data->stop)  
        mmc_wait_for_cmd(mmc, mrq->data->stop, 0);

request_done:
    mrq->done(mrq);//完成后递交给上层进行处理
}
/*
 * sd卡fifo清空函数，！！！！！！！！注意：sd卡在上电时fifo是有数据的，所以必须现将其清空
*/
void  fifo_reset(void)
{
    u32 tmp=0,sdi_con;
    sdi_con = 0x80<<8;
    writel(sdi_con, MMC_CLOCK_CONTROL_V);           		   //配置SD时钟
    udelay(20);
    writel(sdi_con|0x4, MMC_CLOCK_CONTROL_V);                     //打开SD时钟
    udelay(20);
    
    while(!(readl(MMC_INTERRUPT_STATUS_V) & 0x200))  //复位RXFIFO
        {
    	tmp = *(volatile unsigned long*)MMC_READ_BUFER_ACCESS_V;
     udelay(10);
        }
        
    while(!(readl(MMC_INTERRUPT_STATUS_V) & 0x800))  //复位TXFIFO
        {
    	*(volatile unsigned long*)MMC_WRITE_BUFER_ACCESS_V = tmp;
    	udelay(10);
        }

}

static void sep4020sdi_set_ios(struct mmc_host *mmc, struct mmc_ios *ios) 
{
    struct sep4020sdi_host *host = mmc_priv(mmc);
    u32 sdi_con,sdi_reset;

    //Set power
    sdi_con = 0x80<<8;
    switch(ios->power_mode)
          {
        case MMC_POWER_ON:
            dprintk(DEBUG_NORMAL,"MMC_POWER_ON .....MMC_SOFTWARE_RESET_V\n");			
        break;
			
        case MMC_POWER_UP:
            sdi_reset = (0x0);
            printk("Rest sdi controller is reseted!\n");
            writel(sdi_reset, MMC_SOFTWARE_RESET_V);
            udelay(20);
            sdi_reset = (0x1);
            writel(sdi_reset, MMC_SOFTWARE_RESET_V);
            fifo_reset();
            writel(sdi_con|0x4, MMC_CLOCK_CONTROL_V);
            mdelay(1);                              //同步sd卡的clk，需要74个clock的同步时间
        break;
			
        case MMC_POWER_OFF:
        default:
        break;
           }
	
    host->bus_width = ios->bus_width;//设置SD卡数据线宽度
}

static struct mmc_host_ops sep4020sdi_ops = {
    .request	= sep4020sdi_request,
    .set_ios	= sep4020sdi_set_ios,
};

static int sep4020sdi_probe(struct device *dev)
{
    struct platform_device	*pdev = to_platform_device(dev);
    struct mmc_host 	*mmc;
    struct sep4020sdi_host 	*host;
    int ret;
 
    printk("sep4020sdi_probe........................\n");
	
    mmc = mmc_alloc_host(sizeof(struct sep4020sdi_host), dev);
    if (!mmc) 
	      {
        ret = -ENOMEM;
        goto probe_out;
	       }

    
    host = mmc_priv(mmc);
    
    spin_lock_init(&host->complete_lock);
    host->clkdiv = 0x80;
    host->complete_what = COMPLETION_NONE;
    host->mmc = mmc;
    host->dma = sep4020SDI_DMA;
    //host->irq_cd = INTSRC_EXTINT2;//留待扩展用
    memset((char*)host->sd_respond,0,16);
    memset((char*)host->sd_respond_last,0,16);
    host->size = 0;

    dprintk(DEBUG_VERBOSE,"the resource num is %d\n",pdev->num_resources);
    host->mem = platform_get_resource(pdev, IORESOURCE_MEM, 0);
    if (!host->mem) 
          {
        dprintk(DEBUG_VERBOSE,KERN_INFO PFX "failed to get io memory region resouce.\n");
        ret = -ENOENT;
        goto probe_free_host;
           }

    host->mem = request_mem_region(host->mem->start,RESSIZE(host->mem), pdev->name);
    if (!host->mem) 
          {
        dprintk(DEBUG_VERBOSE,KERN_INFO PFX "failed to request io memory region.\n");
        ret = -ENOENT;
        goto probe_free_host;
           }

    host->vaddr = (unsigned char *) dma_alloc_coherent(NULL,4608, (dma_addr_t *) &host->vphyaddr, GFP_KERNEL);
    if(!host->vaddr)
          {
        dprintk(DEBUG_VERBOSE,"fail to allocate dma mem!\n");
        ret = -ENOMEM;
        goto probe_free_mem_region;
	      }
	
    host->irq = platform_get_irq(pdev, 0);
    if (host->irq == 0) 
          {
        dprintk(DEBUG_VERBOSE,KERN_INFO PFX "failed to get interrupt resouce.\n");
        ret = -EINVAL;
        goto probe_irq_req_fail;
           }

    if(request_irq(host->irq, sep4020sdi_irq, 0, DRIVER_NAME, host)) 
          {
        dprintk(DEBUG_VERBOSE,KERN_INFO PFX "failed to request sdi interrupt.\n");
        ret = -ENOENT;
        goto probe_irq_req_fail;
          }
          //配置mmc核心层的时钟
    host->clk = clk_get(&pdev->dev, "sdi");
    if (!host->clk) 
          {
        dprintk(DEBUG_VERBOSE,KERN_INFO PFX "failed to get clock source.\n");
        ret = PTR_ERR(host->clk);
        host->clk = NULL;
        goto probe_free_irq;
          }

    if((ret = clk_enable(host->clk))) 
          {
        dprintk(DEBUG_VERBOSE,KERN_INFO PFX "failed to enable clock source.\n");
        goto clk_free;
           }


    mmc->ops 	= &sep4020sdi_ops;
    mmc->ocr_avail	= MMC_VDD_32_33;
    mmc->f_min 	= 0x1;
    mmc->f_max 	= 0xff;//最大分频因子
    mmc->caps = MMC_CAP_4_BIT_DATA;

    /*
	 * Since we only have a 16-bit data length register, we must
	 * ensure that we don't exceed 2^16-1 bytes in a single request.
	 * Choose 64 (512-byte) sectors as the limit.
	 */
    mmc->max_sectors = 64;

	/*
	 * Set the maximum segment size.  Since we aren't doing DMA
	 * (yet) we are only limited by the data length register.
	 */
    mmc->max_seg_size = mmc->max_sectors << 9;

    if((ret = mmc_add_host(mmc))) 
          {
        dprintk(DEBUG_VERBOSE,KERN_INFO PFX "failed to add mmc host.\n");
        goto free_all;
          }

    dev_set_drvdata(dev, mmc);
  
  
    //INT_ENABLE(host->irq);
  	//sep4020int_unmask(host->irq);..............................................................................................
    printk("MMC/SD initialization done.the irq num is %d\n",host->irq);
    return 0;
	
free_all:

clk_free:
    clk_put(host->clk);

 /*probe_free_irq_cd:
 	free_irq(host->irq_cd, host);
*/
probe_free_irq:
    free_irq(host->irq, host);

probe_irq_req_fail:
    dma_free_coherent(NULL, 4608, host->vaddr, (dma_addr_t )host->vphyaddr);
 	
probe_free_mem_region:
    release_mem_region(host->mem->start, RESSIZE(host->mem));

probe_free_host:	
    mmc_free_host(mmc);
	
probe_out:
    return ret;
}

static int sep4020sdi_remove(struct device *dev)
{
    struct mmc_host 	*mmc  = dev_get_drvdata(dev);
    struct sep4020sdi_host 	*host = mmc_priv(mmc);

    mmc_remove_host(mmc);
    clk_disable(host->clk);
    clk_put(host->clk);
    free_irq(host->irq_cd, host);
    free_irq(host->irq, host);
    release_mem_region(host->mem->start, RESSIZE(host->mem));
    mmc_free_host(mmc);
    dma_free_coherent(NULL, 4608, host->vaddr, (dma_addr_t )host->vphyaddr);
    return 0;
}

static struct device_driver sep4020sdi_driver =
{     
    .name		 = DRIVER_NAME,
    .bus     = &platform_bus_type,
    .probe   = sep4020sdi_probe,
    .remove  = sep4020sdi_remove,
};

static int __init sep4020sdi_init(void)
{
    return driver_register(&sep4020sdi_driver);
}

static void __exit sep4020sdi_exit(void)
{
    driver_unregister(&sep4020sdi_driver);
}

module_init(sep4020sdi_init);
module_exit(sep4020sdi_exit);

MODULE_DESCRIPTION("sep4020 Multimedia Card Interface driver");
MODULE_LICENSE("GPL");
MODULE_AUTHOR("fpmystar@gmail.com");

