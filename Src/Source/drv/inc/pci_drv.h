/**
 * @file pci_drv.h
 * @brief 
 * 
 * @copyright Copyright (c) 2021
 * 
 */
#ifndef __PCI_DRV_H__
#define __PCI_DRV_H__

#include "type.h"
#include "pci_reg.h"
#include "memmap.h"

#define PCI ((PCI_TypeDef *)(PCI_BASE_ADDR))

//nvram address
#define SRAM_START_ADDR_M4 (PCI_BASE_ADDR + 0x100)
#define SRAM_END_ADDR_M4 (PCI_BASE_ADDR + 0x180)

typedef enum
{
	SDIO_PIN0 = 0,
	SDIO_PIN1,
	SDIO_PIN2,
	SDIO_PIN3,
	SDIO_PIN4,
	SDIO_PIN5,
	SDIO_PIN6,
	SDIO_PIN7,

} SDIO_PINx;

#define IS_PCI_PINx(PIN) (((PIN) == SDIO_PIN0) || \
						  ((PIN) == SDIO_PIN1) || \
						  ((PIN) == SDIO_PIN2) || \
						  ((PIN) == SDIO_PIN3) || \
						  ((PIN) == SDIO_PIN4) || \
						  ((PIN) == SDIO_PIN5) || \
						  ((PIN) == SDIO_PIN6) || \
						  ((PIN) == SDIO_PIN7))

//VD
#define PCI_VD_H_LEVEL_1 0x03 //3.62
#define PCI_VD_H_LEVEL_2 0x02 //3.70
#define PCI_VD_H_LEVEL_3 0x01 //3.79
#define PCI_VD_H_LEVEL_4 0x00 //3.88

#define PCI_VD_L_LEVEL_1 0x00 //1.42
#define PCI_VD_L_LEVEL_2 0x01 //1.48
#define PCI_VD_L_LEVEL_3 0x02 //1.53
#define PCI_VD_L_LEVEL_4 0x03 //1.59
#define PCI_VD_L_LEVEL_5 0x04 //1.66
#define PCI_VD_L_LEVEL_6 0x05 //1.73
#define PCI_VD_L_LEVEL_7 0x06 //1.81
#define PCI_VD_L_LEVEL_8 0x07 //1.90

//TD
#define PCI_TD_H_LEVEL_1 0x00  //coldest
#define PCI_TD_H_LEVEL_2 0x01  //colder
#define PCI_TD_H_LEVEL_3 0x02  //-39
#define PCI_TD_H_LEVEL_4 0x03  //-35
#define PCI_TD_H_LEVEL_5 0x04  //-31
#define PCI_TD_H_LEVEL_6 0x05  //-27
#define PCI_TD_H_LEVEL_7 0x06  //-23
#define PCI_TD_H_LEVEL_8 0x07  //-19
#define PCI_TD_H_LEVEL_9 0x08  //-15
#define PCI_TD_H_LEVEL_10 0x09 //-11
#define PCI_TD_H_LEVEL_11 0x0a //-7
#define PCI_TD_H_LEVEL_12 0x0b //-3
#define PCI_TD_H_LEVEL_13 0x0c //1
#define PCI_TD_H_LEVEL_14 0x0d //5
#define PCI_TD_H_LEVEL_15 0x0e //9
#define PCI_TD_H_LEVEL_16 0x0f //13
#define PCI_TD_H_LEVEL_17 0x10 //17
#define PCI_TD_H_LEVEL_18 0x11 //21
#define PCI_TD_H_LEVEL_19 0x12 //25
#define PCI_TD_H_LEVEL_20 0x13 //29
#define PCI_TD_H_LEVEL_21 0x14 //33
#define PCI_TD_H_LEVEL_22 0x15 //37
#define PCI_TD_H_LEVEL_23 0x16 //41
#define PCI_TD_H_LEVEL_24 0x17 //45
#define PCI_TD_H_LEVEL_25 0x18 //49
#define PCI_TD_H_LEVEL_26 0x19 //53
#define PCI_TD_H_LEVEL_27 0x1a //57
#define PCI_TD_H_LEVEL_28 0x1b //61
#define PCI_TD_H_LEVEL_29 0x1c //65
#define PCI_TD_H_LEVEL_30 0x1d //69
#define PCI_TD_H_LEVEL_31 0x1e //73
#define PCI_TD_H_LEVEL_32 0x1f //77
#define PCI_TD_H_LEVEL_33 0x20 //81
#define PCI_TD_H_LEVEL_34 0x21 //85
#define PCI_TD_H_LEVEL_35 0x22 //89
#define PCI_TD_H_LEVEL_36 0x23 //93
#define PCI_TD_H_LEVEL_37 0x24 //97
#define PCI_TD_H_LEVEL_38 0x25 //101
#define PCI_TD_H_LEVEL_39 0x26 //105
#define PCI_TD_H_LEVEL_40 0x27 //109
#define PCI_TD_H_LEVEL_41 0x28 //113
#define PCI_TD_H_LEVEL_42 0x29 //117
#define PCI_TD_H_LEVEL_43 0x2a //121
#define PCI_TD_H_LEVEL_44 0x2b //125

