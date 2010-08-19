/* linux/drivers/char/sep4020_char/sep4020_printer.c
 *
* Copyright (c) 2009 prochip company
*	http://www.prochip.com.cn
*	leeming1203@gmail.com.cn
*
* sep4020 printer driver.
*
* Changelog:
*	20-jan-2009 zcw	Initial version
*	27-Apr-2009 zcw  	fixed	a lot
 *
 * 
 *
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*/
#include <linux/module.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/errno.h>
#include <linux/mm.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/interrupt.h>

#include <linux/delay.h>
#include <asm/system.h>
#include <asm/uaccess.h>
#include <asm/io.h>
#include <asm/hardware.h>
#include "sep4020_printer.h"

#define PRINTER_MAJOR  250
#define PRINTER_ON  1
#define PRINTER_TEST 2
 
PRT_SPACE g_prt;

struct printer_dev
{
	struct cdev cdev;
	int status;
        
};
struct printer_dev *printerdev;

struct dot{
    unsigned short g_high_buf[MAX_BUF_SIZE];
    unsigned short g_low_buf[MAX_BUF_SIZE];
    unsigned short cr_prt_pos_num;
    int status;
}dot_space[20];

UINT16 g_prt_adjust_buf[ADJUST_PRT_BUF_SIZE];
UINT16 g_prt16x16_buf[PRT16x16_BUF_SIZE];
UINT16 g_high_buf[MAX_BUF_SIZE];
UINT16 g_low_buf[MAX_BUF_SIZE];

UINT32 Port_Sel_Regs[9] = { \
                GPIO_PORTA_SEL_V, GPIO_PORTB_SEL_V, GPIO_PORTC_SEL_V, GPIO_PORTD_SEL_V, \
                GPIO_PORTE_SEL_V, GPIO_PORTF_SEL_V, GPIO_PORTG_SEL_V, GPIO_PORTH_SEL_V, GPIO_PORTI_SEL_V};

UINT32 Port_Dir_Regs[9] = { \
                GPIO_PORTA_DIR_V, GPIO_PORTB_DIR_V, GPIO_PORTC_DIR_V, GPIO_PORTD_DIR_V, \
                GPIO_PORTE_DIR_V, GPIO_PORTF_DIR_V, GPIO_PORTG_DIR_V, GPIO_PORTH_DIR_V, GPIO_PORTI_DIR_V};

UINT32 Port_Data_Regs[9] = { \
                GPIO_PORTA_DATA_V, GPIO_PORTB_DATA_V, GPIO_PORTC_DATA_V, GPIO_PORTD_DATA_V, \
                GPIO_PORTE_DATA_V, GPIO_PORTF_DATA_V, GPIO_PORTG_DATA_V, GPIO_PORTH_DATA_V, GPIO_PORTI_DATA_V};

/* GPIO Register Macro */
#define REG32(addr)	(*(volatile unsigned long*)(addr))


/* Motor Movements Table: UINITS: 10us */

UINT16 CR_SlowUp_Table[CR_ACC_STEPS_NUM] = { \
		  5850, 4260, 2150, 1630, 1420, 1290, 1200, 1140, 1090, 1050, 1020, 1000, 1000, 1000};

/* The time value is not consistant with Technical Guide. Oct 8, 2006 */
UINT16 CR_Cnst_Time_Val = 900; 
UINT16 CR_Init_Cnst_Time_Val = 450;

UINT16 CR_SlowDown_Table[CR_DEC_STEPS_NUM] = { \
	 	   1000, 1000, 1000, 1000, 1000, 1020, 1050, 1090, 1140, 1200, 1290, 1420, 1630, 2150};


   


UINT16 PF_Move_Table[PF_MOVE_STEPS_NUM] = {
			1670, 1490, 1360, 1260, 1180, 1110, 1060, 1010, 960, 930, 930, 930, 930, 930,930,\
		    960, 1010, 1060, 1110, 1180, 1260, 1360, 1490, 1670};
			
UINT16 PF_SlowUp_Table[PF_ACC_STEPS_NUM] = {1670, 1490, 1360, 1260, 1180, 1110, 1060, 1010, 960};

UINT16 PF_Cnst_Time_Val1 = 960;    /* This constant move must has a acceleration first. */

UINT16 PF_Cnst_Time_Val2 = 2850;    /* Without Acceleration and Decceration */

UINT16 PF_SlowDown_Table[PF_DEC_STEPS_NUM] = {960, 1010, 1060, 1110, 1180, 1260, 1360, 1490, 1670};


static UINT32 CRA_A2B_Table[4] = { 
	0x00, 0x01, 0x01, 0x00};

static UINT32 CRA_B2A_Table[4] = {
	0x00, 0x01, 0x01, 0x00};

static UINT32 CRB_A2B_Table[4] = { 
	0x00, 0x00, 0x01, 0x01};

static UINT32 CRB_B2A_Table[4] = {
	0x01, 0x01, 0x00, 0x00};
/*
static UINT32 LFA_FW_Table[4] = {
	0x01, 0x01, 0x00, 0x00};

static UINT32 LFB_FW_Table[4] = {
	0x00, 0x01, 0x01, 0x00};

static UINT32 LFA_BW_Table[4] = {
	0x00, 0x01, 0x01, 0x00};

static UINT32 LFB_BW_Table[4] = {
	0x01, 0x01, 0x00, 0x00};
*/
static UINT32  *CR_PA_Table[2] = {CRA_A2B_Table, CRA_B2A_Table};
static UINT32  *CR_PB_Table[2] = {CRB_A2B_Table, CRB_B2A_Table};
//static UINT32  *LF_PA_Table[2] = {LFA_FW_Table, LFA_BW_Table};
//static UINT32  *LF_PB_Table[2] = {LFB_FW_Table, LFB_BW_Table};

/////////ºÚ±ê¶šÎ»/////////
int BM_Step = 0;
int HP_check = 1;
int Broad_Step = 0;
int paper_feed=0;
int END_STEP = 0;

static void prt_port_config(UINT16 prt_port, UINT8 bit_data)
{
    
	UINT16 port = prt_port;
	UINT32 port_grp, port_index, port_dir;

	port_grp =(UINT32) ((port & PORT_GRP_MASK) >> PORT_BITS_NUM);
	port_index =(UINT32) (port & PORT_BIT_MASK);
	port_dir = (UINT32) ((port & PORT_DIR_MASK) >> PORT_DIR_BIT_POS);

	/* PORT is used for General purpose Only. */
	REG32(Port_Sel_Regs[port_grp]) |= (1 << port_index);
	
	if (port_dir == 0x01) /* Input Port, bit_data is useless */
		REG32(Port_Dir_Regs[port_grp]) |= (1 << port_index);
	else { 
		if (bit_data == 0x01) 
			REG32(Port_Data_Regs[port_grp] ) |= (1 << port_index);
		else
			REG32(Port_Data_Regs[port_grp] ) &= (~(1 << port_index));
		REG32(Port_Dir_Regs[port_grp]) &= (~(1 << port_index));
	}
}

