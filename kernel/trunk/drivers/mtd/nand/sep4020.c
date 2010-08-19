
/* linux/drivers/mtd/nand/sep4020.c
 *
* Copyright (c) 2009 prochip company
*	http://www.prochip.com.cn
*	leeming1203@gmail.com.cn
*
* sep4020 nandflash driver.
*
* Changelog:
*	22-sep-2008 leeming		Initial version
*	14-may-2009 leeming  	fixed	a lot, support 512 byte nandflash only
 *
 * 
 *
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*/

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/delay.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>
#include <linux/dma-mapping.h>

#include <asm/io.h>

#include <asm/sizes.h>
#include <asm/arch/hardware.h>



/*
 * MTD struct for SEP4020 board
 */
static struct mtd_info *sep4020_mtd = NULL;

/*vitual addr and phy addr for dma */
static unsigned char * nand_vaddr = NULL;
static unsigned char * nand_phyaddr = NULL;



/*
 * Define static partitions for flash device
 */

#ifdef CONFIG_MTD_PARTITIONS

#define NUM_PARTITIONS 4

/*请保证分区的总和数和nandflash的实际大小一致，目前开发板上的nandflash是64M*/
#define UBOOT_SIZE 	SZ_1M*1
#define KERNEL_SIZE 	SZ_1M*5
#define ROOT_SIZE		SZ_1M*24
#define USER_SIZE 	SZ_1M*34

static struct mtd_partition partition_info[] = {
	{ .name = "U-boot",
	  .offset = 0,
	  .size = UBOOT_SIZE
	 },
	{ .name = "linux 2.6.16 kernel",
	  .offset = UBOOT_SIZE,
	  .size = KERNEL_SIZE
	},
	{ .name = "root",
 	  .offset = UBOOT_SIZE+KERNEL_SIZE,
	  .size = ROOT_SIZE
	},
	{ .name = "user",
	  .offset = UBOOT_SIZE+KERNEL_SIZE+ROOT_SIZE,
	  .size = USER_SIZE
	}
};

#endif


/*
 *	hardware specific access to control-lines
 */
static void sep4020_hwcontrol(struct mtd_info *mtd, int cmd)
{	
	struct nand_chip *this = mtd->priv;
	
	switch (cmd) {
	case NAND_CTL_SETNCE:
	case NAND_CTL_CLRNCE:
		printk(KERN_ERR "%s: called for NCE\n", __FUNCTION__);
		break;

	case NAND_CTL_SETCLE:
		this->IO_ADDR_W = (void __iomem *) EMI_NAND_COM_V;
		break;

	case NAND_CTL_SETALE:
		this->IO_ADDR_W = (void __iomem *) EMI_NAND_ADDR1_V;
		break;
	
	case NAND_READ_ID:
		this->IO_ADDR_R = (void __iomem *) EMI_NAND_ID_V;
		break;
	
	case NAND_READ_STATUS:
		this->IO_ADDR_R = (void __iomem *) EMI_NAND_STA_V;
		break;	
		/* NAND_CTL_CLRCLE: */
		/* NAND_CTL_CLRALE: */
	default:
		this->IO_ADDR_W = (void __iomem *) EMI_NAND_DATA;
		this->IO_ADDR_R = (void __iomem *) EMI_NAND_DATA;
		break;
	}

}


/**
 * nand_command - [DEFAULT] Send command to NAND device
 * @mtd:	MTD device structure
 * @command:	the command to be sent
 * @column:	the column address for this command, -1 if none
 * @page_addr:	the page address for this command, -1 if none
 *
 * Send command to NAND device. This function is used for sep4020 nand
 * devices (256/512 Bytes per page)
 */

static void sep4020_nand_command (struct mtd_info *mtd, unsigned command, int column, int page_addr)
{
	register struct nand_chip *this = mtd->priv;

	/* Begin command latch cycle */
	this->hwcontrol(mtd, NAND_CTL_SETCLE);
	
	/*
	 * Write out the command to the device.
	 */
	if (command == NAND_CMD_SEQIN) 
	{
		int readcmd;

		if (column >= mtd->oobblock) 
		{
			
			column -= mtd->oobblock;
			readcmd = NAND_CMD_READOOB;
		} 
		else if (column < 256) 
			{
			readcmd = NAND_CMD_READ0;
			}
			 else 
			{
			column -= 256;
			readcmd = NAND_CMD_READ1;
			}
		this->write_byte(mtd, readcmd);
			
	}
	this->write_byte(mtd, command);


	if  (command == NAND_CMD_READID)
	{
		*(volatile unsigned long*)EMI_NAND_COM_V		|= 0x80000000;	//使能EMI_NAND_COM
		this->hwcontrol(mtd, NAND_READ_ID);
		return;
	}
	
	if  (command == NAND_CMD_STATUS)
	{
		*(volatile unsigned long*)EMI_NAND_COM_V		|= 0x80000000;	//使能EMI_NAND_COM
		this->hwcontrol(mtd, NAND_READ_STATUS);
	}
	
	if	(command == NAND_CMD_RESET)
	{
		*(volatile unsigned long*)EMI_NAND_COM_V		|= 0x80000000;
		this->hwcontrol(mtd, NAND_CTL_CLRALE);
	}
	

	/* Set ALE and clear CLE to start address cycle */
	//this->hwcontrol(mtd, NAND_CTL_CLRCLE);

	if (column != -1 || page_addr != -1) 
	{

	this->hwcontrol(mtd, NAND_CTL_SETALE);		
	*(volatile unsigned long*)(EMI_NAND_ADDR1_V) = (page_addr<<8) ;

	/* Latch in address */
	this->hwcontrol(mtd, NAND_CTL_CLRALE);
	}

	ndelay (100);

}



