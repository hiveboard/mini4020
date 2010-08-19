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
#include <miiphy.h>
#include <dm9161.h>
#include <malloc.h>

#define MAC_ISR_TXB		0x00000001
#define MAC_ISR_TXE		0x00000002
#define MAC_ISR_RXB 	0x00000004
#define MAC_ISR_RXE		0x00000008
#define MAC_ISR_BUSY	0x00000010
#define MAC_ISR_TXC 	0x00000020
#define MAC_ISR_RXC		0x00000040

#define debug_phy_loopback 0
#define debug_mac_loopback 0
#define DEBUG 1
/* ----- Ethernet Buffer definitions ----- */

/* Receive Transfer descriptor structure */
typedef struct  _AT91S_RxTdDescriptor {
	unsigned int addr;
	union
	{
		unsigned int status;
		struct {
			unsigned int Length:16;
			unsigned int empty:1;
			unsigned int irq:1;
			unsigned int wrap:1;
			unsigned int reserved:4;
			unsigned int pause:1;
			unsigned int miss:1;
			unsigned int overrun:1;
			unsigned int invalid_sym:1;
			unsigned int dibble_nibble:1;
			unsigned int too_long:1;
			unsigned int too_small:1;
			unsigned int crc_err:1;
			unsigned int late_col:1;
		} S_Status;		
	} U_Status;
} AT91S_RxTdDescriptor, *AT91PS_RxTdDescriptor;


/* Transmit Transfer descriptor structure */
typedef struct _AT91S_TxTdDescriptor {
	unsigned int addr;
	union
	{
		unsigned int status;
		struct {
			unsigned int Length:16;
			unsigned int ready:1;
			unsigned int irq:1;
			unsigned int wrap:1;
			unsigned int pad:1;
			unsigned int crc:1;
			unsigned int reserved:2;
			unsigned int underrun:1;
			unsigned int retry_cnt:4;
			unsigned int retry_limit:1;
			unsigned int late_col:1;
			unsigned int defer:1;
			unsigned int cs_lost:1;
		} S_Status;		
	} U_Status;
} AT91S_TxTdDescriptor, *AT91PS_TxTdDescriptor;


#define RBF_ADDR      0xfffffffc
#define RBF_OWNER     (1<<0)
#define RBF_WRAP      (1<<1)
#define RBF_BROADCAST (1<<31)
#define RBF_MULTICAST (1<<30)
#define RBF_UNICAST   (1<<29)
#define RBF_EXTERNAL  (1<<28)
#define RBF_UNKOWN    (1<<27)
#define RBF_SIZE      0x07ff
#define RBF_LOCAL4    (1<<26)
#define RBF_LOCAL3    (1<<25)
#define RBF_LOCAL2    (1<<24)
#define RBF_LOCAL1    (1<<23)
#define RBF_EOF       (1<<15)
#define RBF_SOF       (1<<14)

#define MAX_ETH_FRAME_LEN  0x600        /* 1536 bytes. Max ethernet frame size */

#define RBF_FRAMEMAX 64
#define RBF_FRAMELEN 0x600

#define TBF_FRAMEMAX 64
#define TBF_FRAMELEN 0x600

#define TX_DATA_RAM 0x30550000
#define RX_DATA_RAM 0x30580000

#ifdef CONFIG_DRIVER_ETHER

#if (CONFIG_COMMANDS & CFG_CMD_NET)

/* alignment as per Errata #11 (64 bytes) is insufficient! */

unsigned int RxBuffIndex = 0;
unsigned int TxBuffIndex = 0;


SEP4020_PhyOps PhyOps;

SEP4020P_EMAC p_mac;



