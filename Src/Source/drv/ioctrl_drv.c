#include "type.h"
#include "ioctrl_drv.h"

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
void IO_Ctrl_SCI_Swap(IOCTRL_SG_SEL sg_sel, bool sw)
{
	if(TRUE == sw)//on
	{
		switch(sg_sel)
		{
			case TX1_GINT4:
				IOCTRL->SCI_CONTROL_REG |= SCICR_TX1_GINT4_SWAP_MASK;	//����GINT4��RX1���ù���
				break;
			case RX1_GINT0:
				IOCTRL->SCI_CONTROL_REG |= SCICR_RX1_GINT0_SWAP_MASK;	//����GINT0��TX1���ù���
				break;
			case TX2_GINT5:
				IOCTRL->SCI_CONTROL_REG |= SCICR_TX2_GINT5_SWAP_MASK;	//����GINT5��RX2���ù���
				break;
			case RX2_GINT3:
				IOCTRL->SCI_CONTROL_REG |= SCICR_RX2_GINT3_SWAP_MASK;	//����GINT3��RX2���ù���
				break;
			case TX3_GINT1:
				IOCTRL->SCI_CONTROL_REG |= SCICR_TX3_GINT1_SWAP_MASK;	//����GINT1��TX3���ù���
				break;
			case RX3_GINT2:
				IOCTRL->SCI_CONTROL_REG |= SCICR_RX3_GINT2_SWAP_MASK;	//����GINT2��TX3���ù���
				break;
			default:
				break;
		}
	}
	else	//off
	{
		switch(sg_sel)
		{
			case TX1_GINT4:
				IOCTRL->SCI_CONTROL_REG &= ~SCICR_TX1_GINT4_SWAP_MASK;	//�ر�GINT4��RX1���ù���
				break;
			case RX1_GINT0:
				IOCTRL->SCI_CONTROL_REG &= ~SCICR_RX1_GINT0_SWAP_MASK;	//�ر�GINT0��TX1���ù���
				break;
			case TX2_GINT5:
				IOCTRL->SCI_CONTROL_REG &= ~SCICR_TX2_GINT5_SWAP_MASK;	//�ر�GINT5��RX2���ù���
				break;
			case RX2_GINT3:
				IOCTRL->SCI_CONTROL_REG &= ~SCICR_RX2_GINT3_SWAP_MASK;	//�ر�GINT3��RX2���ù���
				break;
			case TX3_GINT1:
				IOCTRL->SCI_CONTROL_REG &= ~SCICR_TX3_GINT1_SWAP_MASK;	//�ر�GINT1��TX3���ù���
				break;
			case RX3_GINT2:
				IOCTRL->SCI_CONTROL_REG &= ~SCICR_RX3_GINT2_SWAP_MASK;	//�ر�GINT2��TX3���ù���
				break;
			default:
				break;
		}
	}

}