static void sep4020_nand_write_byte(struct mtd_info *mtd, u_char byte)
{
	struct nand_chip *this = mtd->priv;
	*(volatile unsigned long*)(this->IO_ADDR_W) = byte;
}

static u_char sep4020_nand_read_byte(struct mtd_info *mtd)
{
	struct nand_chip *this = mtd->priv;
	return *(volatile unsigned long*)(this->IO_ADDR_R);
}




/* over-ride the standard functions for a little more speed. We can
 * use read/write block to move the data buffers to/from the controller
*/


static void sep4020_nand_write_buf(struct mtd_info *mtd, const u_char *buf, int len)
{
	struct nand_chip *this = mtd->priv;

	if(len == 0) 
			return;	
	
	if(len == 528)
        {   
		memcpy(nand_vaddr, buf, 528);
        }

	if(len == 512) //if len == 512
	{	
		memcpy(nand_vaddr, buf, 512);
		memset(nand_vaddr+512, 0xff, 16);
	}

	if(len == 16)
        {
		//next is read the 512 to the (vphraddr)
		*(volatile unsigned long*)EMI_NAND_COM_V = 0x00;
		*(volatile unsigned long*)DMAC_C0CONTROL_V  = (132<<14) + (1<<13) + (2<<9) +(2<<6) + (3<<3) + 3; 
		*(volatile unsigned long*)DMAC_C0SRCADDR_V  =  (unsigned long)this->IO_ADDR_R ;
		*(volatile unsigned long*)DMAC_C0DESTADD_V  =  (unsigned long)nand_phyaddr ;	
		*(volatile unsigned long*)DMAC_C0CONFIGURATION_V = 0x31d ; 
		*(volatile unsigned long*)EMI_NAND_COM_V		|= 0x80000000;	
 	
		while(1)
		{
			if ((*(volatile unsigned long*)EMI_NAND_IDLE_V & 0x01)!= 0)
			break;
		}
		memcpy(nand_vaddr+512, buf, 16);
	}

		
	// next is put the totally 528 bytes to the nand	
	*(volatile unsigned long*)EMI_NAND_COM_V = 0X80;
	*(volatile unsigned long*)DMAC_C0CONTROL_V  = (132<<14) + (1<<12) + (2<<9) +(2<<6) + (3<<3) + 3 ; 
	*(volatile unsigned long*)DMAC_C0SRCADDR_V  = (unsigned long)nand_phyaddr ;
	*(volatile unsigned long*)DMAC_C0DESTADD_V  = (unsigned long)this->IO_ADDR_W  ;
	*(volatile unsigned long*)DMAC_C0CONFIGURATION_V = 0x300b ;
	*(volatile unsigned long*)EMI_NAND_COM_V		|= 0x80000000;	
 	
	while(1)
        {
		if ((*(volatile unsigned long*)EMI_NAND_IDLE_V & 0x01) != 0)
		break;
        }
}





static void sep4020_nand_read_buf(struct mtd_info *mtd, u_char *buf, int len)
{
	struct nand_chip *this = mtd->priv;

    int readlen=0;

	//read the data, len maybe the 512bytes ,16bytes.
	if (*((unsigned char*)EMI_NAND_COM_V) == 0x00)
	{	
        readlen = 528;
	}    
	//read the oob, len must be 16bytes.
	else
	{
        readlen =len; 			
	}
    *(volatile unsigned long*)DMAC_C0CONTROL_V  = ((readlen>>2)<<14) + (1<<13) + (2<<9) +(2<<6) + (3<<3) + 3; 
    *(volatile unsigned long*)DMAC_C0SRCADDR_V  =  (unsigned long)this->IO_ADDR_R ;
    *(volatile unsigned long*)DMAC_C0DESTADD_V  =  (unsigned long)nand_phyaddr ;	
    *(volatile unsigned long*)DMAC_C0CONFIGURATION_V = 0x31d ; 
    *(volatile unsigned long*)EMI_NAND_COM_V		|= 0x80000000;	
    while(1)
        {
    if ((*(volatile unsigned long*)EMI_NAND_IDLE_V & 0x01) != 0)
    break;
        }

	//如果是读取512字节，或者是直接通过发送oob命令读取最后16字节的
    if (len ==512 || (*((unsigned char*)EMI_NAND_COM_V) == 0x50))
        {
		memcpy(buf, nand_vaddr, len);
        }
        //读取最后16字节，但是是通过发送读整页指令的
    else 
        {
    memcpy(buf, nand_vaddr+512, len);
        }
}






