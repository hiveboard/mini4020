/*
 * (C) Copyright 2003
 * Author : Hamid Ikdoumi (Atmel)
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <asm/io.h>
#include <net.h>
#include <asm/arch/hardware.h>
#include <dm9161.h>

#define debug_phy_loopback 0

#ifdef CONFIG_DRIVER_ETHER

#if (CONFIG_COMMANDS & CFG_CMD_NET)

/*
 * Name:
 *	dm9161a_IsPhyConnected
 * Description:
 *	Reads the 2 PHY ID registers
 * Arguments:
 *	p_mac - pointer to AT91S_EMAC struct
 * Return value:
 *	TRUE - if id read successfully
 *	FALSE- if error
 */
static unsigned int dm9161a_IsPhyConnected (SEP4020P_EMAC p_mac)
{
	unsigned short Id1, Id2;
	
	 phy_hard_reset();	
	 
  Id1 =  NetNIC_PhyRegRd(AT91C_PHY_ADDR, DM9161_PHYID1);
	
	//printf("DM9161A PHY : 0x%x\n\r", Id1);
	
	 Id2 =  NetNIC_PhyRegRd(AT91C_PHY_ADDR, DM9161_PHYID1);
	 
	//printf("DM9161A PHY : 0x%x\n\r", Id2);
	
	if ((Id1 == (DM9161_PHYID1_OUI >> 6)) &&
	    ((Id2 >> 10) == (DM9161_PHYID1_OUI & DM9161_LSB_MASK))) {
	  //printf("DM9161A PHY Detected\n\r");
	  return TRUE;
	}
	
	return TRUE;
}

/*
 * Name:
 *	dm9161a_GetLinkSpeed
 * Description:
 *	Link parallel detection status of MAC is checked and set in the
 *	MAC configuration registers
 * Arguments:
 *	p_mac - pointer to MAC
 * Return value:
 *	TRUE - if link status set succesfully
 *	FALSE - if link status not set
 */
static uchar dm9161a_GetLinkSpeed (SEP4020P_EMAC p_mac)
{
	  unsigned long bmsr;
    unsigned long bmcr;
    unsigned long lpa;


    bmsr = NetNIC_PhyRegRd(AT91C_PHY_ADDR, DM9161AE_BMSR);     /* Get Link Status from PHY status reg. Requires 2 reads */
	bmsr = NetNIC_PhyRegRd(AT91C_PHY_ADDR, DM9161AE_BMSR);     /* Get Link Status from PHY status reg. Requires 2 reads */

    if ((bmsr & BMSR_LSTATUS) == 0) {
        return (NET_PHY_SPD_0);                                 /* No link                                               */
    }

	bmcr = NetNIC_PhyRegRd(AT91C_PHY_ADDR, DM9161AE_BMSR);     /* Read the PHY Control Register                         */
	
    if ((bmcr & BMCR_ANENABLE) == BMCR_ANENABLE) {		        /* If AutoNegotiation is enabled                         */
        if ((bmsr & BMSR_ANEGCOMPLETE) == 0) {                  /* If AutoNegotiation is not complete                    */
			return (NET_PHY_SPD_0);       					    /* AutoNegotitation in progress                          */
        }

		lpa = NetNIC_PhyRegRd(AT91C_PHY_ADDR, DM9161AE_ANLPAR);   /* Read the Link Partner Ability Register                */

        if (((lpa & LPA_100FULL) == LPA_100FULL) || ((lpa & LPA_100HALF) == LPA_100HALF)) {
			return (NET_PHY_SPD_100);
        } else {
			return (NET_PHY_SPD_10);
        }
	} else {                                                    /* Auto-negotiation not enabled, get speed from BMCR     */
        if ((bmcr & BMCR_SPEED100) == BMCR_SPEED100) {
            return (NET_PHY_SPD_100);
        } else {
            return (NET_PHY_SPD_10);
        }
	}
}


/*
 * Name:
 *	dm9161a_ResetPhy
 * Description:
 *	Resets PHY
 * Arguments:
 *	p_mac - pointer to struct AT91S_EMAC
 * Return value:
 *	TRUE - if link status set succesfully
 *	FALSE - if link status not set
 */