static void set_port_high(UINT16 prt_port)
{          
	UINT16 port = prt_port;
	UINT32 port_grp, port_index;

	port_grp =(UINT32) ((port & PORT_GRP_MASK) >> PORT_BITS_NUM);
	port_index =(UINT32) (port & PORT_BIT_MASK);
	REG32(Port_Data_Regs[port_grp] ) |= (1 << port_index);
}

static void set_port_low(UINT16 prt_port)
{
	UINT16 port = prt_port;
	UINT32 port_grp, port_index;

	port_grp =(UINT32) ((port & PORT_GRP_MASK) >> PORT_BITS_NUM);
	port_index =(UINT32) (port & PORT_BIT_MASK);
	REG32(Port_Data_Regs[port_grp] ) &= (~(1 << port_index));
}

static void set_port_level(UINT16 prt_port, UINT32 level) 
{
	UINT16 port = prt_port;
	UINT32 port_grp, port_index;

	port_grp =(UINT32) ((port & PORT_GRP_MASK) >> PORT_BITS_NUM);
	port_index =(UINT32) (port & PORT_BIT_MASK);

	if (level == 0x01)  /* high level */
		REG32(Port_Data_Regs[port_grp] ) |= (1 << port_index);
	else
		REG32(Port_Data_Regs[port_grp] ) &= (~(1 << port_index));
	
}

static int read_port_level(UINT16 prt_port)
{
	UINT16 port = prt_port;
	UINT32 port_grp, port_index, level;

	port_grp =(UINT32) ((port & PORT_GRP_MASK) >> PORT_BITS_NUM);
	port_index =(UINT32) (port & PORT_BIT_MASK);
	level = ((REG32(Port_Data_Regs[port_grp]) & (1 << port_index)) >> port_index);
	return level;
}

static void init_printer_port(void)
{

	prt_port_config(CR_M_A, 0);
	prt_port_config(CR_M_B, 0);
	/* TURN OFF CR MOTOR */
	prt_port_config(CR_I0, 1);
	prt_port_config(CR_I1, 1);  //enable CR MOTOR
	
	prt_port_config(PRT_HEAD_TRIG, 1);
	prt_port_config(PRT_HEAD1, 1);
	prt_port_config(PRT_HEAD2, 1);
	prt_port_config(PRT_HEAD3, 1);
	prt_port_config(PRT_HEAD4, 1);
	prt_port_config(PRT_HEAD5, 1);
	prt_port_config(PRT_HEAD6, 1);
	prt_port_config(PRT_HEAD7, 1);
	prt_port_config(PRT_HEAD8, 1);

	prt_port_config(LF_STEP_A, 1);
	prt_port_config(LF_STEP_B, 1);
	prt_port_config(LF_STEP_SIG, 1);    /* TURN OFF PF MOTOR */

	/* This ports are input, and the data bit for init is useless. */
	prt_port_config(HP_SEN, 0);
	prt_port_config(PE_SEN, 0);
	prt_port_config(BM_SEN, 0);

}

static int pe_detect(void) //检测是否有纸
{
	int pe1 = 0, pe2 =1, read_times = 3;

	while (read_times -- && (pe1 != pe2))
	{
		pe1 = read_port_level(PE_SEN);
		mdelay(5);
		pe2 = read_port_level(PE_SEN);
	}
	
	if (pe1 != pe2) 
  {
		return -1;
	}
  return pe1;
}

/*
 * The simple routine is used to detect HP. Of course, we can
 * judge the carriage position according to HP signal.
 * Return: 0 -- HP is low and carriage is in B area
 * 		  1 -- HP is high and carriage is in A area
 *		 -1 -- HP read error
 */
 
static int hp_detect(void)
{
	int hp1 = 0, hp2 = 1, read_times = 3;

	while (read_times -- && (hp1 != hp2))
  {
		hp1 = read_port_level(HP_SEN);
		mdelay(5);
    hp2 = read_port_level(HP_SEN);
	}

	if (hp1 != hp2) 
  {
    return -1;
	}
	
	return hp1;
}


/*
 * The simple routine is used to detect BM signal.
 * Return: 1 -- BM signal high
 * 		   0 -- BM signal low
 */
/*
static int bm_detect(void)
{
	int bm1 = 0, bm2 = 1, read_times = 3;

	while (read_times -- && (bm1 != bm2))
	{
		bm1 = read_port_level(BM_SEN);
		mdelay(5);
    bm2 = read_port_level(BM_SEN);
	}

	if (bm1 != bm2)
	{
		return -1;
	}

	return bm1;
}
*/