/*---------------------------------------------------------------------------- */
/* \fn    AT91F_EMACInit */
/* \brief This function initialise the ethernet */
/* \return Status ( Success = 0) */
/*---------------------------------------------------------------------------- */
int SEP4020_EMACInit(bd_t * bd)
		  // unsigned int pRxTdList,
		  // unsigned int pTxTdList)
{
	unsigned int tick = 0;
	unsigned short status;
        unsigned int i;
        unsigned long Mac_CTLR;
        unsigned long Mac_CTLR_READ;
        unsigned int tempval;
         unsigned int tempaddr;	  
	/* Wait for PHY auto negotiation completed */
	

	//printf ("End of Autonegociation\n\r");
	
    INTC_IMR |= (0x01<<SEP4020_ID_EMAC);
    MAC_INTMASK = 0x0;
	/* the sequence write EMAC_SA1L and write EMAC_SA1H must be respected */
    MAC_CTRL = 0x0;
     udelay(1000);
     
  
    MAC_ADDR0 = (bd->bi_enetaddr[2] << 24) | (bd->bi_enetaddr[3] << 16)
			 | (bd->bi_enetaddr[4] <<  8) | (bd->bi_enetaddr[5]);
    MAC_ADDR1 = (bd->bi_enetaddr[0] <<  8) | (bd->bi_enetaddr[1]);

	
       
       RxBuffIndex = 0;
       TxBuffIndex = 0;

       tempval= MAC_TXBD_NUM;

        
        //初始化发送和接收描述符
        for(i=0;i<64;i++)
    {
        tempaddr=(MAC_TX_BD+i*8);
        write_reg(tempaddr,0);
        tempaddr=(MAC_TX_BD+i*8+4);
        write_reg(tempaddr,0);
    }
      for(i=0;i<64;i++)
    {
        tempaddr=(MAC_RX_BD+i*8);
        write_reg(tempaddr,0);
        tempaddr=(MAC_RX_BD+i*8+4);
        write_reg(tempaddr,0);
    }
    
    for(i=0;i<64;i++)
    {
        tempaddr=(MAC_RX_BD+i*8);
          if(i==63) tempval=0xe000;
        	else  tempval=0xc000;
        write_reg(tempaddr,tempval);
    }
    
	//填写发送和接收描述符的缓冲区
	for(i=0;i<64;i++)
    {
       

     tempval = TX_DATA_RAM + i*MAX_ETH_FRAME_LEN;

       tempaddr = (MAC_TX_BD+i*8+4);
        write_reg(tempaddr,(unsigned int) tempval);
       

         tempaddr=(MAC_RX_BD+i*8+4);
        

       tempval = RX_DATA_RAM + i*MAX_ETH_FRAME_LEN;

          write_reg(tempaddr,(unsigned int)tempval);
}
		//printf("Malloc all right!\n\r");
		
#if debug_mac_loopback
	Mac_CTLR = 0xaaa3;
#else

	Mac_CTLR = 0xa04b;
	Mac_CTLR &= ~0x0800;
#endif
	/*配置MAC控制寄存器*/
	MAC_CTRL = Mac_CTLR;
	Mac_CTLR_READ = MAC_CTRL;
	 printf ("Set the MAC successful!\n\r");
	return 0;
}

int eth_init (bd_t * bd)
{
	unsigned int periphAEnable, periphBEnable;
	unsigned int val, i;
	int ret;
      unsigned long Mac_CTLR_READ;
      unsigned short  New_Phy_BMCR;

	p_mac = MAC_BASE ;
        
      Mac_CTLR_READ = MAC_CTRL;
      New_Phy_BMCR = NetNIC_PhyRegRd(AT91C_PHY_ADDR, DM9161AE_BMCR);

      if(Mac_CTLR_READ !=0xa04b||New_Phy_BMCR!=0x0)
	{
#ifdef CONFIG_SEP4020_USE_RMII
	//printf("Use RMII\r\n");
	periphBEnable = 0;
#endif
	

	at91_GetPhyInterface (&PhyOps);

	if (!PhyOps.IsPhyConnected (p_mac))
	{
		printf ("PHY not connected!!\n\r");
		return 0;
	}

	/* MII management start from here */
	if (!(ret = PhyOps.Init (p_mac)))
	{
		printf ("MAC: error during RMII initialization\n");
		return 0;
	}
	
	SEP4020_EMACInit(bd); 
}			
	return 0;
}

