#ifndef __SDRAM_H
#define __SDRAM_H


void BasicDataTran(U32 src_addr, U32 dest_addr,U32 length);  /*基本数据传输*/
U32   COMPARE_MEM(U32 src_addr, U32 dest_addr,U32 length);   /*比较原目地址内容是否一致*/

extern void INI_EMI(void);
 

#endif
