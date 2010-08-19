#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/module.h>
#include <linux/platform_device.h>

#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>
#include <linux/mtd/partitions.h>

#include <linux/dma-mapping.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <asm/sizes.h>
#include <linux/delay.h>

/*
 * MTD struct for SEP4020 board
 */
static struct mtd_info *sep4020_mtd = NULL;
unsigned char * vaddr;
unsigned char * vphyaddr;
	

//unsigned char *vaddr;
//unsigned char *vphyaddr;

#ifdef CONFIG_MTD_PARTITIONS
/*
 * Define static partitions for flash device
 */
static struct mtd_partition partition_info[] = {
	{ .name = "U-boot",
	  .offset = 0,
	  .size = 1*1024*1024
	 },
	{ .name = "linux 2.6.16 kernel",
	  .offset = 1*1024*1024,
	  .size = 5*1024*1024
	},
	{ .name = "root",
 	  .offset = 6*1024*1024,
	  .size = 24*1024*1024
	},
	{ .name = "user",
	  .offset = 30*1024*1024,
	  .size = 34*1024*1024
	}
};
#define NUM_PARTITIONS 4

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
	if (command == NAND_CMD_SEQIN) {
		int readcmd;

		if (column >= mtd->oobblock) {
			
			column -= mtd->oobblock;
			readcmd = NAND_CMD_READOOB;
		} else if (column < 256) {
			
			readcmd = NAND_CMD_READ0;
		} else {
			column -= 256;
			readcmd = NAND_CMD_READ1;
		}
		this->write_byte(mtd, readcmd);
			
	}
	
	this->write_byte(mtd, command);
	//printk("after wire_byte is %x\r\n",(*(unsigned long*)EMI_NAND_COM_V));

	if  (command == NAND_CMD_READID)
	{
		*(unsigned long*)EMI_NAND_COM_V		|= 0x80000000;	//使能EMI_NAND_COM
		this->hwcontrol(mtd, NAND_READ_ID);
		return;
	}
	
	if  (command == NAND_CMD_STATUS)
	{
		*(unsigned long*)EMI_NAND_COM_V		|= 0x80000000;	//使能EMI_NAND_COM
		this->hwcontrol(mtd, NAND_READ_STATUS);
	}
	
	if	(command == NAND_CMD_RESET)
		{
			*(unsigned long*)EMI_NAND_COM_V		|= 0x80000000;
			this->hwcontrol(mtd, NAND_CTL_CLRALE);
		}
	/* Set ALE and clear CLE to start address cycle */
	//this->hwcontrol(mtd, NAND_CTL_CLRCLE);

	if (column != -1 || page_addr != -1) {
		this->hwcontrol(mtd, NAND_CTL_SETALE);
		
			*(volatile unsigned long*)(EMI_NAND_ADDR1_V) = (page_addr<<8) ;
 	
		/* Latch in address */

		this->hwcontrol(mtd, NAND_CTL_CLRALE);
	}

	ndelay (100);

//printk("out nand_command\r\n");
//	nand_wait_ready(mtd);
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
	//unsigned char * vaddr;
	//unsigned char * vphyaddr;
	//vaddr = (unsigned char *) dma_alloc_coherent(NULL,530, (dma_addr_t *) &vphyaddr, GFP_KERNEL);
	//printk("the write len is %d\r\n",len);
	//printk("the page address is %x\r\n",*(volatile unsigned long*)EMI_NAND_ADDR1_V);
	
	int dmanum = 0;
	if (len > 500) //if len == 512
		{	
			 	while(dmanum < len)//give the first 512 to the dma space
			{
				vaddr[dmanum] = buf[dmanum];
				dmanum++;
 			}	
 			
 			 while(dmanum < 528)
 			 {
 			 	vaddr[dmanum] = 0xff;
 			 	dmanum++;
 			}
		}
		
	else
		{
			if (len == 0) return;
 	//next is read the 512 to the (vphraddr)
 	*(unsigned long*)EMI_NAND_COM_V = 0x00;
 	*(unsigned long*)DMAC_C0CONTROL_V  = (132<<14) + (1<<13) + (2<<9) +(2<<6) + (3<<3) + 3; 
	*(unsigned long*)DMAC_C0SRCADDR_V  =  EMI_NAND_DATA  ;
 	*(unsigned long*)DMAC_C0DESTADD_V  =   vphyaddr ;	
 	*(unsigned long*)DMAC_C0CONFIGURATION_V = 0x31d ; 
 	*(unsigned long*)EMI_NAND_COM_V		|= 0x80000000;	
 	
 	  	while(1)
 	{
 	ndelay(100);
 	if (*(unsigned long*)EMI_NAND_IDLE_V & (0x01) != 0)
 		break;
  }
 	  
 	  while(dmanum < len)//give the first 512 to the dma space
			{
				vaddr[512+dmanum] = buf[dmanum];
				dmanum++;
 			}
		}
		
	// next is put the totally 528 bytes to the nand	
	*(unsigned long*)EMI_NAND_COM_V = 0X80;
	*(unsigned long*)DMAC_C0CONTROL_V  = (132<<14) + (1<<12) + (2<<9) +(2<<6) + (3<<3) + 3 ; 
	*(unsigned long*)DMAC_C0SRCADDR_V  = vphyaddr ;
 	*(unsigned long*)DMAC_C0DESTADD_V  = EMI_NAND_DATA  ;
 	*(unsigned long*)DMAC_C0CONFIGURATION_V = 0x300b ;
 	*(unsigned long*)EMI_NAND_COM_V		|= 0x80000000;	
 	
 	   	while(1)
 	{
 	ndelay(100);
 	if (*(unsigned long*)EMI_NAND_IDLE_V & (0x01) != 0)
 		break;
  }
 	 
 //	dma_free_coherent(NULL, 530, vaddr, (dma_addr_t )vphyaddr);
}


