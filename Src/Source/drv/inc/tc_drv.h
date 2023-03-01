// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : tc_drv.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __TC_DRV_H__
#define __TC_DRV_H__

#include "memmap.h"
#include "tc_reg.h"

#define TC				((TC_TypeDef *)TC_BASE_ADDR)
#define TC2             ((TC_TypeDef *)TC2_BASE_ADDR)
#define TC3             ((TC_TypeDef *)TC3_BASE_ADDR)
typedef enum
{
   TC_WDP_64,//64ms
   TC_WDP_32,//32ms
   TC_WDP_16,//16ms
   TC_WDP_8,//8ms
   TC_WDP_4,//4ms
   TC_WDP_2,//2ms
   TC_WDP_1,//1ms
   TC_WDP_0_5,//0.5ms

}TC_TIMER_PRESCALER;


typedef enum
{
   TC_OSC128K,//128K
   TC_RTC32K,//32k
   TC_PMU2K,//PMU2K
	 TC_RESERVE,

}TC_CLOCK_SEL;
#define IS_TC_TIMER_PRESCALER(PRESCALER) (((PRESCALER) == TC_WDP_64) || \
                                          ((PRESCALER) == TC_WDP_32) || \
                                          ((PRESCALER) == TC_WDP_16) || \
                                          ((PRESCALER) == TC_WDP_8) || \
                                          ((PRESCALER) == TC_WDP_4) || \
                                          ((PRESCALER) == TC_WDP_2) || \
                                          ((PRESCALER) == TC_WDP_1) || \
                                          ((PRESCALER) == TC_WDP_0_5))
/*******************************************************************************
* Function Name  : TC_Init
* Description    : TC��ʼ��
* Input          : - tc_wdp: TCԤ��Ƶֵ
*                  - Counterval��time��ʼֵ
*
* Output         : None
* Return         : None
******************************************************************************/
extern void TC_Init(TC_TypeDef *TCx,TC_TIMER_PRESCALER tc_wdp, UINT16 Counterval);

/*******************************************************************************
* Function Name  : TC_Close
* Description    : �ر�TC
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
extern void TC_Close(TC_TypeDef *TCx);

/*******************************************************************************
* Function Name  : TC_FeedWatchDog
* Description    : TCι�������TC watchdog����ֹtc time out����ϵͳ�ж�
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
extern void TC_FeedWatchDog(TC_TypeDef *TCx);

/*******************************************************************************
* Function Name  : TC_GetCount
* Description    : ��ȡTC counterֵ
* Input          : None
*
* Output         : Watch-dog counter
* Return         : None
******************************************************************************/
extern UINT32 TC_GetCount(TC_TypeDef *TCx);

/*******************************************************************************
* Function Name  : TC_ResetInit
* Description    : TC reset���ܳ�ʼ��
* Input          : -wdp��TC��Ƶϵ��
*                  -tcmr��TC������ʼֵ
*                  -reset_en���Ƿ����TC reset
* Output         : None
* Return         : None
******************************************************************************/
extern void TC_ResetInit(TC_TypeDef *TCx,TC_TIMER_PRESCALER wdp, UINT16 tcmr, BOOL reset_en);

/*******************************************************************************
* Function Name  : TC_IntInit
* Description    : TC �жϹ��ܳ�ʼ��
* Input          : -wdp��TC��Ƶϵ��
*                  -tcmr��TC������ʼֵ
*                  -int_en���Ƿ����TC int
* Output         : None
* Return         : None
******************************************************************************/
extern void TC_IntInit(TC_TypeDef *TCx,TC_TIMER_PRESCALER wdp, UINT16 tcmr, BOOL int_en);
extern void TC_selectClockSource(unsigned int clk_src);
extern void TC23_selectClockSource(TC_TypeDef *TCx,TC_CLOCK_SEL clk_src);
extern volatile uint8_t g_tc_int_flag;

#endif /* __TC_DRV_H__ */
