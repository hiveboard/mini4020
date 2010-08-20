;/*************************************************************************************
; *	Copyright (c) 2007 by PROCHIP Limited.
; *	PROPRIETARY RIGHTS of PROCHIP Limited are involved in the subject matter of this 
; *	material.  All manufacturing, reproduction, use, and sales rights 
; *	pertaining to this subject matter are governed by the license agreement.
; *	The recipient of this software implicitly accepts the terms of the license.
; *
; *	File Name:  boot_gfd.s
; *
; *	File Description: sep4020的bootloader.
; *
; *    Version         Date            Author           
; *------------------------------------------------------------------------------------    
; *    0.0.1           2006.10.17      wuer 
; *    0.0.2           2008.01.10      Zhang Yang           
; *   		      	
; *************************************************************************************
; */
      
    
;***************************************************************************************
;* 本文件中用到的常量定义
;***************************************************************************************

SP_USR          EQU      0x30700000;0x0400fff0;       ; USR模式下的堆栈首指针
SP_SYS          EQU      0x30701000;0x0400fef0;       ; SYS模式下的堆栈首指针
SP_SVC          EQU      0x30702000;0x0400fdf0;       ; SVC模式下的堆栈首指针
SP_IRQ          EQU      0x30703000;0x0400fcf0;       ; IRQ模式下的堆栈首指针
SP_FIQ          EQU      0x30704000;0x0400fbf0;       ; FIQ模式下的堆栈首指针
SP_UND          EQU      0x30705000;0x0400faf0;       ; UND模式下的堆栈首指针
SP_ABT          EQU      0x30706000;0x0400f9f0;       ; ABT模式下的堆栈首指针

PMU_PLTR        EQU      0x10001000       ; PLL的稳定过渡时间
PMU_PMCR        EQU      0x10001004       ; 系统主时钟PLL的控制寄存器
PMU_PUCR        EQU      0x10001008       ; USB时钟PLL的控制寄存器
PMU_PCSR        EQU      0x1000100C       ; 内部模块时钟源供给的控制寄存器
PMU_PDSLOW      EQU      0x10001010       ; SLOW状态下时钟的分频因子
PMU_PMDR        EQU      0x10001014       ; 芯片工作模式寄存器
PMU_RCTR        EQU      0x10001018       ; Reset控制寄存器
PMU_CLRWAKUP    EQU      0x1000101C       ; WakeUp清除寄存器

EMI_CSACONF     EQU      0x11000000       ; CSA参数配置寄存器
EMI_CSECONF     EQU      0x11000010       ; CSE参数配置寄存器
EMI_CSFCONF     EQU      0x11000014       ; CSF参数配置寄存器
EMI_SDCONF1     EQU      0x11000018       ; SDRAM时序配置寄存器1
EMI_SDCONF2     EQU      0x1100001C       ; SDRAM时序配置寄存器2, SDRAM初始化用到的配置信息
EMI_REMAPCONF   EQU      0x11000020       ; 片选空间及地址映射REMAP配置寄存器

    AREA BOOT, CODE, READONLY                                                     
    ENTRY                                 ; Mark first instruction to execute 
 	
 	EXPORT RST_DO


RST_DO

;*************************************************************************************
;* 分别切换到IRQ,FIQ,ABT,UND,SYS模式
;* 并初始化各种模式下的堆栈首指针，最后返回SVC模式。 到此步，完成环境的建立。
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
;	关于功耗管理模块的实现机制在后续版本可能会作出调整, 使其层次化更加清晰，封装性更好
;***************************************************************************************
    ldr    r4,    =PMU_PCSR         ; 打开所有模块时钟
    ldr    r5,    =0x0001FFFF       ; 由低位到高位依次(一共11bit有效)表示：esram lcdc pmu dmac emi mac adc ssi uart2 uart1 uart0       
    str    r5,    [ r4 ] 

    ldr    r4,    =PMU_PMDR         ; 由SLOW模式进入NORMAL模式
    ldr    r5,    =0x00000001       ; 00:slow,01:normal,10:sleep,11:idle         
    str    r5,    [ r4 ] 
    
    ldr    r4,    =PMU_PLTR         ; 配置PLL稳定过度时间为保守值50us.
    ldr    r5,    =0x00FA00FA       ; 其实，低16bit reserved
    str    r5,    [ r4 ] 
    
 
    ldr    r4,    =PMU_PMCR         ; 配置系统时钟为80MHz       
    ldr    r5,    =0x0000400a       ; 400a -- 80M
    str    r5,    [ r4 ] 
    
    ;//PMU_PMCR寄存器第15位需要有从低到高的翻转，才能触发PLL的时钟配置
    ldr    r4,    =PMU_PMCR        
    ldr    r5,    =0x0000c00a               
    str    r5,    [ r4 ]
 
    
;***************************************************************************************
;* 初始化EMI
;***************************************************************************************
	
   ; ldr    r4,    =EMI_CSACONF      ; CSA片选时序参数配置
    ;ldr    r5,    =0x08a6a6a1                 
    ;str    r5,    [ r4 ]     
 
    ;ldr    r4,    =EMI_CSECONF      ; CSE片选时序参数配置,最保守配置
    ;ldr    r5,    =0x8cfffff1                 
    ;str    r5,    [ r4 ]
 
    ;ldr    r4,    =EMI_SDCONF1 	    ; SDRAM参数配置1
    ;ldr    r5,    =0x1e104177                 
    ;str    r5,    [ r4 ] 

    ;ldr    r4,    =EMI_SDCONF2 	    ; SDRAM参数配置2
    ;ldr    r5,    =0x80001860                 
    ;str    r5,    [ r4 ]

    ;ldr    r4,    =EMI_REMAPCONF   	; 重映射cse到0地址，将零地址映射到SDRAM
    ;ldr    r5,    =0x0000000b                 
    ;str    r5,    [ r4 ]

;**************************************************************************************
;* 打开IRQ中断。CPSR寄存器第七位，置零表示打开IRQ中断。
;**************************************************************************************

    mrs		R4, cpsr
    bic		R4, R4, #0x80  		    ; set bit7 to zero
    msr		cpsr_c, R4

;**************************************************************************************
;* 跳转到主函数 main()
;**************************************************************************************

    IMPORT	__main
    b		__main 
	
;**************************************************************************************

    END
                                                              
