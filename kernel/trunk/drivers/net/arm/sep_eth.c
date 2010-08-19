/*
 *  linux/drivers/net/arm/sep_eth.c
 *
 *  by fpmystar <fpmystar@gmail.com> 2008-2010.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * Derived from various things including skeleton.c
 
 *	Copyright (c) 2008 by PROCHIP Limited.
 *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
 *	material.  All manufacturing, reproduction, use, and sales rights 
 *	pertaining to this subject matter are governed by the license agreement.
 *	The recipient of this software implicitly accepts the terms of the license.
 *
 *	File Description: sep4020的网络驱动
 *
 *    Version         Date            Author           
 *------------------------------------------------------------------------------------    
 *                 0.0.1                              2008.9.25                            方攀          
 *   		    3.0.1				 2009.5.31                              方攀
 *
 */
  
 #include <linux/module.h>
 #include <linux/init.h>
 #include <linux/config.h>
 #include <linux/mii.h>
 #include <linux/netdevice.h>
 #include <linux/etherdevice.h>
 #include <linux/skbuff.h>
 #include <linux/dma-mapping.h>
 #include <linux/ethtool.h>
 #include <linux/platform_device.h>
 #include <linux/clk.h>
 #include <linux/delay.h>
 
 #include <asm/io.h>
 #include <asm/uaccess.h>
 #include <asm/mach-types.h>
 
 #include <asm/arch/hardware.h>
 #include <asm/arch/board.h>

 #include "sep_eth.h"
 
 #define DRV_NAME        "sep4020_ether"
 #define DRV_VERSION     "1.0"
 #define SEP_DEBUG		 5
 
 static struct net_device *sep_dev;
 static struct clk *ether_clk;
 extern struct platform_device sep4020_eth_device;
 static unsigned int Transmit_int_count;

 static void sepether_RUNOVER(struct sep_private *lp);
  /*
   * Read from a EMAC register.
   */
 static inline unsigned long sep_emac_read(unsigned int reg)
  {
	void __iomem *emac_base = (void __iomem *)reg;
 
	return __raw_readl(emac_base);
  }
 
  /*
 * Write to a EMAC register.
 */
 static inline void sep_emac_write(unsigned int reg, unsigned long value)
  {
	void __iomem *emac_base = (void __iomem *)reg;
 
	__raw_writel(value, emac_base);
  }
  
  /* ........................... PHY INTERFACE ........................... */
  
  /*         可以进行MAC配置操作
* Enable the MDIO bit in MAC control register
* When not called from an interrupt-handler, access to the PHY must be
*  protected by a spinlock.
  */
 static void enable_mdi(void)           //need think more
  {
	unsigned long ctl;
  
	ctl = sep_emac_read(MAC_CTRL_V);
	sep_emac_write(MAC_CTRL_V, ctl&(~0x3));    /* enable management port */
	return;
  }
  
 /*          不可以进行MAC配置操作
* Disable the MDIO bit in the MAC control register
 */
 static void disable_mdi(void)
  {
	unsigned long ctl;
  
	ctl = sep_emac_read(MAC_CTRL_V);
	sep_emac_write(MAC_CTRL_V, ctl|(0x3));    /* disable management port */
	return;
  }
 
  /*
 * Wait until the PHY operation is complete.
 */
 static inline void sep_phy_wait(void) 
  {
	unsigned long timeout = jiffies + 2;

	while ((sep_emac_read(MAC_MII_STATUS_V) & 0x2)) 
	{
		if (time_after(jiffies, timeout)) 
		{
			printk("sep_ether: MDIO timeout\n");
			break;
		}
		
		cpu_relax();
	}
	return;
 }
 
 /*
* Write value to the a PHY register
* Note: MDI interface is assumed to already have been enabled.
 */
 static void write_phy(unsigned char phy_addr, unsigned char address, unsigned int value)
  {
//	unsigned long mii_status;
	unsigned short mii_txdata;
//	unsigned long timeout = jiffies + 2;
	
	mii_txdata = value;
	sep_emac_write(MAC_MII_ADDRESS_V,(unsigned long)(address<<8) | phy_addr);
	sep_emac_write(MAC_MII_TXDATA_V ,mii_txdata);
	sep_emac_write(MAC_MII_CMD_V ,0x4);
	udelay(40);

	sep_phy_wait();
	return;	
 }
 
 /*
* Read value stored in a PHY register.
* Note: MDI interface is assumed to already have been enabled.
 */
 static void read_phy(unsigned char phy_addr, unsigned char address, unsigned int *value)
 {
	unsigned short mii_rxdata;
//	unsigned long mii_status;
	
	sep_emac_write(MAC_MII_ADDRESS_V,(unsigned long)(address<<8) | phy_addr);
	sep_emac_write(MAC_MII_CMD_V ,0x2);
	udelay(40);
	sep_phy_wait();
		
	mii_rxdata = sep_emac_read(MAC_MII_RXDATA_V);
	*value =  mii_rxdata;
	return;
 }
 
 /* ........................... PHY MANAGEMENT .......................... */
 
 /*
* Access the PHY to determine the current link speed and mode, and update the
* MAC accordingly.
* If no link or auto-negotiation is busy, then no changes are made.
 */
 static void update_linkspeed(struct net_device *dev)
 {
	struct sep_private *lp = (struct sep_private *) dev->priv;
	unsigned int bmsr, bmcr, lpa, mac_cfg;
	unsigned int speed, duplex;
	
	spin_lock_irq(&lp->lock);
	//enable_mdi();
	
	if (!mii_link_ok(&lp->mii))     /* no link */
	{       
		netif_carrier_off(dev);
		printk(KERN_INFO "%s: Link down.\n", dev->name);
		goto result;
	}
 
        /* Link up, or auto-negotiation still in progress */
	read_phy(lp->phy_address, MII_BMSR, &bmsr);
	read_phy(lp->phy_address, MII_BMCR, &bmcr);
	if (bmcr & BMCR_ANENABLE)                /* AutoNegotiation is enabled */
	{                         
		if (!(bmsr & BMSR_ANEGCOMPLETE))              /* Do nothing - another interrupt generated when negotiation complete */
			goto result;              

		read_phy(lp->phy_address, MII_LPA, &lpa);
		
		if ((lpa & LPA_100FULL) || (lpa & LPA_100HALF)) 
			speed = SPEED_100;
		else 
			speed = SPEED_10;
			
		if ((lpa & LPA_100FULL) || (lpa & LPA_10FULL)) 
			duplex = DUPLEX_FULL;
		else 
			duplex = DUPLEX_HALF;
	} 
	else 
	{
		speed = (bmcr & BMCR_SPEED100) ? SPEED_100 : SPEED_10;
		duplex = (bmcr & BMCR_FULLDPLX) ? DUPLEX_FULL : DUPLEX_HALF;
	 }
 
         /* Update the MAC */
	mac_cfg = sep_emac_read(MAC_CTRL_V);
	if (speed == SPEED_100) 
	{
		mac_cfg |= 0x800;			/* set speed 100 M */
		//bmcr &=(~0x2000);
		//write_phy(lp->phy_address, MII_BMCR, bmcr); //将dm9161的速度设为10M
		if (duplex == DUPLEX_FULL)              /* 100 Full Duplex */
			mac_cfg |= 0x400;
		else                                    /* 100 Half Duplex */
			mac_cfg &= (~0x400);
	} 
	else 
	{
		mac_cfg &= (~0x800);             /* set speed 10 M */
		
		if (duplex == DUPLEX_FULL)              /* 10 Full Duplex */
			mac_cfg |= 0x400;
		else                                /* 10 Half Duplex */
			mac_cfg &= (~0x400);
	}
	
	sep_emac_write(MAC_CTRL_V, mac_cfg);
        
	printk(KERN_INFO "%s: Link now %i M-%s\n", dev->name, speed, (duplex == DUPLEX_FULL) ? "FullDuplex" : "HalfDuplex");
	netif_carrier_on(dev);
	
result:
	//disable_mdi();
	spin_unlock_irq(&lp->lock);
	return;
 }
 
 /*
 * Handle interrupts from the PHY
 */
