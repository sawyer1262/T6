/*
 * pit32_drv.h
 *
 *  Created on: 2017��11��29��
 *      Author: nltao
 */

#ifndef PIT32_DRV_H_
#define PIT32_DRV_H_

#include "type.h"
#include "pit32_reg.h"

// #define PIT32_ADDR        (PIT1_BASE_ADDR)
// //#define PIT32_ADDR        (PIT2_BASE_ADDR)

// #define PIT32             ((PIT32_TypeDef *)PIT32_ADDR)      

#define PIT1                (PIT32_TypeDef *)(PIT1_BASE_ADDR)
#define PIT2                (PIT32_TypeDef *)(PIT2_BASE_ADDR)

typedef enum
{
	PIT32_CLK_DIV_1,//1��Ƶ
	PIT32_CLK_DIV_2,//2��Ƶ
	PIT32_CLK_DIV_4,//4��Ƶ
	PIT32_CLK_DIV_8,//8��Ƶ
	PIT32_CLK_DIV_16,//16��Ƶ
	PIT32_CLK_DIV_32,//32��Ƶ
	PIT32_CLK_DIV_64,//64��Ƶ
	PIT32_CLK_DIV_128,//128��Ƶ
	PIT32_CLK_DIV_256,//256��Ƶ
	PIT32_CLK_DIV_512,//512��Ƶ
	PIT32_CLK_DIV_1024,//1024��Ƶ
	PIT32_CLK_DIV_2048,//2048��Ƶ
	PIT32_CLK_DIV_4096,//4096��Ƶ
	PIT32_CLK_DIV_8192,//8192��Ƶ
	PIT32_CLK_DIV_16384,//16384��Ƶ
	PIT32_CLK_DIV_32768,//32768��Ƶ
}PIT32_CLKDIV;


/*******************************************************************************
* Function Name  : PIT32_Init
* Description    : PIT��ʼ��
* Input          : - Prescaler��PITʱ�ӷ�Ƶֵ
*                  - PMRReloadCounterVal:PIT����������ʼֵ
*                  - pitINT :ʹ��PIT�жϷ�ʽ
*
* Output         : None
* Return         : None
******************************************************************************/
extern void PIT32_Init(PIT32_TypeDef *PITx, PIT32_CLKDIV Prescaler, UINT32 PMRReloadCounterVal,BOOL pitINT);

/*******************************************************************************
* Function Name  : PIT32_Start
* Description    : PIT����
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
extern void PIT32_Start(PIT32_TypeDef *PITx);


/*******************************************************************************
* Function Name  : PIT32_Stop
* Description    : PIT�ر�
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
extern void PIT32_Stop(PIT32_TypeDef *PITx);



/*******************************************************************************
* Function Name  : PIT32_Cmd
* Description    : PITʹ�ܿ���
* Input          : - NewState��new state of the specified PIT.
*                              This parameter can be: ENABLE or DISABLE.
*
* Output         : None
* Return         : None
******************************************************************************/
extern void PIT32_Cmd(PIT32_TypeDef *PITx, FunctionalState NewState);



/*******************************************************************************
* Function Name  : PIT32_CmdPIT_ITConfig
* Description    : PIT�ж�ʹ�ܿ���
* Input          : - NewState��new state of the specified PIT interrupts.
*                              This parameter can be: ENABLE or DISABLE.
*
* Output         : None
* Return         : None
******************************************************************************/
extern void PIT32_ITConfig(PIT32_TypeDef *PITx, FunctionalState NewState);


/*******************************************************************************
* Function Name  : PIT32_ReadCNTR
* Description    : ��ȡPIT����ֵ
* Input          : -None
*
* Output         : ����ֵ
* Return         : ����ֵ
******************************************************************************/
extern UINT32 PIT32_ReadCNTR(PIT32_TypeDef *PITx);

#endif /* PIT32_DRV_H_ */
