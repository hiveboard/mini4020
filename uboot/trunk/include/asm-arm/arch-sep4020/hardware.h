/********************************************************/
/*							*/
/* Samsung S3C44B0					*/
/* tpu <tapu@371.net>					*/
/*							*/
/********************************************************/
#ifndef __ASM_ARCH_HARDWARE_H
#define __ASM_ARCH_HARDWARE_H

#include "sep4020.h"

#define FALSE 0
#define TRUE 1


typedef unsigned char U8;
typedef unsigned long U32;
typedef unsigned short U16;

typedef unsigned char UCHAR;


typedef volatile U32 *         RP;
typedef volatile U16 *         RP16;
typedef volatile U8  *         RP8;

typedef volatile unsigned int SEP4020_REG;		/* Hardware register definition */

typedef struct _SEP4020_EMAC {
SEP4020_REG SEP4020_MAC_CTRL;                        /* MAC控制寄存器 */
SEP4020_REG SEP4020_MAC_INTSRC;                       /* MAC中断源寄存器 */
SEP4020_REG SEP4020_MAC_INTMASK;                     /* MAC中断屏蔽寄存器 */
SEP4020_REG SEP4020_MAC_IPGT;                          /* 连续帧间隔寄存器 */
SEP4020_REG SEP4020_MAC_IPGR1;                         /* 等待窗口寄存器 */
SEP4020_REG SEP4020_MAC_IPGR2;                        /* 等待窗口寄存器 */
SEP4020_REG SEP4020_MAC_PACKETLEN;                     /* 帧长度寄存器 */
SEP4020_REG SEP4020_MAC_COLLCONF;                      /* 碰撞重发寄存器 */
SEP4020_REG SEP4020_MAC_TXBD_NUM;                     /* 发送描述符寄存器 */
SEP4020_REG SEP4020_MAC_FLOWCTRL;                      /* 流控寄存器 */
SEP4020_REG SEP4020_MAC_MII_CTRL;                  /* PHY控制寄存器 */
SEP4020_REG SEP4020_MAC_MII_CMD;                    /* PHY命令寄存器 */
SEP4020_REG SEP4020_MAC_MII_ADDRESS;                  /* PHY地址寄存器 */
SEP4020_REG SEP4020_MAC_MII_TXDATA;                 /* PHY写数据寄存器 */
SEP4020_REG SEP4020_MAC_MII_RXDATA;                  /* PHY读数据寄存器 */
SEP4020_REG SEP4020_MAC_MII_STATUS;                   /* PHY状态寄存器 */
SEP4020_REG SEP4020_MAC_ADDR0;                       /* MAC地址寄存器 */
SEP4020_REG SEP4020_MAC_ADDR1;                      /* MAC地址寄存器 */
SEP4020_REG SEP4020_MAC_HASH0;                         /* MAC HASH寄存器 */
SEP4020_REG SEP4020_MAC_HASH1;                         /* MAC HASH寄存器 */
SEP4020_REG SEP4020_MAC_TXPAUSE;                     /* MAC控制帧寄存器 */
SEP4020_REG SEP4020_MAC_BD[128];            		  //接收发送描述符
} SEP4020_EMAC, *SEP4020P_EMAC;

typedef struct _SEP4020_PhyOps
{
	unsigned char (*Init)(SEP4020P_EMAC *pmac);
	unsigned int (*IsPhyConnected)(SEP4020_EMAC  *pmac);
	unsigned char (*GetLinkSpeed)(SEP4020_EMAC *pmac);
	unsigned char (*AutoNegotiate)(SEP4020_EMAC *pmac, int *);

} SEP4020_PhyOps,*SEP4020P_PhyOps;


