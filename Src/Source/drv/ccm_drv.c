// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : ccm_drv.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "ccm_drv.h"


/*******************************************************************************
* Function Name  : read_chip_cid
* Description    : read  the chip��s ID and Revision number
* Input          : None
*
* Output         : - cid:  cid[15~8]��a unique version identification number for the chip
*                          cid[15~8]��the full-layer mask revision number;
*                          The revision numbers are assigned in chronological order.
* Return         : None
******************************************************************************/
void read_chip_cid(UINT16 *cid)
{
	*cid = CCM->CIR;//OK
}


/*******************************************************************************
* Function Name  : config_freq_detect_point
* Description    : Configuration the frequency detect point
* Input          :  - fd_hi:  Configure the high frequency detect point
*                   - fd_low: Configure the low frequency detect point
*
* Output         : None
*
* Return         : None
******************************************************************************/
//void config_freq_detect_point(UINT8 fd_hi, UINT8 fd_low)
//{
//	CCM->FDCR = (fd_hi<<8) + fd_low;
//}


/*******************************************************************************
* Function Name  : config_osc_bias_trim_value
* Description    : config osc bias trim value
* Input          :  - trim_val:  osc bias trim value
*
*
* Output         : None
*
* Return         : None
******************************************************************************/
void config_osc_bias_trim_value(UINT8 trim_val)
{
	CCM->RTCCFG12 &= 0xfffffff0;
	CCM->RTCCFG12 |= (trim_val&0x0f);
}


/*******************************************************************************
* Function Name  : CCM_ConfigGpio
* Description    : ��оƬclock out��POR(reset out)�ܽ����ó�GPIO��;
* Input          : - CCM_name: оƬ��Ӧ��PIN�ţ�ȡֵCLOCK_OUT����RESET_OUT
*                  - CCM_Dir������GPIO����   GPIO_OUTPUT�����  GPIO_INPUT������
*
* Output         : None
* Return         : None
******************************************************************************/
void CCM_ConfigGpio(UINT8 CCM_Name, UINT8 CCM_Dir)
{
	if (CCM_Name == CLOCK_OUT)
	{
		//ʹ��Clock Out GPIO����
		*(volatile unsigned int *)(0x40000044) &= ~(1 << 28);
		*(volatile unsigned int *)(0x40000044) |= (1 << 30);
		CCM->PCFG12 |= (1 << 4);

		if(CCM_Dir == GPIO_OUTPUT)
		{
			CCM->PCFG12 &= ~(1 << 7);
			CCM->PCFG12 |= (1 << 5);
		}
		else if(CCM_Dir == GPIO_INPUT)
		{
			CCM->PCFG12 &= ~(1 << 5);
			CCM->PCFG12 |= (1 << 7);
		}
	}
	else if (CCM_Name == RESET_OUT)
	{
		//ʹ��Reset Out GPIO����
		*(volatile unsigned int *)(0x40000044) &= ~(1 << 29);
		*(volatile unsigned int *)(0x40000044) |= ((UINT32)1 << 31);
		CCM->PCFG12 |= 1;

		if(CCM_Dir == GPIO_OUTPUT)
		{
			CCM->PCFG12 &= ~(1 << 3);
			CCM->PCFG12 |= (1 << 1);
		}
		else if(CCM_Dir == GPIO_INPUT)
		{
			CCM->PCFG12 &= ~(1 << 1);
			CCM->PCFG12 |= (1 << 3);
		}
	}
}

/*******************************************************************************
* Function Name  : CCM_Gpio_Diable
* Description    : ��оƬclock out��POR(reset out)�ܽŵ�GPIO���ܹص����ָ���֮ǰ�Ĺ���
* Input          : - CCM_name: оƬ��Ӧ��PIN�ţ�ȡֵCLOCK_OUT����RESET_OUT
*
* Output         : None
* Return         : None
******************************************************************************/
void CCM_Gpio_Diable(UINT8 CCM_Name)
{
	if (CCM_Name == CLOCK_OUT)
	{
		CCM->PCFG12 &= ~(1 << 4);
	}
	else if (CCM_Name == RESET_OUT)
	{
		CCM->PCFG12 &= ~1;
	}
}

/*******************************************************************************
* Function Name  : CCM_ReadGpioData
* Description    : ��ȡоƬGPIO��Ӧ���ŵĵ�ƽ
* Input          : - CCM_name: оƬ��Ӧ��PIN�ţ�ȡֵCLOCK_OUT����RESET_OUT
*
* Output         : None
* Return         : Bit_SET:�ߵ�ƽ  Bit_RESET���͵�ƽ
******************************************************************************/
UINT8 CCM_ReadGpioData(UINT8 CCM_Name)
{
	UINT32 bitstatus = 0x00;

	bitstatus = CCM->PCFG12;

	if (CCM_Name == CLOCK_OUT)
	{
		if (bitstatus & (Bit_SET << 9))
		{
			bitstatus = Bit_SET;
		}
		else
		{
			bitstatus = Bit_RESET;
		}
	}
	else if (CCM_Name == RESET_OUT)
	{
		if (bitstatus & (Bit_SET << 8))
		{
			bitstatus = Bit_SET;
		}
		else
		{
			bitstatus = Bit_RESET;
		}
	}

	return bitstatus;
}

/*******************************************************************************
* Function Name  : CCM_WriteGpioData
* Description    : ����оƬGPIO�ܽŶ�Ӧ�ܽŵĵ�ƽ
* Input          : - CCM_name: оƬ��Ӧ��PIN�ţ�ȡֵCLOCK_OUT����RESET_OUT
*                  - bitVal�����õĵ�ƽ��Bit_SET������Ϊ�ߵ�ƽ  Bit_RESET������Ϊ�͵�ƽ
*
* Output         : None
* Return         : None
******************************************************************************/
void CCM_WriteGpioData(UINT8 CCM_Name, UINT8 bitVal)
{
	if (CCM_Name == CLOCK_OUT)
	{
		if (bitVal == Bit_SET)
		{
			CCM->PCFG12 |= (Bit_SET << 6);
		}
		else
		{
			CCM->PCFG12 &= (~(Bit_SET << 6));
		}
	}
	else if (CCM_Name == RESET_OUT)
	{
		if (bitVal == Bit_SET)
		{
			CCM->PCFG12 |= (Bit_SET << 2);
		}
		else
		{
			CCM->PCFG12 &= (~(Bit_SET << 2));
		}
	}
}