#define PCI_TD_L_LEVEL_1 0x0a  //-39
#define PCI_TD_L_LEVEL_2 0x0b  //-35
#define PCI_TD_L_LEVEL_3 0x0c  //-31
#define PCI_TD_L_LEVEL_4 0x0d  //-27
#define PCI_TD_L_LEVEL_5 0x0e  //-23
#define PCI_TD_L_LEVEL_6 0x0f  //-19
#define PCI_TD_L_LEVEL_7 0x10  //-15
#define PCI_TD_L_LEVEL_8 0x11  //-11
#define PCI_TD_L_LEVEL_9 0x12  //-7
#define PCI_TD_L_LEVEL_10 0x13 //-3
#define PCI_TD_L_LEVEL_11 0x14 //1
#define PCI_TD_L_LEVEL_12 0x15 //5
#define PCI_TD_L_LEVEL_13 0x16 //9
#define PCI_TD_L_LEVEL_14 0x17 //13
#define PCI_TD_L_LEVEL_15 0x18 //17
#define PCI_TD_L_LEVEL_16 0x19 //21
#define PCI_TD_L_LEVEL_17 0x1a //25
#define PCI_TD_L_LEVEL_18 0x1b //29
#define PCI_TD_L_LEVEL_19 0x1c //33
#define PCI_TD_L_LEVEL_20 0x1d //37
#define PCI_TD_L_LEVEL_21 0x1e //41
#define PCI_TD_L_LEVEL_22 0x1f //45

#define PCI_TD_TRIM_1 0x00	//-32
#define PCI_TD_TRIM_2 0x01	//-30
#define PCI_TD_TRIM_3 0x02	//-28
#define PCI_TD_TRIM_4 0x03	//-26
#define PCI_TD_TRIM_5 0x04	//-24
#define PCI_TD_TRIM_6 0x05	//-22
#define PCI_TD_TRIM_7 0x06	//-20
#define PCI_TD_TRIM_8 0x07	//-18
#define PCI_TD_TRIM_9 0x08	//-16
#define PCI_TD_TRIM_10 0x09 //-14
#define PCI_TD_TRIM_11 0x0a //-12
#define PCI_TD_TRIM_12 0x0b //-10
#define PCI_TD_TRIM_13 0x0c //-8
#define PCI_TD_TRIM_14 0x0d //-6
#define PCI_TD_TRIM_15 0x0e //-4
#define PCI_TD_TRIM_16 0x0f //-2
#define PCI_TD_TRIM_17 0x10 //0
#define PCI_TD_TRIM_18 0x11 //2
#define PCI_TD_TRIM_19 0x12 //4
#define PCI_TD_TRIM_20 0x13 //6
#define PCI_TD_TRIM_21 0x14 //7
#define PCI_TD_TRIM_22 0x15 //10
#define PCI_TD_TRIM_23 0x16 //12
#define PCI_TD_TRIM_24 0x17 //14
#define PCI_TD_TRIM_25 0x18 //16
#define PCI_TD_TRIM_26 0x19 //18
#define PCI_TD_TRIM_27 0x1a //20
#define PCI_TD_TRIM_28 0x1b //22
#define PCI_TD_TRIM_29 0x1c //24
#define PCI_TD_TRIM_30 0x1d //26
#define PCI_TD_TRIM_31 0x1e //28
#define PCI_TD_TRIM_32 0x1f //30

//sdio
#define PCI_SDIO_STATIC_CMP_DATA 0xFF
#define PCI_SDIO_STATIC_CHANNEL_0 0x01
#define PCI_SDIO_STATIC_CHANNEL_1 0x02
#define PCI_SDIO_STATIC_CHANNEL_2 0x04
#define PCI_SDIO_STATIC_CHANNEL_3 0x08
#define PCI_SDIO_STATIC_CHANNEL_4 0x10
#define PCI_SDIO_STATIC_CHANNEL_5 0x20
#define PCI_SDIO_STATIC_CHANNEL_6 0x40
#define PCI_SDIO_STATIC_CHANNEL_7 0x80

#define PCI_SDIO_DYNAMIC_CHANNEL_0 0x01
#define PCI_SDIO_DYNAMIC_CHANNEL_1 0x02
#define PCI_SDIO_DYNAMIC_CHANNEL_2 0x04
#define PCI_SDIO_DYNAMIC_CHANNEL_3 0x08