// get the nand controller status
static int sep4020_nand_dev_ready(struct mtd_info *mtd)
{
	return 1;
}


//nand_erase has its own cmd
static void sep4020_nand_cmd_erase(struct mtd_info *mtd, int page)
{
	
	*(volatile unsigned long*)EMI_NAND_ADDR1_V = page;
	*(volatile unsigned long*)EMI_NAND_COM_V = 0X80000060;
	while(!(*(volatile unsigned long*)EMI_NAND_IDLE_V));
}



//sep4020 nand controller donot have this function
static void sep4020_nand_select_chip(struct mtd_info *mtd, int chip)
{
	return;
}

static void sep4020_emi_init(void)
{
	/*
	* the dauflt value of GPIO Port F control register is configured
	* to be outputs,and nandflash interface default ,so we needn't to change it
	* the fuction of the below codes are init the emi to configure the nandcontroller and
	* open the wp# of nandflash 
	*/	 

	*(volatile unsigned long*)EMI_NAND_CONF1_V = 0x06302857;//TW2R:6个时钟周期； TALE：4个地址；  TWB：8； 读脉冲：5个； 写脉冲：7个；
	*(volatile unsigned long*)EMI_NAND_CONF2_V = 0x00514353;//page:512bytes; ecc:soft; TRR:4; TAR:20; TREH:3; CLH:1; TALH:1; TWH:3
  
	*(volatile unsigned long*)GPIO_PORTG_SEL_V |= 0x0080;	
	*(volatile unsigned long*)GPIO_PORTG_DATA_V |= 0x0080;
	*(volatile unsigned long*)GPIO_PORTG_DIR_V &= ~0x0080;
	
}




/*
 * Main initialization routine
 */
  static int __init sep4020_nand_init(void)
 {
	struct nand_chip *this;
 	
	/* Allocate memory for MTD device structure and private data */
	sep4020_mtd = kmalloc (sizeof(struct mtd_info) + sizeof (struct nand_chip),GFP_KERNEL);
	if (!sep4020_mtd) 
	{
		printk ("Unable to allocate SEP4020 NAND MTD device structure.\n");
		return -ENOMEM;
        }			

	/* Get pointer to private data */
	this = (struct nand_chip *) (&sep4020_mtd[1]);// i think this way is very cool,


	/* Initialize structures */
	memset((char *) sep4020_mtd, 0, sizeof(struct mtd_info) + sizeof (struct nand_chip));

	
	nand_vaddr = (unsigned char *) dma_alloc_coherent(NULL,528, (dma_addr_t *) &nand_phyaddr, GFP_KERNEL);
	if(nand_vaddr == NULL)
        {
		kfree(sep4020_mtd);
        } 
	

	/* Link the private data with the MTD structure */
	sep4020_mtd->priv = this;

	/* Set address of NAND IO lines */
	//here is the phy address, because the data transport depends on dma
	this->IO_ADDR_R = (void __iomem *) EMI_NAND_DATA;
	this->IO_ADDR_W = (void __iomem *) EMI_NAND_DATA;

	/* Set address of hardware control function */
	this->hwcontrol = sep4020_hwcontrol;

	/* 15 us command delay time */
	this->dev_ready = sep4020_nand_dev_ready;
	this->chip_delay = 15;
	this->write_buf	= sep4020_nand_write_buf;
	this->read_buf  = sep4020_nand_read_buf;
	this->write_byte = sep4020_nand_write_byte;
	this->read_byte =  sep4020_nand_read_byte;
	
	/* Here use the mtd's soft ecc */
	this->eccmode = NAND_ECC_NONE;
	this->select_chip = sep4020_nand_select_chip;
	this->cmdfunc = sep4020_nand_command;
	this->erase_cmd = sep4020_nand_cmd_erase;
	
	/*init the gpio port and the nandflash configure register*/
	sep4020_emi_init();

	if (nand_scan(sep4020_mtd, 1)) 
	{
		kfree (sep4020_mtd);
		dma_free_coherent(NULL, 528, nand_vaddr, (dma_addr_t )nand_phyaddr);
		return -ENXIO;
        }

	/* Register the partitions */
	add_mtd_partitions(sep4020_mtd, partition_info, NUM_PARTITIONS);

	/* Return happy */
	return 0;
 }

module_init(sep4020_nand_init);

/*
 * Clean up routine
 */
 static void __exit sep4020_cleanup(void)
 {
    /* Release resources, unregister device */
    nand_release (sep4020_mtd);

    /* Free the MTD device structure */
    kfree (sep4020_mtd);

	/* Free dma space */
    dma_free_coherent(NULL, 528, nand_vaddr, (dma_addr_t )nand_phyaddr);
 }

module_exit(sep4020_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Leeming Zhang <leeming1203@gmail.com>");
MODULE_DESCRIPTION("Board-sepcific for Nand flash on evb4020 board");