static uchar dm9161a_ResetPhy(SEP4020P_EMAC p_mac)
{
        unsigned short bmcr;
	int timeout = 10;
	uchar ret = TRUE;


	bmcr = DM9161_RESET;
	
  NetNIC_PhyRegWr(AT91C_PHY_ADDR, DM9161AE_BMCR, bmcr);
	do {
		bmcr =  NetNIC_PhyRegRd(AT91C_PHY_ADDR, DM9161_BMCR);
	  timeout--;
	} while ((bmcr & DM9161_RESET) && timeout);


	if (!timeout)
	  ret = FALSE;

	return ret;
}


/*
 * Name:
 *	dm9161a_AutoNegotiate
 * Description:
 *	MAC Autonegotiates with the partner status of same is set in the
 *	MAC configuration registers
 * Arguments:
 *	dev - pointer to struct net_device
 * Return value:
 *	TRUE - if link status set successfully
 *	FALSE - if link status not set
 */
static uchar dm9161a_AutoNegotiate (SEP4020P_EMAC p_mac, int *status)
{
	  unsigned short   i;
    unsigned short   reg_val;
    BOOLEAN	 link;


    i               = DM9161AE_INIT_AUTO_NEG_RETRIES;                   /* Set the # of retries before declaring a timeout  */
    link            = NetNIC_PhyLinkState();                        /* Get the current link state. 1=linked, 0=no link  */

    if (link == DEF_OFF) {
        reg_val     = NetNIC_PhyRegRd(AT91C_PHY_ADDR, DM9161AE_BMCR);  /* Get current control register value               */
        reg_val    |= DEF_BIT_09;                                       /* Set the auto-negotiation start bit               */

        NetNIC_PhyRegWr(AT91C_PHY_ADDR, DM9161AE_BMCR, reg_val);       /* Initiate auto-negotiation                        */

        do {                                                            /* Do while auto-neg incomplete, or retries expired */
            DM9161AE_DlyAutoNegAck();                                   /* Wait for a while auto-neg to proceed (net_bsp.c) */
            reg_val = NetNIC_PhyRegRd(AT91C_PHY_ADDR, DM9161AE_BMSR);  /* Read the Basic Mode Status Register              */
            reg_val = NetNIC_PhyRegRd(AT91C_PHY_ADDR, DM9161AE_BMSR);  /* Read the Basic Mode Status Register              */
            i--;
        } while (((reg_val & BMSR_LSTATUS) == 0) && (i > 0));           /* While link not established and retries remain    */
    }

    if (i == 0) {                                                       /* If we are out of retries...                      */
        return NET_PHY_ERR_AUTONEG_TIMEOUT;                      /* Return a timeout error                           */
    } else {
        return NET_PHY_ERR_NONE;                                 /* Link up and retries remain                       */
    }
}



/*
 * Name:
 *	dm9161a_InitPhy
 * Description:
 *	MAC starts checking its link by using parallel detection and
 *	Autonegotiation and the same is set in the MAC configuration registers
 * Arguments:
 *	p_mac - pointer to struct AT91S_EMAC
 * Return value:
 *	TRUE - if link status set succesfully
 *	FALSE - if link status not set
 */
