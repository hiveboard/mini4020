/* linux/drivers/char/sep4020_char/thermal_printer.c
*
*   Copyright (c) 2009 prochip company
*   http://www.prochip.com.cn
*	
*
*   sep4020 thermal_printer.
*
*   Changelog:
*   22-Jan-2009     fpmystar    Initial version
*   12-May-2009     mecxon      Current Version   	
*   24-May-2009     mecxon      Codereview Version   		
* 
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*/

#define U32 unsigned int
#define U16 unsigned short
#define S32 int
#define CHAR char
#define S16 short int
#define U8 unsigned char
#define E_OK      1
     
typedef	volatile U32 *		RP;
typedef	volatile U16 *		RP16;
typedef	volatile U8  *		RP8;

#define PORT_BITS_NUM		5
#define PORT_GRP_NUM		3
#define PORT_GRP_MASK		(0x7 << PORT_BITS_NUM)
#define PORT_BIT_MASK		0x1f

#define PORTA_ID		(0 << PORT_BITS_NUM)
#define PORTB_ID		(1 << PORT_BITS_NUM)
#define PORTC_ID		(2 << PORT_BITS_NUM)
#define PORTD_ID		(3 << PORT_BITS_NUM)
#define PORTE_ID		(4 << PORT_BITS_NUM)
#define PORTF_ID		(5 << PORT_BITS_NUM)
#define PORTG_ID		(6 << PORT_BITS_NUM)
#define PORTH_ID		(7 << PORT_BITS_NUM)

#define PRint_PORTS        PORTB_ID
#define PRint_TIMER_int     (1 << 28)     /* It is the same as timer. */

#define PORT_DIR_IN (0x1 << 8)
#define PORT_DIR_OUT (0x0 << 8)
#define PORT_DIR_MASK (0x1 << 8)
#define PORT_DIR_BIT_POS 8
 
#define PR_D0		(PORT_DIR_OUT | PORTG_ID | 0x00)
#define PR_D1		(PORT_DIR_OUT | PORTG_ID | 0x01)
#define PR_D2		(PORT_DIR_OUT | PORTG_ID | 0x02)
#define PR_D3		(PORT_DIR_OUT | PORTG_ID | 0x03)
#define PR_D4		(PORT_DIR_OUT | PORTG_ID | 0x04)
#define PR_D5		(PORT_DIR_OUT | PORTG_ID | 0x05)
#define PR_D6		(PORT_DIR_OUT | PORTG_ID | 0x06)
#define PR_D7		(PORT_DIR_OUT | PORTG_ID | 0x07)

#define TC1			(PORT_DIR_OUT | PORTA_ID | 0x06)

#define BL_CHK_IN	(PORT_DIR_IN | PORTA_ID | 0x03)		//δ֪
#define BLK_SEN		(PORT_DIR_IN | PORTA_ID | 0x02)	//
/*�¶ȿ��Ƽ���ź�PA4*/
#define TE_MPO		(PORT_DIR_IN | PORTA_ID | 0x09)

/*ʹ�ܲ������(14)PB5*/
#define PR_PWR		(PORT_DIR_OUT | PORTH_ID | 0x06)
	
/*��������(15)PB4*/
#define PR_LAT		(PORT_DIR_OUT | PORTC_ID | 0x02)
/*���Ƽ��ȿ�(16,17)PB3,PB2*/
#define PR_DST2		(PORT_DIR_OUT | PORTC_ID | 0x01)
#define PR_DST1		(PORT_DIR_OUT | PORTC_ID | 0x00)

/*������������ź�(18,19)PB1,PB0*/
#define CR_M_PB		(PORT_DIR_OUT | PORTE_ID | 0x04)
#define CR_M_PA		(PORT_DIR_OUT | PORTE_ID | 0x03)

/*��������ʹ���ź�(20)PC8*/
#define DATA_CS		(PORT_DIR_OUT | PORTE_ID | 0x02)

/*ȱֽLED�ź�PF6*/
#define NO_PAGE_LED	(PORT_DIR_OUT | PORTH_ID | 0x06)

