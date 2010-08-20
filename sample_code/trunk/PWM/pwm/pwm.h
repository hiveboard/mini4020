#ifndef PWM_H
#define PWM_H

extern void PWMInit(void);
extern void PWMModeSet(U32 PwmNum,U32 MatchOut,U32 GpioDirection,U32 Function);
extern void PWMEnable(U32 PwmNum);
extern void PWMDisable(U32 PwmNum);
extern void PWMSet(U32 PwmNum,U32 PwmDiv,U32 PwmPeriod,U32 PwmDuty);
extern void PWMTimerSet(U32 PwmNum,U32 TimerDiv,U32 TimerPeriod);
extern void PWMGPIOSet(U32 PwmNum,U32 PwmDiv,U32 PwmDuty);
extern void PWMIntEnable(U32 PwmNum);
extern void PWMIntDisable(U32 PwmNum);
extern void PWMIntHandler(void);

#define write_reg(reg, data)       *(RP)reg = data
#define read_reg(reg)              *(RP)reg

#endif