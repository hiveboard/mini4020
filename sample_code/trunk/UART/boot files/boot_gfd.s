;/*************************************************************************************
; *	Copyright (c) 2007 by PROCHIP Limited.
; *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
; *	material.  All manufacturing, reproduction, use, and sales rights 
; *	pertaining to this subject matter are governed by the license agreement.
; *	The recipient of this software implicitly accepts the terms of the license.
; *
; *	File Name:  boot_gfd.s
; *
; *	File Description: sep4020��bootloader.
; *
; *    Version         Date            Author           
; *------------------------------------------------------------------------------------    
; *    0.0.1           2006.10.17      wuer 
; *    0.0.2           2008.01.10      Zhang Yang           
; *   		      	
; *************************************************************************************
; */
      
    
;***************************************************************************************
;* ���ļ����õ��ĳ�������
;***************************************************************************************

SP_USR          EQU      0x30700000;0x0400fff0;       ; USRģʽ�µĶ�ջ��ָ��
SP_SYS          EQU      0x30701000;0x0400fef0;       ; SYSģʽ�µĶ�ջ��ָ��
SP_SVC          EQU      0x30702000;0x0400fdf0;       ; SVCģʽ�µĶ�ջ��ָ��
SP_IRQ          EQU      0x30703000;0x0400fcf0;       ; IRQģʽ�µĶ�ջ��ָ��
SP_FIQ          EQU      0x30704000;0x0400fbf0;       ; FIQģʽ�µĶ�ջ��ָ��
SP_UND          EQU      0x30705000;0x0400faf0;       ; UNDģʽ�µĶ�ջ��ָ��
SP_ABT          EQU      0x30706000;0x0400f9f0;       ; ABTģʽ�µĶ�ջ��ָ��

PMU_PLTR        EQU      0x10001000       ; PLL���ȶ�����ʱ��
PMU_PMCR        EQU      0x10001004       ; ϵͳ��ʱ��PLL�Ŀ��ƼĴ���
PMU_PUCR        EQU      0x10001008       ; USBʱ��PLL�Ŀ��ƼĴ���
PMU_PCSR        EQU      0x1000100C       ; �ڲ�ģ��ʱ��Դ�����Ŀ��ƼĴ���
PMU_PDSLOW      EQU      0x10001010       ; SLOW״̬��ʱ�ӵķ�Ƶ����
PMU_PMDR        EQU      0x10001014       ; оƬ����ģʽ�Ĵ���
PMU_RCTR        EQU      0x10001018       ; Reset���ƼĴ���
PMU_CLRWAKUP    EQU      0x1000101C       ; WakeUp����Ĵ���

EMI_CSACONF     EQU      0x11000000       ; CSA�������üĴ���
EMI_CSECONF     EQU      0x11000010       ; CSE�������üĴ���
EMI_CSFCONF     EQU      0x11000014       ; CSF�������üĴ���
EMI_SDCONF1     EQU      0x11000018       ; SDRAMʱ�����üĴ���1
EMI_SDCONF2     EQU      0x1100001C       ; SDRAMʱ�����üĴ���2, SDRAM��ʼ���õ���������Ϣ
EMI_REMAPCONF   EQU      0x11000020       ; Ƭѡ�ռ估��ַӳ��REMAP���üĴ���

    AREA BOOT, CODE, READONLY                                                     
    ENTRY                                 ; Mark first instruction to execute 
 	
 	EXPORT RST_DO


RST_DO