static void cr_timer_isr(void)
{
	PRT_SPACE *pprt = &g_prt;
	UINT16 cr_pos = pprt->cr_pos_index;//CR status  acc cnst dec
	UINT16 cr_dir =pprt->cr_mov_dir; 
	UINT32 *cr_pa = CR_PA_Table[cr_dir],  *cr_pb = CR_PB_Table[cr_dir], cr_pa_level, cr_pb_level;
	static UINT32 cr_phase = 0; /* index to CR phase table */
	
  CR_HOLD_MS(2);

	cr_pa_level = *(cr_pa + (cr_phase  & CR_PHASE_MASK));
	cr_pb_level = *(cr_pb + (cr_phase  & CR_PHASE_MASK));
	cr_phase ++;
	set_port_level(CR_M_A, cr_pa_level);
	set_port_level(CR_M_B, cr_pb_level);
	
	switch (cr_pos)
	{
		case CR_ACC_AREA:
			pprt->cr_timer.val = CR_SlowUp_Table[pprt->cr_acc_steps ++];
			if (pprt->cr_acc_steps == CR_ACC_STEPS_NUM) 
			{
				pprt->cr_cnst_steps = 0;   /* OKay, we reach the print area. */
				pprt->cr_pos_index = CR_CNST_AREA;
			}
			break;
		case CR_CNST_AREA:		/* CR is in print area. */
			pprt->cr_timer.val = CR_Cnst_Time_Val;
			
			/* 1 step contains 2 positions and we must walk a postion first before printing */
			if (pprt->cr_prt_start_pos == ((pprt->cr_cnst_steps << 1 ) + 1))
			{ 

				pprt->prt_pulse_timer.val = HEAD_TRIG_CYCLE;   
				set_timer_val(pprt->prt_pulse_timer.timer_index, pprt->prt_pulse_timer.val);
				(*pprt->prt_pulse_timer.enable)(pprt->prt_pulse_timer.timer_index);
			}
			else if (pprt->cr_prt_start_pos <= pprt->cr_cnst_steps << 1)
			{ 
        /* Carriage walk on the printing area. */
				UINT16 end_pos = pprt->cr_prt_start_pos + pprt->cr_prt_pos_num;
        if (end_pos > (pprt->cr_cnst_steps << 1)) 
				{ 
          /* On each constant step, we print the full dots */
				  print_pulse_drive();
				  pprt->prt_pulse_timer.val = HEAD_TRIG_CYCLE;   
				  set_timer_val(pprt->prt_pulse_timer.timer_index, pprt->prt_pulse_timer.val);
				  (*pprt->prt_pulse_timer.enable)(pprt->prt_pulse_timer.timer_index);
				}
		  }
		
			if ( ++ pprt->cr_cnst_steps == END_STEP)
			{
				 pprt->cr_dec_steps = 0; /* CR is at the boundry of print area, goto slow up area! */
			   pprt->cr_pos_index = CR_DEC_AREA;   
			}
			break;
		case CR_DEC_AREA:
			pprt->cr_timer.val = CR_SlowDown_Table[pprt->cr_dec_steps ++];
			/* Disable the timer and we will enable it in cr_mov_acc(),
			 * cr_pos_index will also be set there. */
			if (pprt->cr_dec_steps == CR_DEC_STEPS_NUM)
			{  
				/* we has walked CR_DEC_STEPS_NUM - 1 steps,
				 * and we left last step. */
				pprt->cr_pos_index = CR_REACH_END;
			}
      break;
		case CR_REACH_END:  /* OK, we have reach the end. */
			pprt->cr_reach_end = 1;
			cr_phase = 0; /* we must change direction later ! */
			STOP_CR_MOTOR();   /* FIXME: Oct 8, 2006 */
			(*pprt->cr_timer.disable)(pprt->cr_timer.timer_index);
			return;
		default:
			break;
	}

	set_timer_val(pprt->cr_timer.timer_index, pprt->cr_timer.val);
	(*pprt->cr_timer.enable)(pprt->cr_timer.timer_index);
	START_CR_MOTOR();
}

/*
 * Move carriage at a constant speed.
 * Args: dir -- direction
 *  	      steps --move steps
 * Returns:
 */
static void cr_mov_cnst(int dir, int steps)
{
	g_prt.cr_mov_dir = dir;
	g_prt.cr_cnst_steps = 0;
	g_prt.cr_pos_index = CR_ACC_AREA;  //CR_ACC_AREA = 0
	HP_check = 0;
	END_STEP = steps;
	g_prt.cr_dec_steps=0;
	g_prt.cr_acc_steps=0;
	g_prt.cr_timer.isr = cr_timer_isr;
}

/*
void cr_mov_cst(int dir)
{
	g_prt.cr_mov_dir = dir;
	g_prt.cr_cnst_steps = 0;
	g_prt.cr_pos_index = CR_CNST_AREA;  //CR_CNST_AREA = 1
	g_prt.cr_reach_end = 0;
	
}
*/
/*
 *Move carriage at acc and dec speed
 *Args: dir -- direction
 */
/*
void cr_mov_dec(int dir)
{
	g_prt.cr_mov_dir = dir;
	g_prt.cr_dec_steps = 0; // CR will reach another end.
	g_prt.cr_pos_index = CR_DEC_AREA;  //CR_DEC_AREA = 2
	g_prt.cr_reach_end = 0;
}

void cr_mov_acc(int dir)
{
	g_prt.cr_mov_dir = dir;
	g_prt.cr_acc_steps = 0;  // CR is set out from one end of movements area. 
	g_prt.cr_pos_index = CR_ACC_AREA; //CR_ACC_AREA = 0
	g_prt.cr_reach_end = 0;
	END_STEP = CR_CNST_STEPS_NUM;
	//g_prt.cr_cnst_steps  = CR_CNST_STEPS_NUM - step ;
	}
*/
//ÍÐŒÜµç»ú×ª¶¯Öž¶š²œ
static void cr_mov_acc_step(int dir , int step)
{
	g_prt.cr_mov_dir = dir;
	g_prt.cr_acc_steps = 0;  /* CR is set out from one end of movements area. */
	g_prt.cr_pos_index = CR_ACC_AREA; //CR_ACC_AREA = 0
	g_prt.cr_reach_end = 0;
	END_STEP = step ;
	//g_prt.cr_cnst_steps  = CR_CNST_STEPS_NUM - step ;
}

/* Paper feed motor move routine. 
 * Args: steps
 *	     check_bm : flag if check black mark. 
 * Return:
 */