static uchar dm9161a_InitPhy (SEP4020P_EMAC p_mac)
{
	unsigned int tick = 0;
	unsigned short IntValue;
	uchar ret = TRUE;
        U16 Phy_BMCR;
	U16 New_Phy_BMCR;
	
  	dm9161a_AutoNegotiate(p_mac,&tick);
  
  /*
	do {
		// Link status is latched, so read twice to get current value 
		IntValue =  NetNIC_PhyRegRd(AT91C_PHY_ADDR, DM9161_BMSR);
		IntValue =  NetNIC_PhyRegRd(AT91C_PHY_ADDR, DM9161_BMSR);
	
		tick++;
	} while (!(IntValue & DM9161_LINK_STATUS) && (tick < SEP4020_ETH_TIMEOUT));

	if (!(IntValue & DM9161_LINK_STATUS)){
	
		printf ("No link (dm9161a)\n\r");
		ret = FALSE;
		return ret;
	}
*/
	/* Force RMII mode */
/*	at91_EmacReadPhy (p_mac, SAM9260EK_PHY_ADDRESS, DM9161_DSCR, &IntValue);
	IntValue |= DM9161_RMII_ENABLE;
	at91_EmacWritePhy (p_mac, SAM9260EK_PHY_ADDRESS, DM9161_DSCR, &IntValue);
*/
	/* Force speed to 10Mbps and disable auto-neg */
	IntValue =  NetNIC_PhyRegRd(AT91C_PHY_ADDR, DM9161_BMCR);
	
	IntValue &= ~(DM9161_SPEED_SELECT | DM9161_AUTONEG);
	
	NetNIC_PhyRegWr(AT91C_PHY_ADDR, DM9161_BMCR, IntValue);

	/* Restart auto-neg */
/*	IntValue |= DM9161_RESTART_AUTONEG;
	at91_EmacWritePhy (p_mac, SAM9260EK_PHY_ADDRESS, DM9161_BMCR, &IntValue);
*/
/*	if (!dm9161a_GetLinkSpeed (p_mac)) {
		// Try another time 
		ret = dm9161a_GetLinkSpeed (p_mac);
	}
  */
  Phy_BMCR = NetNIC_PhyRegRd(AT91C_PHY_ADDR, DM9161AE_BMCR);
	

#if debug_phy_loopback
	Phy_BMCR |= 0x4100;
	Phy_BMCR &= ~0x3000;
	NetNIC_PhyRegWr(AT91C_PHY_ADDR, DM9161AE_BMCR, Phy_BMCR);
	New_Phy_BMCR = NetNIC_PhyRegRd(AT91C_PHY_ADDR, DM9161AE_BMCR);
	if(New_Phy_BMCR != Phy_BMCR)
		return -1;
#else 
    Phy_BMCR = 0x0000;
	//Phy_BMCR &= ~0x3000;
	NetNIC_PhyRegWr(AT91C_PHY_ADDR, DM9161AE_BMCR, Phy_BMCR);
	New_Phy_BMCR = NetNIC_PhyRegRd(AT91C_PHY_ADDR, DM9161AE_BMCR);
	if(New_Phy_BMCR != Phy_BMCR)
		return -1;
#endif
        //printf("Phy_BMCR is %d\n\r",New_Phy_BMCR);
	/* Disable PHY Interrupts */
/*	IntValue =  NetNIC_PhyRegRd(AT91C_PHY_ADDR, DM9161_MDINTR);
	
	// clear FDX, SPD, Link, INTR masks 
	IntValue &= ~(DM9161_FDX_MASK | DM9161_SPD_MASK |
		      DM9161_LINK_MASK | DM9161_INTR_MASK);
	NetNIC_PhyRegWr(AT91C_PHY_ADDR, DM9161_MDINTR, IntValue);
	*/
	return (ret);
}



/*
 * Name:
 *	at91_GetPhyInterface
 * Description:
 *	Initialise the interface functions to the PHY
 * Arguments:
 *	None
 * Return value:
 *	None
 */
void at91_GetPhyInterface(SEP4020P_PhyOps p_phyops)
{
	p_phyops->Init           = dm9161a_InitPhy;
	p_phyops->IsPhyConnected = dm9161a_IsPhyConnected;
	p_phyops->GetLinkSpeed   = dm9161a_GetLinkSpeed;
	p_phyops->AutoNegotiate  = dm9161a_AutoNegotiate;
}

void DM9161AE_DlyAutoNegAck() 
{
	U32 i;
	for(i = 0;i < 500000;i++);
}                                              
               
                   	 				 	    	 	 	 		    	     	 	 	 		 	  	  	  	     	 	      	   		 	 	 	   		   			 	     			  			 		   	 			       	  	 		  	 	  	 		 		   		  	  			 	  	 		 	 	 			 	 		 		 	 		 	   		 	 	 	   		     			  			 		  	 		 	  			 	 	 	 	  		  	   		   	   	 				 		 			 			  			 		   		 		 				 		 	    		   	  		 


void phy_hard_reset()
{
	  U32 i;
	GPIO_PORTH_SEL |= 0X40;
   	GPIO_PORTH_DATA |= 0X40;
   	GPIO_PORTH_DIR &= ~0X40;
    
    for(i=0;i<100000;i++);
  	GPIO_PORTH_SEL |= 0X40;
	GPIO_PORTH_DATA &= ~0X40;
  	GPIO_PORTH_DIR &= ~0X40;
    
    for(i=0;i<100000;i++);
 	GPIO_PORTH_SEL |= 0X40;
 	GPIO_PORTH_DATA |= 0X40;
  	GPIO_PORTH_DIR &= ~0X40;
}