static irqreturn_t sepether_phy_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	struct net_device *dev = (struct net_device *) dev_id;
	struct sep_private *lp = (struct sep_private *) dev->priv;
	unsigned int phy,bmsr;


	/*
	 * This hander is triggered on both edges, but the PHY chips expect
	 * level-triggering.  We therefore have to check if the PHY actually has
	 * an IRQ pending.
	 */
	//enable_mdi();
	
	if ((lp->phy_type == MII_DM9161_ID) || (lp->phy_type == MII_DM9161A_ID)) 
	{
		read_phy(lp->phy_address, MII_DSINTR_REG, &phy);	// ack interrupt in Davicom PHY 
		if (!(phy & (1 << 0)))
			goto done;
	}
	else if (lp->phy_type == MII_LXT971A_ID) 
	{
		read_phy(lp->phy_address, MII_ISINTS_REG, &phy);	// ack interrupt in Intel PHY 
		if (!(phy & (1 << 2)))
			goto done;
	}
	else if (lp->phy_type == MII_BCM5221_ID) 
	{
		read_phy(lp->phy_address, MII_BCMINTR_REG, &phy);	// ack interrupt in Broadcom PHY 
		if (!(phy & (1 << 0)))
			goto done;
	}
	else if (lp->phy_type == MII_KS8721_ID) 
	{
		read_phy(lp->phy_address, MII_TPISTATUS, &phy);		// ack interrupt in Micrel PHY 
		if (!(phy & ((1 << 2) | 1)))
			goto done;
	}
	read_phy(lp->phy_address, MII_BMSR, &bmsr);
	
	update_linkspeed(dev);

done:
	//disable_mdi();
	
	*(volatile unsigned long*)GPIO_PORTA_INTRCLR_V |= 0x0080;    //清除中断
	*(volatile unsigned long*)GPIO_PORTA_INTRCLR_V = 0x0000;          //清除中断
	return IRQ_HANDLED;
}

/*
 * Initialize and enable the PHY interrupt for link-state changes
 */
static void enable_phyirq(struct net_device *dev)
{
	struct sep_private *lp = (struct sep_private *) dev->priv;
	unsigned int dsintr, irq_number;
	int status;
	
	if (lp->phy_type == MII_RTL8201_ID)	/* RTL8201 does not have an interrupt */
		return;
	if (lp->phy_type == MII_DP83847_ID)	/* DP83847 does not have an interrupt */
		return;
	if (lp->phy_type == MII_AC101L_ID)	/* AC101L interrupt not supported yet */
		return;

	irq_number = lp->board_data.phy_irq_pin;
	
	*(volatile unsigned long*)GPIO_PORTA_DIR_V |= 0x0080 ;          //1 stands for in
	*(volatile unsigned long*)GPIO_PORTA_SEL_V |= 0x0080 ;       //for common use
	*(volatile unsigned long*)GPIO_PORTA_INCTL_V |= 0x0080;      //中断输入方式
	*(volatile unsigned long*)GPIO_PORTA_INTRCTL_V |= (0x3UL<<14);    //中断类型为低电平解发
	*(volatile unsigned long*)GPIO_PORTA_INTRCLR_V |= 0x0080;    //清除中断
	*(volatile unsigned long*)GPIO_PORTA_INTRCLR_V = 0x0000;          //清除中断
        
	status = request_irq(irq_number, sepether_phy_interrupt, 0, dev->name, dev);
	
	
	if (status) 
	{
		printk(KERN_ERR "sep_ether: PHY IRQ %d request failed - status %d!\n", irq_number, status);
		return;
	}
	INT_ENABLE(irq_number);                                 //enable phy irq
	spin_lock_irq(&lp->lock);

	if ((lp->phy_type == MII_DM9161_ID) || (lp->phy_type == MII_DM9161A_ID)) /* for Davicom PHY */
	{	
		read_phy(lp->phy_address, MII_DSINTR_REG, &dsintr);
		dsintr = dsintr & ~0xf00;		/* clear bits 8..11 */
		write_phy(lp->phy_address, MII_DSINTR_REG, dsintr);
	}
	else if (lp->phy_type == MII_LXT971A_ID) /* for Intel PHY */
	{	
		read_phy(lp->phy_address, MII_ISINTE_REG, &dsintr);
		dsintr = dsintr | 0xf2;			/* set bits 1, 4..7 */
		write_phy(lp->phy_address, MII_ISINTE_REG, dsintr);
	}
	else if (lp->phy_type == MII_BCM5221_ID) /* for Broadcom PHY */
	{	
		dsintr = (1 << 15) | ( 1 << 14);
		write_phy(lp->phy_address, MII_BCMINTR_REG, dsintr);
	}
	else if (lp->phy_type == MII_KS8721_ID) 	/* for Micrel PHY */
	{
		dsintr = (1 << 10) | ( 1 << 8);
		write_phy(lp->phy_address, MII_TPISTATUS, dsintr);
	}

	spin_unlock_irq(&lp->lock);
	return;
}

