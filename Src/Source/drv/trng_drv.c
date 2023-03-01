// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : trng_drv.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
#include "cpm_drv.h"
#include "trng_drv.h"

/*******************************************************************************
* Function Name  : Init_Trng
* Description    : ��������ܳ�ʼ��������
*                  ���������ģ��ʱ�ӣ������������ģ��ʱ��Ƶ��Ϊϵͳʱ��Ƶ�ʵ�1/8
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void Init_Trng(void)
{
	CPM->CPM_IPSCGTCR |= TRNG_ENABLE;

	//ϵͳʱ�ӵ�60��Ƶ
	rTRNGIF_CTRL = 59;
	rTRNGIF_CTRL |= TRNGIF_CTRL_EN_MASK;

	rTRNGIF_CTRL |= (0xFF << 16);
}

/*******************************************************************************
* Function Name  : Trng_Disable
* Description    : �ر������ģ��ʱ��
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
void Trng_Disable(void)
{
	CPM->CPM_IPSCGTCR &= ~(TRNG_ENABLE);
}

/*******************************************************************************
* Function Name  : GetRandomWord
* Description    : ��������ɺ���
* Input          : None
* Output         : None
* Return         : 1��UINT32�������
******************************************************************************/
UINT32 GetRandomWord(void)
{
	UINT32 random_value, tmp;

	tmp = rTRNGIF_CTRL;

	while(!(tmp & TRNGIF_CTRL_STA_MASK))
	{
		tmp = rTRNGIF_CTRL;
	}

	random_value = rTRNGIF_DATA;

	rTRNGIF_CTRL |= TRNGIF_CTRL_CLR_MASK;

	return random_value;
}