static int pf_mov(int steps, int dir, int check_bm ,int check_pe)
{
  PRT_SPACE *pprt = &g_prt;
	STATUS status =	PF_INIT_OK ;
	    
  BM_Step = 0;

  pprt->pf_mov_dir = dir;
	pprt->bm_check = check_bm;
	pprt->pe_check = check_pe;
	
	
   STOP_PF_MOTOR();
   mdelay(50);

  if (steps < PF_MOVE_STEPS_NUM)
  {
		pprt->pf_cst_steps = steps - 1 ; /* we move pf 1 step first */
		pprt->pf_dec_steps = PF_DEC_STEPS_NUM; /* we do not need slow down process */
		pprt->pf_timer.val = PF_Cnst_Time_Val2;  // 2850
		pprt->pf_pos_index = PF_CNST_AREA; //PF_CNST_AREA = 1

		START_PF_MOTOR();
		
		set_timer_val(pprt->pf_timer.timer_index, pprt->pf_timer.val);
		(*pprt->pf_timer.enable)(pprt->pf_timer.timer_index);
		WAIT_PF_REACH((volatile unsigned short)pprt->pf_cst_steps, 0xffff);   /* See of pf_timer_isr ???*/
  } 
  else 
  {
     int cnst_step = steps - PF_ACC_STEPS_NUM - PF_DEC_STEPS_NUM;
	   pprt->pf_acc_steps = 0;   /* we move pf 1 step first */
		 pprt->pf_cst_steps = cnst_step;
		 pprt->pf_dec_steps = 0; /* there is a slow down process */

		 /* Slow UP process */
		 pprt->pf_timer.val =  PF_SlowUp_Table[0];
		 pprt->pf_pos_index = PF_ACC_AREA;
		 /* Walk 1st step */
		 START_PF_MOTOR();
		 set_timer_val(pprt->pf_timer.timer_index, pprt->pf_timer.val);
		 (*pprt->pf_timer.enable)(pprt->pf_timer.timer_index);

		 /* OKay, we reach the position we wanna */
		 if(pprt->bm_check)
		 {
			 pprt->bm_found = 0;
			 while(!pprt->bm_found&&(!(pprt->pf_dec_steps == PF_DEC_STEPS_NUM + 1))){;}
			 (*pprt->pf_timer.disable)(pprt->pf_timer.timer_index);
		   if(pprt->bm_found) 
			     status = BM_INIT_OK;
		   else 
			     status = BM_UNFOUND;
		 }
	   else if(pprt->pe_check)
		 {
			 while(pe_detect() && (!(pprt->pf_dec_steps == PF_DEC_STEPS_NUM + 1))){;}
			 (*pprt->pf_timer.disable)(pprt->pf_timer.timer_index);
			 if(!pe_detect())
			 {
				pprt->pf_coordinate = 0;
				status = SMP_EAGE_PAPER;
			 }
			 else 
			  	status = SMP_BOARD_PAPER;	
		}
	  else
	  { 
			WAIT_PF_REACH((volatile unsigned short)pprt->pf_dec_steps, PF_DEC_STEPS_NUM + 1);   /* See of pf_timer_isr */
    	if(pprt->prt_status == SMP_BOARD_PAPER)
    		status = pprt->prt_status;
    }
  }
  
	STOP_PF_MOTOR();
  return status;
}


/* Print Buffer Initialization */
static void init_print_buf(void)
{
	g_prt.prt_buf = (UINT8 *)g_prt_adjust_buf;
	g_prt.prt_buf_size = ADJUST_PRT_BUF_SIZE; //20*16
	memset((void *)g_prt_adjust_buf, 0x00, ADJUST_PRT_BUF_SIZE<<1);
	g_prt.prt_buf_num = 0;	
	memset((void *)g_high_buf, 0x00, MAX_BUF_SIZE<<1); //g_high_bufÊÇ16Î»
	memset((void *)g_low_buf,  0x00, MAX_BUF_SIZE<<1);
}



static void set_timer_val(int timer, UINT32 val)
{
     UINT32 timer_val;

     timer_val = val * TIMER_UINT;
     switch (timer)
      {
	      case 2:
	 	       REG32(TIMER_T4CR_V) &= ~0x01;   /* disable the timer */
	 	       REG32(TIMER_T4LCR_V) = timer_val;
	 	       break;
	      case 3:
	 	       REG32(TIMER_T5CR_V) &= ~0x01;   /* disable the timer */
	 	       REG32(TIMER_T5LCR_V) = timer_val;
	 	       break;
	      case 1:
	 	       REG32(TIMER_T3CR_V) &= ~0x01;   /* disable the timer */
	 	       REG32(TIMER_T3LCR_V) = timer_val;
	 	       break;
	      default:
		       break;	 	
      }
     return;
}

static void enable_timer(UINT32 timer)
{

   switch (timer)
    {
	    case 2:
		     REG32(TIMER_T4CR_V) |= 0x07;
		     break;
	    case 3:
		     REG32(TIMER_T5CR_V) |= 0x07;
		     break;
	   case 1:
		     REG32(TIMER_T3CR_V) |= 0x07;
		    break;
	   default:
		    break;
    }
  return ;
}  

static void disable_timer(UINT32 timer)
{	
    switch (timer)
    {
	    case 2:
		     REG32(TIMER_T4CR_V) &= ~0x01;
		     while(REG32(TIMER_T4ISCR_V) & 0x01);
		     break;
	    case 3:
		     REG32(TIMER_T5CR_V) &= ~0x01;
		     while(REG32(TIMER_T5ISCR_V) & 0x01);
		     break;
	    case 1:
		     REG32(TIMER_T3CR_V) &= ~0x01;
		     while(REG32(TIMER_T3ISCR_V) & 0x01);
		     break;
	    default:
		     break;
	 }
    return ;
}

/*
 * Forgive me, it is ugly when we wanna  a hard real-time printer.
 */
static irqreturn_t sep4020_printer_irqhandler(int irq, void *dev_id, struct pt_regs *reg)
{
 	UINT32 int_stat;
	PRT_SPACE *pprt = &g_prt;

	int_stat = REG32(TIMER_TIMSR_V);

	if (int_stat & (1 << 3) ) //CR_TIMER_INDEX = 2
	   {
		(*pprt->cr_timer.disable)(pprt->cr_timer.timer_index);
		(*pprt->cr_timer.isr)();
	   }

	if (int_stat & (1 << 4)) //PF_TIMER_INDEX = 3
	   {
		(*pprt->pf_timer.disable)(pprt->pf_timer.timer_index);
		(*pprt->pf_timer.isr)();
	   }
	
	if (int_stat & (1 << 2)) //PRINT_PULSE_TIMER = 1
	   {
		(*pprt->prt_pulse_timer.disable)(pprt->prt_pulse_timer.timer_index);
		(*pprt->prt_pulse_timer.isr)();
	   }

   return IRQ_HANDLED;	   
}

static void print_pulse_drive(void)
{
	/* TEST font16x16 first !!! */
	UINT8 **prt_buf = &g_prt.prt_buf;
	UINT32 *buf_size = &g_prt.prt_buf_num;
	UINT8 tmp8;
	UINT32 tmp_bit;
        
	if (*buf_size > 0)
	{
		tmp8 = **prt_buf;
		tmp_bit = (tmp8 & 1 ? 0 : 1);
		set_port_level(PRT_HEAD1, tmp_bit);
		tmp_bit = (tmp8 & 2 ? 0 : 1) ;
		set_port_level(PRT_HEAD2, tmp_bit);
		tmp_bit = (tmp8 & 4 ? 0 : 1); 
		set_port_level(PRT_HEAD3, tmp_bit);
		tmp_bit = (tmp8 & 8 ?  0 : 1);
		set_port_level(PRT_HEAD4, tmp_bit);
		tmp_bit = (tmp8 & 16 ?  0 : 1);
		set_port_level(PRT_HEAD5, tmp_bit);
		tmp_bit = (tmp8 & 32 ?  0 : 1);
		set_port_level(PRT_HEAD6, tmp_bit);
		tmp_bit = (tmp8 & 64 ?  0 : 1);
		set_port_level(PRT_HEAD7, tmp_bit);
		tmp_bit = (tmp8 & 128 ?  0 : 1);
		set_port_level(PRT_HEAD8, tmp_bit);

		/* enable head trigger Last! FIXME: Oct 8, 2006 */
		set_port_low(PRT_HEAD_TRIG);

		(*prt_buf) += 1;
		(*buf_size) -= 1;
  }
}

