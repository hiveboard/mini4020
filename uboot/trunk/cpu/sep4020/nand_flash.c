//#include <linux/kernel.h>
//#include <linux/init.h>
//#include <linux/slab.h>
//#include <linux/module.h>
//#include <linux/platform_device.h>
//#include <linux/mtd/partitions.h>
//#include <linux/dma-mapping.h>
#include <asm/io.h>
#include <asm/arch/hardware.h>
#include <common.h>
#include <malloc.h>
//#include <asm/sizes.h>
//#include <linux/delay.h>
#define NAND_MAX_CHIPS 		1
#include <nand.h>
#include <linux/mtd/mtd.h>
#include <linux/mtd/nand.h>

/*
 * MTD struct for SEP4020 board
 */
static struct mtd_info *sep4020_mtd = NULL;
unsigned char * vaddr;
unsigned char * vphyaddr;
	

/*
 *	hardware specific access to control-lines
 */
static void sep4020_hwcontrol(struct mtd_info *mtd, int cmd)
{	
	//printf("sep4020_hwcontrol cmd is 0x%x\n",cmd);	
	struct nand_chip *this = mtd->priv;
	
	switch (cmd) 
	{
	case NAND_CTL_SETNCE:
	case NAND_CTL_CLRNCE:
		//printf("%s: called for NCE\n", __FUNCTION__);
		break;

	case NAND_CTL_SETCLE:
		this->IO_ADDR_W = (void __iomem *) EMI_NAND_COM_RAW;
		break;

	case NAND_CTL_SETALE:
		this->IO_ADDR_W = (void __iomem *) EMI_NAND_ADDR1_RAW;
		break;
	
	case NAND_READ_ID:
		this->IO_ADDR_R = (void __iomem *) EMI_NAND_ID_RAW;
			
		break;
	
	case NAND_READ_STATUS:
		this->IO_ADDR_R = (void __iomem *) EMI_NAND_STA_RAW;
		break;
		
		/* NAND_CTL_CLRCLE: */
		/* NAND_CTL_CLRALE: */
	default:
		this->IO_ADDR_W = (void __iomem *) EMI_NAND_DATA_RAW;
		this->IO_ADDR_R = (void __iomem *) EMI_NAND_DATA_RAW;
		break;
	}

}

/**
 * nand_command - [DEFAULT] Send command to NAND device
 * @mtd:	MTD device structure
 * @command:	the command to be sent
 * @column:	the column address for this command, -1 if none
 * @page_addr:	the page address for this command, -1 if none
 * Send command to NAND device. This function is used for sep4020 nand
 * devices (256/512 Bytes per page)
 */

static void sep4020_nand_command (struct mtd_info *mtd, unsigned command, int column, int page_addr)
{
	//printf("sep4020_nand_command is 0x%x \n",command);
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
		} else if (column < 256) 
		{			
			readcmd = NAND_CMD_READ0;
		} else 
		{
			column -= 256;
			readcmd = NAND_CMD_READ1;
		}
		this->write_byte(mtd, readcmd);			
	}
	
	this->write_byte(mtd, command);
	//printk("after wire_byte is %x\r\n",EMI_NAND_COM);

	if  (command == NAND_CMD_READID)
	{
		EMI_NAND_COM		|= 0x80000000;	//使能EMI_NAND_COM
		this->hwcontrol(mtd, NAND_READ_ID);  
		return;
	}
	
	if  (command == NAND_CMD_STATUS)
	{
		EMI_NAND_COM		|= 0x80000000;	//使能EMI_NAND_COM
		this->hwcontrol(mtd, NAND_READ_STATUS);
	}
	
	if	(command == NAND_CMD_RESET)
	{
		EMI_NAND_COM		|= 0x80000000;
		this->hwcontrol(mtd, NAND_CTL_CLRALE);
	}
	/* Set ALE and clear CLE to start address cycle */
	//this->hwcontrol(mtd, NAND_CTL_CLRCLE);

	if (column != -1 || page_addr != -1) 
	{
		this->hwcontrol(mtd, NAND_CTL_SETALE);		
		EMI_NAND_ADDR1 = (page_addr<<8) ; 	
		/* Latch in address */
		this->hwcontrol(mtd, NAND_CTL_CLRALE);
	}
//udelay (100); //kyon
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
{	//printk("sep4020_nand_write_buf   len is %d\n",len);
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
 		EMI_NAND_COM 		= 	0x00;
 		DMAC_C0CONTROL  		= 	(132<<14) + (1<<13) + (2<<9) +(2<<6) + (3<<3) + 3; 
		DMAC_C0SRCADDR  		=  	EMI_NAND_DATA_RAW  ;
 		DMAC_C0DESTADD 		=   	vaddr;	
 		DMAC_C0CONFIGURATION 	= 	0x31d ; 
 		EMI_NAND_COM		|= 	0x80000000;	
 	
 		while(1)
 		{
 			ndelay(10);
 			if (EMI_NAND_IDLE & (0x01) != 0) break;
  		}
 	  
 		while(dmanum < len)//give the first 512 to the dma space
		{
			vaddr[512+dmanum] = buf[dmanum];
			dmanum++;
 		}
	}		
	// next is put the totally 528 bytes to the nand	
		EMI_NAND_COM = 0X80;
		DMAC_C0CONTROL  = (132<<14) + (1<<12) + (2<<9) +(2<<6) + (3<<3) + 3 ; 
		DMAC_C0SRCADDR  = vaddr ;
 		DMAC_C0DESTADD  = EMI_NAND_DATA_RAW  ;
 		DMAC_C0CONFIGURATION = 0x300b ;
 		EMI_NAND_COM		|= 0x80000000;	
 	
 	while(1)
 	{
 		ndelay(10);
 		if (EMI_NAND_IDLE & (0x01) != 0) break;
  	}	 
}

