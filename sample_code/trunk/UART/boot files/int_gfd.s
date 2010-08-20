    AREA INT, CODE, READONLY                                                     
    IMPORT RST_DO
    ENTRY    
    
    bal		RST_DO  
    bal		EXTENT_INSTRU
    bal		SWI_DO
    bal		ABORT_PREFETCH_DO
    bal		ABORT_DATA_DO
    mov 	R1,    R1		;reserved exception
    bal		IRQ_DO
    bal		FIQ_DO   
    ldr		pc,=IRQ_DO
    

    
IRQ_DO      
	
	stmfd	sp!, {r0,r1}           

	ldr	r0, =IRQ_R1          
	str	r1, [r0]
	
	ldmfd	sp!, {r0}
	ldr	r1, =IRQ_R0           
	str	r0, [r1]                ;保存R0和R1寄存器（因为这两个寄存器再后面要用到）
	
	add	r13, r13, #4		;restore the sp_irq top to original irq top
	
	sub	r14, r14, #4
	mov	r0, r14                 ;LR_irq(R14)减4并保存在R0
	
	mrs	r1, spsr
	orr	r1, r1, #0x80           ;将SPSR_irq的中断屏蔽位置‘1’（屏蔽中断），并保存再R1 中
	msr	cpsr_cxsf, r1		;将模式切换到中断前的模式
;---------------------------------------------------------------------------------------------	
	bic	r1, r1, #0x80		;将原先保存的SPSR_irq的R1的中断屏蔽位清零（允许中断）
	
	stmfd	sp!, {r0}
	stmfd	sp!, {r14}
	stmfd	sp!, {r1}               ;依次将R0,R14,R1的值压入中断前模式下的堆栈（当前R0,R14,R1中存放的分别是LR_irq-4，中断前模式下的LR，SPSR_irq)
	
	ldr	r0, =IRQ_R1    
	ldr	r1, [r0]
	stmfd	sp!, {r1}
		
	ldr	r1, =IRQ_R0    
	ldr	r0, [r1]
	stmfd	sp!, {r0}                 
	
	ldmfd	sp!, {r0,r1}            ;恢复原先保存的R0和R1
	stmfd	sp!, {r0-r12}		;将r0--r12全部压入中断以前模式下的堆栈

;-----------------------------      
	IMPORT	int_vector_handler
	bl	int_vector_handler      ;跳转到中断源判断和中断处理程序

;-----------------------------          ;restore the register
	ldmfd	sp!, {r0-r12}           ;恢复原先保存的R0-R12
	ldmfd	sp!, {r14}
	msr	cpsr_cxsf, r14
	ldmfd	sp!, {r14}              ;将原先保存的SPSR_irq恢复到CPSR中
	ldmfd	sp!, {pc}

EXTENT_INSTRU
    b    EXTENT_INSTRU
	                                                                  
SWI_DO
    stmfd	sp!, {r14}
    ldmfd	sp!, {pc}^
	                                                                  
ABORT_PREFETCH_DO
    b    ABORT_PREFETCH_DO
	                                                                  
ABORT_DATA_DO
    b    ABORT_DATA_DO
                                                            
FIQ_DO                                                                
    b    FIQ_DO
                                         ;中断处理结束，恢复SPSR_svc并将PC指针返回中断前的位置，重新执行被中断的指令
                                   
IRQ_R1		DCD		0X0
IRQ_R0		DCD		0X0
    
    END