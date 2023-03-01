// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : dac_drv.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef __DAC_DRV_H__
#define __DAC_DRV_H__

#include "dac_reg.h"
#include "main.h"

#ifdef DAC_DEMO
#define DAC_SYSTICK_INT
#define DAC_DMA_INT
#endif

typedef enum _DAC_DATA_FORMAT_
{
	LEFTALIGNED_8BITS = 0,
	LEFTALIGNED_12BITS,
	RIGHTALIGNED_8BITS,
	RIGHTALIGNED_12BITS
}DAC_DATA_FORMAT;

typedef enum _DAC_TRIGGER_SEL_
{
	TRIGGER_SOFTWARE = 0,
	TRIGGER_EXTERNAL,
	TRIGGER_PIT
}DAC_TRIGGER_SEL;

typedef enum _DAC_EXT_TMOD_
{
	MOD_RESERVED = 0,
	DET_ON_RISING = 1,
	DET_ON_FALLING,
	DET_ON_BOTH,
	DET_ON_HIGH,
	DET_ON_LOW
}DAC_EXT_TMOD;

/*******************************************************************************
* Function Name  : DAC_Init
* Description    : DAC��ʼ��
* Input          : -data_format:
*                      LEFTALIGNED_8BITS��8bits�����
*                      LEFTALIGNED_12BITS��12bits�����
*                      RIGHTALIGNED_8BITS��8bits�Ҷ���
*                      RIGHTALIGNED_12BITS��12bits�Ҷ���
*                  -trigger_select:
*                      TRIGGER_SOFTWARE���������
*                      TRIGGER_EXTERNAL���ⲿ����(SS_SPI6)
*                      TRIGGER_PIT����ʱ������(PIT2)
*                  -external_mode:
*                      DET_ON_RISING���ⲿ������ʽΪ������
*                      DET_ON_FALLING���ⲿ������ʽΪ�½��ش���
*                      DET_ON_BOTH���ⲿ������ʽΪ˫�ش���
*                      DET_ON_HIGH���ⲿ������ʽΪ�ߵ�ƽ����
*                      DET_ON_LOW���ⲿ������ʽΪ�͵�ƽ����
* Output         : None
* Return         : None
******************************************************************************/
extern void DAC_Init(DAC_DATA_FORMAT data_format, DAC_TRIGGER_SEL trigger_select, DAC_EXT_TMOD external_mode);
/*******************************************************************************
* Function Name  : DAC_Start
* Description    : DAC����
* Input          : -trigger_select:
*                      TRIGGER_SOFTWARE���������
*                      TRIGGER_EXTERNAL���ⲿ����(SS_SPI6)
*                      TRIGGER_PIT����ʱ������(PIT2)
* Output         : None
* Return         : None
******************************************************************************/
extern void DAC_Start(DAC_TRIGGER_SEL trigger_select,unsigned int rate);
/*******************************************************************************
* Function Name  : DAC_Wait_Load_Done
* Description    : DAC�ȴ��������
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void DAC_Wait_Load_Done(void);

extern void DAC_Wait_Load_Cear(void);

extern void DAC_SW_Trig(void);

extern void Send_DAC_data(unsigned char data);
extern void DAC_Systick_ISR(void);
extern void DAC_close(void);
extern void DAC_Write_Data(UINT16 data);
#endif /* __DAC_DRV_H__ */