static void sep4020_nand_read_buf(struct mtd_info *mtd, u_char *buf, int len)
{
	//printk("sep4020_nand_read_buf   len is %d\n",len);
	int read528=0;
	if (*((volatile unsigned char*)EMI_NAND_COM_RAW) == 0x00)
		read528 = 528;
	else
		read528 =len; 
	
	DMAC_C0CONTROL  		= 	((read528>>2)<<14) + (1<<13) + (2<<9) +(2<<6) + (3<<3) + 3; 
	DMAC_C0SRCADDR  		=  	EMI_NAND_DATA_RAW  ;
 	DMAC_C0DESTADD  		=  	vaddr ;	
 	DMAC_C0CONFIGURATION 	= 	0x31d ; 
 	EMI_NAND_COM		|= 	0x80000000;	
 	while(1)
 	{
 		//ndelay(10);
 		if (EMI_NAND_IDLE & (0x01) != 0) break;
  	}

      unsigned int dmanum = 0;
	//u-boot need 528?
	if ( len == 528 || len == 512 ||(*((volatile unsigned char*)EMI_NAND_COM_RAW) == 0x50))
	{
		/*while(dmanum<len)
		{
		buf[dmanum] = vaddr[dmanum];
		dmanum++;
  		}*/
		memcpy(buf,vaddr,len);
 	 }
  	else 
  	{
  		while(dmanum<len)
		{
		buf[dmanum] = vaddr[512+dmanum];
		//printk("buf[%d] is %x\r\n",dmanum,buf[dmanum]);
		dmanum++;
  		}
	}
}


// get the nand controller status
static int sep4020_nand_dev_ready(struct mtd_info *mtd)
{
	return 1;
	//return (*(volatile unsigned long*)(EMI_NAND_STA_V));
}

//nand_erase has its own cmd
static void sep4020_nand_cmd_erase(struct mtd_info *mtd, int page)
{
	EMI_NAND_ADDR1 = page;
	EMI_NAND_COM = 0X80000060;
	while(!EMI_NAND_IDLE);
}

//sep4020 nand controller donot have this function
static void sep4020_nand_select_chip(struct mtd_info *mtd, int chip)
{
	//printf("sep4020_nand_select_chip!\n");
}

/*
 * Main initialization routine
 */
  //static int __init sep4020_nand_init(void) //kyon
int board_nand_init( struct nand_chip *chip )
 {
	//printf ("board nand init.\n"); 	
	//struct nand_chip *this;
 	
 	/* Allocate memory for MTD device structure and private data */
	
 	//sep4020_mtd = malloc (sizeof(struct mtd_info) + sizeof (struct nand_chip));
	//if (!sep4020_mtd) {
	//printf ("Unable to allocate SEP4020 NAND MTD device structure.\n");
	//	return -1;
	//}
				
	/* Get pointer to private data */
	//this = (struct nand_chip *) (&sep4020_mtd[1]);

	/* Initialize structures */
	//memset((char *) sep4020_mtd, 0, sizeof(struct mtd_info));
	memset((char *) chip, 0, sizeof(struct nand_chip));
	
	/* Link the private data with the MTD structure */
	//sep4020_mtd->priv = this;

	/*
	 * the dauflt value of GPIO Port F control register is configured
	 * to be outputs ,so we needn't to change it
	 * the fuction of the below codes are init the emi to configure the nandcontroller and
	 * open the wp# of nandflash 
	 */
	 //init hardware


	 INTC_IMR = 0XFFFFFFFF;
	 INTC_IMR = 0X00000000;
	 
	 EMI_NAND_CONF1 = 0x06302857;
   	 EMI_NAND_CONF2 = 0x00514353;
  
   	 GPIO_PORTG_SEL |= 0x0080;	
   	 GPIO_PORTG_DATA |= 0x0080;
	 GPIO_PORTG_DIR &= ~0x0080;


	 //vaddr = (unsigned char *) dma_alloc_coherent(NULL,530, (dma_addr_t *) &vphyaddr, GFP_KERNEL);//kyon
	vaddr = malloc(530);	
	memset(vaddr,0,530);
	
	int erasepage;

	/* Set address of NAND IO lines */
	chip->IO_ADDR_R = (void *) EMI_NAND_DATA_RAW;
	chip->IO_ADDR_W = (void *) EMI_NAND_DATA_RAW;
	/* Set address of hardware control function */
	chip->hwcontrol = sep4020_hwcontrol;
	/* 15 us command delay time */
	chip->dev_ready = sep4020_nand_dev_ready;
	chip->chip_delay = 15;
	chip->write_buf	= sep4020_nand_write_buf;
	chip->read_buf  = sep4020_nand_read_buf;
	chip->write_byte = sep4020_nand_write_byte;
	chip->read_byte =  sep4020_nand_read_byte;
	chip->eccmode = NAND_ECC_SOFT;
	chip->select_chip = sep4020_nand_select_chip;
//	chip->dev_ready = sep4020_device_ready;
	chip->cmdfunc = sep4020_nand_command;
	chip->erase_cmd = sep4020_nand_cmd_erase;

	/* Return happy */
	return 0;
 }


 
