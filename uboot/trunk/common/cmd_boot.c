/*
 * (C) Copyright 2000-2003
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
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

/*
 * Misc boot support
 */
#include <common.h>
#include <command.h>
#include <net.h>

/* Allow ports to override the default behavior */
__attribute__((weak))
unsigned long do_go_exec (ulong (*entry)(int, char *[]), int argc, char *argv[])
{
	return entry (argc, argv);
}

void BasicDataTran(ulong src_addr, ulong dest_addr,ulong length)
{
    ulong i;
    
   for (i=0; i<length; i++)
    {
  	    *(volatile u16 *)(dest_addr+2*i) = *(volatile u16 *)(src_addr+2*i);
    }
    
    
}

/***************************************************
* FUNCTION :
*             COMPARE_MEM
*
* DESCRIPTION:
*             比较搬运后,目的地址上的内容
*             与源地址上的内容是否一致
* INPUTS:
*             src_addr     源地址
*             dest_addr    目的地址
*             length       所搬运数据的长度
* OUTPUTS:
*             返回flag=E_OK表示成功
*             返回flag=E_HA表示失败 
***************************************************/

ulong COMPARE_MEM(ulong src_addr, ulong dest_addr, ulong length)
{
    ulong i;
    ulong flag = 0;

    for(i=0; i<length; i++)
    {
        if(*(volatile u16 *)(src_addr+i*2) != *(volatile u16 *)(dest_addr+i*2))
        {
    	    flag = -1;
    	    
    	   // break;
        }
    }
    return flag;  
}


int test_sdram(void)
{
   u16 i;
   
    /*对空间从0x30001000起，向后4K的空间初始化*/
   for(i=0;i<1024;i++)
    *(volatile u16 *)(0x30001000+2*i) = i;
   
  /********** 开始SDRAM数据传输测试 *****************************************************************
  * 在AXD中跟踪观察.此项测试也可直接打开AXD写SDRAM,看有没有写是否都正确
  **************************************************************************************************/ 
   
    
    printf("Tests begin!\n");                                  /*SDRAM测试*/

    //多块传输
    
    for(i=0;i<=0x1000;i++) 
    {
        BasicDataTran(0x30001000, 0x31000000+i*(0x1000),1024);
        
        if (COMPARE_MEM(0x30001000, 0x31000000+i*(0x1000),1024) != 0)
        {
          printf("Move data from 0x30001000 to 0x30003000 + %d *0x1000  failure!\n",i);
        }  
        else
        {
          // printf("Move data from 0x30001000 to 0x30003000 + %d *0x1000  successfully!\n",i);
        }
    }
    
    printf("sdram test over!\n");         

    return 0; //为了去除警告信息
}

int do_go (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	ulong	addr, rc;
	int     rcode = 0;
/*
	if (argc < 2) {
		printf ("Usage:\n%s\n", cmdtp->usage);
		return 1;
	}

	addr = simple_strtoul(argv[1], NULL, 16);
*/
	printf ("## Starting application at 0x%08lX ...\n", addr);

	/*
	 * pass address parameter as argv[0] (aka command name),
	 * and all remaining args
	 */
	//rc = do_go_exec ((void *)addr, argc - 1, argv + 1);
	//if (rc != 0) rcode = 1;
	*(volatile ulong *)(0x10003008) = 0x0;
	*(volatile ulong *)(0x10003000) = 0xff000000;
	*(volatile ulong *)(0x10003008) = 0x7;
	rc = *(volatile ulong *)(0x10003004);
	test_sdram();
	rc = rc - *(volatile ulong *)(0x10003004);
	*(volatile ulong *)(0x10003008) = 0x0;
/*
int count1=0,count2=1,count3=2,count4=3,i=0;
   
   
   for(i=0; i < 30000000; i++)
   {
   		count1 += i;
   		count3 += (count1 + count2)*3;
   		count2 = (count3 & i + count1) & count4;
   		count3 +=count2;
   		count4 &= (count1 | count2);
   } 
*/
	//printf ("## Application terminated, rc = 0x%lX\n", rc);
printf ("## Application terminated,the time is %d\n",rc/88);
	return rcode;
}

/* -------------------------------------------------------------------- */

U_BOOT_CMD(
	go, CFG_MAXARGS, 1,	do_go,
	"go      - start application at address 'addr'\n",
	"addr [arg ...]\n    - start application at address 'addr'\n"
	"      passing 'arg' as arguments\n"
);

extern int do_reset (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[]);

U_BOOT_CMD(
	reset, 1, 0,	do_reset,
	"reset   - Perform RESET of the CPU\n",
	NULL
);
