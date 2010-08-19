/*
 * asm/arch-samsung/irqs.h:
 * Mac Wang <mac@os.nctu.edu.tw>
 */
#ifndef __ASM_ARCH_IRQS_H__
#define __ASM_ARCH_IRQS_H__

#define NR_IRQS			32
#define VALID_IRQ(i)		(i<NR_IRQS)
                                                
#define  INTSRC_RTC                31
#define  INTSRC_DMAC               30
#define  INTSRC_EMI                29    
#define  INTSRC_MAC                28
#define  INTSRC_TIMER1             27 
#define  INTSRC_TIMER2             26
#define  INTSRC_TIMER3             25
#define	 INTSRC_UART0              24
#define  INTSRC_UART1              23
#define  INTSRC_UART2              22
#define  INTSRC_UART3              21
#define  INTSRC_PWM                20
#define  INTSRC_LCDC               19
#define  INTSRC_I2S		   18
#define  INTSRC_SSI                17

#define  INTSRC_USB                15
#define  INTSRC_SMC0               14
#define  INTSRC_SMC1               13
#define  INTSRC_SDIO               12

#define  INTSRC_EXTINT10           11    			    
#define	 INTSRC_EXTINT9            10       			    
#define	 INTSRC_EXTINT8             9      			    
#define	 INTSRC_EXTINT7             8      			    
#define	 INTSRC_EXTINT6             7      			    
#define	 INTSRC_EXTINT5             6     			    
#define	 INTSRC_EXTINT4             5    			    
#define	 INTSRC_EXTINT3             4    			    
#define	 INTSRC_EXTINT2             3     			    
#define	 INTSRC_EXTINT1             2    			    
#define	 INTSRC_EXTINT0             1      			    

#endif /* __ASM_ARCH_IRQS_H__ */
