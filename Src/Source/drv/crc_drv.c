// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : crc_drv.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "crc_drv.h"


#define CRC           CRC1

/*******************************************************************************
* Function Name  : CRC_Config
* Description    : CRC��������.
* Input          :  - CRC_InitTypeDef : CRC_InitTypeDef�ṹ��ָ�룬�������£�
*                    CRC_InitStruct->CRC_Mode:CRC����ģʽ��ȡֵΪCRC_MODE_CRC_x��xȡֵ8,16,32
*                    CRC_InitStruct->CRC_SourceSel: CRC����Դѡ��ȡֵCRC_SOURCE_SEL_FROM_DMA��CRC_SOURCE_SEL_FROM_CPU
*                    CRC_InitStruct->CRC_DataFormat��CRC���ݸ�ʽ�����ģʽ��С��ģʽ��CRC_MODE_CRC_32�£�����Ч��
*
*
* Output         : None
* Return         : None
*******************************************************************************/
void CRC_Config(CRC_InitTypeDef *CRC_InitStruct)
{
	if(CRC_InitStruct->CRC_Mode == CRC_MODE_CRC_32)
	{
		CRC->CRC_CR = ((CRC_InitStruct->CRC_Mode<<29) \
					  |(CRC_InitStruct->CRC_DataFormat<<26) \
					  |(CRC_InitStruct->CRC_SourceSel<<27));
	}
	else
	{
		CRC->CRC_CR = ((CRC_InitStruct->CRC_Mode<<29) \
					  |(CRC_InitStruct->CRC_SourceSel<<27));
	}

}

/*******************************************************************************
* Function Name  : CRC_SetInitValue
* Description    : ����CRC��ʼֵ
* Input          : - InitValue : ���õ�CRC��ʼֵ
*
* Output         : None
* Return         : None
*******************************************************************************/
void CRC_SetInitValue(UINT32 InitValue)
{
	CRC->CRC_IDR = InitValue;
	CRC->CRC_CR |= CRC_UPDATA_MASK;
}

/*******************************************************************************
* Function Name  : CRC_CalcCRC32
* Description    : CRC 32 ����
* Input          : - pBuffer : ��Ҫ����CRC�����ݻ���
*                  - BufferLen : ��Ҫ�����CRC���ݻ���ĳ���
*
* Output         : CRC������
* Return         : None
*******************************************************************************/
UINT32 CRC_CalcCRC32(UINT8 *pBuffer, UINT32 BufferLen)
{
	UINT32 i;
	UINT32 aSize = BufferLen;
	UINT8 *point = pBuffer;

	if(aSize%4 == 0)
	{
		for(i = 0; i < aSize; i = i+4)
		{
			CRC->CRC_DR =(*(point+i)<<24)|(*(point+i+1)<<16)|(*(point+i+2)<<8)|(*(point+i+3));  //������ݵ��ڴ���֯��ʽ������λ�����ڵ͵�ַ
		}
	}
	else if(aSize%4 == 1)
	{
		for(i = 0; i < aSize-1; i = i+4)
		{
			CRC->CRC_DR =(*(point+i)<<24)|(*(point+i+1)<<16)|(*(point+i+2)<<8)|(*(point+i+3));
		}
		CRC->CRC_DR =(*(point+i)<<24);

	}
	else if(aSize%4 == 2)
	{
		for(i=0;i<aSize-2;i=i+4)
		{
			CRC->CRC_DR =(*(point+i)<<24)|(*(point+i+1)<<16)|(*(point+i+2)<<8)|(*(point+i+3));
		}
		CRC->CRC_DR =(*(point+i)<<24)|(*(point+i+1)<<16);
	}
	else
	{
		for(i = 0; i < aSize-3; i = i+4)
		{
			CRC->CRC_DR =(*(point+i)<<24)|(*(point+i+1)<<16)|(*(point+i+2)<<8)|(*(point+i+3));
		}
		CRC->CRC_DR =(*(point+i)<<24)|(*(point+i+1)<<16)|(*(point+i+2)<<8);
	}

	return (CRC->CRC_RR);
}


/*******************************************************************************
* Function Name  : CRC_CalcCRC16
* Description    : CRC 16 ����
* Input          : - pBuffer : ��Ҫ����CRC�����ݻ���
*                  - BufferLen : ��Ҫ�����CRC���ݻ���ĳ���
*
* Output         : CRC������
* Return         : None
*******************************************************************************/
UINT16 CRC_CalcCRC16(UINT8 *pBuffer, UINT32 BufferLen)
{
	UINT32 i;

	if(BufferLen%2 == 0)
	{
		for(i = 0; i < BufferLen; i = i+2)
		{
			CRC->CRC_DR = (*(pBuffer+i)<<8)|(*(pBuffer+i+1));
		}
	}
	else
	{
		for(i = 0; i < BufferLen-1; i = i+2)
		{
			CRC->CRC_DR =(*(pBuffer+i)<<24)|(*(pBuffer+i+1)<<16);
		}
		CRC->CRC_DR =(*(pBuffer+i)<<24);
	}
	return ((UINT16)(CRC->CRC_RR));
}




/*******************************************************************************
* Function Name  : CRC_CalcCRC8
* Description    : CRC 8 ����
* Input          : - pBuffer : ��Ҫ����CRC�����ݻ���
*                  - BufferLen : ��Ҫ�����CRC���ݻ���ĳ���
*
* Output         : CRC������
* Return         : None
*******************************************************************************/
UINT8 CRC_CalcCRC8(UINT8 *pBuffer, UINT32 BufferLen)
{
	UINT32 i;

	for(i = 0; i < BufferLen; i++)
	{
		CRC->CRC_DR = *(pBuffer+i);
	}
	return ((UINT8)(CRC->CRC_RR));
}

