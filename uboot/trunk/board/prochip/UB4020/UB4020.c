/*
 * (C) Copyright 2004
 * DAVE Srl
 * http://www.dave-tech.it
 * http://www.wawnet.biz
 * mailto:info@wawnet.biz
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
#include <asm/arch/hardware.h>
#include "sep4020_nand.h"
#include <dm9161.h>
#include <net.h>

unsigned int * NF_Buf;

/*
 * Miscelaneous platform dependent initialization
 */

int board_init (void)
{
	
	DECLARE_GLOBAL_DATA_PTR;

	/*cancel the protect of NorFlash*/
	GPIO_PORTG_SEL |= (unsigned int)(1 << 7);
	GPIO_PORTG_DATA |= (unsigned int)(1 << 7);
	GPIO_PORTG_DIR &= (unsigned int)~(1 << 7);
//机器号和参数列表的地址在此写死的
	gd->bd->bi_arch_number = 0xc2;
	gd->bd->bi_boot_params = 0x30000100;

	return 0;
}
/*  //kyon
void nand_init(void)
{

	NF_Init();
}*/

int board_late_init(void)
{
  DECLARE_GLOBAL_DATA_PTR;
  
  /* Fix Ethernet Initialization Bug when starting Linux from U-Boot */

#if (CONFIG_COMMANDS & CFG_CMD_NET)

    eth_init(gd->bd);

#endif
	

      return 0;
}

int dram_init (void)
{
	DECLARE_GLOBAL_DATA_PTR;

	gd->bd->bi_dram[0].start = PHYS_SDRAM_1;
	gd->bd->bi_dram[0].size = PHYS_SDRAM_1_SIZE;

	return (0);
}
