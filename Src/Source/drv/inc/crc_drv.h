// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : crc_drv.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef CRC_DRV_H_
#define CRC_DRV_H_

#include "type.h"
#include "crc_reg.h"
#include "memmap.h"

#define CRC0        ((CRC_TypeDef *)CRC0_BASE_ADDR)   
#define CRC1        ((CRC_TypeDef *)CRC1_BASE_ADDR)   

#define CRC_DATA_FORMAT_BIGENDDING         (0x01)//Data format is big endding
#define CRC_DATA_FORMAT_LITTLEENDDING      (0x00)//Data format is little endding

#define IS_CRC_DATA_FORMAT(DATA_FORMAT) (((DATA_FORMAT) == CRC_DATA_FORMAT_BIGENDDING) || \
                                         ((DATA_FORMAT) == CRC_DATA_FORMAT_LITTLEENDDING))


#define CRC_SOURCE_SEL_FROM_DMAC           (0x01)//Data from DMAC write
#define CRC_SOURCE_SEL_FROM_CPU            (0x00)//Data from CPU transport

#define IS_CRC_SOURCE_SEL(SOURCE_SEL) (((SOURCE_SEL) == CRC_SOURCE_SEL_FROM_DMAC) || \
                                       ((SOURCE_SEL) == CRC_SOURCE_SEL_FROM_CPU))

#define CRC_MODE_CRC_32                    (0x04)//32bit CRC data mode
#define CRC_MODE_CRC_16                    (0x02)//16bit CRC data mode
#define CRC_MODE_CRC_8                     (0x01)//8bit CRC data mode

#define IS_CRC_MODE(MODE) (((MODE) == CRC_MODE_CRC_32) || \
                           ((MODE) == CRC_MODE_CRC_16) || \
                           ((MODE) == CRC_MODE_CRC_8))

typedef struct
{

  UINT8 CRC_Mode;//CRC data mode
  UINT8 CRC_SourceSel;//Data from sram transport or CPU write
  UINT8 CRC_DataFormat;//Data format when in CRC32 mode

}CRC_InitTypeDef;


/*******************************************************************************
* Function Name  : CRC_Config
* Description    : CRC��������.
* Input          : - CRC_InitTypeDef : CRC_InitTypeDef�ṹ��ָ�룬�������£�
*                    CRC_InitStruct->CRC_Mode:CRC����ģʽ��ȡֵΪCRC_MODE_CRC_x��xȡֵ8,16,32
*                    CRC_InitStruct->CRC_SourceSel: CRC����Դѡ��ȡֵCRC_SOURCE_SEL_FROM_DMA��CRC_SOURCE_SEL_FROM_CPU
*                    CRC_InitStruct->CRC_DataFormat��CRC���ݸ�ʽ�����ģʽ��С��ģʽ��CRC_MODE_CRC_32�£�����Ч��
*
*
* Output         : None
* Return         : None
*******************************************************************************/
extern void CRC_Config(CRC_InitTypeDef *CRC_InitStruct);

/*******************************************************************************
* Function Name  : CRC_SetInitValue
* Description    : ����CRC��ʼֵ
* Input          :  - InitValue : ���õ�CRC��ʼֵ
*
* Output         : None
* Return         : None
*******************************************************************************/
void CRC_SetInitValue(UINT32 InitValue);

/*******************************************************************************
* Function Name  : CRC_CalcCRC32
* Description    : CRC 32 ����
* Input          : - pBuffer : ��Ҫ����CRC�����ݻ���
*                  - BufferLen : ��Ҫ�����CRC���ݻ���ĳ���
*
* Output         : CRC������
* Return         : None
*******************************************************************************/
UINT32 CRC_CalcCRC32(UINT8 *pBuffer, UINT32 BufferLen);

/*******************************************************************************
* Function Name  : CRC_CalcCRC16
* Description    : CRC 16 ����
* Input          : - pBuffer : ��Ҫ����CRC�����ݻ���
*                  - BufferLen : ��Ҫ�����CRC���ݻ���ĳ���
*
* Output         : CRC������
* Return         : None
*******************************************************************************/
UINT16 CRC_CalcCRC16(UINT8 *pBuffer, UINT32 BufferLen);

/*******************************************************************************
* Function Name  : CRC_CalcCRC8
* Description    : CRC 8 ����
* Input          : - pBuffer : ��Ҫ����CRC�����ݻ���
*                  - BufferLen : ��Ҫ�����CRC���ݻ���ĳ���
*
* Output         : CRC������
* Return         : None
*******************************************************************************/
UINT8 CRC_CalcCRC8(UINT8 *pBuffer, UINT32 BufferLen);

#endif /* CRC_DRV_H_ */
