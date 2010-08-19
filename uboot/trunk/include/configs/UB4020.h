/*
 * (C) Copyright 2004
 * DAVE Srl
 *
 * http://www.dave-tech.it
 * http://www.wawnet.biz
 * mailto:info@wawnet.biz
 *
 * Configuation settings for the B2 board.
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

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define CONFIG_ARM7			1	/* This is a ARM7 CPU	*/
#define CONFIG_UB4020				1	/* on an B2 Board      */
#define CONFIG_ARM_THUMB		1	/* this is an ARM7TDMI */
#undef  CONFIG_ARM7_REVD	 		/* disable ARM720 REV.D Workarounds */

#define CONFIG_S3C44B0_CLOCK_SPEED	80 	/* we have a 75Mhz S3C44B0*/


#define CONFIG_USE_IRQ

#define CONFIG_ZIMAGE_BOOT			/*add by kyon for load zimage @ 090721*/				


/*
 * Size of malloc() pool
 */
#define CFG_MONITOR_LEN		(256 * 1024)			/* Reserve 256 kB for Monitor	*/
#define CFG_ENV_SIZE		0x4000				/* 1024 bytes may be used for env vars*/
#define CFG_MALLOC_LEN		(CFG_ENV_SIZE + 128*1024 )
#define CFG_GBL_DATA_SIZE	128					/* size in bytes reserved for initial data */

/*
 * Hardware drivers
 */
//#define CONFIG_UDP_CHECKSUM 	1
#define BOARD_LATE_INIT			1
#define CONFIG_DRIVER_ETHER         1
#define CONFIG_SEP4020_USE_RMII	1
#define SEP4020_PHY_ADDR		0
#define SEP4020_ETH_TIMEOUT		500000
#define CONFIG_NET_RETRY_COUNT	100
#define CONFIG_MAC_BASE			0x11003000 /* base address         */

#undef  CONFIG_SHOW_ACTIVITY 

/*
 * select serial console configuration
 */
#define CONFIG_SERIAL1			1	/* we use Serial line 1 */

/* allow to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

#define CONFIG_BAUDRATE		115200

#define CONFIG_BOOTP_MASK    	(CONFIG_BOOTP_DEFAULT|CONFIG_BOOTP_BOOTFILESIZE)



/******************************************************/
//add by kyon							//
/******************************************************/
/*
 * Command line configuration.
 */
#define CONFIG_CMD_AUTOSCRIPT	/* Autoscript Support		*/
#define CONFIG_CMD_BDI	       	/* bdinfo			*/
#define CONFIG_CMD_BOOTD	/* bootd			*/
#define CONFIG_CMD_CONSOLE	/* coninfo			*/
#define CONFIG_CMD_ECHO		/* echo arguments		*/
#define CONFIG_CMD_ENV		/* saveenv			*/
#define CONFIG_CMD_FLASH	/* flinfo, erase, protect	*/
//#define CONFIG_CMD_FPGA		/* FPGA configuration Support	*/
#define CONFIG_CMD_IMI		/* iminfo			*/
#define CONFIG_CMD_IMLS		/* List all found images	*/
#define CONFIG_CMD_ITEST	/* Integer (and string) test	*/
#define CONFIG_CMD_LOADB	/* loadb			*/
#define CONFIG_CMD_LOADS	/* loads			*/
#define CONFIG_CMD_MEMORY	/* md mm nm mw cp cmp crc base loop mtest */
#define CONFIG_CMD_MISC		/* Misc functions like sleep etc*/
#define CONFIG_CMD_NET		/* bootp, tftpboot, rarpboot	*/
//#define CONFIG_CMD_NFS		/* NFS support			*/
#define CONFIG_CMD_RUN		/* run command in env variable	*/
//#define CONFIG_CMD_SETGETDCR	/* DCR support on 4xx		*/
//#define CONFIG_CMD_XIMG		/* Load part of Multi Image	*/