//detect
#define PCI_VD_L_FLAG 0x00000001
#define PCI_VD_H_FLAG 0x00000002
#define PCI_TD_L_FLAG 0x00000010
#define PCI_TD_H_FLAG 0x00000020

#define PCI_NVRAM_FLAG 0x00010000

#define PCI_SDIO_FLAG 0x800000

#define PCI_SDIO_CHANNEL_0 0x0100
#define PCI_SDIO_CHANNEL_1 0x0200
#define PCI_SDIO_CHANNEL_2 0x0400
#define PCI_SDIO_CHANNEL_3 0x0800
#define PCI_SDIO_CHANNEL_4 0x1000
#define PCI_SDIO_CHANNEL_5 0x2000
#define PCI_SDIO_CHANNEL_6 0x4000
#define PCI_SDIO_CHANNEL_7 0x8000

//SDIO pull up or pull down
#define SDIO_PULL_DOWN 0x00
#define SDIO_PULL_UP 0x01

//SDIO pull up or down current trim
#define SDIO_CURRENT_LEVEL_0 0x00 //Ĭ��,�ر�
#define SDIO_CURRENT_LEVEL_1 0x01
#define SDIO_CURRENT_LEVEL_2 0x02 //20M
#define SDIO_CURRENT_LEVEL_3 0x03
#define SDIO_CURRENT_LEVEL_4 0x04 //10M
#define SDIO_CURRENT_LEVEL_5 0x05
#define SDIO_CURRENT_LEVEL_6 0x06
#define SDIO_CURRENT_LEVEL_7 0x07

/*******************************************************************************
* Function Name  : PCI_Init
* Description    : PCIģ���ʼ��
* 				         ע�⣺
* 				         ÿ���ϵ�ǰҪִ�иó��򣬷����޷���дnvram�е�����
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void PCI_Init(UINT8 clk_sel);

/*******************************************************************************
* Function Name  : Nvram_Init
* Description    : nvramģ���ʼ��
* 				         ע�⣺
* 				    дNVRAMǰ����Ҫʹ�ܣ���ȡ��ʱ����Ҫ��
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void Nvram_Init(void);

extern void Nvram_Write(void);

extern void Nvram_Read(void);

/*******************************************************************************
* Function Name  : Nvram_Status
* Description    : �ж�NVram�Ƿ����
* Input          : None
*
* Output         : 0 - û�б����
*                  1 - �����
* Return         : None
******************************************************************************/
extern int Nvram_Status(void);

/*******************************************************************************
* Function Name  : PCI_SDIO_Init
* Description    : SDIO��ʼ������fail num����
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void PCI_SDIO_Init(void);

/*******************************************************************************
* Function Name  : PCI_SDIO_Static_Set
* Description    : ����SDIO��̬������ʽ����������SDIO�ܽſ��Խӵأ�cmp_dataֵд0����
* Input          : channel - ������SDIOͨ����
*                  cmp_data - �Ƚϵ�ѹֵ:0 - ��·SDIOΪ�͵�ѹά�֣��ߵ�ѹ������1 - ��·SDIOΪ�ߵ�ѹά�֣��͵�ѹ����
* Output         : None
* Return         : None
******************************************************************************/
extern void PCI_SDIO_Static_Set(unsigned char channel, unsigned char cmp_data);

/*******************************************************************************
* Function Name  : PCI_SDIO_Dynamic_Set
* Description    : ����SDIO��̬������ʽ
* Input          : channel - ������SDIOͨ����
* Output         : None
* Return         : None
******************************************************************************/
extern void PCI_SDIO_Dynamic_Set(unsigned char channel);

/*******************************************************************************
* Function Name  : PCI_VD_Set
* Description    : ����VD
* Input          : VDH_Value:��ѹ�����ֵ
*                  VDL_Value:��ѹ�����ֵ
* Output         : None
* Return         : None
******************************************************************************/
extern void PCI_VD_Set(unsigned char VDH_Value, unsigned char VDL_Value);

/*******************************************************************************
* Function Name  : PCI_TD_Set
* Description    : ����TD
* Input          : TDH_Value:���¼����ֵ
*                  TDL_Value:���¼����ֵ
* Output         : None
* Return         : None
******************************************************************************/
extern void PCI_TD_Set(unsigned char TDH_Value, unsigned char TDL_Value, unsigned char TD_Trim);

/*******************************************************************************
* Function Name  : PCI_Clear_Status
* Description    : ���PCI��ȫ������־λ
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void PCI_Clear_Status(void);

/*******************************************************************************
* Function Name  : PCI_Detect
* Description    : �ж�PCI��ȫ����Դ
* Input          : None
* Output         : None
* Return         : 0 - û�д���
*                  1 - �д���
******************************************************************************/
extern int PCI_Detect(void);

