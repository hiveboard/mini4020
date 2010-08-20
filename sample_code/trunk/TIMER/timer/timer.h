#ifndef __TIMER_H
#define __TIMER_H

#define write_reg(reg, data)       *(RP)reg = data
#define read_reg(reg)              *(RP)reg


void InitTmier(void);
void Timer_IRQ_Service1(void);
void Timer_IRQ_Service2(void);

#endif