static void disable_print_pulse(void)
{
    set_port_high(PRT_HEAD_TRIG);
    set_port_high(PRT_HEAD1);
	  set_port_high(PRT_HEAD2);
	  set_port_high(PRT_HEAD3);
	  set_port_high(PRT_HEAD4);
	  set_port_high(PRT_HEAD5);
	  set_port_high(PRT_HEAD6);
	  set_port_high(PRT_HEAD7);
	  set_port_high(PRT_HEAD8);
}

static void prt_pulse_isr(void)
{
	PRT_SPACE *pprt = &g_prt;
	static int ent_isr = 0;

	disable_print_pulse();
	
	if (ent_isr == 0)
	{
		ent_isr ++;
		udelay(50);
    print_pulse_drive();
		pprt->prt_pulse_timer.val = HEAD_TRIG_CYCLE;
		set_timer_val(pprt->prt_pulse_timer.timer_index, pprt->prt_pulse_timer.val);
		(*pprt->prt_pulse_timer.enable)(pprt->prt_pulse_timer.timer_index);
	}
	else if (ent_isr == 1)
	{
		ent_isr --;
		(*pprt->prt_pulse_timer.disable)(pprt->prt_pulse_timer.timer_index);
	}
}


static void pf_timer_isr(void)
{
	PRT_SPACE *pprt = &g_prt;
	UINT16 lf_dir = pprt->pf_mov_dir, /* forward ONLY */ pos = pprt->pf_pos_index;
	UINT32 lf_pa_level, lf_pb_level;

	if((pprt->pf_coordinate>0) && lf_dir) 
	     pprt->pf_coordinate -- ;
	else if ( (pprt->pf_coordinate < PAPER_LEN) && (!lf_dir) )
	     pprt->pf_coordinate++ ;
	
	if((pprt->pf_coordinate == PAPER_LEN) && (!lf_dir)) 
	{	
			pprt->pf_coordinate = 0;
			pprt->paper_change = 1;
	}
	
  if(pprt->bm_check && (pprt->pf_coordinate >= BL_Start) && (pprt->pf_coordinate <= BL_End))
	{
   if(!read_port_level(BM_SEN))
	 { 
		 BM_Step++;
	 }
	 if(BM_Step>BM_Sure)
	 {
		pprt->bm_found = 1;
		BM_Step = 0;
		return;
	 }
	}
	
	if(pprt->bm_found)
	{
	 if(pprt->pf_coordinate > BL_End)
	 {
		if(!read_port_level(BM_SEN))
		{ 
			Broad_Step ++;
		}	
		
		if(Broad_Step > BRO_STEP)
		{
			Broad_Step = 0;
			pprt->pf_dec_steps = PF_DEC_STEPS_NUM + 1 ;
			pprt->prt_status = SMP_BOARD_PAPER;
			STOP_PF_MOTOR();
			return;
		}
	 }
  }
  
	lf_pa_level = read_port_level(LF_STEP_A);
	lf_pb_level = read_port_level(LF_STEP_B);
	
	if(!lf_dir)
	{
		if(lf_pa_level == 0x01 && lf_pb_level == 0x00 )
		{
			lf_pa_level = 0x01;
			lf_pb_level = 0x01;
			goto pf_move_step;
		}
		if(lf_pa_level == 0x01 && lf_pb_level == 0x01 )
		{
			lf_pa_level = 0x00;
			lf_pb_level = 0x01;
			goto pf_move_step;
		}
		if(lf_pa_level == 0x00 && lf_pb_level == 0x01 )
		{
			lf_pa_level = 0x00;
			lf_pb_level = 0x00;
			goto pf_move_step;
		}
		if(lf_pa_level == 0x00 && lf_pb_level == 0x00 )
		{
			lf_pa_level = 0x01;
			lf_pb_level = 0x00;
			goto pf_move_step;
		}
	}
	else
	{
		if(lf_pa_level == 0x00 && lf_pb_level == 0x01 )
		{
			lf_pa_level = 0x01;
			lf_pb_level = 0x01;
			goto pf_move_step;
		}
		if(lf_pa_level == 0x01 && lf_pb_level == 0x01 )
		{
			lf_pa_level = 0x01;
			lf_pb_level = 0x00;
			goto pf_move_step;
		}
		if(lf_pa_level == 0x01 && lf_pb_level == 0x00 )
		{
			lf_pa_level = 0x00;
			lf_pb_level = 0x00;
			goto pf_move_step;
		}
		if(lf_pa_level == 0x00 && lf_pb_level == 0x00 )
		{
			lf_pa_level = 0x00;
			lf_pb_level = 0x01;
			goto pf_move_step;
		}
	}
	pf_move_step:
	             set_port_level(LF_STEP_A, lf_pa_level);
		           set_port_level(LF_STEP_B, lf_pb_level);
	
	
	switch (pos) 
	{
		case PF_ACC_AREA:
			if (pprt->pf_acc_steps == PF_ACC_STEPS_NUM)
			{
				pprt->pf_cst_steps --;
				pprt->pf_timer.val = PF_Cnst_Time_Val1;
				pprt->pf_pos_index = PF_CNST_AREA;
			}
		  else
				pprt->pf_timer.val = PF_SlowUp_Table[pprt->pf_acc_steps ++ ];
			break;
		case PF_CNST_AREA:
			if (pprt->pf_cst_steps -- == 0) 
			{
				if (pprt->pf_dec_steps == 0)
				{
					pprt->pf_timer.val = PF_SlowDown_Table[pprt->pf_dec_steps ++];
					pprt->pf_pos_index = PF_DEC_AREA;
				 }
			   else 
			   {  /* ATTENTION ! pf_cst_steps is now 0xffff !!!*/
				   return; /* there is not a slow down process. */
				 }
			}
			break;
		case PF_DEC_AREA:
			if (pprt->pf_dec_steps ++ == PF_DEC_STEPS_NUM) 
			{
				/* ATTENTION!!! pf_dec_steps is now PF_DEC_STEPS_NUM + 1 */
				STOP_PF_MOTOR();
				return;  /* we reach the pos now */
			} 
			else 
				pprt->pf_timer.val = PF_SlowDown_Table[pprt->pf_dec_steps - 1];
			break;
		default:
			break;
	}
	set_timer_val(pprt->pf_timer.timer_index, pprt->pf_timer.val);
	(*pprt->pf_timer.enable)(pprt->pf_timer.timer_index);
}