/*
 * Disable the PHY interrupt
 */
static void disable_phyirq(struct net_device *dev)
{
	struct sep_private *lp = (struct sep_private *) dev->priv;
	unsigned int dsintr;
	unsigned int irq_number;

	if (lp->phy_type == MII_RTL8201_ID) 	/* RTL8201 does not have an interrupt */
		return;
	if (lp->phy_type == MII_DP83847_ID)	/* DP83847 does not have an interrupt */
		return;
	if (lp->phy_type == MII_AC101L_ID)	/* AC101L interrupt not supported yet */
		return;

	spin_lock_irq(&lp->lock);

	if ((lp->phy_type == MII_DM9161_ID) || (lp->phy_type == MII_DM9161A_ID)) 
	{	/* for Davicom PHY */
		read_phy(lp->phy_address, MII_DSINTR_REG, &dsintr);
		dsintr = dsintr | 0xf00;			/* set bits 8..11 */
		write_phy(lp->phy_address, MII_DSINTR_REG, dsintr);
	}
	else if (lp->phy_type == MII_LXT971A_ID) 
	{	/* for Intel PHY */
		read_phy(lp->phy_address, MII_ISINTE_REG, &dsintr);
		dsintr = dsintr & ~0xf2;			/* clear bits 1, 4..7 */
		write_phy(lp->phy_address, MII_ISINTE_REG, dsintr);
	}
	else if (lp->phy_type == MII_BCM5221_ID) 
	{	/* for Broadcom PHY */
		read_phy(lp->phy_address, MII_BCMINTR_REG, &dsintr);
		dsintr = ~(1 << 14);
		write_phy(lp->phy_address, MII_BCMINTR_REG, dsintr);
	}
	else if (lp->phy_type == MII_KS8721_ID) 
	{	/* for Micrel PHY */
		read_phy(lp->phy_address, MII_TPISTATUS, &dsintr);
		dsintr = ~((1 << 10) | (1 << 8));
		write_phy(lp->phy_address, MII_TPISTATUS, dsintr);
	}

	spin_unlock_irq(&lp->lock);
	
	*(volatile unsigned long*)GPIO_PORTA_DIR_V |= 0x0080 ;          //1 stands for in
	*(volatile unsigned long*)GPIO_PORTA_SEL_V |= 0x0080 ;       //for common use
	*(volatile unsigned long*)GPIO_PORTA_INCTL_V &= (~0x0080);      //中断输入方式
	irq_number = lp->board_data.phy_irq_pin;
	INT_DISABLE(irq_number);                               //disable phy irq
	free_irq(irq_number, dev);			/* Free interrupt handler */
	
	return;
}

/*
 * Perform a software reset of the PHY.
 */
#if 0
static void reset_phy(struct net_device *dev)
{
	struct sep_private *lp = (struct sep_private *) dev->priv;
	unsigned int bmcr;
 
// printk("reset_phy!\n\r");
	spin_lock_irq(&lp->lock);
	enable_mdi();

	/* Perform PHY reset */
	write_phy(lp->phy_address, MII_BMCR, BMCR_RESET);

	/* Wait until PHY reset is complete */
	do {
		read_phy(lp->phy_address, MII_BMCR, &bmcr);
	} while (!(bmcr && BMCR_RESET));

	disable_mdi();
	spin_unlock_irq(&lp->lock);
}
#endif

/* ......................... ADDRESS MANAGEMENT ........................ */

/*
 * NOTE: Your bootloader must always set the MAC address correctly before
 * booting into Linux.
 *
 * - It must always set the MAC address after reset, even if it doesn't
 *   happen to access the Ethernet while it's booting. 
 *
 * - Likewise it must store the addresses in the correct byte order.
 *   MicroMonitor (uMon) on the CSB337 and SEP4020 does this incorrectly (and
 *   continues to do so, for bug-compatibility).
 */

static short __init unpack_mac_address(struct net_device *dev, unsigned int hi, unsigned int lo)
{
	char addr[6];
	unsigned long hms,timer_value;
 // printk("unpack_mac_address!\n\r");
 	hms = readl(RTC_STA_HMS_V);
 	timer_value = readl(TIMER_T1CCR_V);
	if (machine_is_csb337()||machine_is_sep4020()) 
	{  
		//addr[5] = timer_value & 0xff;        //产生一个随即的MAC地址，防止MAC地址重复
		//addr[4] = (timer_value>>8) & 0xff;
		//addr[3] = hms & 0xff;
		//addr[2] = (hms>>8) & 0xff;
		addr[5] = (lo & 0xff);			/* The CSB337 bootloader stores the MAC the wrong-way around */
		addr[4] = (lo & 0xff00) >> 8;
		addr[3] = (lo & 0xff0000) >> 16;
		addr[2] = (lo & 0xff000000) >> 24;
		addr[1] = (hi & 0xff);
		addr[0] = (hi & 0xff00) >> 8;
	}
	else 
	{
		addr[0] = (lo & 0xff);
		addr[1] = (lo & 0xff00) >> 8;
		addr[2] = (lo & 0xff0000) >> 16;
		addr[3] = (lo & 0xff000000) >> 24;
		addr[4] = (hi & 0xff);
		addr[5] = (hi & 0xff00) >> 8;
	}

	if (is_valid_ether_addr(addr)) 
	{
		memcpy(dev->dev_addr, &addr, 6);
		return 1;
	}
	return 0;
}

