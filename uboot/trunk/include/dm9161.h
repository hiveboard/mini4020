/*
 * NOTE:	DAVICOM ethernet Physical layer
 *
 * Version:	@(#)DM9161.h	1.0.0	01/10/2001
 *
 * Authors:	ATMEL Rousset
 *
 *
 *		This program is free software; you can redistribute it and/or
 *		modify it under the terms of the GNU General Public License
 *		as published by the Free Software Foundation; either version
 *		2 of the License, or (at your option) any later version.
 */
#ifndef DM9161AE_H
#define DM9161AE_H

/* DAVICOM PHYSICAL LAYER TRANSCEIVER DM9161 */

#define	DM9161_BMCR 		0	/* Basic Mode Control Register */
#define DM9161_BMSR		1	/* Basic Mode Status Register */
#define DM9161_PHYID1		2	/* PHY Idendifier Register 1 */
#define DM9161_PHYID2		3	/* PHY Idendifier Register 2 */
#define DM9161_ANAR		4	/* Auto_Negotiation Advertisement Register  */
#define DM9161_ANLPAR		5	/* Auto_negotiation Link Partner Ability Register */
#define DM9161_ANER		6	/* Auto-negotiation Expansion Register  */
#define DM9161_DSCR		16	/* Specified Configuration Register */
#define DM9161_DSCSR		17	/* Specified Configuration and Status Register */
#define DM9161_10BTCSR		18	/* 10BASE-T Configuration and Satus Register */
#define DM9161_MDINTR		21	/* Specified Interrupt Register */
#define DM9161_RECR		22	/* Specified Receive Error Counter Register */
#define DM9161_DISCR		23	/* Specified Disconnect Counter Register */
#define DM9161_RLSR		24	/* Hardware Reset Latch State Register */


/* --Bit definitions: DM9161_BMCR */
#define DM9161_RESET   	         (1 << 15)	/* 1= Software Reset; 0=Normal Operation */
#define DM9161_LOOPBACK	         (1 << 14)	/* 1=loopback Enabled; 0=Normal Operation */
#define DM9161_SPEED_SELECT      (1 << 13)	/* 1=100Mbps; 0=10Mbps */
#define DM9161_AUTONEG	         (1 << 12)
#define DM9161_POWER_DOWN        (1 << 11)
#define DM9161_ISOLATE           (1 << 10)
#define DM9161_RESTART_AUTONEG   (1 << 9)
#define DM9161_DUPLEX_MODE       (1 << 8)
#define DM9161_COLLISION_TEST    (1 << 7)

/*--Bit definitions: DM9161_BMSR */
#define DM9161_100BASE_T4        (1 << 15)
#define DM9161_100BASE_TX_FD     (1 << 14)
#define DM9161_100BASE_T4_HD     (1 << 13)
#define DM9161_10BASE_T_FD       (1 << 12)
#define DM9161_10BASE_T_HD       (1 << 11)
#define DM9161_MF_PREAMB_SUPPR   (1 << 6)
#define DM9161_AUTONEG_COMP      (1 << 5)
#define DM9161_REMOTE_FAULT      (1 << 4)
#define DM9161_AUTONEG_ABILITY   (1 << 3)
#define DM9161_LINK_STATUS       (1 << 2)
#define DM9161_JABBER_DETECT     (1 << 1)
#define DM9161_EXTEND_CAPAB      (1 << 0)

/*--definitions: DM9161_PHYID1 */
#define DM9161_PHYID1_OUI	 0x606E
#define DM9161_LSB_MASK	         0x3F

/*--Bit definitions: DM9161_ANAR, DM9161_ANLPAR */
#define DM9161_NP               (1 << 15)
#define DM9161_ACK              (1 << 14)
#define DM9161_RF               (1 << 13)
#define DM9161_FCS              (1 << 10)
#define DM9161_T4               (1 << 9)
#define DM9161_TX_FDX           (1 << 8)
#define DM9161_TX_HDX           (1 << 7)
#define DM9161_10_FDX           (1 << 6)
#define DM9161_10_HDX           (1 << 5)
#define DM9161_AN_IEEE_802_3	0x0001

/*--Bit definitions: DM9161_ANER */
#define DM9161_PDF              (1 << 4)
#define DM9161_LP_NP_ABLE       (1 << 3)
#define DM9161_NP_ABLE          (1 << 2)
#define DM9161_PAGE_RX          (1 << 1)
#define DM9161_LP_AN_ABLE       (1 << 0)

/*--Bit definitions: DM9161_DSCR */
#define DM9161_BP4B5B           (1 << 15)
#define DM9161_BP_SCR           (1 << 14)
#define DM9161_BP_ALIGN         (1 << 13)
#define DM9161_BP_ADPOK         (1 << 12)
#define DM9161_REPEATER         (1 << 11)
#define DM9161_TX               (1 << 10)
#define DM9161_RMII_ENABLE      (1 << 8)
#define DM9161_F_LINK_100       (1 << 7)
#define DM9161_SPLED_CTL        (1 << 6)
#define DM9161_COLLED_CTL       (1 << 5)
#define DM9161_RPDCTR_EN        (1 << 4)
#define DM9161_SM_RST           (1 << 3)
#define DM9161_MFP SC           (1 << 2)
#define DM9161_SLEEP            (1 << 1)
#define DM9161_RLOUT            (1 << 0)

