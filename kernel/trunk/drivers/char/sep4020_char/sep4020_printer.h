#ifndef SMP150_H
#define SMP150_H

typedef char           INT8;  
typedef char           CHAR; 
typedef unsigned char  UINT8; 
typedef signed short   INT16; 
typedef unsigned short UINT16;
typedef signed long    INT32; 
typedef unsigned long  UINT32;
typedef int            STATUS;
typedef int            INT;
typedef void (*TIMER_ISR ) (void);
typedef void (*TIMER_ENABLE) (UINT32);
typedef void (*TIMER_DISALBE) (UINT32);

typedef struct printer_timer {
	UINT32    timer_index;
	UINT32    val;
	TIMER_ISR isr;
	TIMER_ENABLE enable;
	TIMER_DISALBE disable;
}PRT_TIMER;

/* Global variables of Printer space,  this space 
 * contains the common control vars about printer:
 * such as carriage, paper feed motor and timer .
 */
typedef struct printer_space {
	UINT32    cr_mov_dir;
	UINT16    cr_acc_steps;      /* index of slow-up table */
	UINT16    cr_cnst_steps;
	UINT16    cr_dec_steps;      /* index of slow-down table */
	UINT16    cr_reach_end;      /* flag indicate if we have reach the cr end */
	UINT16    cr_prt_start_pos;  /* print starts at one position of constant area, 
                                                        smp136 has a 180 constant steps area and 1 step 
                                                        contains 2 print positions. */
	UINT16    cr_prt_pos_num;    /* num of print-postion is obtained from the num of 
	                                                  charaters to be printed.  */
	
	UINT16    cr_pos_index;  
	
	UINT32    pf_mov_dir;
	UINT16    pf_acc_steps;
	UINT16    pf_cst_steps;
	UINT16    pf_dec_steps;
	
	UINT16     pf_coordinate;

	UINT16     pf_pos_index;

	PRT_TIMER    cr_timer;
	PRT_TIMER    pf_timer;
	PRT_TIMER	 prt_pulse_timer;

	UINT8    *prt_buf;         /* low level print buffer */
	UINT32    prt_buf_size;    /* size of prt buffer */
	UINT32    prt_buf_num;     /* num of print data */

	UINT32    hp_found;    /* flag indicates if found home position */
	UINT32    bm_found;    /* flag indicates if found black mark */
	UINT32    bm_check;
	UINT32    pe_check;
	
	INT       paper_change; 
	STATUS    prt_status;
	CHAR *    prt_row_str;
	INT       prt_pos;
    
} PRT_SPACE;

extern PRT_SPACE g_prt;

#define  CR_NO_PRTINT   0xffff  /* indicates there are no printing positions */
#define  PRINT_MAX_POS  320
#define  PRT_STA_POS    0x10

#define CR_PHASE_MASK		0x03
#define CR_ACC_AREA			0
#define CR_CNST_AREA		1
#define CR_DEC_AREA			2
#define CR_REACH_END		3

#define CR_MOV_A2B		0
#define CR_MOV_B2A		1
#define CR_ACC_STEPS_NUM	14
#define CR_CNST_STEPS_NUM  180
#define CR_DEC_STEPS_NUM	14

#define PF_PHASE_MASK		0x03
#define PF_ACC_AREA			0
#define PF_CNST_AREA		1
#define PF_DEC_AREA			2

#define PF_MOV_FW    0
#define PF_MOV_BW    1
#if 0
#define PF_ACC_STEPS_NUM	4
#define PF_CNST_STEPS_NUM 	16
#define PF_DEC_STEPS_NUM	4
#else
#define PF_ACC_STEPS_NUM	9
#define PF_CNST_STEPS_NUM 	6
#define PF_DEC_STEPS_NUM	9	
#endif
#define PF_MOVE_STEPS_NUM		(PF_ACC_STEPS_NUM + PF_CNST_STEPS_NUM + PF_DEC_STEPS_NUM)

#define SMP_GET_HP      0
#define SMP_INIT_OK		0

#define SMP_MISS_HP		-100
#define SMP_MISS_PAPER  -101

#define	PF_INIT_OK      0
#define	BM_INIT_OK      1
#define BM_UNFOUND      -1

#define	SMP_BOARD_PAPER -102
#define SMP_EAGE_PAPER  2
#define PRT_OK				0
#define PRT_INVALID_ARG	-10


#define HP_CHECK()		do{tmp_hp = hp_detect();} while(0)
#define HP_HIGH_CHECK()	  do{if(!hp_detect()) {\
							status =  SMP_MISS_HP; return status; }} while(0)
							
#define HP_LOW_CHECK()	  do{if(hp_detect()) { \
							status = SMP_MISS_HP; return status;}} while(0)

#define WAIT_UNTIL_CR2END()  do{pprt->cr_dec_steps = 0;  (*pprt->cr_timer.enable)(pprt->cr_timer.timer_index);\
                                                   while (pprt->cr_reach_end == 0);}while(0)
                                                   
                                                  
												   	
#define WAIT_PF_REACH(step, VAL) do{while (step != VAL);}while(0)

#define START_PF_MOTOR()  do{set_port_low(LF_STEP_SIG);}while(0)
#define STOP_PF_MOTOR()    do{set_port_high(LF_STEP_SIG);}while(0)

