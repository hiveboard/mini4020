//#define CLKPOL		(1 << 21)		//LCD ��λʱ�Ӽ��ԡ�����LCD ��λʱ�ӵ���Ч�صļ���,�½�����Ч. 10.27 ����Ϊ1


#ifndef __ASM_ARCH_FB_H
#define __ASM_ARCH_FB_H


#define LCDWIDTH	320
#define LCDHEIGHT	240

//Macro for SIZE register setting
#define XMAX		((LCDWIDTH/16)	<< 20)
#define YMAX		(LCDHEIGHT)

//Macro for PCR register setting
#define TFT		   (1 << 31)
#define COLOR		(1 << 30)		//1Ϊɫ��.
#define PBSIZE		(0 << 28)
#define BPIX		(4 << 25)		//�洢����һ�����ض�Ӧ16bit
#define PIXPOL		(0 << 24)		//���ؼ��ԡ��������صļ���,����Ч
#define FLMPOL		(1 << 23)		//���б�־���ԡ��������б�־�ļ���,����Ч.
#define LPPOL		(1 << 22)		//�����弫�ԡ������������źŵļ���,����Ч.
#define CLKPOL		(1 << 21)		//LCD ��λʱ�Ӽ��ԡ�����LCD ��λʱ�ӵ���Ч�صļ���,�½�����Ч. 1025 ����Ϊ1
#define OEPOL		(1 << 20)		//���ʹ�ܼ��ԡ��������ʹ���źŵļ���,����Ч.
#define END_SEL	(0 << 18)		//ӡ�ڰ���ʽѡ��,Сӡ�ڰ�.
#define ACD_SEL	(0 << 15)		//ACDʱ��Դѡ��ѡ����任��������ʱ��,ʹ��FLM ��ΪACD ����ʱ��
#define ACD		(0 << 8 )		//����ģʽ.
#define PCD		(10)

//Macro for HCR regiter setting	//ˮƽ���üĴ���.
//#define H_WIDTH	(50 << 26)
//#define H_WAIT_1	(15 << 8)
//#define H_WAIT_2	(16)
//Macro for HCR regiter setting	//ˮƽ���üĴ���.
#define H_WIDTH	(68 << 26)		//46
#define H_WAIT_1	(20 << 8)		//20
#define H_WAIT_2	(20)			//20

//Macro for VCR register setting	//��ֱ���üĴ���.
//#define V_WIDTH	(3 << 26)
//#define PASS_FRAME_WAIT	(0 <<16)
//#define V_WAIT_1	(10 << 8)
//#define V_WAIT_2	(10)
//Macro for VCR register setting	//��ֱ���üĴ���.
#define V_WIDTH	(18 << 26)
#define PASS_FRAME_WAIT	(0 <<16)
#define V_WAIT_1	(3 << 8)
#define V_WAIT_2	(3)

//Macro for LGPR register setting
#define GRAY		(4)			//�Ҷȵȼ�.

//Macro for PWMR register setting
#define SCR		(0 << 9)		//ʱ��Դѡ��.������.
#define CC_EN		(0 << 8)		//�Աȶȿ���ʹ��,��.
#define PW		(64)			//���������Ŀ,

//Macro for DMACR register setting
#define BL		(1 << 31)		//�̶�����.
#define HM		(7 << 16)
#define TM		(3)

#endif


