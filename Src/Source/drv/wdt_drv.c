// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : wdt_drv.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "wdt_drv.h"


/*******************************************************************************
* Function Name  : WDT_Init
* Description    : WDT��ʼ��
* Input          : - WMRCounterVal: ���Ź�������ֵ
*
* Output         : None
* Return         : None
******************************************************************************/
void WDT_Init(UINT16 WMRCounterVal)
{
	WDT->WDT_WMR = WMRCounterVal;
	WDT->WDT_WCR = WDT_EN;
}

/*******************************************************************************
* Function Name  : WDT_FeedDog
* Description    : WDTι��
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void WDT_FeedDog(void)
{
	WDT->WDT_WSR = 0x5555;
	WDT->WDT_WSR = 0xAAAA;
}

/*******************************************************************************
* Function Name  : WDT_Open
* Description    : ��WDT
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void WDT_Open(void)
{
	WDT->WDT_WCR |= WDT_EN;
}
/*******************************************************************************
* Function Name  : WDT_Close
* Description    : �ر�WDT
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void WDT_Close(void)
{
	WDT->WDT_WCR &= ~WDT_EN;
}

/*******************************************************************************
* Function Name  : WDT_CloseClkGate
* Description    : �ر�WDTʱ����
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void WDT_CloseClkGate(void)
{
	*(uint32_t *)0x4000404C &= ~(1<<1); 
}

/*******************************************************************************
* Function Name  : WDT_OpenClkGate
* Description    : ��WDTʱ����
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void WDT_OpenClkGate(void)
{
	*(uint32_t *)0x4000404C |= (1<<1); 
    WDT_FeedDog();
}

/*******************************************************************************
* Function Name  : Get_WDTCount
* Description    : ��ȡWatch-dog counterֵ
* Input          : None
*
* Output         : Watch-dog counter
* Return         : None
******************************************************************************/
UINT32 Get_WDTCount(void)
{
	return (WDT->WDT_WCNTR);
}