/*
static void sep4020_nand_write_buf(struct mtd_info *mtd, const u_char *buf, int len)
{
	printk("write len is %d\r\n",len);
	//printk("write idle fist %x\r\n",(*(unsigned long*)EMI_NAND_IDLE_V));
	
	// to allocate a dma space ,vaddr is the virtul address ,vphyaddr is the physics address
	//unsigned char * vaddr;
//	unsigned char * vphyaddr;
	//vaddr = (unsigned char *) dma_alloc_coherent(NULL,530, (dma_addr_t *) &vphyaddr, GFP_KERNEL);
	
	int dmanum = 0;
	while(dmanum < len)
	{
		vaddr[dmanum] = buf[dmanum];
		dmanum++;
	}
	
	//add by kyon  512 to 528
	if ( len == 512 )
		{
			*(volatile unsigned char*)(0xc0002000+512) = 0xff;
			*(volatile unsigned char*)(0xc0002000+513) = 0xff;
			*(volatile unsigned char*)(0xc0002000+514) = 0xff;
			*(volatile unsigned char*)(0xc0002000+515) = 0xff;
			*(volatile unsigned char*)(0xc0002000+516) = 0xff;
			*(volatile unsigned char*)(0xc0002000+517) = 0xff;
			*(volatile unsigned char*)(0xc0002000+518) = 0xff;
			*(volatile unsigned char*)(0xc0002000+519) = 0xff;
			*(volatile unsigned char*)(0xc0002000+520) = 0xff;
			*(volatile unsigned char*)(0xc0002000+521) = 0xff;
			*(volatile unsigned char*)(0xc0002000+522) = 0xff;
			*(volatile unsigned char*)(0xc0002000+523) = 0xff;
			*(volatile unsigned char*)(0xc0002000+524) = 0xff;
			*(volatile unsigned char*)(0xc0002000+525) = 0xff;
			*(volatile unsigned char*)(0xc0002000+526) = 0xff;
			*(volatile unsigned char*)(0xc0002000+527) = 0xff;
		

	*(unsigned long*)DMAC_C0CONTROL_V  = (132<<14) + (1<<12) + (2<<9) +(2<<6) + (3<<3) + 3 ; 
	
	*(unsigned long*)DMAC_C0SRCADDR_V  = vphyaddr ;
 	*(unsigned long*)DMAC_C0DESTADD_V  = EMI_NAND_DATA  ;
 	*(unsigned long*)DMAC_C0CONFIGURATION_V = 0x300b ;
 	*(unsigned long*)EMI_NAND_COM_V		|= 0x80000000;	
 	
 	ndelay(100000);//延时，使得DMA搬完
 //	dma_free_coherent(NULL, 530, vaddr, (dma_addr_t )vphyaddr);
	
	}
 	
// 	printk("dma status%x\r\n",*(unsigned long*)DMAC_INTTCSTATUS_V);
 		//dmanum=0;
 	 	//while(!(*(unsigned long*)EMI_NAND_IDLE_V));
 	// printk("EMI_NAND_IDLE_V last %x\r\n",(*(unsigned long*)EMI_NAND_IDLE_V));
 	// printk("zlm write is over\r\n");
}
*/