;*************************************************************************************
;* �ֱ��л���IRQ,FIQ,ABT,UND,SYSģʽ
;* ����ʼ������ģʽ�µĶ�ջ��ָ�룬��󷵻�SVCģʽ�� ���˲�����ɻ����Ľ�����
;*************************************************************************************	
	
    ldr		sp, =SP_SVC  		;init sp_svc
	
    mov		R4, #0xD2			;chmod to irq and init sp_irq
    msr		cpsr_cf, R4   
    ldr		sp, =SP_IRQ
	
    mov		R4, #0XD1			;chmod to fiq and init sp_fiq
    msr		cpsr_cf, R4   
    ldr		sp, =SP_FIQ  
	
    mov		R4, #0XD7			;chmod to abt and init sp_ABT
    msr		cpsr_cf, R4   
    ldr		sp, =SP_ABT 
	
    mov		R4, #0XDB			;chmod to undf and init sp_UNDF
    msr		cpsr_cf, R4   
    ldr		sp, =SP_UND 	
    
                                ;chmod to abt and init sp_sys
    mov 	R4, #0xDF           ;all interrupts disabled      
    msr		cpsr_cxsf, R4       ;SYSTEM mode, @32-bit code mode     
    ldr		sp, =SP_SYS   	
    
    mov		R4, #0XD3           ;chmod to svc modle, CPSR IRQ bit  is disable
    msr		cpsr_c, R4


;***************************************************************************************
; 	init the PMU and get the memory space;
;	���ڹ��Ĺ���ģ���ʵ�ֻ����ں����汾���ܻ���������, ʹ���λ�������������װ�Ը���
;***************************************************************************************
    ldr    r4,    =PMU_PCSR         ; ������ģ��ʱ��
    ldr    r5,    =0x0001FFFF       ; �ɵ�λ����λ����(һ��11bit��Ч)��ʾ��esram lcdc pmu dmac emi mac adc ssi uart2 uart1 uart0       
    str    r5,    [ r4 ] 

    ldr    r4,    =PMU_PMDR         ; ��SLOWģʽ����NORMALģʽ
    ldr    r5,    =0x00000001       ; 00:slow,01:normal,10:sleep,11:idle         
    str    r5,    [ r4 ] 
    
    ldr    r4,    =PMU_PLTR         ; ����PLL�ȶ�����ʱ��Ϊ����ֵ50us.
    ldr    r5,    =0x00FA00FA       ; ��ʵ����16bit reserved
    str    r5,    [ r4 ] 
    
 
    ldr    r4,    =PMU_PMCR         ; ����ϵͳʱ��Ϊ80MHz       
    ldr    r5,    =0x0000400a       ; 400a -- 80M
    str    r5,    [ r4 ] 
    
    ;//PMU_PMCR�Ĵ�����15λ��Ҫ�дӵ͵��ߵķ�ת�����ܴ���PLL��ʱ������
    ldr    r4,    =PMU_PMCR        
    ldr    r5,    =0x0000c00a               
    str    r5,    [ r4 ]
 
    
;***************************************************************************************
;* ��ʼ��EMI
;***************************************************************************************
	
   ; ldr    r4,    =EMI_CSACONF      ; CSAƬѡʱ���������
    ;ldr    r5,    =0x08a6a6a1                 
    ;str    r5,    [ r4 ]     
 
    ;ldr    r4,    =EMI_CSECONF      ; CSEƬѡʱ���������,�������
    ;ldr    r5,    =0x8cfffff1                 
    ;str    r5,    [ r4 ]
 
    ;ldr    r4,    =EMI_SDCONF1 	    ; SDRAM��������1
    ;ldr    r5,    =0x1e104177                 
    ;str    r5,    [ r4 ] 

    ;ldr    r4,    =EMI_SDCONF2 	    ; SDRAM��������2
    ;ldr    r5,    =0x80001860                 
    ;str    r5,    [ r4 ]

    ;ldr    r4,    =EMI_REMAPCONF   	; ��ӳ��cse��0��ַ�������ַӳ�䵽SDRAM
    ;ldr    r5,    =0x0000000b                 
    ;str    r5,    [ r4 ]

;**************************************************************************************
;* ��IRQ�жϡ�CPSR�Ĵ�������λ�������ʾ��IRQ�жϡ�
;**************************************************************************************

    mrs		R4, cpsr
    bic		R4, R4, #0x80  		    ; set bit7 to zero
    msr		cpsr_c, R4

;**************************************************************************************
;* ��ת�������� main()
;**************************************************************************************

    IMPORT	__main
    b		__main 
	
;**************************************************************************************

    END
                                                              
