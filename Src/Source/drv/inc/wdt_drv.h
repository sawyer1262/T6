// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : wdt_drv.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __WDT_DRV_H__
#define __WDT_DRV_H__

#include "memmap.h"
#include "wdt_reg.h"

#define WDT		    ((WDT_TypeDef *)(WDT_BASE_ADDR))

/*******************************************************************************
* Function Name  : WDT_Init
* Description    : WDT��ʼ��
* Input          : - WMRCounterVal: ���Ź�������ֵ
*
* Output         : None
* Return         : None
******************************************************************************/
extern void WDT_Init(UINT16 WMRCounterVal);


/*******************************************************************************
* Function Name  : WDT_FeedDog
* Description    : WDTι��
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
extern void WDT_FeedDog(void);

/*******************************************************************************
* Function Name  : WDT_Open
* Description    : ��WDT
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
extern void WDT_Open(void);

/*******************************************************************************
* Function Name  : WDT_Close
* Description    : �ر�WDT
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
extern void WDT_Close(void);

/*******************************************************************************
* Function Name  : Get_WDTCount
* Description    : ��ȡWatch-dog counterֵ
* Input          : None
*
* Output         : Watch-dog counter
* Return         : None
******************************************************************************/
extern UINT32 Get_WDTCount(void);

#endif /* __WDT_DRV_H__ */