static void sep4020_nand_read_buf(struct mtd_info *mtd, u_char *buf, int len)
{


	//unsigned char dma_read[528]={0};
//	printk("read_len%d\r\n",len);
	//printk("read idle fist%x\r\n",*((unsigned long*)EMI_NAND_IDLE_V));
	//unsigned char zlmdma[528]={0};
	//unsigned char * vaddr;
	//unsigned char * vphyaddr;
	//vaddr = (unsigned char *) dma_alloc_coherent(NULL,530, (dma_addr_t *) &vphyaddr, GFP_KERNEL);
	int read528=0;
	if (*((unsigned char*)EMI_NAND_COM_V) == 0x00)
		read528 = 528;
	else
		read528 =len; 
		
	
	*(unsigned long*)DMAC_C0CONTROL_V  = ((read528>>2)<<14) + (1<<13) + (2<<9) +(2<<6) + (3<<3) + 3; 

	*(unsigned long*)DMAC_C0SRCADDR_V  =  EMI_NAND_DATA  ;
 	*(unsigned long*)DMAC_C0DESTADD_V  =   vphyaddr ;	
 	*(unsigned long*)DMAC_C0CONFIGURATION_V = 0x31d ; 
 	*(unsigned long*)EMI_NAND_COM_V		|= 0x80000000;	
 	 	while(1)
 	{
 	ndelay(100);
 	if (*(unsigned long*)EMI_NAND_IDLE_V & (0x01) != 0)
 		break;
  }
      int dmanum = 0;
	if (len ==512 ||(*((unsigned char*)EMI_NAND_COM_V) == 0x50))
	{
			while(dmanum<len)
		{
		buf[dmanum] = vaddr[dmanum];
		dmanum++;
  		}
 	 }
  else 
  	{
  		while(dmanum<len)
		{
		buf[dmanum] = vaddr[0x200+dmanum];
		//printk("buf[%d] is %x\r\n",dmanum,buf[dmanum]);
		dmanum++;
  		}
  	}
//	dma_free_coherent(NULL, 530, vaddr, (dma_addr_t )vphyaddr);
}


/*
static void sep4020_nand_read_buf(struct mtd_info *mtd, u_char *buf, int len)
{
	//struct nand_chip *this = mtd->priv;

	//unsigned char * vaddr;
	//unsigned char * vphyaddr;
	//dma_addr_t * vaddr;
	//dma_addr_t vphyaddr;
	//vaddr = (unsigned char *) dma_alloc_coherent(NULL,530, &vphyaddr, GFP_KERNEL);
	//printk("vaddr is %x vphyaddr is %x \r\n",vaddr,vphyaddr);
	int read528=0;
	if (*((unsigned char*)EMI_NAND_COM_V) == 0x00)
		read528 = 528;
	else
		read528 =len; 
	
 int dmanum = 0;

	*(unsigned long*)DMAC_C0CONTROL_V  = ((read528>>2)<<14) + (1<<13) + (2<<9) +(2<<6) + (3<<3) + 3; 

	*(unsigned long*)DMAC_C0SRCADDR_V  =  EMI_NAND_DATA   ;
 	*(unsigned long*)DMAC_C0DESTADD_V  =   vphyaddr ;	

 	*(unsigned long*)DMAC_C0CONFIGURATION_V = 0x31d ; //打开DMA通道 
 	*(unsigned long*)EMI_NAND_COM_V		|= 0x80000000;	//使能EMI_NAND_COM
 	//ndelay(100000);
	while(1)
	 	{
		 	ndelay(100);
		 	if (*(unsigned long*)EMI_NAND_IDLE_V & (0x01) != 0)
	 		break;
		  }


  dmanum = 0;
	if (len ==512 ||(*((unsigned char*)EMI_NAND_COM_V) == 0x50))
	{
			while(dmanum<len)
		{
			buf[dmanum] = vaddr[dmanum];
			dmanum++;
  		}
  	}
  else 
  	{
  		while(dmanum<len)
		{
		buf[dmanum] = vaddr[512+dmanum];
		dmanum++;
  		}
 	 }

}
*/



// get the nand controller status
static int sep4020_nand_dev_ready(struct mtd_info *mtd)
{
	return 1;
	//return (*(volatile unsigned long*)(EMI_NAND_STA_V));
}


//nand_erase has its own cmd
static void sep4020_nand_cmd_erase(struct mtd_info *mtd, int page)
{
	*(unsigned long*)EMI_NAND_ADDR1_V = page;
	*(unsigned long*)EMI_NAND_COM_V = 0X80000060;
	while(!(*(unsigned long*)EMI_NAND_IDLE_V));
}



//sep4020 nand controller donot have this function
static void sep4020_nand_select_chip(struct mtd_info *mtd, int chip)
{

}





/*
 * Main initialization routine
 */
  static int __init sep4020_nand_init(void)