//************** write MII reg **************//
//*******************************************//

void NetNIC_PhyRegWr(U16 Phy_addr, U16 address, U16 data)
{

	U32 mii_status;

	MAC_MII_ADDRESS  = (U32)(address) | Phy_addr;
	MAC_MII_TXDATA = (U32)data;
	MAC_MII_CMD = 0x4;
	udelay(8000);
	while(1) {
		mii_status = MAC_MII_STATUS;
		if ((mii_status & 0x2) == 0) break;
		}
	
	return;	
}

//*************** read MII reg ***************//
//********************************************//

U16 NetNIC_PhyRegRd(U16 Phy_addr, U16 address)
{
	U16 mii_rxdata;
	U32 mii_status;
	
	MAC_MII_ADDRESS = (U32)(address) | Phy_addr;
	MAC_MII_CMD = 0x2;
	udelay(4000);
	while(1) {
		mii_status = MAC_MII_STATUS;
		if ((mii_status & 0x2) == 0) break;
		}
	
	mii_rxdata = (U16)MAC_MII_RXDATA;
	return(mii_rxdata);
}

                 
/*
*********************************************************************************************************
*                                        NetNIC_PhyAutoNeg()
*
* Description : Do link auto-negotiation
*
* Argument(s) : none.
*
* Return(s)   : 1 = no error, 0 = error
*
* Caller(s)   : NetNIC_PhyInit.
*
* Note(s)     : none.
*********************************************************************************************************
*/
short  NetNIC_PhyAutoNeg ()
{
    U16   i;
    U16   reg_val;
    BOOLEAN	 link;


    i               = DM9161AE_INIT_AUTO_NEG_RETRIES;                   /* Set the # of retries before declaring a timeout  */
    link            = NetNIC_PhyLinkState();                        /* Get the current link state. 1=linked, 0=no link  */

    if (link == DEF_OFF) {
        reg_val     = NetNIC_PhyRegRd(AT91C_PHY_ADDR, DM9161AE_BMCR);  /* Get current control register value               */
        reg_val    |= DEF_BIT_09;                                       /* Set the auto-negotiation start bit               */

        NetNIC_PhyRegWr(AT91C_PHY_ADDR, DM9161AE_BMCR, reg_val);       /* Initiate auto-negotiation                        */

        do {                                                            /* Do while auto-neg incomplete, or retries expired */
            DM9161AE_DlyAutoNegAck();                                   /* Wait for a while auto-neg to proceed (net_bsp.c) */
            reg_val = NetNIC_PhyRegRd(AT91C_PHY_ADDR, DM9161AE_BMSR);  /* Read the Basic Mode Status Register              */
            reg_val = NetNIC_PhyRegRd(AT91C_PHY_ADDR, DM9161AE_BMSR);  /* Read the Basic Mode Status Register              */
            i--;
        } while (((reg_val & BMSR_LSTATUS) == 0) && (i > 0));           /* While link not established and retries remain    */
    }

    if (i == 0) {                                                       /* If we are out of retries...                      */
        return NET_PHY_ERR_AUTONEG_TIMEOUT;                      /* Return a timeout error                           */
    } else {
        return NET_PHY_ERR_NONE;                                 /* Link up and retries remain                       */
    }
}


/*
*********************************************************************************************************
*                                    NetNIC_PhyAutoNegState()
*
* Description : Returns state of auto-negotiation
*               This instance probe the Davicom DM9161AE '3.3V Dual-Speed Fast Ethernet Transceiver'
*
* Argument(s) : none.
*
* Return(s)   : State of auto-negociation (DEF_OFF = not completed, DEF_ON = completed).
*
* Caller(s)   : NetNIC_PhyInit.
*
* Note(s)     : If any error is encountered while reading the PHY, this function
*               will return Auto Negotiation State = DEF_OFF (incomplete).
*********************************************************************************************************
*/

BOOLEAN  NetNIC_PhyAutoNegState ()
{
    U32  reg_val;


    reg_val = NetNIC_PhyRegRd(AT91C_PHY_ADDR, DM9161AE_BMSR);
    reg_val = NetNIC_PhyRegRd(AT91C_PHY_ADDR, DM9161AE_BMSR);

    if ((reg_val & BMSR_ANEGCOMPLETE) == BMSR_ANEGCOMPLETE) {   /* DM9161AE register 0x01: Basic Status Register #1      */
        return (DEF_ON);                                        /* BIT 5 Signal the result of the auto negotiation       */
    } else {                                                    /* 1 = complete, 0 = incomplete                          */
        return (DEF_OFF);
    }
}