/*
//add 0325
#define CONFIG_CMD_NAND
//#define CONFIG_CMD_NET
//#define CONFIG_CMD_PING
#define CONFIG_CMD_DHCP
#define CONFIG_CMD_DATE
#define CONFIG_CMD_ELF

#define CONFIG_COMMANDS	( CFG_CMD_NET | CFG_CMD_PING 	| CFG_CMD_DHCP )
*/
//end

#define CONFIG_COMMANDS	    ( CONFIG_CMD_DFL 	| \
										CFG_CMD_NET 		| \
			            			CFG_CMD_PING 		| \
                              CFG_CMD_DHCP 		| \
										CFG_CMD_DATE 		| \
										CFG_CMD_ELF			| \
										CFG_CMD_ENV			| \
										CFG_CMD_FLASH 		)

//CFG_CMD_NAND	| \


/* this must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <cmd_confdefs.h>  //kyon

#define CONFIG_BOOTDELAY	1
#define CONFIG_ETHADDR		00:50:c2:1e:af:fb
#define CONFIG_BOOTARGS  	"root=/dev/nfs rw nfsroot=192.168.0.3:/nfs ip=192.168.0.2:192.168.0.3:192.168.0.1:255.255.255.0:sep4020:eth0:off console=ttyS0,115200 mem=32mb" 
#define CONFIG_NETMASK  	255.255.255.0
#define CONFIG_IPADDR   	192.168.0.2
#define CONFIG_SERVERIP		192.168.0.1
#define CONFIG_GATEWAYIP 	192.168.0.1

#define CONFIG_BOOTFILE		"zImage.bin"
#define CONFIG_BOOTCOMMAND	"tftp 31000000 zImage;bootm"

/*
 * Miscellaneous configurable options
 */
#define	CFG_LONGHELP							/* undef to save memory		*/
#define	CFG_PROMPT		"SEP4020=>"					/* Monitor Command Prompt	*/
#define	CFG_CBSIZE		256						/* Console I/O Buffer Size	*/
#define	CFG_PBSIZE 		(CFG_CBSIZE+sizeof(CFG_PROMPT)+16) 	/* Print Buffer Size */
#define	CFG_MAXARGS		16						/* max number of command args	*/
#define 	CFG_BARGSIZE	CFG_CBSIZE					/* Boot Argument Buffer Size	*/

#define 	CFG_MEMTEST_START	0x30740000					/* memtest works on	*/
#define 	CFG_MEMTEST_END	0x30780000					/* 4 ... 8 MB in DRAM	*/

#undef  	CFG_CLKS_IN_HZ							/* everything, incl board info, in Hz */

#define	CFG_LOAD_ADDR	0x30008000					/* default load address	*/

#define	CFG_HZ		88000000					/* 72MHz */

						
#define 	CFG_BAUDRATE_TABLE	{ 9600, 19200, 38400, 57600, 115200 }	/* valid baudrates */

/*-----------------------------------------------------------------------
 * Stack sizes
 *
 * The stack sizes are set up in start.S using the settings below
 */
#define CONFIG_STACKSIZE	(128*1024)	/* regular stack */
#ifdef CONFIG_USE_IRQ
#define CONFIG_STACKSIZE_IRQ	(4*1024)	/* IRQ stack */
#define CONFIG_STACKSIZE_FIQ	(4*1024)	/* FIQ stack */
#endif

/*-----------------------------------------------------------------------
 * Physical Memory Map
 */
#define CONFIG_NR_DRAM_BANKS	1	   	/* we have 1 banks of DRAM */
#define PHYS_SDRAM_1		0x30000000 	/* SDRAM Bank #1 *//*modified by shixq*/

#define PHYS_SDRAM_1_SIZE	0x02000000 	/* 16 MB *//*32MB modified by shixq*/

#define PHYS_FLASH_1		0x20000000 	/* Flash Bank #1 *//*modified by shixq*/
#define PHYS_FLASH_SIZE		0x00200000 	/* 2 MB */