/*
 * Set the ethernet MAC address in dev->dev_addr
 */
static void __init get_mac_address(struct net_device *dev)
{
	/* Check Specific-Address 1 */
	if (unpack_mac_address(dev, sep_emac_read(MAC_ADDR1_V), sep_emac_read(MAC_ADDR0_V)))
		return;

	printk(KERN_ERR "sep_ether: Your bootloader did not configure a MAC address.\n");
}

/*
 * Program the hardware MAC address from dev->dev_addr.
 */
static void update_mac_address(struct net_device *dev)
{
//	printk("update_mac_address!\n\r");
	sep_emac_write(MAC_ADDR0_V, (dev->dev_addr[2] << 24) | (dev->dev_addr[3] << 16) | (dev->dev_addr[4] << 8) | (dev->dev_addr[5]));
	sep_emac_write(MAC_ADDR1_V, (dev->dev_addr[0] << 8) | (dev->dev_addr[1]));	return;
}

/*
 * Store the new hardware address in dev->dev_addr, and update the MAC.
 */
static int set_mac_address(struct net_device *dev, void* addr)
{
	struct sockaddr *address = addr;

	if (!is_valid_ether_addr(address->sa_data))
		return -EADDRNOTAVAIL;

	memcpy(dev->dev_addr, address->sa_data, dev->addr_len);
	update_mac_address(dev);

	printk("%s: Setting MAC address to %02x:%02x:%02x:%02x:%02x:%02x\n", dev->name,
		dev->dev_addr[0], dev->dev_addr[1], dev->dev_addr[2],
		dev->dev_addr[3], dev->dev_addr[4], dev->dev_addr[5]);

	return 0;
}

static int inline hash_bit_value(int bitnr, __u8 *addr)
{
	if (addr[bitnr / 8] & (1 << (bitnr % 8)))
		return 1;
	return 0;
}

/*
 * The hash address register is 64 bits long and takes up two locations in the memory map.
 * The least significant bits are stored in EMAC_HSL and the most significant
 * bits in EMAC_HSH.
 *
 * The unicast hash enable and the multicast hash enable bits in the network configuration
 *  register enable the reception of hash matched frames. The destination address is
 *  reduced to a 6 bit index into the 64 bit hash register using the following hash function.
 * The hash function is an exclusive or of every sixth bit of the destination address.
 *   hash_index[5] = da[5] ^ da[11] ^ da[17] ^ da[23] ^ da[29] ^ da[35] ^ da[41] ^ da[47]
 *   hash_index[4] = da[4] ^ da[10] ^ da[16] ^ da[22] ^ da[28] ^ da[34] ^ da[40] ^ da[46]
 *   hash_index[3] = da[3] ^ da[09] ^ da[15] ^ da[21] ^ da[27] ^ da[33] ^ da[39] ^ da[45]
 *   hash_index[2] = da[2] ^ da[08] ^ da[14] ^ da[20] ^ da[26] ^ da[32] ^ da[38] ^ da[44]
 *   hash_index[1] = da[1] ^ da[07] ^ da[13] ^ da[19] ^ da[25] ^ da[31] ^ da[37] ^ da[43]
 *   hash_index[0] = da[0] ^ da[06] ^ da[12] ^ da[18] ^ da[24] ^ da[30] ^ da[36] ^ da[42]
 * da[0] represents the least significant bit of the first byte received, that is, the multicast/
 *  unicast indicator, and da[47] represents the most significant bit of the last byte
 *  received.
 * If the hash index points to a bit that is set in the hash register then the frame will be
 *  matched according to whether the frame is multicast or unicast.
 * A multicast match will be signalled if the multicast hash enable bit is set, da[0] is 1 and
 *  the hash index points to a bit set in the hash register.
 * A unicast match will be signalled if the unicast hash enable bit is set, da[0] is 0 and the
 *  hash index points to a bit set in the hash register.
 * To receive all multicast frames, the hash register should be set with all ones and the
 *  multicast hash enable bit should be set in the network configuration register.
 */

/*
 * Return the hash index value for the specified address.
 */
static int hash_get_index(__u8 *addr)
{
	int i, j, bitval;
	int hash_index = 0;

	for (j = 0; j < 6; j++) 
	{
		for (i = 0, bitval = 0; i < 8; i++)
			bitval ^= hash_bit_value(i*6 + j, addr);

		hash_index |= (bitval << j);
	}

        return hash_index;
}

/*
 * Add multicast addresses to the internal multicast-hash table.
 */
static void sepether_sethashtable(struct net_device *dev)
{
	struct dev_mc_list *curr;
	unsigned long mc_filter[2];
	unsigned int i, bitnr;
	mc_filter[0] = mc_filter[1] = 0;

	curr = dev->mc_list;
	for (i = 0; i < dev->mc_count; i++, curr = curr->next) 
	{
		if (!curr) break;	/* unexpected end of list */

		bitnr = hash_get_index(curr->dmi_addr);
		//printk("bitnr is 0x%x\n",bitnr);
		mc_filter[bitnr >> 5] |= 1 << (bitnr & 31);
	}

	sep_emac_write(MAC_HASH0_V, mc_filter[1]);
	sep_emac_write(MAC_HASH1_V, mc_filter[0]);
	
}

/*
 * Enable/Disable promiscuous and multicast modes.
 */
static void sepether_set_rx_mode(struct net_device *dev)
{
	unsigned long cfg;
	
	//enable_mdi();

	cfg = sep_emac_read(MAC_CTRL_V);

	if (dev->flags & IFF_PROMISC)			/* Enable promiscuous mode */
		cfg |= 0x20;
 	else if (dev->flags & (~IFF_PROMISC))		/* Disable promiscuous mode */
		cfg &= ~0x20;

	if (dev->flags & IFF_ALLMULTI) 
	{		/* Enable all multicast mode */
		sep_emac_write(MAC_HASH1_V, 0xffffffff);
		sep_emac_write(MAC_HASH0_V, 0xffffffff);
		cfg |= 0x10;
	} 
	else if (dev->mc_count > 0) 
	{			/* Enable specific multicasts */
		sepether_sethashtable(dev);
		cfg |= 0x10;
		//cfg |= 0x20;
	} 
	else if (dev->flags & (~IFF_ALLMULTI)) 
	{	/* Disable all multicast mode */
		sep_emac_write(MAC_HASH1_V, 0);
		sep_emac_write(MAC_HASH0_V, 0);
		cfg &= ~0x10;
	}

	sep_emac_write(MAC_CTRL_V, cfg);
	//printk("MAC_CTRL_V 0x%x\n",sep_emac_read(MAC_CTRL_V));
	//disable_mdi();
	return;
}


