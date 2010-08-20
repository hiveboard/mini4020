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
	str	r0, [r1]                ;����R0��R1�Ĵ�������Ϊ�������Ĵ����ٺ���Ҫ�õ���
	
	add	r13, r13, #4		;restore the sp_irq top to original irq top
	
	sub	r14, r14, #4
	mov	r0, r14                 ;LR_irq(R14)��4��������R0
	
	mrs	r1, spsr
	orr	r1, r1, #0x80           ;��SPSR_irq���ж�����λ�á�1���������жϣ�����������R1 ��
	msr	cpsr_cxsf, r1		;��ģʽ�л����ж�ǰ��ģʽ
;---------------------------------------------------------------------------------------------	
	bic	r1, r1, #0x80		;��ԭ�ȱ����SPSR_irq��R1���ж�����λ���㣨�����жϣ�
	
	stmfd	sp!, {r0}
	stmfd	sp!, {r14}
	stmfd	sp!, {r1}               ;���ν�R0,R14,R1��ֵѹ���ж�ǰģʽ�µĶ�ջ����ǰR0,R14,R1�д�ŵķֱ���LR_irq-4���ж�ǰģʽ�µ�LR��SPSR_irq)
	
	ldr	r0, =IRQ_R1    
	ldr	r1, [r0]
	stmfd	sp!, {r1}
		
	ldr	r1, =IRQ_R0    
	ldr	r0, [r1]
	stmfd	sp!, {r0}                 
	
	ldmfd	sp!, {r0,r1}            ;�ָ�ԭ�ȱ����R0��R1
	stmfd	sp!, {r0-r12}		;��r0--r12ȫ��ѹ���ж���ǰģʽ�µĶ�ջ

;-----------------------------      
	IMPORT	int_vector_handler
	bl	int_vector_handler      ;��ת���ж�Դ�жϺ��жϴ������

;-----------------------------          ;restore the register
	ldmfd	sp!, {r0-r12}           ;�ָ�ԭ�ȱ����R0-R12
	ldmfd	sp!, {r14}
	msr	cpsr_cxsf, r14
	ldmfd	sp!, {r14}              ;��ԭ�ȱ����SPSR_irq�ָ���CPSR��
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
                                         ;�жϴ���������ָ�SPSR_svc����PCָ�뷵���ж�ǰ��λ�ã�����ִ�б��жϵ�ָ��
                                   
IRQ_R1		DCD		0X0
IRQ_R0		DCD		0X0
    
    END