/*
*********************************************************************************************************
*                                     NetNIC_PhyLinkState()
*
* Description : Returns state of ethernet link
*               This instance probe the Davicom DM9161AE '3.3V Dual-Speed Fast Ethernet Transceiver'
*
* Argument(s) : none.
*
* Return(s)   : State of ethernet link (DEF_OFF = link down, DEF_ON = link up).
*
* Caller(s)   : NetNIC_PhyISR_Handler.
*
* Note(s)     : If any error is encountered while reading the PHY, this function
*               will return link state = DEF_OFF.
*********************************************************************************************************
*/

BOOLEAN  NetNIC_PhyLinkState ()
{
    U16  reg_val;
                                                                /* DM9161AE register 0x01: Basic Status Register #1      */
                                                                /* BIT 2 , Link Status, 1 = linked, 0 = not linked.      */
    reg_val      = NetNIC_PhyRegRd(AT91C_PHY_ADDR, DM9161AE_BMSR);
    reg_val      = NetNIC_PhyRegRd(AT91C_PHY_ADDR, DM9161AE_BMSR);

    reg_val     &= BMSR_LSTATUS;

    if (reg_val == BMSR_LSTATUS) {
        return (DEF_ON);
    } else {
        return (DEF_OFF);
    }
}



/*
*********************************************************************************************************
*                                     NetPHY_GetDuplex()
*
* Description : Returns the duplex mode of the current Ethernet link
*               This probes the Davicom DM9161AE '3.3V Dual-Speed Fast Ethernet Transceiver'
*
* Argument(s) : none.
*
* Return(s)   : 0 = Unknown (Auto-Neg in progress), 1 = Half Duplex, 2 = Full Duplex
*
* Caller(s)   : AT91SAM7X256_EMAC_Init()
*
* Note(s)     : none.
*********************************************************************************************************
*/

U32 NetPHY_GetLinkDuplex ()
{
    U32 bmcr;
    U32 bmsr;
    U32 lpa;


    bmsr = NetNIC_PhyRegRd(AT91C_PHY_ADDR, DM9161AE_BMSR);     /* Get Link Status from PHY status reg. Requires 2 reads */
	bmsr = NetNIC_PhyRegRd(AT91C_PHY_ADDR, DM9161AE_BMSR);     /* Get Link Status from PHY status reg. Requires 2 reads */


    if ((bmsr & BMSR_LSTATUS) == 0) {
        return (NET_PHY_DUPLEX_UNKNOWN);                        /* No link, return 'Duplex Uknown'                       */
    }

	bmcr = NetNIC_PhyRegRd(AT91C_PHY_ADDR, DM9161AE_BMSR);     /* Read the PHY Control Register                         */
	
    if ((bmcr & BMCR_ANENABLE) == BMCR_ANENABLE) {		        /* If AutoNegotiation is enabled                         */
        if ((bmsr & BMSR_ANEGCOMPLETE) == 0) {                  /* If AutoNegotiation is not complete                    */
			return (NET_PHY_DUPLEX_UNKNOWN);      			    /* AutoNegotitation in progress                          */
        }

		lpa = NetNIC_PhyRegRd(AT91C_PHY_ADDR, DM9161AE_ANLPAR);   /* Read the Link Partner Ability Register                */

        if (((lpa & LPA_100FULL) == LPA_100FULL) || ((lpa & LPA_10FULL) == LPA_10FULL)) {
			return (NET_PHY_DUPLEX_FULL);
        } else {
            return (NET_PHY_DUPLEX_HALF);
        }
	} else {                                                    /* Auto-negotiation not enabled, get duplex from BMCR    */
        if ((bmcr & BMCR_FULLDPLX) == BMCR_FULLDPLX) {
            return (NET_PHY_DUPLEX_FULL);
        } else {
            return (NET_PHY_DUPLEX_HALF);
        }
	}
}

#endif	/* CONFIG_COMMANDS & CFG_CMD_NET */

#endif	/* CONFIG_DRIVER_ETHER */
