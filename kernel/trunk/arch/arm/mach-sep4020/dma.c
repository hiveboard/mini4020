
#include <linux/config.h>


#include <linux/module.h>
#include <linux/init.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#include <linux/interrupt.h>
#include <linux/sysdev.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/delay.h>

#include <asm/semaphore.h>
#include <asm/io.h>
#include <asm/mach-types.h>
#include <asm/arch/hardware.h>
#include <asm/arch/dma.h>
static spinlock_t dmac_lock = SPIN_LOCK_UNLOCKED;

int sep4020_dma_set(struct sep4020dma_param *dma)
{						
    //u32 	tempcontrol;
    //u32  tempconfig;
    u32 	srcreg;
    u32  destreg;
    u32  dmactrlreg;
    u32  dmaconfreg;
  

    spin_lock_irq(&dmac_lock);

    srcreg = DMAC_BASE_V  + (dma->channelnum)*0x100;			
    destreg = DMAC_BASE_V + 0x4 + (dma->channelnum)*0x100;
    dmactrlreg = DMAC_BASE_V + 0x0c + (dma->channelnum)*0x100;
    dmaconfreg = DMAC_BASE_V + 0x010 + (dma->channelnum)*0x100;

    //tempcontrol = (dma->transize<<14)|(dma->destinc<<13)|(dma->srcinc<<12)|(dma->destsize<<9)|(dma->srcsize<<6)|(dma->destburst<<3)|(dma->srcburst);
    //tempconfig = (dma->destnum<<11)|(dma->srcnum<<7)|(dma->controller<<6)|(dma->compintmask<<4)|(dma->errintmask<<3)|(dma->transtype<<1)|(dma->channelenable);

    *(u32 *)srcreg = dma->srcadd;
    *(u32 *)destreg = dma->destadd;
    *(u32 *)dmactrlreg = (dma->transize<<14)|(dma->destinc<<13)|(dma->srcinc<<12)|(dma->destsize<<9)|(dma->srcsize<<6)|(dma->destburst<<3)|(dma->srcburst); 
    *(u32 *)dmaconfreg = (dma->destnum<<11)|(dma->srcnum<<7)|(dma->controller<<6)|(dma->compintmask<<4)|(dma->errintmask<<3)|(dma->transtype<<1)|0x1;

    spin_unlock_irq(&dmac_lock);

    return 0;
}

