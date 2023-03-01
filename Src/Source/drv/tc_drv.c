/**
 * @file tc_drv.c
 * @author zhang.fujiang
 * @brief 
 * @version 1.1
 * @date 2021-05-26
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#include "debug.h"
#include "tc_drv.h"
#include "cpm_drv.h"

volatile uint8_t g_tc_int_flag = 0;
/*******************************************************************************
* Function Name  : TC_ISR
* Description    : TC �жϴ�����
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void TC_ISR(TC_TypeDef *TCx)
{
	TCx->TCCR |= TC_IF;
	while (TCx->TCCR & TC_IS)
		g_tc_int_flag = 1;
}

void TC_IRQHandler(void)
{
	TC_ISR(TC);
//    printf("\r\nThis is the TC Interrupt\r\n");
//    TC_Close(TC);
//    printf("\r\nClose TC Already\r\n");
}

void TC2_IRQHandler(void)
{
	TC_ISR(TC2);
//    printf("This is the TC2 Interrupt");
//    TC_Close(TC2);
//    printf("\r\nClose TC2 Already\r\n");
}

void TC3_IRQHandler(void)
{
	TC_ISR(TC3);
//    printf("This is the TC3 Interrupt");
//    TC_Close(TC3);
//    printf("\r\nClose TC3 Already\r\n");
}
/*******************************************************************************
* Function Name  : TC_Init
* Description    : TC��ʼ��
* Input          : - tc_wdp: TCԤ��Ƶֵ
*                  - Counterval��time��ʼֵ
*
* Output         : None
* Return         : None
******************************************************************************/
void TC_Init(TC_TypeDef *TCx,TC_TIMER_PRESCALER tc_wdp, UINT16 Counterval)
{
	TCx->TCMR = Counterval;
	TCx->TCCR = (tc_wdp << 4) | TC_IE;
	TCx->TCCR |= TC_CU;
}

/*******************************************************************************
* Function Name  : TC_Close
* Description    : �ر�TC
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void TC_Close(TC_TypeDef *TCx)
{
	TCx->TCCR &= ~TC_RN;
	TCx->TCCR &= ~TC_IE;
}

/*******************************************************************************
* Function Name  : TC_FeedWatchDog
* Description    : TCι�������TC watchdog����ֹtc time out����ϵͳ�ж�
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void TC_FeedWatchDog(TC_TypeDef *TCx)
{
	TCx->TCSR = 0x5555;
	TCx->TCSR = 0xAAAA;
}

/*******************************************************************************
* Function Name  : TC_GetCount
* Description    : ��ȡTC counterֵ
* Input          : None
*
* Output         : TC counter
* Return         : None
******************************************************************************/
UINT32 TC_GetCount(TC_TypeDef *TCx)
{
	return (TCx->TCCNTR);
}

/*******************************************************************************
* Function Name  : TC_ResetInit
* Description    : TC reset���ܳ�ʼ��
* Input          : -wdp��TC��Ƶϵ��
*                  -tcmr��TC������ʼֵ
*                  -reset_en���Ƿ����TC reset
* Output         : None
* Return         : None
******************************************************************************/
void TC_ResetInit(TC_TypeDef *TCx,TC_TIMER_PRESCALER wdp, UINT16 tcmr, BOOL reset_en)
{
	UINT16 temp;

	if (FALSE == reset_en) //close reset
	{
		TCx->TCCR &= ~TC_RN;
		TCx->TCMR = tcmr;
		temp = TCx->TCCR;
		temp &= (~0x7f);
		if(TCx == TC)
		NVIC_Init(3, 3, TC_IRQn, 2);
		else if(TCx == TC2)
		NVIC_Init(3, 3, TC2_IRQn, 2);
		else
		NVIC_Init(3, 3, TC3_IRQn, 2);
		TCx->TCCR = temp | TC_IE | (wdp << TC_WDP_SHIFT);
	}
	else
	{
		TCx->TCMR = tcmr;
		temp = TCx->TCCR;
		temp &= (~0x7f);
		TCx->TCCR = temp | TC_RN | (wdp << TC_WDP_SHIFT);
		TCx->TCCR &= ~TC_STOP;
	}
	TCx->TCCR |= TC_CU; // update WDP & WMR
}

/*******************************************************************************
* Function Name  : TC_IntInit
* Description    : TC �жϹ��ܳ�ʼ��
* Input          : -wdp��TC��Ƶϵ��
*                  -tcmr��TC������ʼֵ
*                  -int_en���Ƿ����TC int
* Output         : None
* Return         : None
******************************************************************************/
void TC_IntInit(TC_TypeDef *TCx,TC_TIMER_PRESCALER wdp, UINT16 tcmr, BOOL int_en)
{
	UINT16 temp;

	TCx->TCMR = tcmr;
	temp = TCx->TCCR;
	temp &= (~0x7f);

	if (TRUE == int_en)
	{
		if(TCx == TC)
		NVIC_Init(3, 3, TC_IRQn, 2);
		else if(TCx == TC2)
		NVIC_Init(3, 3, TC2_IRQn, 2);
		else
		NVIC_Init(3, 3, TC3_IRQn, 2);	
		TCx->TCCR = temp | TC_IE | (wdp << TC_WDP_SHIFT);
	}
	else
	{
		TCx->TCCR = temp | (wdp << TC_WDP_SHIFT);
	}

	TCx->TCCR &= ~(0xfu << 8);
	TCx->TCCR &= ~(0x1u << 0); // close reset function

	TCx->TCCR |= TC_CU;
}

/**
 * @brief TC select RTC32K source.
 * 1. TC_selectClockSource(1); //ѡ���ⲿ32K������ΪTCʱ��Դ.
 * @param clk_src 
 */
void TC_selectClockSource(unsigned int clk_src)
{
	clk_src = !!clk_src;
	CPM->CPM_CSWCFGR = (CPM->CPM_CSWCFGR & ~OSCL_SEL) | (clk_src << 6);
}
/*******************************************************************************
* Function Name  : TC2/3_ClockSource_Select
* Description    : TC2/3 ʱ��Դ��ѡ��
*
 * Input          : clk_src - 00 ѡ�� OSC128K ʱ��
 *                            01 ѡ�� RTC32K  ʱ��
 *                            10 ѡ�� PMU2K   ʱ��
 *                            11 ����
* Output         : None
* Return         : None
******************************************************************************/
void TC23_selectClockSource(TC_TypeDef *TCx,TC_CLOCK_SEL clk_src)
{
    if(clk_src>=3){
        return;
    }
    if(TCx==TC){
        return;
    }
    else if(TCx == TC2)
	{
        CPM->CPM_CSWCFGR2 &= ~(TC2_SEL_MASK);
        CPM->CPM_CSWCFGR2 |= (clk_src << TC2_SEL_ST_SHIFT);	
	}
	else
	{
        CPM->CPM_CSWCFGR2 &= ~(TC3_SEL_MASK);	
        CPM->CPM_CSWCFGR2 |= (clk_src << TC3_SEL_ST_SHIFT);	
	}
}