/* ......................... ETHTOOL SUPPORT ........................... */

static int mdio_read(struct net_device *dev, int phy_id, int location)
{
	unsigned int value;

	read_phy(phy_id, location, &value);
	
	return value;
}

static void mdio_write(struct net_device *dev, int phy_id, int location, int value)
{
	write_phy(phy_id, location, value);
	return;
}

static int sepether_get_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	struct sep_private *lp = (struct sep_private *) dev->priv;
	int ret;
//printk("sepether_get_settings!\n\r");
	spin_lock_irq(&lp->lock);
	ret = mii_ethtool_gset(&lp->mii, cmd);

	spin_unlock_irq(&lp->lock);

	if (lp->phy_media == PORT_FIBRE) 	/* override media type since mii.c doesn't know */
	{	
		cmd->supported = SUPPORTED_FIBRE;
		cmd->port = PORT_FIBRE;
	}

	return ret;
}

static int sepether_set_settings(struct net_device *dev, struct ethtool_cmd *cmd)
{
	struct sep_private *lp = (struct sep_private *) dev->priv;
	int ret;
 printk("sepether_set_settings!\n\r");
	spin_lock_irq(&lp->lock);

	ret = mii_ethtool_sset(&lp->mii, cmd);

	spin_unlock_irq(&lp->lock);

	return ret;
}

static int sepether_nwayreset(struct net_device *dev)
{
	struct sep_private *lp = (struct sep_private *) dev->priv;
	int ret;
  printk("sepether_nwayreset!\n\r");
	spin_lock_irq(&lp->lock);
	//enable_mdi();

	ret = mii_nway_restart(&lp->mii);

	//disable_mdi();
	spin_unlock_irq(&lp->lock);

	return ret;
}

static void sepether_get_drvinfo(struct net_device *dev, struct ethtool_drvinfo *info)
{
	// printk("sepether_get_drvinfo!\n\r");
	strlcpy(info->driver, DRV_NAME, sizeof(info->driver));
	strlcpy(info->version, DRV_VERSION, sizeof(info->version));
	strlcpy(info->bus_info, dev->class_dev.dev->bus_id, sizeof(info->bus_info));
}

static struct ethtool_ops sepether_ethtool_ops = {
	.get_settings	= sepether_get_settings,
	.set_settings	= sepether_set_settings,
	.get_drvinfo		= sepether_get_drvinfo,
	.nway_reset		= sepether_nwayreset,
	.get_link		= ethtool_op_get_link,
};


/* ................................ MAC ................................ */

/*
 * Initialize and start the Receiver and Transmit subsystems
 */
static void sepether_start(struct net_device *dev)
{
	struct sep_private *lp = (struct sep_private *) dev->priv;
	int i;
	unsigned int tempaddr;
		 
    	sep_emac_write(MAC_TXBD_NUM_V,MAX_TX_DESCR);
   
    	lp->skb = NULL;
	 //初始化发送和接收描述符
	for (i = 0; i < MAX_TX_DESCR; i++)
	{
		tempaddr=(MAC_TX_BD_V+i*8);
        sep_emac_write(tempaddr,0);
        tempaddr=(MAC_TX_BD_V+i*8+4);
        sep_emac_write(tempaddr,0);
	}
	for (i = 0; i < MAX_RX_DESCR; i++)
	{
        tempaddr=(MAC_TX_BD_V + MAX_TX_DESCR*8+i*8);
        sep_emac_write(tempaddr,0);
        tempaddr=(MAC_TX_BD_V + MAX_TX_DESCR*8+i*8+4);
        sep_emac_write(tempaddr,0);
	}
    
	for (i = 0; i < MAX_RX_DESCR; i++) 
	{
		tempaddr=(MAC_TX_BD_V + MAX_TX_DESCR*8+i*8);
        sep_emac_write(tempaddr,0xc000);
        tempaddr=(MAC_TX_BD_V + MAX_TX_DESCR*8+i*8+4);
		sep_emac_write(tempaddr,ESRAM_BASE+ MAX_TX_DESCR*0x600+i*0x600);
	}

	/* Set the Wrap bit on the last descriptor */
	tempaddr=(MAC_TX_BD_V + MAX_TX_DESCR*8+i*8-8);
	sep_emac_write(tempaddr,0xe000);
	/* Reset buffer index */
	lp->rxBuffIndex = 0;     

	for (i = 0; i < MAX_TX_DESCR; i++) 
	{
        tempaddr=(MAC_TX_BD_V+i*8);
        sep_emac_write(tempaddr,0x0);
        tempaddr=(MAC_TX_BD_V+i*8+4);
        sep_emac_write(tempaddr,ESRAM_BASE+i*0x600);
	}
    /* Set the Wrap bit on the last descriptor */
	/* Reset buffer index */
	lp->txBuffIndex = 0;   
	
	return;
}
/*
 * Open the ethernet interface
 */
static int sepether_open(struct net_device *dev)
{
	struct sep_private *lp = (struct sep_private *) dev->priv;

	if (!is_valid_ether_addr(dev->dev_addr))
		return -EADDRNOTAVAIL;
		
	enable_mdi();
  	INT_DISABLE(28);
 	
  	sep_emac_write(MAC_INTMASK_V,0x0);  //首先屏蔽中断
  
	clk_enable(ether_clk);		/* Re-enable Peripheral clock */


	/* Update the MAC address (incase user has changed it) */
	//update_mac_address(dev);
 	sepether_start(dev);

	/* Enable PHY interrupt */
	enable_phyirq(dev);

	/* Determine current link speed */
	spin_lock_irq(&lp->lock);

	update_linkspeed(dev);
	
	spin_unlock_irq(&lp->lock);
	
	netif_start_queue(dev);
	/************************************************************************************/
	/* Enable MAC interrupts */
	sep_emac_write(MAC_INTMASK_V,0xff);  //open中断
  	sep_emac_write(MAC_INTSRC_V,0xff);   //clear all mac irq
	INT_ENABLE(28);
	disable_mdi();
	
	printk("SEP4020 ethernet interface open!\n\r");
	return 0;
}