/*******************************************************************************
* Function Name  : PCI_SDIO_PullControl
* Description    : PCI��ȫ�����жϴ�����
*                  PCI�жϹ��ܣ���оƬ�����ϵ��Ҫ�������á����ϵ�����ò��ᱣ����
* Input          : channel - ������SDIOͨ���ţ�����PCI_SDIO_STATIC_CHANNEL_0
*                  mode    - Pull up or Pull down
* Output         : None
* Return         : None
******************************************************************************/
extern void PCI_SDIO_PullControl(unsigned char channel, unsigned char mode);

/*******************************************************************************
* Function Name  : PCI_SDIO_Current_Trim
* Description    : PCI SDIO���������ܵ���Trimֵ
* Input          : channel - ������SDIOͨ���ţ�����PCI_SDIO_STATIC_CHANNEL_0
*                  mode    - Pull up or Pull down
* Output         : None
* Return         : None
******************************************************************************/
extern void PCI_SDIO_Current_Trim(unsigned char value);

/*******************************************************************************
* Function Name  : SDIO_ConfigGpio
* Description    : SDIO���ó�GPIO��;
* Input          : - SDIO_PINx: EPORT Pin��where x can be 0~7 to select the SDIO peripheral.
*                  - GpioDir������GPIO����   GPIO_OUTPUT�����  GPIO_INPUT������
*
* Output         : None
* Return         : 0 - �ɹ�;other - ʧ��
******************************************************************************/
extern int SDIO_ConfigGpio(SDIO_PINx GpioNo, UINT8 GpioDir);

/*******************************************************************************
* Function Name  : SDIO_WriteGpioData
* Description    : ����SDIO_PINx��Ӧ���ŵĵ�ƽ
* Input          : - SDIO_PINx: SDIO Pin��where x can be 0~7 to select the SDIO peripheral.
*                  - bitVal�����õĵ�ƽ��Bit_SET������Ϊ�ߵ�ƽ  Bit_RESET������Ϊ�͵�ƽ
*
* Output         : None
* Return         : 0: ���óɹ�    other������ʧ��
******************************************************************************/
extern int SDIO_WriteGpioData(SDIO_PINx GpioNo, UINT8 bitVal);

/*******************************************************************************
* Function Name  : SDIO_ReadGpioData
* Description    : ��ȡSDIO_PINx��Ӧ���ŵĵ�ƽ
* Input          : - SDIO_PINx: SDIO Pin��where x can be 0~7 to select the SDIO peripheral.
*
* Output         : None
* Return         : Bit_SET:�ߵ�ƽ  Bit_RESET���͵�ƽ  -1��ʧ��
******************************************************************************/
extern int SDIO_ReadGpioData(SDIO_PINx GpioNo);

/*******************************************************************************
* Function Name  : PCI_Interrupt_Init
* Description    : ����PCI�İ�ȫ�ж�
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void PCI_Interrupt_Init(void);

/*******************************************************************************
* Function Name  : PCI_NVSRAM_WtiteByte
* Description    : ���ֽ���nvramд����
* Input          : - addr: д�����ݵĵ�ַ.
*                  - dat��д�������
*
* Output         : None
* Return         : other - �ɹ�;0 - ʧ��
******************************************************************************/
extern BOOL PCI_NVSRAM_WtiteByte(UINT32 addr, UINT8 dat);

/*******************************************************************************
* Function Name  : PCI_NVSRAM_ReadByte
* Description    : ���ֽڶ�ȡnvram����
* Input          : - addr: ���ݵĵ�ַ.
*                  - dat������
*
* Output         : None
* Return         : other - �ɹ�;0 - ʧ��
******************************************************************************/
extern BOOL PCI_NVSRAM_ReadByte(UINT32 addr, UINT8 *dat);

/*******************************************************************************
* Function Name  : PCI_Open
* Description    : PCIģ��Open
*                  PCI_Init�ļ򻯰棬ִ��PCI_Open���ܹ�����TD��VD��SDIO�ȼĴ����������ܹ���дNVSRAM�������������ȫ������־λ
*                  �ú���ʹ�õ�ǰ���ǣ����ù�PCI_Init 
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void PCI_Open(void);

/*******************************************************************************
* Function Name  : PCI_Close
* Description    : PCIģ��Close
* 				   �ر�PCIģ��Ķ�д���ܣ�������PCI_Init��PCI_Open�������ӵ�3uA���ģ�
*                  ͬʱ���޷�������ȡNVSRAM���ݣ���ȡPCI�Ĵ�����־λ��
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
extern void PCI_Close(void);

extern void PCI_InitWakeup(void);
extern void PCI_unlockNVSRAM(void);
extern void ASYNC_TIMER_IRQHandler(void);
extern uint32_t PCI_AsyncTC_Init(UINT32 timeCount);
extern uint32_t PCI_AsyncTC_Close(void);
extern void PCI_AsycTimer_callback(void);
#endif /* __PCI_DRV_H__ */
