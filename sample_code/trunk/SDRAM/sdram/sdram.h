#ifndef __SDRAM_H
#define __SDRAM_H


void BasicDataTran(U32 src_addr, U32 dest_addr,U32 length);  /*�������ݴ���*/
U32   COMPARE_MEM(U32 src_addr, U32 dest_addr,U32 length);   /*�Ƚ�ԭĿ��ַ�����Ƿ�һ��*/

extern void INI_EMI(void);
 

#endif