#define CLK_CS	(PORT_DIR_OUT | PORTE_ID | 0x08)

#define set_dst1_high	set_port_low(PR_DST1);
#define set_dst1_low	set_port_high(PR_DST1);

#define set_dst2_high	set_port_low(PR_DST2);
#define set_dst2_low	set_port_high(PR_DST2);

#define set_lat_high	set_port_low(PR_LAT);
#define set_lat_low		set_port_high(PR_LAT);

#define	DATA_PORT_DIR		GPIO_PORTG_DIR_V
#define	DATA_PORT_SEL		GPIO_PORTG_SEL_V
#define	DATA_PORT_DATA		GPIO_PORTG_DATA_V

#define 	PRT_FONT24X24        	24				//һ�к�����ռ�ĵ�����
#define 	MAX_NUM_FONT24x24		12				//һ����������
#define 	MAX_ROW_FONT24x24		20				//һҳ��������

#define		SECRETSTART				30
#define		SECRETEND				40

#define	 NOWFONT					-1
#define  SUCCESS					0x00
#define  INITTION					0x01
#define  UNKONWENDES				0x02
#define  PRintNOPAPER				0x03
#define  PRintNOCONTENT				0x04
#define  PRintERROR					0x05
#define  PRintPAPERERROR			0x05
#define  UNDESERROR					0x0C

/* UintS: 1us */
#define TIMER_Uint		50     /* system clock is 50MHz */

#define OS_TIMER_INDEX  0
#define MOTOR_TIMER_INDEX   3
#define TEM_TIMER_INDEX  2
#define PRint_PULSE_TIMER  4

#define INDEX_FONT16X16       0
#define INDEX_FONT9X9	      1
#define INDEX_FONT12X16       2
#define PRT_FONT16X16        16
#define PRT_FONT12X16        12
#define MAX_NUM_FONT16x16	 20		
#define PRT_FONT9X9		     9	
#define MAX_NUM_FONT9x9	40	

#define MAX_COL_FONT16x16	 24			
#define MAX_BUF_SIZE         180
#define FONT16x16_ROW_MSB		(0x0001 << 15)
#define PRT16x16_BUF_SIZE	(PRT_FONT16X16 * MAX_NUM_FONT16x16)
#define PRT9x9_BUF_SIZE    (PRT_FONT9X9 * MAX_NUM_FONT9x9)
#define ADJUST_PRT_BUF_SIZE PRT16x16_BUF_SIZE

#define CHECK_BM  1
#define NO_CHECK_BM 0
#define dly_test1    (1500/2)


/* Global variables of Printer space,  this space 
 * contains the common control vars about printer:
 * such as carriage, paper feed motor and timer .
 */
typedef struct printer_space {
	unsigned long	point_row[MAX_ROW_FONT24x24*PRT_FONT24X24][2];		//ÿ������еĵ�������->����ʱ��					
	
	unsigned short	momtor_step;                                    //��ֽ���������ȫ�ּ�����
	unsigned short	tem_step;                                       //���ȵ��������ȫ�ּ�����
	
	struct cdev cdev;
	unsigned long  total_rownum;						//��Ҫ��ӡ��������
	unsigned long  momtor_speed;   						//��ֽ������ٶ�(��ֽ�����ʱ���ļ��)
	
	unsigned char   row_interval;   						//ÿһ�к���֮��ļ��(�Ƽ�ȡֵΪ���ֵ����2/3,��24*24�ĺ���,��ȡ18)
	unsigned char	word_interval;						//ÿ������֮��ļ��(��ֵΪ8�ı���,��ȡ1,����֮��ļ��Ϊ8������
	unsigned char	black_check;							 //�ڱ����־(1-��Ҫ���,0-����Ҫ���)    
	int	status;
 	spinlock_t my_lock;						    		//����һ��������
 	char font_buf[17280];                            			//thermal printer buffer
	struct completion	complete_request;               			//define spin-lock
} PRT_SPACE;

extern PRT_SPACE t_prt;
void init_thermal(void);
void motor_timer_isr(void);
void tem_timer_isr(void);

