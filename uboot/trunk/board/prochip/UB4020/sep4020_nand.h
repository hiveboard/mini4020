
#ifndef _SEP4020_NAND_H
#define _SEP4020_NAND_H
#include <common.h>
#include <malloc.h>
#include <asm/arch/hardware.h>


#define NAND_ADDR_PHY			EMI_BASE+0x200


typedef enum {
	NFCE_LOW,
	NFCE_HIGH
} NFCE_STATE;

extern unsigned int * NF_Buf;

static inline void NF_Conf(unsigned int conf1,unsigned int conf2)
{
	EMI_NAND_CONF1 = conf1;
	EMI_NAND_CONF2 = conf2;
}

static inline void NF_Cmd(unsigned char command,unsigned long nandptr)
{
	EMI_NAND_COM = (0x80000000+command);

}

static inline void NF_CmdW(u8 cmd)
{
	NF_Cmd(cmd,0);
	udelay(1);
}

static inline void NF_Addr(unsigned int ofs,unsigned long nandptr)
{
	EMI_NAND_ADDR1 = ofs;
}

static inline void NF_SetCE()
{
	
}

static inline void NF_WaitRB(void)
{
	unsigned long tmp,i=0;

	while(!EMI_NAND_IDLE){		//Not IDLE? Read the FIFO until empty!
		tmp=EMI_NAND_DATA;
		if(i++>1000)break;
	}
	
	if(!EMI_NAND_IDLE)		//Still BUSY? Wait 1000 uS!
		udelay(1000);

	if(!EMI_NAND_IDLE){		//My GOD,I give up!
		printf("\There mustbe something wrong with the EMI,I can't send command to read status!\n");
		return ;
	}
	
	do{
		EMI_NAND_COM =0x80000070;	//read the status of nand
		while(!(EMI_NAND_STA &0x0100));	//wait until get the status
	}while (!(EMI_NAND_STA & 0x40));    //wait until get ready
}

static inline void NF_Write(unsigned int data, unsigned long ptr)
{
	EMI_NAND_DATA = data;
}

static inline unsigned int NF_Read(void)
{
	return EMI_NAND_DATA;
}

static inline void NF_Init_ECC(void)
{

}

static inline u32 NF_Read_ECC(void)
{
	return 0;
}

static inline void NF_Init(void)
{
//	NF_Conf(0x06302857,0x00514353);
	NF_Conf(0x02300622,0x00485252);
	printf ("%4lu MB,nand_config1 is 0x%x\n", nand_probe((ulong)NAND_ADDR_PHY) >> 20,0x02300622);
}

#endif