static void cr_init_isr(void)
{
	PRT_SPACE *pprt = &g_prt;
	UINT16 cr_dir = pprt->cr_mov_dir; 
	UINT32 *cr_pa = CR_PA_Table[cr_dir],  *cr_pb = CR_PB_Table[cr_dir], cr_pa_level, cr_pb_level;
	static UINT32 cr_phase = 1; /* index to CR phase table */

	CR_HOLD_MS(2);  /* hold carriage sometime */

	/* move carriage at a constant speed */
	cr_pa_level = *(cr_pa + (cr_phase  & CR_PHASE_MASK));
	cr_pb_level = *(cr_pb + (cr_phase  & CR_PHASE_MASK));
	cr_phase ++;
	set_port_level(CR_M_A, cr_pa_level);
	set_port_level(CR_M_B, cr_pb_level);
	pprt->cr_cnst_steps ++;
	if(HP_check)
	{
		if (!hp_detect())
		 {
		   pprt->hp_found = 1;
	     return;
		 }
	}
   (*pprt->cr_timer.enable)(pprt->cr_timer.timer_index);
   START_CR_MOTOR();
}


static void init_global_print(PRT_SPACE *pprt)
{
	/* Print Buffer Initialization */
	pprt->cr_prt_start_pos = CR_NO_PRTINT;
	pprt->prt_buf = (UINT8 *)g_prt_adjust_buf;
	pprt->prt_buf_size = ADJUST_PRT_BUF_SIZE;  //16*20
	pprt->prt_buf_num = 0;

	/* Print Timer Initialization */
	pprt->cr_timer.disable = disable_timer;
	pprt->cr_timer.enable = enable_timer;
	pprt->cr_timer.isr = 0; /* enable it later */
	pprt->cr_timer.timer_index = CR_TIMER_INDEX;

	pprt->pf_timer.disable = disable_timer;
	pprt->pf_timer.enable = enable_timer;
	pprt->pf_timer.isr = pf_timer_isr;
	pprt->pf_timer.timer_index = PF_TIMER_INDEX;

	pprt->prt_pulse_timer.disable = disable_timer;
	pprt->prt_pulse_timer.enable = enable_timer;
	pprt->prt_pulse_timer.isr = prt_pulse_isr;
	pprt->prt_pulse_timer.timer_index = PRINT_PULSE_TIMER;
}

static void config_print_timer(PRT_SPACE *pprt)
{	

	 /* Config all timer except GPT1 (for OS timer tick)
	  * normal mode, disable timer and unmask timer. */ 
	 REG32(TIMER_T3CR_V) = 0x2;
	 REG32(TIMER_T4CR_V) = 0x2;
	 REG32(TIMER_T5CR_V) = 0x2;

	 /* Clear the interrupt status */
	 while (REG32(TIMER_T3IMSR_V) & 0x01);
	 while (REG32(TIMER_T4IMSR_V) & 0x01);
	 while (REG32(TIMER_T5IMSR_V) & 0x01);

	
}

static STATUS init_printer(void)
{
 PRT_SPACE * pprt = &g_prt;
 init_global_print(pprt);
 config_print_timer(pprt);
 init_printer_port();
 return 0;
}

/*
  *托架电机返回原位
  */
static STATUS back_HP(void)
{	
	
	PRT_SPACE * pprt = &g_prt;
	STATUS status;
	UINT32 *cr_pa,  *cr_pb, cr_pa_level, cr_pb_level;


       if (!hp_detect())
	{
    /* carriage is in A area. */
		pprt->cr_mov_dir = CR_MOV_B2A;
		status = SMP_GET_HP;
		return status;
        } 
	else
	{
      /* carriage is in B area. */
	   HP_check = 1;
	   pprt->hp_found = 0;
	   pprt->cr_timer.isr = cr_init_isr;
	   pprt->cr_timer.val = CR_Init_Cnst_Time_Val;
	   set_timer_val(pprt->cr_timer.timer_index, pprt->cr_timer.val);
	
	   pprt->cr_mov_dir = CR_MOV_A2B;
	
	/* Walk first step */
	   pprt->cr_cnst_steps = 1;
	   cr_pa = CR_PA_Table[pprt->cr_mov_dir];
	   cr_pb = CR_PB_Table[pprt->cr_mov_dir];
	   cr_pa_level = *cr_pa; cr_pb_level = *cr_pb;
	
	  set_port_level(CR_M_A, cr_pa_level);
	  set_port_level(CR_M_B, cr_pb_level);

	  START_CR_MOTOR();
	 (*pprt->cr_timer.enable)(pprt->cr_timer.timer_index);

	  while (!pprt->hp_found) 
	  { 
       udelay(100);

       /* wait until hp found */
		   if (pprt->cr_cnst_steps > (CR_CNST_STEPS_NUM + CR_DEC_STEPS_NUM + CR_ACC_STEPS_NUM))
		   {
			   STOP_CR_MOTOR();
			   (*pprt->cr_timer.disable)(pprt->cr_timer.timer_index);
		     (*pprt->cr_timer.disable)(pprt->cr_timer.timer_index);
			   status = SMP_MISS_HP;
			   return status;
			}
	 }
	}	

	if (!hp_detect()) 
	{ /* carriage is in A area. */
		pprt->cr_mov_dir = CR_MOV_B2A;
		status = SMP_GET_HP;
	}
	else 
	    status = SMP_MISS_HP;
	return status;
}

