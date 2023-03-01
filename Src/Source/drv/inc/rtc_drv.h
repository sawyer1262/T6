// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : rtc_drv.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef RTC_DRV_H_
#define RTC_DRV_H_

#include "memmap.h"
#include "rtc_reg.h"
#include "ccm_drv.h"
#include "cpm_drv.h"

#define RTC		                      ((RTC_TypeDef *)(PMURTC_BASE_ADDR))

#define RTC_SEL_MASK                (0x03 << 8)
#define RTC_LOW_LEVEL_INT           (0x00 << 8)
#define RTC_RISING_EDGE_INT         (0x01 << 8)
#define RTC_FALLING_EDGE_INT        (0x02 << 8)
#define RTC_RISING_FALLING_EDGE_INT (0x03 << 8)

#define DAY_ALARM                    0x01
#define HOUR_ALARM                   0x02
#define MINUTE_ALARM                 0x03
#define SECOND_ALARM                 0x04
#define ALL_ALARM                    0x05

#define IS_RTC_ALARM_MOD(MOD) (((MOD) == DAY_ALARM) ||    \
                               ((MOD) == HOUR_ALARM) ||   \
                               ((MOD) == MINUTE_ALARM) || \
                               ((MOD) == SECOND_ALARM) || \
                               ((MOD) == ALL_ALARM))


#define	Day_intf	    0x00400000
#define	Hou_intf	    0x00200000
#define	Min_intf	    0x00100000
#define	Sec_intf	    0x00080000
#define	Ala_intf	    0x00040000
#define	CLK_1KHz_intf	0x00020000
#define	CLK_32KHz_intf	0x00010000

#define SEC_PULSE_WK    0x01
#define MIN_PULSE_WK    0x02
#define HOUR_PULSE_WK   0x04
#define DAY_PULSE_WK    0x08


typedef struct _tm{
	// date and time components
	UINT16 day;
	UINT8  hour;
	UINT8  minute;
	UINT8  second;
}tm;




extern UINT32 g_rtc_int_sta;

/*******************************************************************************
* Function Name  : RTC_ISR
* Description    : RTC�жϴ���
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
extern void RTC_ISR(void);

extern void RTC_WakeupConfig(UINT8 mode);

/*******************************************************************************
* Function Name  : RTC_Init
* Description    : RTC��ʼ��
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
extern void RTC_Init(UINT8 clk_sel);

/*******************************************************************************
* Function Name  : RTC_SetTime
* Description    : ����RTC��ֵ
* Input          :  - t��tm�ṹ��
*                        t->day:����RTC��dayֵ
*                        t->hour:����RTC��hourֵ
*                        t->minute:����RTC��minuteֵ
*                        t->second:����RTC��secondֵ
*
* Output         : None
* Return         : None
******************************************************************************/
extern void RTC_SetTime(tm t);

/*******************************************************************************
* Function Name  : RTC_SetAlarm
* Description    : ����RTC��ֵ
* Input          :  - t��tm�ṹ�壬����alarm��������ֵ
*                   - AlarmMod��alarm��ģʽ
*                               DAY_ALARM��day����
*                               HOUR_ALARM��hour����
*                               MINUTE_ALARM��minute����
*                               SECOND_ALARM��second����
*                               ALL_ALARM��day��hour��minute��second����
*
* Output         : None
* Return         : None
******************************************************************************/
extern void RTC_SetAlarm(tm t, UINT8 AlarmMod);

/*******************************************************************************
* Function Name  : RTC_GetTime
* Description    : ��ȡRTC������ֵ
* Input          : None
*
* Output         : - t��tm�ṹ�壬
*                       t->day:��ȡ��RTC��dayֵ
*                       t->hour:��ȡ��RTC��hourֵ
*                       t->minute:��ȡ��RTC��minuteֵ
*                       t->second:��ȡ��RTC��secondֵ
* Return         : None
******************************************************************************/
extern void RTC_GetTime(tm *t);




#endif /* RTC_DRV_H_ */
