/*
 * usi_dev.h
 *
 *  Created on: 2017��5��26��
 *      Author: YangWenfeng
 */

#ifndef USI_DEV_H_
#define USI_DEV_H_

#include "usi_reg.h"
#include "reset_reg.h"

//#define USI_BASE_ADDR      USI1_BASE_ADDR
#define USI_BASE_ADDR      USI2_BASE_ADDR

#define USI         ((USI_TypeDef *)USI_BASE_ADDR)

#if(USI_BASE_ADDR == USI1_BASE_ADDR)
	#define USI_IRQn    USI1_IRQn
#else
	#define USI_IRQn    USI2_IRQn
#endif

#define USI1         ((USI_TypeDef *)USI1_BASE_ADDR)
#define USI2         ((USI_TypeDef *)USI2_BASE_ADDR)

typedef enum
{
    USI_RST = 0,
    USI_CLK,
    USI_DAT,
}USI_PIN;

#define IS_USI_PINx(pin) (((pin) == USI_RST) || \
                          ((pin) == USI_CLK) ||\
                         ((pin) == USI_DAT) )
                          
#define ISO7816_1    1
#define ISO7816_2    2

#define card_1v8			0x0
#define card_3v				0x1
#define card_3v3			0x2
#define card_5v			    0x3

extern USI_TypeDef * g_USIReg;

/*******************************************************************************
* Function Name  : USI_ConfigGpio
* Description    : USI���ó�GPIO��;
* Input          : - USIx    : USI�Ļ���ַ��USI1����USI2
*                  - USI_PINx��USI��Ӧ��PIN�ţ�ȡֵUSI_RST��USI_CLK��USI_DAT
*                  - GPIO_Dir������GPIO����   GPIO_OUTPUT�����  GPIO_INPUT������
*
* Output         : None
* Return         : None
******************************************************************************/
extern void USI_ConfigGpio(USI_TypeDef * USIx, USI_PIN USI_PINx, UINT8 GPIO_Dir);

/*******************************************************************************
* Function Name  : USI_ReadGpioData
* Description    : ��ȡUSI_PINx��Ӧ���ŵĵ�ƽ
* Input          : - USIx    : USI�Ļ���ַ��USI1����USI2
*                  - USI_PINx��USI��Ӧ��PIN�ţ�ȡֵUSI_RST��USI_CLK��USI_DAT
*
* Output         : None
* Return         : Bit_SET:�ߵ�ƽ  Bit_RESET���͵�ƽ
******************************************************************************/
extern UINT8 USI_ReadGpioData(USI_TypeDef * USIx, USI_PIN USI_PINx);

/*******************************************************************************
* Function Name  : USI_WriteGpioData
* Description    : ����USI_PINx��Ӧ���ŵĵ�ƽ
* Input          : - USIx    : USI�Ļ���ַ��USI1����USI2
*                  - USI_PINx��SPI��Ӧ��PIN�ţ�ȡֵUSI_RST��USI_CLK��USI_DAT
*                  - bitVal�����õĵ�ƽ��Bit_SET������Ϊ�ߵ�ƽ  Bit_RESET������Ϊ�͵�ƽ
*
* Output         : None
* Return         : None
******************************************************************************/
extern void USI_WriteGpioData(USI_TypeDef * USIx, USI_PIN USI_PINx, UINT8 bitVal);

extern void USI_Init(void);
extern void USI_DeInit(void);

extern void USI_PullLow(void);
extern void USI_Resume(void);

extern void USIMasterInit(void);
extern void USIMasterHotInit(void);

extern void Card2_Power_On(void);
extern void Card2_Power_Off(void);
extern void Card2_Power_Switch(UINT8 mode);

extern void Card3_Power_On(void);
extern void Card3_Power_Off(void);
extern void Card3_Power_Switch(UINT8 mode);

extern void USI_OpenWTC(UINT32 etu);

extern void TransmitByte(UINT8 data);
extern UINT8 ReceiveByte(void);
#endif /* USI_DEV_H_ */