static STATUS cr_ori(void )
{
	
	PRT_SPACE * pprt = &g_prt;
	STATUS status;
	UINT32 *cr_pa,  *cr_pb, cr_pa_level, cr_pb_level;

	pprt->cr_timer.isr = cr_init_isr;
	pprt->cr_timer.val = CR_Init_Cnst_Time_Val;
	set_timer_val(pprt->cr_timer.timer_index, pprt->cr_timer.val);
	pprt->hp_found=0;

	if(!hp_detect())
	{
	   pprt->cr_mov_dir = CR_MOV_B2A;
	   
	   /* Walk first step */
	   pprt->cr_cnst_steps = 1;
	   cr_pa = CR_PA_Table[pprt->cr_mov_dir]; 
	   cr_pb = CR_PB_Table[pprt->cr_mov_dir];
	   cr_pa_level = *cr_pa; cr_pb_level = *cr_pb;
	
	   set_port_level(CR_M_A, cr_pa_level);
	   set_port_level(CR_M_B, cr_pb_level);
		   
     START_CR_MOTOR();
	   (*pprt->cr_timer.enable)(pprt->cr_timer.timer_index);

	   while (!pprt->hp_found) 
	   {  // wait until hp found
		  if (pprt->cr_cnst_steps > (CR_CNST_STEPS_NUM + CR_DEC_STEPS_NUM + CR_ACC_STEPS_NUM))
		  {
			  STOP_CR_MOTOR();
			  (*pprt->cr_timer.disable)(pprt->cr_timer.timer_index);
		    (*pprt->cr_timer.disable)(pprt->cr_timer.timer_index);
		    status = SMP_MISS_HP;
   	  }
		  else if(!hp_detect())
		  {
		  	STOP_CR_MOTOR();
		 	  pprt->hp_found=1;
		  }
	  }
	} 
  else
  {   
   /* carriage is in B area. */
	 pprt->cr_mov_dir = CR_MOV_A2B;
	
	 /* Walk first step */
	 pprt->cr_cnst_steps = 1;
	 cr_pa = CR_PA_Table[pprt->cr_mov_dir]; 
	 cr_pb = CR_PB_Table[pprt->cr_mov_dir];
	 cr_pa_level = *cr_pa; cr_pb_level = *cr_pb;
	
	 set_port_level(CR_M_A, cr_pa_level);
	 set_port_level(CR_M_B, cr_pb_level);
	
	 cr_mov_cnst(CR_MOV_A2B,180);

	 START_CR_MOTOR();
	 (*pprt->cr_timer.enable)(pprt->cr_timer.timer_index);
	
	while (!pprt->hp_found)
	{ 
    /* wait until hp found */
		if (pprt->cr_cnst_steps > (CR_CNST_STEPS_NUM + CR_DEC_STEPS_NUM + CR_ACC_STEPS_NUM))
		{
			STOP_CR_MOTOR();
			(*pprt->cr_timer.disable)(pprt->cr_timer.timer_index);
      (*pprt->cr_timer.disable)(pprt->cr_timer.timer_index);
    	status = SMP_MISS_HP;
  	}
		else if(!hp_detect())
		{
		 	STOP_CR_MOTOR();
		 	pprt->hp_found=1;
		}
	}
}	

	/* OKay, we found HP and timer is already disabled in cr_init_isr , 
	 * we also hold the cr motor there. */
	//pprt->cr_timer.isr = cr_timer_isr; /* launch a new isr */

	cr_mov_cnst(CR_MOV_B2A,62); //14+62+14
       START_CR_MOTOR();
      (*pprt->cr_timer.enable)(pprt->cr_timer.timer_index);
      while(!(pprt->cr_dec_steps == CR_DEC_STEPS_NUM));
      (*pprt->cr_timer.disable)(pprt->cr_timer.timer_index);
      STOP_CR_MOTOR();
  return 0;
}

static STATUS pf_ori(UINT16 pf_step )
{	
	STATUS status = -1;
	
	if(!pe_detect())
	{
		status = pf_mov(120,PF_MOV_FW,0,0);
	}
	if(!pe_detect())
	{	
		status = SMP_MISS_PAPER;
		return status;
	}
	else
	{ 
		status =pf_mov(PAPER_LEN,PF_MOV_BW,0,1);
	}

	if(status == SMP_EAGE_PAPER)
		status = pf_mov(pf_step,PF_MOV_FW,0,0);
	else 
	{
		status =pf_mov(PAPER_LEN,PF_MOV_FW,0,1);
		status = pf_mov(pf_step+paper_feed,PF_MOV_BW,0,0);
		
	 }
	paper_feed=0;
	return status;

}

//进纸电机打印过程中定位
static STATUS pf_prt_ori(UINT16 pf_step )
{	
	STATUS status = -1;
	PRT_SPACE * pprt = &g_prt;
	
	if(!pe_detect())
	{
		status = SMP_MISS_PAPER;
		return status;
	}
	else
	{ 
		status =pf_mov(pf_step,PF_MOV_FW,1,0);
	}

	 status = pf_mov(pprt->pf_coordinate - PRT_Start ,PF_MOV_BW,0,0);
	
	return status;
}

static STATUS printer_ready()
{
  STATUS status;
	
  *(volatile unsigned long*)GPIO_PORTD_SEL_V  |= 0X80;
	*(volatile unsigned long*)GPIO_PORTD_DIR_V  &= ~0X80;
	*(volatile unsigned long*)GPIO_PORTD_DATA_V |= 0X80;

 	cr_ori();  //
	status = pf_ori(PAP_Ready); //
	
	*(volatile unsigned long*)GPIO_PORTD_SEL_V |= 0X80;
  *(volatile unsigned long*)GPIO_PORTD_DIR_V |= 0X80;
	return status;
}

static STATUS prt_row(STATUS prt_status , INT font_index, INT printflag)
{	
	PRT_SPACE *pprt = &g_prt;
	PRT_SPACE temp_pprt;
	STATUS status = PRT_OK;
	INT pos;
	INT step;
	INT pf_step = 0;
	status = back_HP();
        if(status)
	{	
		return status;
	}
	
	/* DBG: font16x16 test,  print buffer size, byte unit */
	memcpy(g_prt_adjust_buf,g_high_buf,  pprt->cr_prt_pos_num );
	memcpy(&g_prt_adjust_buf[pprt->cr_prt_pos_num/2 ],g_low_buf,pprt->cr_prt_pos_num  ); 
	
	pprt->prt_buf_num = pprt->cr_prt_pos_num * 2;  //TBD ...
	pprt->cr_timer.isr = cr_timer_isr;
	pprt->cr_prt_start_pos = 0x10;
	if(printflag==0)
	{
		memcpy(&temp_pprt, pprt, sizeof(PRT_SPACE));
		pprt->cr_prt_start_pos = CR_NO_PRTINT;
		cr_mov_acc_step(CR_MOV_B2A ,62);
		START_CR_MOTOR();
		WAIT_UNTIL_CR2END();
		cr_mov_acc_step(CR_MOV_A2B,62);
		START_CR_MOTOR();
		WAIT_UNTIL_CR2END();
		printflag=1;
    memcpy(pprt, &temp_pprt, sizeof(PRT_SPACE));
	 }

	step = (pprt->cr_prt_start_pos + pprt->cr_prt_pos_num) / 2;
  END_STEP = step;
	pos = pprt->cr_prt_start_pos;

	/* print first pass */
	if(pprt->cr_prt_pos_num)
	{
    pprt->cr_prt_start_pos = pos ;
    cr_mov_acc_step(CR_MOV_B2A ,step);
    START_CR_MOTOR();
    WAIT_UNTIL_CR2END();

	  /* carriage turn back */
	  pprt->cr_prt_start_pos = CR_NO_PRTINT;
	  cr_mov_acc_step(CR_MOV_A2B,step);
	  START_CR_MOTOR();
	  WAIT_UNTIL_CR2END();

	  pf_mov(1,PF_MOV_FW ,0 ,0);

  	 /* second pass */
	  pprt->cr_prt_start_pos = pos ;
	  cr_mov_acc_step(CR_MOV_B2A,step);
	  START_CR_MOTOR();
	  WAIT_UNTIL_CR2END();

	 /* carriage turn back */
	  pprt->cr_prt_start_pos = CR_NO_PRTINT;
	  cr_mov_acc_step(CR_MOV_A2B,step);
	  START_CR_MOTOR();
	  WAIT_UNTIL_CR2END();
	}
	
	switch ( pprt->prt_status )
	{
		case  CHANGE_PAGE :
			pf_step = PAP_Ready + PAPER_LEN - pprt->pf_coordinate ; 	
			pf_mov(pf_step ,0,0,0);
			break;
		case  CHANGE_ROW  :
			pf_mov(PF_MOVE_STEPS_NUM, 0 ,0,0);
			break;
	}
		
	return status;
	
}

