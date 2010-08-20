#ifndef RTC_H
#define RTC_H


#define SAMPLE 		0
#define WATCHDOG	1
#define SECOND		2
#define MINUTE		3
#define	ALARM		4
#define RESET		5

typedef struct RTCDateStruct
{
	U32 year;
	U32 month;
	U32 day;
} DATE;

typedef struct RTCTimeStruct
{
	U32 hour;
	U32 minute;
	U32 second;
} TIME;

extern void InitRTC(void);
extern int RTCSetDate(U32 year, U32 month, U32 day);
extern void RTCGetDate(struct RTCDateStruct * pdate);
extern int RTCSetTime( U32 hour,U32 minute,U32 second);
extern void RTCGetTime( struct RTCTimeStruct * ptime );
extern void RTCSetAlarm(U32 month,U32 day,U32 hour, U32 minute);
extern void RTCSetSample(U32 frequency);
extern void RTCEnableInt(U32 int_num);
extern void RTCDisableInt(U32 int_num);
extern void RTCEnableSample(void);
extern void RTCDisableSample(void);
extern void RTCWatchDogInit(U32 ClockSource,U32 ClockDivider,U32 ResetType);
extern void RTCWatchDogPause(U32 pause);
extern void RTCEnableWatchDog(void);
extern void RTCDisableWatchDog(void);
extern void RTCSetWatchDog(U32 watchdog);
extern void RTCWatchDogService(void);
extern void RTCSetKey(U32 key);
extern void RTCIntHandler(void);

#define write_reg(reg, data)       *(RP)reg = data
#define read_reg(reg)              *(RP)reg


#define delay(j)                                 \
{                                                \
  U16 i;                                         \
  for(i=0;i<j;i++){};                            \
}

		
#endif