/*
 * Close the interface
 */
static int sepether_close(struct net_device *dev)
{
	printk("SEP4020 ethernet interface close!\n\r");
	
	/* Disable Receiver and Transmitter */
	disable_mdi();
	//INT_ENABLE(28);
	/* Disable PHY interrupt */
	disable_phyirq(dev);
	
	/* Disable MAC interrupts */
	sep_emac_write(MAC_INTMASK_V,0);  //屏蔽中断

	netif_stop_queue(dev);
	
	INT_DISABLE(28);
	
	clk_disable(ether_clk);			/* Disable Peripheral clock */

	return 0;
}

/*
 * Transmit packet.
 */
static int sepether_tx(struct sk_buff *skb, struct net_device *dev)
{
	struct sep_private *lp = (struct sep_private *) dev->priv;
	unsigned long  leng;	  /* The length of a packet*/
	unsigned int  i;
	unsigned long  Adress;
	unsigned long  Temp_Tx_BD;
	unsigned long  temp_val;
	unsigned long count;
	
	if(lp->skb == NULL)
	{
		spin_lock_irq(&lp->lock);
		for(i = lp->txBuffIndex; i < MAX_TX_DESCR;)
		{
			Adress = MAC_TX_BD_V+ i*8;
			Temp_Tx_BD = sep_emac_read(Adress);
			if(!(Temp_Tx_BD & 0x8000))
			{
				if(i == (MAX_TX_DESCR-1)) 
					lp->txBuffIndex = 0;
				else 
					lp->txBuffIndex = i+1;
				break;
			 }
			if(i == MAX_TX_DESCR-1) 
				i = 0;
			else 
				i++;
		}
		netif_stop_queue(dev);
		count = Transmit_int_count;
		
		/* Store packet information (to free when Tx completed) */
		lp->skb = skb;
		lp->skb_length = skb->len;		
		lp->stats.tx_bytes += skb->len;
		lp->stats.tx_packets +=1;
		
		temp_val = ESRAM_BASE_V + i*0x600;
		memcpy((unsigned char*)temp_val,skb->data,skb->len);
	
		/* Set length of the packet in the Transmit Control register */
		leng = lp->skb_length;
		leng <<=16;
		if(i == MAX_TX_DESCR - 1)
			leng |= 0xb800;
		else 
			leng |= 0x9800;	
			
		spin_unlock_irq(&lp->lock);
		Adress = (MAC_TX_BD_V + i*8);
		sep_emac_write(Adress,leng);
		dev->trans_start = jiffies;
		
		while( count == Transmit_int_count)
		{
			temp_val = sep_emac_read(Adress);
			if(!(temp_val & 0x8000)) 
			{
				//printk("free tx skb 0x%x in sep_tx!!\n",lp->txBuffIndex);	
				dev_kfree_skb(lp->skb);
				lp->skb = NULL;
				netif_wake_queue(dev);
				break;
			}
		}
	
	}

	return 0;
}

/*
 * Update the current statistics from the internal statistics registers.
 */
static struct net_device_stats *sepether_stats(struct net_device *dev)
{
	struct sep_private *lp = (struct sep_private *) dev->priv;

	return &lp->stats;
}


/*
 * Extract received frame from buffer descriptors and sent to upper layers.
 * (Called from interrupt context)
 */
static void sepether_rx(struct net_device *dev)
{
	struct sep_private *lp = (struct sep_private *) dev->priv;
	struct sk_buff *skb;
	unsigned char *p_recv;
	unsigned int pktlen;
	unsigned int Temp_Rx_BD,Adress;
	unsigned int i;
	unsigned int length;
	unsigned char change_flage;  //has receive data;
	 
	change_flage =0;
	
	i = lp->rxBuffIndex;
	
	while (1) 
	{
		
		Adress = MAC_TX_BD_V + MAX_TX_DESCR*8+ i*8;
		Temp_Rx_BD = sep_emac_read(Adress);
	
		if(!(Temp_Rx_BD & 0x8000))
		{
		//printk("the lp->rxBuffIndex 0x%x\n",lp->rxBuffIndex);
			lp->rxBuffIndex = i;
			/*获得接收数据长度*/
			length = Temp_Rx_BD;
			length = length>>16;
			
			/*重置相应的RX_BD用于接收数据*/
			change_flage =1;		
		
			p_recv = (unsigned char *)(ESRAM_BASE_V + MAX_TX_DESCR*0x600+ i*0x600);
		
			pktlen = length;
		
			skb = alloc_skb(pktlen + 2, GFP_ATOMIC); //alloc a skb for the receive frame
			if (skb != NULL) 
			{
				skb_reserve(skb, 2);		
				memcpy(skb_put(skb, pktlen), p_recv, pktlen);
	 
				skb->dev = dev;
				skb->protocol = eth_type_trans(skb, dev);
				skb->len = pktlen;
				netif_rx(skb);
				dev->last_rx = jiffies;
				lp->stats.rx_bytes += pktlen;
				lp->stats.rx_packets +=1;
			}
			else 
			{
				lp->stats.rx_dropped += 1;
				printk(KERN_NOTICE "%s: Memory squeeze, dropping packet.\n", dev->name);
			}
       		if(i == (MAX_RX_DESCR-1))
					sep_emac_write(Adress, 0xe000);
			else 
					sep_emac_write(Adress, 0xc000);
	
			if (lp->rxBuffIndex == MAX_RX_DESCR-1)				/* wrap after last buffer */
				lp->rxBuffIndex = 0;
			else
				lp->rxBuffIndex++;
				
			i = lp->rxBuffIndex;
		}
		else 
		{	
			if (i == MAX_RX_DESCR-1)				/* wrap after last buffer */
				i = 0;
			else
				i++;
				
			if(i == lp->rxBuffIndex)
				break;
		
		}
	}
	return;
}