/*--Bit definitions: DM9161_DSCSR */
#define DM9161_100FDX           (1 << 15)
#define DM9161_100HDX           (1 << 14)
#define DM9161_10FDX            (1 << 13)
#define DM9161_10HDX            (1 << 12)

/*--Bit definitions: DM9161_10BTCSR */
#define DM9161_LP_EN           (1 << 14)
#define DM9161_HBE             (1 << 13)
#define DM9161_SQUELCH         (1 << 12)
#define DM9161_JABEN           (1 << 11)
#define DM9161_10BT_SER        (1 << 10)
#define DM9161_POLR            (1 << 0)


/*--Bit definitions: DM9161_MDINTR */
#define DM9161_INTR_PEND       (1 << 15)
#define DM9161_FDX_MASK        (1 << 11)
#define DM9161_SPD_MASK        (1 << 10)
#define DM9161_LINK_MASK       (1 << 9)
#define DM9161_INTR_MASK       (1 << 8)
#define DM9161_FDX_CHANGE      (1 << 4)
#define DM9161_SPD_CHANGE      (1 << 3)
#define DM9161_LINK_CHANGE     (1 << 2)
#define DM9161_INTR_STATUS     (1 << 0)


/******************  function prototypes **********************/
unsigned int  dm9161_IsPhyConnected(SEP4020P_EMAC p_mac);		//kyon
unsigned char dm9161_GetLinkSpeed(SEP4020P_EMAC p_mac);
unsigned char dm9161_AutoNegotiate(SEP4020P_EMAC p_mac, int *status);
unsigned char dm9161_InitPhy(SEP4020P_EMAC p_mac);

/*
 added by fangpan*/
void at91_GetPhyInterface(SEP4020P_PhyOps p_phyops);           

typedef int  BOOLEAN;


#define RMII

#define DM9161AE_INIT_AUTO_NEG_RETRIES         3
#define DEF_OFF				0
#define DEF_ON				1
#define DEF_BIT_09			0x01<<9
#define DM9161AE_BMCR		0x00
#define DM9161AE_BMSR		0x01
#define DM9161AE_PHYID1		0x02
#define DM9161AE_PHYID2		0x03
#define DM9161AE_ANAR		0x04
#define DM9161AE_ANLPAR		0x05
#define DM9161AE_ANER		0x06

#define DM9161AE_DSCR		0x10
#define DM9161AE_DSCSR		0X11
#define DM9161AE_10BTCSR	0x12

#define DM9161AE_MDINTR		0x15
#define DM9161AE_RECR		0x16
#define DM9161AE_DISCR		0x17
#define DM9161AE_RLSR		0x18



                                                                /* Basic mode control register. */
#define BMCR_RESV                         0x007F                /* Unused...                   */
#define BMCR_CTST                         0x0080                /* Collision test              */
#define BMCR_FULLDPLX                     0x0100                /* Full duplex                 */
#define BMCR_ANRESTART                    0x0200                /* Auto negotiation restart    */
#define BMCR_ISOLATE                      0x0400                /* Disconnect DP83840 from MII */
#define BMCR_PDOWN                        0x0800                /* Powerdown the DP83840       */
#define BMCR_ANENABLE                     0x1000                /* Enable auto negotiation     */
#define BMCR_SPEED100                     0x2000                /* Select 100Mbps              */
#define BMCR_LOOPBACK                     0x4000                /* TXD loopback bits           */
#define BMCR_RESET                        0x8000                /* Reset the DP83840           */

                                                                /* Basic mode status register. */
#define BMSR_ERCAP                        0x0001                /* Ext-reg capability          */
#define BMSR_JCD                          0x0002                /* Jabber detected             */
#define BMSR_LSTATUS                      0x0004                /* Link status                 */
#define BMSR_ANEGCAPABLE                  0x0008                /* Able to do auto-negotiation */
#define BMSR_RFAULT                       0x0010                /* Remote fault detected       */
#define BMSR_ANEGCOMPLETE                 0x0020                /* Auto-negotiation complete   */
#define BMSR_RESV                         0x07C0                /* Unused...                   */
#define BMSR_10HALF                       0x0800                /* Can do 10mbps, half-duplex  */
#define BMSR_10FULL                       0x1000                /* Can do 10mbps, full-duplex  */
#define BMSR_100HALF                      0x2000                /* Can do 100mbps, half-duplex */
#define BMSR_100FULL                      0x4000                /* Can do 100mbps, full-duplex */
#define BMSR_100BASE4                     0x8000                /* Can do 100mbps, 4k packets  */

                                                                /* Advertisement control reg   */