//static int  sep4020_nand_probe(struct platform_device *pdev)
 {
 	struct nand_chip *this;
 //	unsigned long zlm[50]={0};
 	
 	/* Allocate memory for MTD device structure and private data */
 	sep4020_mtd = kmalloc (sizeof(struct mtd_info) + sizeof (struct nand_chip),GFP_KERNEL);
	if (!sep4020_mtd) {
	printk ("Unable to allocate SEP4020 NAND MTD device structure.\n");
		return -ENOMEM;
	}			
//	printk("zlm nand come\r\n");	
	/* Get pointer to private data */
	this = (struct nand_chip *) (&sep4020_mtd[1]);

	/* Initialize structures */
	memset((char *) sep4020_mtd, 0, sizeof(struct mtd_info));
	memset((char *) this, 0, sizeof(struct nand_chip));
	
	/* Link the private data with the MTD structure */
	sep4020_mtd->priv = this;
	
	/*
	 * the dauflt value of GPIO Port F control register is configured
	 * to be outputs ,so we needn't to change it
	 * the fuction of the below codes are init the emi to configure the nandcontroller and
	 * open the wp# of nandflash 
	 */
	 
	 *(unsigned long*)INTC_IMR_V = 0XFFFFFFFF;
	 *(unsigned long*)INTC_IMR_V = 0X00000000;
	 
	 *(unsigned long*)EMI_NAND_CONF1_V = 0x06302857;
   *(unsigned long*)EMI_NAND_CONF2_V = 0x00514353;
  
   *(unsigned long*)GPIO_PORTG_SEL_V |= 0x0080;	
   *(unsigned long*)GPIO_PORTG_DATA_V |= 0x0080;
	 *(unsigned long*)GPIO_PORTG_DIR_V &= ~0x0080;
	 
	 vaddr = (unsigned char *) dma_alloc_coherent(NULL,530, (dma_addr_t *) &vphyaddr, GFP_KERNEL);
	
	int erasepage;
	//init, erase all the nand page
	
//	 erasepage=0;
/*
	while(erasepage<0x20000)
	{
		sep4020_nand_cmd_erase(sep4020_mtd,erasepage);
		erasepage+=0x20;
	}
 */
 
 //erase the mtdblock0
 /*
	 erasepage=0;
	while(erasepage<0x800)
	{
		sep4020_nand_cmd_erase(sep4020_mtd,erasepage);
		erasepage+=0x20;
	}
 */
 
 //erase the mtdblock1
 /*
	 erasepage=0x800;
	while(erasepage<0x3000)
	{
		sep4020_nand_cmd_erase(sep4020_mtd,erasepage);
		erasepage+=0x20;
	}
 */
 
 //erase the mtdblock2
 
	/* erasepage=3000;
	while(erasepage<0xf000)
	{
		sep4020_nand_cmd_erase(sep4020_mtd,erasepage);
		erasepage+=0x20;
	}
 */
 
 //erase the mtdblock3
 /*
	 erasepage=0xf000;
	while(erasepage<0x20000)
	{
		sep4020_nand_cmd_erase(sep4020_mtd,erasepage);
		erasepage+=0x20;
	}
 */
 
	/* Set address of NAND IO lines */
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
	this->eccmode = NAND_ECC_NONE;
	this->select_chip = sep4020_nand_select_chip;
//	this->dev_ready = sep4020_device_ready;
	this->cmdfunc = sep4020_nand_command;
	this->erase_cmd = sep4020_nand_cmd_erase;
	
	if (nand_scan(sep4020_mtd, 1)) {
		kfree (sep4020_mtd);
		return -ENXIO;
	}

  /* Register the partitions */
	add_mtd_partitions(sep4020_mtd, partition_info, NUM_PARTITIONS);
	//	add_mtd_device(sep4020_mtd);
	/* Return happy */
	return 0;
 }
 
 
/* 
 static int sep4020_nand_remove(struct platform_device *pdev)
{
	return 1;
}
 
 static struct platform_driver sep4020_nand_driver = {
	.probe		= sep4020_nand_probe,
	.remove		= sep4020_nand_remove,
	.driver		= {
		.name	= "sep4020_nand",
		.owner	= THIS_MODULE,
	},
};
 
 static int __init sep4020_nand_init(void)
{
	printk("sep4020 NAND Driver, (c) 2008 Southeast University\n");

	platform_driver_register(&sep4020_nand_driver);
	
	return platform_driver_register(&sep4020_nand_driver);
}
*/ 
 
module_init(sep4020_nand_init);

/*
 * Clean up routine
 */
 static void __exit sep4020_cleanup(void)
 {
 	struct nand_chip *this = (struct nand_chip *) &sep4020_mtd[1];
 	
 	/* Release resources, unregister device */
	nand_release (sep4020_mtd);

	/* Free internal data buffer */
	kfree (this->data_buf);

	/* Free the MTD device structure */
	kfree (sep4020_mtd);
	dma_free_coherent(NULL, 530, vaddr, (dma_addr_t )vphyaddr);
 }
module_exit(sep4020_cleanup);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Leeming Zhang <leeming1203@gmail.com>");
MODULE_DESCRIPTION("Board-sepcific for Nand flash on evb4020 board");