#define STOP_CR_MOTOR()    do{set_port_high(CR_I0); set_port_high(CR_I1);}while(0)
#define HOLD_CR_MOTOR()    do{set_port_high(CR_I1); set_port_low(CR_I0);}while(0)
#define START_CR_MOTOR()  do{set_port_low(CR_I0); set_port_low(CR_I1);}while(0)

#define CR_HOLD_MS(ms)     do{HOLD_CR_MOTOR(); udelay(500);}while(0)
//#define PF_HOLD_MS(ms)     do{STOP_PF_MOTOR(); mdelay(5*ms);}while(0)

/* Printer PORT Definition 
 * USE LOW 5 bits to indicate the port bit ID, HIGH 3 bits for 
 * PORT GROUP ID (GPIO PORT A - H).
 */
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

#define PRINT_PORTS        PORTB_ID
#define PRINT_TIMER_INT     (1 << 28)     /* It is the same as timer. */

/* FORMAT: (DIR |PORTX_ID | BITS_ID) 
 * BITS_ID is range between 0 and 31 
 */

/* DIR is bit8 0 -- mcu to printer
 *		       1 -- mcu from printer 
 */
#define PORT_DIR_IN        (0x1 << 8)
#define PORT_DIR_OUT       (0x0 << 8)
#define PORT_DIR_MASK      (0x1 << 8)
#define PORT_DIR_BIT_POS   8
 
#define CR_M_A		(PORT_DIR_OUT | PORTF_ID | 0x03)   //GPF3
#define CR_M_B		(PORT_DIR_OUT | PORTE_ID | 0x07)   //GPE7


#define CR_I0		(PORT_DIR_OUT | PORTF_ID | 0x01)   //GPF1		/* CR_I01_I02 */
#define CR_I1		(PORT_DIR_OUT | PORTF_ID | 0x02)   //GPF2		/* CR_I11_I12 */


#define PRT_HEAD_TRIG	(PORT_DIR_OUT | PORTF_ID | 0x04)   //GPF4


#define PRT_HEAD1	 (PORT_DIR_OUT | PORTC_ID | 0x00)   //GPC0		
#define PRT_HEAD2    (PORT_DIR_OUT | PORTC_ID | 0x01)   //GPC1
#define PRT_HEAD3    (PORT_DIR_OUT | PORTC_ID | 0x02)   //GPC2
#define PRT_HEAD4    (PORT_DIR_OUT | PORTC_ID | 0x03)   //GPC3
#define PRT_HEAD5    (PORT_DIR_OUT | PORTC_ID | 0x04)   //GPC4
#define PRT_HEAD6    (PORT_DIR_OUT | PORTC_ID | 0x05)   //GPC5
#define PRT_HEAD7    (PORT_DIR_OUT | PORTC_ID | 0x06)   //GPC6
#define PRT_HEAD8    (PORT_DIR_OUT | PORTC_ID | 0x07)   //GPC7

#define LF_STEP_A     (PORT_DIR_OUT | PORTG_ID | 0x0B)  //GPG11
#define LF_STEP_B     (PORT_DIR_OUT | PORTG_ID | 0x0C)  //GPG12
#define LF_STEP_SIG   (PORT_DIR_OUT | PORTG_ID | 0x0A)  //GPG10

//#define IO_TEST      (PORT_DIR_OUT | PORTC_ID | 0x06)  //GPC6


#define HP_SEN		(PORT_DIR_IN | PORTE_ID | 0x0A)	  //GPE10
#define PE_SEN		(PORT_DIR_IN | PORTA_ID | 0x09)	  //GPA9
#define BM_SEN		(PORT_DIR_IN | PORTE_ID | 0x0B)   //GPE11

/* UINTS: 1us */
//#define TIMER_UINT		56     /* system clock is 50MHz */
#define TIMER_UINT		88     /*system clock is 88MHz*/

#define HEAD_TRIG_CYCLE	310
#define PRINT_PULSE_CYCLE	310
#define PRINT_HEAD_IDLE_CYCLE 210

#define OS_TIMER_INDEX  0
#define CR_TIMER_INDEX   2
#define PF_TIMER_INDEX  3
#define PRINT_PULSE_TIMER  1

#if 1
#define BL_Start 265
#define BL_End   345
#define BL_Width 75
#define BM_Sure    65
#define PAP_Ready  185       //切纸位置
#define PRT_Start  140      //打印起始行

#define BRO_STEP   5
#endif

#define PAPER_LEN  577


#define PRT_POS    180
#define BM_WINDOW  40

#define BM_WINDOW_NUM  8 

#define BM_Max    0x4A
#define BM_Min    0x40 
#define Print_Start 30

#define CHANGE_PAGE 0x01
#define CHANGE_ROW  0x02

#define  cr_ori_num     90
#define  pf_ori_num     200
#define  pf_mov_num     38

#define Paper_miss       0x00
#define Print_OK         0x01
#define Print_Failed     0x02
#define Print_board      0x03
#define Print_error      0x04
#define Print_Unkown     0x05

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
/*
typedef int (*PRINT_FONT) (char*, int);

typedef struct {
	int font_size;
	unsigned short *prt_buf;
	PRINT_FONT prt_fun; 	
}PRINT_FONT_LIST;
*/
static void set_timer_val(int timer, UINT32 val);
static void print_pulse_drive(void);
static STATUS cr_ori(void);
static STATUS pf_ori(UINT16 pf_step);
static STATUS printer_ready(void);

#endif
//////////////////////////////////////////////////////////////////////////////////////////////5.28