static void sepether_timeout(struct net_device *dev)
{
	struct sep_private *lp = (struct sep_private *) dev->priv;
	printk(KERN_ERR "%s: transmit timed out, resetting\n",dev->name);
	
	/* Try to restart the adaptor. */
	/* FIXME: Give it a real kick here */
	 //sepether_close(dev);
	//sepether_open(dev);
	lp->stats.tx_errors++;
	lp->stats.tx_dropped++;
	dev->trans_start = jiffies;
	netif_wake_queue(dev);
}

static void sepether_RUNOVER(struct sep_private *lp)
{
	unsigned int tempaddr,Adress,i;
	tempaddr = sep_emac_read(MAC_TXBD_NUM_V);	
	Adress = sep_emac_read(MAC_FLOWCTRL_V);
	 
	printk("the TXD descriptor NUM is 0x%x,THE MAC_FLOWCTRL_V IS 0X%x\n",tempaddr,Adress);
	for (i = 0; i < MAX_RX_DESCR; i++) 
	{
		Adress = MAC_TX_BD_V + MAX_TX_DESCR*8 + i*8;
		tempaddr = sep_emac_read(Adress);	
		printk("the %d receive descriptor is 0x%x\n",i,tempaddr);
	}	
	return;
}

/*
 * MAC interrupt handler
 */
static irqreturn_t sepether_interrupt(int irq, void *dev_id, struct pt_regs *regs)
{
	struct net_device *dev = (struct net_device *) dev_id;
	struct sep_private *lp = (struct sep_private *) dev->priv;
	unsigned long intstatus;
	unsigned long Adress;
////////////////////////
	spin_lock(&lp->lock);	INT_DISABLE(28);
	/* MAC Interrupt Status register indicates what interrupts are pending.
	   It is automatically cleared once read. */
	intstatus = sep_emac_read(MAC_INTSRC_V);
//while(intstatus)
//{
	sep_emac_write(MAC_INTSRC_V,intstatus);
	
	
	if (intstatus & 0x04)		/* Receive complete */
		sepether_rx(dev);
		
	if (intstatus & 0x08)		/* Receive error */
	{
		printk("%s: receive error\n", dev->name);
		lp->stats.rx_errors++;
	}
	
	if (intstatus & 0x03) /* Transmit complete */
	{		
		/* The TCOM bit is set even if the transmission failed. */
		if (intstatus & 0x02)
			lp->stats.tx_errors += 1;

		if (lp->skb) 
		{
			if(lp->txBuffIndex == 0) 
				Adress = (MAC_TX_BD_V +(MAX_TX_DESCR-2)*8);
			else if(lp->txBuffIndex == 1) 
				Adress = (MAC_TX_BD_V +(MAX_TX_DESCR-1)*8);
			else 
				Adress = (MAC_TX_BD_V + lp->txBuffIndex*8-16);
			//printk("free tx skb 0x%x in inter!!\n",lp->txBuffIndex);	
			sep_emac_write(Adress,0x0);
			
			dev_kfree_skb (lp->skb);
			lp->skb = NULL;	
			Transmit_int_count++;	
		}
		//printk("lp->skb is null\n");
		netif_wake_queue(dev);
	}

	if (intstatus & 0x10)
	{
		printk("%s: ROVR error\n", dev->name);                  //FP 修改于2009.3.5
		sepether_RUNOVER(lp);
	}
  
	while(intstatus)
	{
		sep_emac_write(MAC_INTSRC_V,intstatus);
		intstatus = sep_emac_read(MAC_INTSRC_V);
	}
	//}
	INT_ENABLE(28);
	spin_unlock(&lp->lock);
	return IRQ_HANDLED;
}

/*
 * Initialize the ethernet interface
 */