static STATUS SMP150_Print(void)
{
	STATUS status;
	int i=0;
  int k;
  int row_num;
	PRT_SPACE * pprt = &g_prt;

	
	pprt->prt_status = 0 ;
	row_num = 0;
  status = pf_prt_ori(pf_ori_num);
	if( status != PF_INIT_OK )
	{   
		return(status);
	}
	else 
	  pf_mov(pf_mov_num,PF_MOV_FW,0,0);
	
	do{
		init_print_buf();
    for(k=0 ;k < MAX_BUF_SIZE;k++)
    {
		 g_high_buf[k] = dot_space[row_num].g_high_buf[k];
	   g_low_buf[k] = dot_space[row_num].g_low_buf[k];
    }
    pprt->cr_prt_pos_num = dot_space[row_num].cr_prt_pos_num;
    pprt->prt_status = dot_space[row_num].status;
    mdelay(3);
		status = prt_row(g_prt.prt_status  ,INDEX_FONT16X16,i++  );
		
		if(status == SMP_BOARD_PAPER )
		   return (status);
		
		row_num ++ ;
		
		if(row_num == 20) 
		{
			status = Print_error;
			printk("the end of page!\n");
			return(status);
		}
	 }while(!(pprt->prt_status == CHANGE_PAGE) );
	
	cr_ori();
	
	return(pprt->prt_status);	
}	


STATUS Print(void)
{
	STATUS status;
	
	*(volatile unsigned long*)GPIO_PORTD_SEL_V  |= 0X80;
	*(volatile unsigned long*)GPIO_PORTD_DIR_V  &= ~0X80;
	*(volatile unsigned long*)GPIO_PORTD_DATA_V |= 0X80;	
	
	status = SMP150_Print();
	
	*(volatile unsigned long*)GPIO_PORTD_SEL_V |= 0X80;
        *(volatile unsigned long*)GPIO_PORTD_DIR_V |= 0X80;
        return 0;
	
}

int sep4020_printer_open(struct inode *inode,struct file *filp)
{
	init_printer();
	return 0;
}

int sep4020_printer_release(struct inode *inode, struct file *filp)
{
	return 0;
}

static ssize_t sep4020_printer_read(struct file *filp, char __user *buf, size_t size, loff_t *ppos)
{

	return 0;
}

static ssize_t sep4020_printer_write(struct file *filp, const char __user *buf, size_t size, loff_t *ppos)
{
   ssize_t retval = -ENOMEM;
   loff_t pos = *ppos;
       
   pos += size;
   if(copy_from_user(dot_space + *ppos, buf, size))
   {
      retval = -EFAULT;
      goto out;
    }
    *ppos = pos;
         
    return size;
    out:  
       return retval;
}

int sep4020_printer_ioctl(struct inode *inode, struct file *filp, unsigned int cmd, unsigned long arg)
{
	STATUS status;
     
	//struct printer_dev *dev = filp->private_data;

	switch (cmd)
	{
		case PRINTER_ON:
         init_printer();
			   printer_ready();
			   status = Print();
			   break;
		case PRINTER_TEST:
		     printer_ready();
			   break;
		default:
         return -ENOTTY;
	}
	
	return 0;
}
static struct file_operations sep4020_printer_fops = 
{
	.owner = THIS_MODULE,
	.read  = sep4020_printer_read,
	.write = sep4020_printer_write,
	.ioctl = sep4020_printer_ioctl,
	.open  = sep4020_printer_open,
	.release = sep4020_printer_release,
};



static int __init sep4020_printer_init(void)
{
	int err,result;
	
	dev_t devno = MKDEV(PRINTER_MAJOR, 0);
	
	result = register_chrdev_region(devno, 1, "sep4020_printer");
  if(result < 0)
		return result;
	
	printerdev = kmalloc(sizeof(struct printer_dev),GFP_KERNEL);
	if (!printerdev)
	{
		result = -ENOMEM;
		goto fail_malloc;
	}
	memset(printerdev,0,sizeof(struct printer_dev));
	
  if(request_irq(INTSRC_TIMER2,sep4020_printer_irqhandler,SA_INTERRUPT,"4020printer",printerdev))	
	{
		printk("request TIMER2 failed!\n");
		goto fail_irq;
				
	}
        cdev_init(&(printerdev->cdev), &sep4020_printer_fops);
	printerdev->cdev.owner = THIS_MODULE;
	err = cdev_add(&printerdev -> cdev, devno, 1);
	if(err)
	{	
		printk("dot-matrix printer adding err\r\n");	
	  result = err;
	  goto fail_cdevadd;
	}
	return 0;
	fail_cdevadd:
		           free_irq(INTSRC_TIMER2,printerdev);
	fail_irq:    
		           kfree(printerdev);
	fail_malloc: 
		           unregister_chrdev_region(devno,1);
	return result;
}

static void __exit sep4020_printer_exit(void)
{
	free_irq(INTSRC_TIMER2,printerdev);
	cdev_del(&printerdev->cdev);
	kfree(printerdev);
	unregister_chrdev_region(MKDEV(PRINTER_MAJOR, 0),1);
}

module_init(sep4020_printer_init);
module_exit(sep4020_printer_exit);

MODULE_AUTHOR("prochip");
MODULE_LICENSE("GPL");