#define ADVERTISE_SLCT                    0x001F                /* Selector bits               */
#define ADVERTISE_CSMA                    0x0001                /* Only selector supported     */
#define ADVERTISE_10HALF                  0x0020                /* Try for 10mbps half-duplex  */
#define ADVERTISE_10FULL                  0x0040                /* Try for 10mbps full-duplex  */
#define ADVERTISE_100HALF                 0x0080                /* Try for 100mbps half-duplex */
#define ADVERTISE_100FULL                 0x0100                /* Try for 100mbps full-duplex */
#define ADVERTISE_100BASE4                0x0200                /* Try for 100mbps 4k packets  */
#define ADVERTISE_RESV                    0x1C00                /* Unused...                   */
#define ADVERTISE_RFAULT                  0x2000                /* Say we can detect faults    */
#define ADVERTISE_LPACK                   0x4000                /* Ack link partners response  */
#define ADVERTISE_NPAGE                   0x8000                /* Next page bit               */

#define ADVERTISE_FULL (ADVERTISE_100FULL | ADVERTISE_10FULL | \
			ADVERTISE_CSMA)
#define ADVERTISE_ALL (ADVERTISE_10HALF | ADVERTISE_10FULL | \
                       ADVERTISE_100HALF | ADVERTISE_100FULL)

                                                                /* Link partner ability reg    */
#define LPA_SLCT                          0x001F                /* Same as advertise selector  */
#define LPA_10HALF                        0x0020                /* Can do 10mbps half-duplex   */
#define LPA_10FULL                        0x0040                /* Can do 10mbps full-duplex   */
#define LPA_100HALF                       0x0080                /* Can do 100mbps half-duplex  */
#define LPA_100FULL                       0x0100                /* Can do 100mbps full-duplex  */
#define LPA_100BASE4                      0x0200                /* Can do 100mbps 4k packets   */
#define LPA_RESV                          0x1C00                /* Unused...                   */
#define LPA_RFAULT                        0x2000                /* Link partner faulted        */
#define LPA_LPACK                         0x4000                /* Link partner acked us       */
#define LPA_NPAGE                         0x8000                /* Next page bit               */

#define LPA_DUPLEX		(LPA_10FULL | LPA_100FULL)
#define LPA_100			(LPA_100FULL | LPA_100HALF | LPA_100BASE4)

                                                                /* Expansion reg for auto-neg  */
#define EXPANSION_NWAY                    0x0001                /* Can do N-way auto-nego      */
#define EXPANSION_LCWP                    0x0002                /* Got new RX page code word   */
#define EXPANSION_ENABLENPAGE             0x0004                /* This enables npage words    */
#define EXPANSION_NPCAPABLE               0x0008                /* Link partner supports npage */
#define EXPANSION_MFAULTS                 0x0010                /* Multiple faults detected    */
#define EXPANSION_RESV                    0xFFE0                /* Unused...                   */

                                                                /* N-way test register         */
#define NWAYTEST_RESV1                    0x00FF                /* Unused...                   */
#define NWAYTEST_LOOPBACK                 0x0100                /* Enable loopback for N-way   */
#define NWAYTEST_RESV2                    0xFE00                /* Unused...                   */

#define SPD_10				                  10
#define SPD_100		                         100
                                                                 /* Duplex, half or full        */
#define DUPLEX_HALF				            0x00
#define DUPLEX_FULL				            0x01


#define MII_DM9161_ID                 0x0181b8a0                /* PHY ID                      */

#define AT91C_PHY_ADDR	                      31                /* PHY Address                 */

#define SPECIFIED_INTR_REG                    21                /* PHY Specified Interrupt Reg */

/*
 * *********************************************************************************************************
 * *                                   PHY ERROR CODES 12,000 -> 13,000
 * *********************************************************************************************************
 * */

#define  NET_PHY_ERR_NONE                   0x2EE0
#define  NET_PHY_ERR_REGRD_TIMEOUT          0x2EEA
#define  NET_PHY_ERR_REGWR_TIMEOUT          0x2EF4
#define  NET_PHY_ERR_AUTONEG_TIMEOUT        0x2EFE


#define NET_PHY_SPD_0					-1
#define NET_PHY_SPD_10					0
#define NET_PHY_SPD_100					1

#define NET_PHY_DUPLEX_UNKNOWN			-1
#define NET_PHY_DUPLEX_HALF				0
#define NET_PHY_DUPLEX_FULL				1



/*
 * *********************************************************************************************************
 * *                                         FUNCTION PROTOTYPES
 * *********************************************************************************************************
 * */

                                                                /* ------------------- STATUS FNCTS ------------------- */
BOOLEAN  NetNIC_PhyAutoNegState  ();           /* Get PHY auto-negotiation state                       */
BOOLEAN  NetNIC_PhyLinkState     ();           /* Get PHY link state                                   */

U32   NetPHY_GetLinkSpeed     ();           /* Get PHY link speed                                   */
U32   NetPHY_GetLinkDuplex    ();           /* Get PHY duplex mode                                  */

void NetNIC_PhyRegWr(U16 Phy_addr, U16 address, U16 data);
U16 NetNIC_PhyRegRd(U16 Phy_addr, U16 address);

short NetNIC_PhyAutoNeg();           /* Do link auto-negotiation                             */

void phy_hard_reset();
void DM9161AE_DlyAutoNegAck();

#endif