int eth_send (volatile void *packet, int length)
{

        U32  leng;	  /* The length of a packet*/
	U32  i,j;
	U32 Adress;
	U32  Temp_Tx_BD;
	U32 temp_val;


	  leng = length;
	  
	for(i=TxBuffIndex;i<TBF_FRAMEMAX;)
		{
		    Adress = MAC_TX_BD+ i*8;
			Temp_Tx_BD = read_reg(Adress);
			if(!(Temp_Tx_BD & 0x8000))
			{
;
			if(i == TBF_FRAMEMAX-1) TxBuffIndex = 0;
			else TxBuffIndex = i+1;
			 break;
			 }
			if(i == TBF_FRAMEMAX-1) i = 0;
			else i++;
		}
		
		Adress = ((MAC_TX_BD + 4) + i*8);
		temp_val = read_reg(Adress);

		memcpy((unsigned char *)(temp_val), (unsigned char *)packet, leng);

               /*设置描述符的bit15位，发送帧数据*/
		leng = leng <<16;
	
		if(i == TBF_FRAMEMAX - 1)
			leng |= 0xb800;
		else 
			leng |= 0x9800;
			
		Adress = (MAC_TX_BD + i*8);
		write_reg(Adress, leng);
		
		//等待发送过程完成
		while(1)
		{
			//for(j=0;j<0x10000;j++);	
			Adress = MAC_TX_BD+ i*8;
			Temp_Tx_BD = read_reg(Adress);
			if(!(Temp_Tx_BD & 0x8000)) break;
		}

	return 0;
}


int eth_rx (void)
{
	int size = 0;
	unsigned char *rx_pkt_tmp = (unsigned char *) NetRxPackets[0];
	U32  length;	  /* The length of a packet*/
	U32  i,j,old;
	U32 Adress;
	U32  Temp_Rx_BD;
	U32 Temp_Rx_Ram;

   size=0;
	old = RxBuffIndex;

			/*接收数据中断*/
			/*记录接收错误，或者接受成功*/
			
			/*调用Mac_Receive接收数据*/
		
		
	for(i=old;i<old+4;i++)
		{  
                      if(i>=64) j=i-64;
                      else j=i;

                       Adress = MAC_RX_BD+ j*8;
			Temp_Rx_BD = read_reg(Adress);
			if(!(Temp_Rx_BD & 0x8000))
			{
				udelay(20);
				/*获得接收数据长度*/
				Adress = MAC_RX_BD+ j*8;
				length = read_reg(Adress);

				if(length&0x0040)
					printf("The receive data %d is overrun",j);
				if(length&0x0020)
					printf("The receive data %d is invalid_sym",j);
				if(length&0x0010)
					printf("The receive data %d is dibble_nibble",j);
					if(length&0x0008)
					printf("The receive data %d is too_long",j);
					if(length&0x0004)
					printf("The receive data %d is too_small",j);
					if(length&0x0002)
					printf("The receive data %d is crc_error",j);

				length = length>>16;
				size=length&0xffff;
				/*获得该描述符的数据存放地址*/
				write_reg(Adress,0xc000);
                            if(j==RBF_FRAMEMAX-1)  write_reg(Adress,0xe000);
  
				Adress = MAC_RX_BD+ j*8 + 4;
				Temp_Rx_Ram = read_reg(Adress);

			memcpy((unsigned char *)rx_pkt_tmp, (unsigned char *)(Temp_Rx_Ram), size);
				
				
				memset((unsigned char *)Temp_Rx_Ram,0,MAX_ETH_FRAME_LEN);
		   NetReceive (NetRxPackets[0], size);
		
				RxBuffIndex = j +1;    //接收描述符向后移一步
			if(RxBuffIndex == 64) RxBuffIndex = 0;
				return size;

			 }

		
  }

	return size;
}

void eth_halt (void)
{
	return;
};


#endif	/* CONFIG_COMMANDS & CFG_CMD_NET */

#endif	/* CONFIG_DRIVER_ETHER */