static int __init sepether_setup(unsigned long phy_type, unsigned short phy_address, struct platform_device *pdev)
{
	struct sep_eth_data *board_data = pdev->dev.platform_data;
	struct net_device *dev;
	struct sep_private *lp;
	unsigned int val;
	int res;

	if (sep_dev)			/* already initialized */
		return 0;

	dev = alloc_etherdev(sizeof(struct sep_private));
	if (!dev)
		return -ENOMEM;

	dev->base_addr = MAC_BASE_V;
	dev->irq = INTSRC_MAC;
	SET_MODULE_OWNER(dev);

	/* Install the interrupt handler */
	if (request_irq(dev->irq, sepether_interrupt, 0, dev->name, dev)) 
	{
		free_netdev(dev);
		return -EBUSY;
	}

	/* Allocate memory for DMA Receive descriptors */
	lp = (struct sep_private *)dev->priv;
	lp->board_data = *board_data;
	platform_set_drvdata(pdev, dev);

	spin_lock_init(&lp->lock);

	ether_setup(dev);
	dev->open 				= sepether_open;
	dev->stop 				= sepether_close;
	dev->hard_start_xmit 	= sepether_tx;
	dev->get_stats 			= sepether_stats;
	dev->set_multicast_list	= sepether_set_rx_mode;
	dev->set_mac_address 	= set_mac_address;
	dev->ethtool_ops 		= &sepether_ethtool_ops;
	dev->tx_timeout			= sepether_timeout;
	dev->watchdog_timeo		= (400 * HZ) / 1000;
	
	SET_NETDEV_DEV(dev, &pdev->dev);
	spin_lock_irq(&lp->lock);
	enable_mdi();
	
	get_mac_address(dev);		/* Get ethernet address and store it in dev->dev_addr */
	update_mac_address(dev);	/* Program ethernet address into MAC */

	sep_emac_write(MAC_CTRL_V, 0xa413);

	if (lp->board_data.is_rmii)
		printk("%s: The ethernet is working under RMII mode\n", dev->name);
	
	/* Perform PHY-specific initialization */
	if ((phy_type == MII_DM9161_ID) || (lp->phy_type == MII_DM9161A_ID)) 
	{
		read_phy(phy_address, MII_DSCR_REG, &val);
		if ((val & (1 << 10)) == 0)			/* DSCR bit 10 is 0 -- fiber mode */
			lp->phy_media = PORT_FIBRE;
	} 
	else if (machine_is_csb337()) 
	{
		/* mix link activity status into LED2 link state */
		write_phy(phy_address, MII_LEDCTRL_REG, 0x0d22);
	}
	disable_mdi();
	spin_unlock_irq(&lp->lock);

	lp->mii.dev = dev;		/* Support for ethtool */
	lp->mii.mdio_read = mdio_read;
	lp->mii.mdio_write = mdio_write;
	lp->mii.phy_id = phy_address;
	
	lp->phy_type = phy_type;	/* Type of PHY connected */
	lp->phy_address = phy_address;	/* MDI address of PHY */

	/* Register the network interface */
	res = register_netdev(dev);
	if (res) {
		free_irq(dev->irq, dev);
		free_netdev(dev);
		return res;
	}
	sep_dev = dev;

	/* Determine current link speed */
	update_linkspeed(dev);
	netif_carrier_off(dev);		/* will be enabled in open() */

	/* Display ethernet banner */
	printk(KERN_INFO "%s: sep ethernet at 0x%08x int=%d 10M-%s (%02x:%02x:%02x:%02x:%02x:%02x)\n",
		dev->name, (uint) dev->base_addr, dev->irq,
		sep_emac_read(MAC_CTRL_V) & 0x400 ? "FullDuplex" : "HalfDuplex",
		dev->dev_addr[0], dev->dev_addr[1], dev->dev_addr[2],
		dev->dev_addr[3], dev->dev_addr[4], dev->dev_addr[5]);
	if ((phy_type == MII_DM9161_ID) || (lp->phy_type == MII_DM9161A_ID))
		printk(KERN_INFO "%s: Davicom 9196 PHY %s\n", dev->name, (lp->phy_media == PORT_FIBRE) ? "(Fiber)" : "(Copper)");
	else if (phy_type == MII_LXT971A_ID)
		printk(KERN_INFO "%s: Intel LXT971A PHY\n", dev->name);
	else if (phy_type == MII_RTL8201_ID)
		printk(KERN_INFO "%s: Realtek RTL8201(B)L PHY\n", dev->name);
	else if (phy_type == MII_BCM5221_ID)
		printk(KERN_INFO "%s: Broadcom BCM5221 PHY\n", dev->name);
	else if (phy_type == MII_DP83847_ID)
		printk(KERN_INFO "%s: National Semiconductor DP83847 PHY\n", dev->name);
	else if (phy_type == MII_AC101L_ID)
		printk(KERN_INFO "%s: Altima AC101L PHY\n", dev->name);
	else if (phy_type == MII_KS8721_ID)
		printk(KERN_INFO "%s: Micrel KS8721 PHY\n", dev->name);

	return 0;
}

/*
 * Detect MAC and PHY and perform initialization
 */
static int __init sepether_probe(struct platform_device *pdev)
{
	unsigned int phyid1, phyid2;
	int detected = -1;
	unsigned long phy_id;
	unsigned short phy_address = 0;
 
	printk("sep4020_ether: sepether_probe!\n\r");
	ether_clk = clk_get(&pdev->dev, "ether_clk");
	if (!ether_clk) 
	{
		printk(KERN_ERR "sep_ether: no clock defined\n");
		return -ENODEV;
	}
	clk_enable(ether_clk);					/* Enable Peripheral clock */
  
	while ((detected != 0) && (phy_address < 32)) 
	{
		/* Read the PHY ID registers */
		enable_mdi();
		read_phy(phy_address, MII_PHYSID1, &phyid1);
		read_phy(phy_address, MII_PHYSID2, &phyid2);
	
		disable_mdi();

		phy_id = (phyid1 << 16) | (phyid2 & 0xfff0);
		switch (phy_id) 
		{
			case MII_DM9161_ID:		/* Davicom 9161: PHY_ID1 = 0x181, PHY_ID2 = B881 */
			case MII_DM9161A_ID:		/* Davicom 9161A: PHY_ID1 = 0x181, PHY_ID2 = B8A0 */
			case MII_LXT971A_ID:		/* Intel LXT971A: PHY_ID1 = 0x13, PHY_ID2 = 78E0 */
			case MII_RTL8201_ID:		/* Realtek RTL8201: PHY_ID1 = 0, PHY_ID2 = 0x8201 */
			case MII_BCM5221_ID:		/* Broadcom BCM5221: PHY_ID1 = 0x40, PHY_ID2 = 0x61e0 */
			case MII_DP83847_ID:		/* National Semiconductor DP83847:  */
			case MII_AC101L_ID:		/* Altima AC101L: PHY_ID1 = 0x22, PHY_ID2 = 0x5520 */
			case MII_KS8721_ID:		/* Micrel KS8721: PHY_ID1 = 0x22, PHY_ID2 = 0x1610 */
				detected = sepether_setup(phy_id, phy_address, pdev);
				break;
		}

		phy_address++;
	}

	clk_disable(ether_clk);					/* Disable Peripheral clock */
	return detected;
}

static int __devexit sepether_remove(struct platform_device *pdev)
{
	printk("sep4020_ether_remove!\n\r");
	unregister_netdev(sep_dev);
	free_irq(sep_dev->irq, sep_dev);

	clk_put(ether_clk);
  	INT_DISABLE(28);
	free_netdev(sep_dev);
	sep_dev = NULL;
	return 0;
}

static struct platform_driver sepether_driver = {
	.probe		= sepether_probe,
	.remove		= __devexit_p(sepether_remove),
	/* FIXME:  support suspend and resume */
	.driver		= {
		.name	= DRV_NAME,
		.owner	= THIS_MODULE,
	},
};

static int __init sepether_init(void)
{
	return platform_driver_register(&sepether_driver);
}

static void __exit sepether_exit(void)
{
	platform_driver_unregister(&sepether_driver);
}

module_init(sepether_init)
module_exit(sepether_exit)

MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("sep4020 EMAC Ethernet driver");
MODULE_AUTHOR("fpmystar@gmail.com");
