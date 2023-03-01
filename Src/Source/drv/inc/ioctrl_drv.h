#ifndef __IOCTRL_DEV_H__
#define __IOCTRL_DEV_H__
#include "ioctrl_reg.h"
#include "memmap.h"

#define IOCTRL     ((IOCTRL_TypeDef *)IOCTRL_BASE_ADDR)

#define rETIMCFG    (*(__IO UINT32 *)0x4000381C)

#define SCICR_TX1_GINT4_SWAP_MASK		(1<<28)
#define SCICR_RX1_GINT0_SWAP_MASK		(1<<24)
#define SCICR_TX2_GINT5_SWAP_MASK		(1<<29)
#define SCICR_RX2_GINT3_SWAP_MASK		(1<<27)
#define SCICR_TX3_GINT1_SWAP_MASK		(1<<25)
#define SCICR_RX3_GINT2_SWAP_MASK		(1<<26)

typedef enum 
{
	TX1_GINT4 = 0,
	RX1_GINT0,
	TX2_GINT5,
	RX2_GINT3,
	TX3_GINT1,
	RX3_GINT2
}IOCTRL_SG_SEL;


/*******************************************************************************
* Function Name  :IO_Ctrl_SCI_Swap
* Description    :SCI GINT ���ù����л�
* Input          :-sg_sel   ����ѡ��
*									-sw 			:���ù��ܿ���
*									 TRUE  :���ܴ�
*                  FALSE :���ܹر�
*
* Output         : None
* Return         : None
******************************************************************************/
extern void IO_Ctrl_SCI_Swap(IOCTRL_SG_SEL sg_sel, bool sw);

#endif
