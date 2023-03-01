// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : adc_drv.h
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


#ifndef __ADC_DRV_H__
#define __ADC_DRV_H__

#include "type.h"
#include "adc_reg.h"
#include "memmap.h"


#define ADCCH_0	    0
#define ADCCH_1	    8
#define ADCCH_2	    1
#define ADCCH_9	    9 		//ͨ��9ֱ�������ڲ�1.1V��׼��ѹ
#define ADCCH_IN_0  4
#define ADCCH_IN_1  12
#define ADCCH_IN_2  5
#define ADCCH_IN_3  13
#define ADCCH_IN_4  6
#define ADCCH_IN_5  14
#define ADCCH_IN_6  7
#define ADCCH_IN_7  15

#define ADC_DIS     0
#define ADC_EN      1


#define ADC		((ADC_TypeDef *)(ADC_BASE_ADDR))


/*******************************************************************************
* Function Name  : ADC_Init
* Description    : ADC ��ʼ��
* Input          :  - cfgr1��ADC configuration register 1
*                   - cfgr2��ADC configuration register 2
*                   - chselr1�� ADC channel selection register 1
*                   - chselr2: ADC channel selection register 2
*                   - smpr��    ADC sampling time register
*
* Output         : None
* Return         : None
******************************************************************************/
extern void ADC_Init(UINT32 cfgr1, UINT32 cfgr2, UINT32 chselr1, UINT32 chselr2, UINT32 smpr);



/*******************************************************************************
* Function Name  : ADC_GetConversionValue
* Description    : ADC ����������
* Input          :  - ADC_CHx��ADC����ͨ��  ��ӦоƬ���� ADC_INx (����xȡֵ0~7)
*
* Output         : None
* Return         : ��Ӧ����AD�ɼ���ֵ
******************************************************************************/
extern UINT32 ADC_GetConversionValue(UINT8 ADC_CHx);


 /*******************************************************************************
 * Function Name  : ADC_Cmd
 * Description    : Enable or Disable ADC
 * Input          :  - en��ADC_DIS----disable ADC   ADC_EN----enable ADC
 *
 * Output         : None
 * Return         : None
 ******************************************************************************/
 extern void ADC_Cmd(BOOL en);


 /*******************************************************************************
 * Function Name  : ADC_StartConv
 * Description    : ADC ת������
 * Input          : None
 *
 * Output         : None
 * Return         : None
 ******************************************************************************/

 extern void ADC_StartConv(void);

 /*******************************************************************************
 * Function Name  : ADC_StopConv
 * Description    : ADC ת��ֹͣ
 * Input          : None
 *
 * Output         : None
 * Return         : None
 ******************************************************************************/
 extern void ADC_StopConv(void);
 
/*******************************************************************************
* Function Name  : ADC_Get_Battery
* Description    : �ɼ�﮵�ص���
* Input          : - ADC_CHx��ADC����ͨ��  ��ӦоƬ���� ADC_INx (����xȡֵ0~7)
*
* Output         : None
* Return         : None
******************************************************************************/
extern unsigned int ADC_Get_Battery(unsigned char ADC_CHx);


#endif /* __ADC_DRV_H__ */