// -------- EMAC_NCR : (EMAC Offset: 0x0)  -------- 
#define SEP4020C_EMAC_LB         ((unsigned int) 0x1 <<  0) // (EMAC) Loopback. Optional. When set, loopback signal is at high level.
#define SEP4020C_EMAC_LLB        ((unsigned int) 0x1 <<  1) // (EMAC) Loopback local. 
#define SEP4020C_EMAC_RE         ((unsigned int) 0x1 <<  2) // (EMAC) Receive enable. 
#define SEP4020C_EMAC_TE         ((unsigned int) 0x1 <<  3) // (EMAC) Transmit enable. 
#define SEP4020C_EMAC_MPE        ((unsigned int) 0x1 <<  4) // (EMAC) Management port enable. 
#define SEP4020C_EMAC_CLRSTAT    ((unsigned int) 0x1 <<  5) // (EMAC) Clear statistics registers. 
#define SEP4020C_EMAC_INCSTAT    ((unsigned int) 0x1 <<  6) // (EMAC) Increment statistics registers. 
#define SEP4020C_EMAC_WESTAT     ((unsigned int) 0x1 <<  7) // (EMAC) Write enable for statistics registers. 
#define SEP4020C_EMAC_BP         ((unsigned int) 0x1 <<  8) // (EMAC) Back pressure. 
#define SEP4020C_EMAC_TSTART     ((unsigned int) 0x1 <<  9) // (EMAC) Start Transmission. 
#define SEP4020C_EMAC_THALT      ((unsigned int) 0x1 << 10) // (EMAC) Transmission Halt. 
#define SEP4020C_EMAC_TPFR       ((unsigned int) 0x1 << 11) // (EMAC) Transmit pause frame 
#define SEP4020C_EMAC_TZQ        ((unsigned int) 0x1 << 12) // (EMAC) Transmit zero quantum pause frame
// -------- EMAC_NCFGR : (EMAC Offset: 0x4) Network Configuration Register -------- 
#define SEP4020C_EMAC_SPD        ((unsigned int) 0x1 <<  0) // (EMAC) Speed. 
#define SEP4020C_EMAC_FD         ((unsigned int) 0x1 <<  1) // (EMAC) Full duplex. 
#define SEP4020C_EMAC_JFRAME     ((unsigned int) 0x1 <<  3) // (EMAC) Jumbo Frames. 
#define SEP4020C_EMAC_CAF        ((unsigned int) 0x1 <<  4) // (EMAC) Copy all frames. 
#define SEP4020C_EMAC_NBC        ((unsigned int) 0x1 <<  5) // (EMAC) No broadcast. 
#define SEP4020C_EMAC_MTI        ((unsigned int) 0x1 <<  6) // (EMAC) Multicast hash event enable
#define SEP4020C_EMAC_UNI        ((unsigned int) 0x1 <<  7) // (EMAC) Unicast hash enable. 
#define SEP4020C_EMAC_BIG        ((unsigned int) 0x1 <<  8) // (EMAC) Receive 1522 bytes. 
#define SEP4020C_EMAC_EAE        ((unsigned int) 0x1 <<  9) // (EMAC) External address match enable. 
#define SEP4020C_EMAC_CLK        ((unsigned int) 0x3 << 10) // (EMAC) 
#define 	SEP4020C_EMAC_CLK_HCLK_8               ((unsigned int) 0x0 << 10) // (EMAC) HCLK divided by 8
#define 	SEP4020C_EMAC_CLK_HCLK_16              ((unsigned int) 0x1 << 10) // (EMAC) HCLK divided by 16
#define 	SEP4020C_EMAC_CLK_HCLK_32              ((unsigned int) 0x2 << 10) // (EMAC) HCLK divided by 32
#define 	SEP4020C_EMAC_CLK_HCLK_64              ((unsigned int) 0x3 << 10) // (EMAC) HCLK divided by 64
#define SEP4020C_EMAC_RTY        ((unsigned int) 0x1 << 12) // (EMAC) 
#define SEP4020C_EMAC_PAE        ((unsigned int) 0x1 << 13) // (EMAC) 
#define SEP4020C_EMAC_RBOF       ((unsigned int) 0x3 << 14) // (EMAC) 
#define 	SEP4020C_EMAC_RBOF_OFFSET_0             ((unsigned int) 0x0 << 14) // (EMAC) no offset from start of receive buffer
#define 	SEP4020C_EMAC_RBOF_OFFSET_1             ((unsigned int) 0x1 << 14) // (EMAC) one byte offset from start of receive buffer
#define 	SEP4020C_EMAC_RBOF_OFFSET_2             ((unsigned int) 0x2 << 14) // (EMAC) two bytes offset from start of receive buffer
#define 	SEP4020C_EMAC_RBOF_OFFSET_3             ((unsigned int) 0x3 << 14) // (EMAC) three bytes offset from start of receive buffer
#define SEP4020C_EMAC_RLCE       ((unsigned int) 0x1 << 16) // (EMAC) Receive Length field Checking Enable
#define SEP4020C_EMAC_DRFCS      ((unsigned int) 0x1 << 17) // (EMAC) Discard Receive FCS
#define SEP4020C_EMAC_EFRHD      ((unsigned int) 0x1 << 18) // (EMAC) 
#define SEP4020C_EMAC_IRXFCS     ((unsigned int) 0x1 << 19) // (EMAC) Ignore RX FCS
// -------- EMAC_NSR : (EMAC Offset: 0x8) Network Status Register -------- 
#define SEP4020C_EMAC_LINKR      ((unsigned int) 0x1 <<  0) // (EMAC) 
#define SEP4020C_EMAC_MDIO       ((unsigned int) 0x1 <<  1) // (EMAC) 
#define SEP4020C_EMAC_IDLE       ((unsigned int) 0x1 <<  2) // (EMAC) 
// -------- EMAC_TSR : (EMAC Offset: 0x14) Transmit Status Register -------- 
#define SEP4020C_EMAC_UBR        ((unsigned int) 0x1 <<  0) // (EMAC) 
#define SEP4020C_EMAC_COL        ((unsigned int) 0x1 <<  1) // (EMAC) 
#define SEP4020C_EMAC_RLES       ((unsigned int) 0x1 <<  2) // (EMAC) 
#define SEP4020C_EMAC_TGO        ((unsigned int) 0x1 <<  3) // (EMAC) Transmit Go
#define SEP4020C_EMAC_BEX        ((unsigned int) 0x1 <<  4) // (EMAC) Buffers exhausted mid frame
#define SEP4020C_EMAC_COMP       ((unsigned int) 0x1 <<  5) // (EMAC) 
#define SEP4020C_EMAC_UND        ((unsigned int) 0x1 <<  6) // (EMAC) 
// -------- EMAC_RSR : (EMAC Offset: 0x20) Receive Status Register -------- 
#define SEP4020C_EMAC_BNA        ((unsigned int) 0x1 <<  0) // (EMAC) 
#define SEP4020C_EMAC_REC        ((unsigned int) 0x1 <<  1) // (EMAC) 
#define SEP4020C_EMAC_OVR        ((unsigned int) 0x1 <<  2) // (EMAC) 
// -------- EMAC_ISR : (EMAC Offset: 0x24) Interrupt Status Register -------- 
#define SEP4020C_EMAC_MFD        ((unsigned int) 0x1 <<  0) // (EMAC) 
#define SEP4020C_EMAC_RCOMP      ((unsigned int) 0x1 <<  1) // (EMAC) 
#define SEP4020C_EMAC_RXUBR      ((unsigned int) 0x1 <<  2) // (EMAC) 
#define SEP4020C_EMAC_TXUBR      ((unsigned int) 0x1 <<  3) // (EMAC) 
#define SEP4020C_EMAC_TUNDR      ((unsigned int) 0x1 <<  4) // (EMAC) 
#define SEP4020C_EMAC_RLEX       ((unsigned int) 0x1 <<  5) // (EMAC) 
#define SEP4020C_EMAC_TXERR      ((unsigned int) 0x1 <<  6) // (EMAC) 
#define SEP4020C_EMAC_TCOMP      ((unsigned int) 0x1 <<  7) // (EMAC) 
#define SEP4020C_EMAC_LINK       ((unsigned int) 0x1 <<  9) // (EMAC) 
#define SEP4020C_EMAC_ROVR       ((unsigned int) 0x1 << 10) // (EMAC) 
#define SEP4020C_EMAC_HRESP      ((unsigned int) 0x1 << 11) // (EMAC) 
#define SEP4020C_EMAC_PFRE       ((unsigned int) 0x1 << 12) // (EMAC) 
#define SEP4020C_EMAC_PTZ        ((unsigned int) 0x1 << 13) // (EMAC) 
// -------- EMAC_IER : (EMAC Offset: 0x28) Interrupt Enable Register -------- 
// -------- EMAC_IDR : (EMAC Offset: 0x2c) Interrupt Disable Register -------- 
// -------- EMAC_IMR : (EMAC Offset: 0x30) Interrupt Mask Register -------- 
// -------- EMAC_MAN : (EMAC Offset: 0x34) PHY Maintenance Register -------- 
#define SEP4020C_EMAC_DATA       ((unsigned int) 0xFFFF <<  0) // (EMAC) 
#define SEP4020C_EMAC_CODE       ((unsigned int) 0x3 << 16) // (EMAC) 
#define SEP4020C_EMAC_REGA       ((unsigned int) 0x1F << 18) // (EMAC) 
#define SEP4020C_EMAC_PHYA       ((unsigned int) 0x1F << 23) // (EMAC) 
#define SEP4020C_EMAC_RW         ((unsigned int) 0x3 << 28) // (EMAC) 
#define SEP4020C_EMAC_SOF        ((unsigned int) 0x3 << 30) // (EMAC) 
// -------- EMAC_USRIO : (EMAC Offset: 0xc0) USER Input Output Register -------- 
#define SEP4020C_EMAC_RMII       ((unsigned int) 0x1 <<  0) // (EMAC) Reduce MII
#define SEP4020C_EMAC_CLKEN      ((unsigned int) 0x1 <<  1) // (EMAC) Clock Enable
// -------- EMAC_WOL : (EMAC Offset: 0xc4) Wake On LAN Register -------- 
#define SEP4020C_EMAC_IP         ((unsigned int) 0xFFFF <<  0) // (EMAC) ARP request IP address
#define SEP4020C_EMAC_MAG        ((unsigned int) 0x1 << 16) // (EMAC) Magic packet event enable
#define SEP4020C_EMAC_ARP        ((unsigned int) 0x1 << 17) // (EMAC) ARP request event enable
#define SEP4020C_EMAC_SA1        ((unsigned int) 0x1 << 18) // (EMAC) Specific address register 1 event enable
// -------- EMAC_REV : (EMAC Offset: 0xfc) Revision Register -------- 
#define SEP4020C_EMAC_REVREF     ((unsigned int) 0xFFFF <<  0) // (EMAC) 
#define SEP4020C_EMAC_PARTREF    ((unsigned int) 0xFFFF << 16) // (EMAC) 


#define CLEAR_PEND_INT(n)       I_ISPC = (1<<(n))
#define INT_ENABLE(n)		INTMSK &= ~(1<<(n))
#define INT_DISABLE(n)		INTMSK |= (1<<(n))

#define HARD_RESET_NOW()

#endif /* __ASM_ARCH_HARDWARE_H */