#define CFG_FLASH_BASE		PHYS_FLASH_1

/*-----------------------------------------------------------------------
 * FLASH and environment organization
 */
/*-----------------------------------------------------------------------
 * FLASH organization
 */
#define CFG_MAX_FLASH_BANKS	1	/* max number of memory banks		*/
#define CFG_MAX_FLASH_SECT	64	/* max number of sectors on one chip	*/

#define CFG_FLASH_ERASE_TOUT	120000	/* Timeout for Flash Erase (in ms)	*/
#define CFG_FLASH_WRITE_TOUT	1000	/* Timeout for Flash Write (in ms)	*/

#define CFG_FLASH_WORD_SIZE	unsigned short	/* flash word size (width)	*/
#define CFG_FLASH_ADDR0		0x5555	/* 1st address for flash config cycles	*/
#define CFG_FLASH_ADDR1		0x2AAA	/* 2nd address for flash config cycles	*/
/*
 * The following defines are added for buggy IOP480 byte interface.
 * All other boards should use the standard values (CPCI405 etc.)
 */
#define CFG_FLASH_READ0		0x0000	/* 0 is standard			*/
#define CFG_FLASH_READ1		0x0001	/* 1 is standard			*/
#define CFG_FLASH_READ2		0x0002	/* 2 is standard			*/

#define CFG_FLASH_EMPTY_INFO		/* print 'E' for empty sector on flinfo */



/*-----------------------------------------------------------------------
 * Environment Variable setup
 */

//#define CFG_ENV_IS_IN_FLASH 1
#undef CFG_ENV_IS_NOWHERE
//#define CFG_ENV_ADDR (CFG_FLASH_BASE + 0x00030000) /* environment start address */ 
#define CFG_ENV_IS_IN_NAND  1
#define CFG_ENV_OFFSET    (0xF0000)
#define CFG_ENV_SECT_SIZE 0x10000 /* Total Size of Environment Sector */

/* Flash banks JFFS2 should use */
/*
#define CFG_JFFS2_FIRST_BANK    	0
#define CFG_JFFS2_FIRST_SECTOR	2
#define CFG_JFFS2_NUM_BANKS     	1
*/

/*
	Linux TAGs (see lib_arm/armlinux.c)
*/
#define CONFIG_CMDLINE_TAG
#undef CONFIG_SETUP_MEMORY_TAGS
#define CONFIG_INITRD_TAG


/*--------------------------------------------------------------------------
*NandFlash organization
*/
//add by kyon

#define CONFIG_CMD_NAND


#define NAND_MAX_CHIPS 		1
#define CFG_NAND_BASE		0x11000200
//#define NAND_MAX
//end

#define CFG_MAX_NAND_DEVICE	1	/* Max number of NAND devices		*/
#define SECTORSIZE 		512

#define ADDR_COLUMN 		1
#define ADDR_PAGE 		2
#define ADDR_COLUMN_PAGE 	3

#define NAND_ChipID_UNKNOWN	0x00
#define NAND_MAX_FLOORS 	1

/*

#define NAND_WAIT_READY(nand) NF_WaitRB()

#define NAND_DISABLE_CE(nand) NF_SetCE()
#define NAND_ENABLE_CE(nand) 	NF_SetCE()


#define WRITE_NAND_COMMAND(command, nandptr) NF_Cmd(command, nandptr)
#define WRITE_NAND_COMMANDW(d, adr) NF_CmdW(d)
#define WRITE_NAND_ADDRESS(ofs, nandptr) NF_Addr(ofs, nandptr)
#define WRITE_NAND(d, adr) NF_Write(d, adr)
#define READ_NAND(adr) NF_Read()

*/

/* the following functions are NOP's because SEP4020 handles this in hardware */
#define NAND_CTL_CLRALE(nandptr)
#define NAND_CTL_SETALE(nandptr)
#define NAND_CTL_CLRCLE(nandptr)
#define NAND_CTL_SETCLE(nandptr)


#endif	/* __CONFIG_H */
