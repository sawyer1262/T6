#ifndef  __BAT_H_
#define  __BAT_H_


#include <stdint.h>


#define     BAT_RAW_MAX         50
#define     BAT_CHK_GAP         120


#define     LIBAT_LOW_VAL       6700
#define     AAABAT_LOW_VAL      4800 
#define     AAABAT_HI_VAL       6000 


extern uint8_t  BatLow;
extern uint16_t PowerOnCheckBatDelay;
extern uint8_t BatPerCent;
extern uint16_t batvolatge;

void Bat_GetVoltage(void);
void Bat_PrintVoltage(void);
uint8_t GetVoltagePercent(uint16_t volt);


#endif


