// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : adc_drv.c
// Version      : V0.1
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#include "type.h"
#include "adc_drv.h"
#include "cpm_drv.h"
#include "debug.h"
#include "delay.h"

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
void ADC_Init(UINT32 cfgr1, UINT32 cfgr2, UINT32 chselr1, UINT32 chselr2, UINT32 smpr)
{
	ADC->CFGR1 = cfgr1;
	ADC->CFGR2 = cfgr2;
	ADC->CHSELR1 = chselr1;
	ADC->CHSELR2 = chselr2;
	ADC->SMPR = smpr;
}
/*******************************************************************************
* Function Name  : ADC_ChannelSelEn
* Description    : ADC ͨ��ʹ��
* Input          :  - ADC_CHx��ADC����ͨ��  ��ӦоƬ���ţ�ȡֵ����
*                     //ADC_INxģ��ӿڣ��Ƽ�ʹ��
*                     ADC_IN0:0
*                     ADC_IN1:8
*                     ADC_IN2:1
*                     ADC_IN9:9  //ͨ��9ֱ�������ڲ�1.1V��׼��ѹ
*                     //ADCCH_IN_x���ýӿ�
*                     ADCCH_IN_1:12
*                     ADCCH_IN_2:5
*                     ADCCH_IN_3:13
*                     ADCCH_IN_4:6
*                     ADCCH_IN_5:14
*                     ADCCH_IN_6:7
*                     ADCCH_IN_7:15
*
* Output         : None
* Return         : None
******************************************************************************/
void ADC_ChannelSelEn(UINT8 ADC_CHx)
{
    ADC->CHSELR3 |= (1<<ADC_CHx);
}

/*******************************************************************************
* Function Name  : ADC_ChannelSelEn
* Description    : ADC ͨ��ʧ��
* Input          :  - ADC_CHx��ADC����ͨ��  ��ӦоƬ���ţ�ȡֵ����
*                     //ADC_INxģ��ӿڣ��Ƽ�ʹ��
*                     ADC_IN0:0
*                     ADC_IN1:8
*                     ADC_IN2:1
*                     ADC_IN9:9  //ͨ��9ֱ�������ڲ�1.1V��׼��ѹ
*                     //ADCCH_IN_x���ýӿ�
*                     ADCCH_IN_1:12
*                     ADCCH_IN_2:5
*                     ADCCH_IN_3:13
*                     ADCCH_IN_4:6
*                     ADCCH_IN_5:14
*                     ADCCH_IN_6:7
*                     ADCCH_IN_7:15
*
* Output         : None
* Return         : None
******************************************************************************/
void ADC_ChannelSelDis(UINT8 ADC_CHx)
{
    ADC->CHSELR3 &= ~(1<<ADC_CHx);
}

/*******************************************************************************
* Function Name  : ADC_GetConversionValue
* Description    : ADC ����������
* Input          :  - ADC_CHx��ADC����ͨ��  ��ӦоƬ���ţ�ȡֵ����
*                     //ADC_INxģ��ӿڣ��Ƽ�ʹ��
*                     ADC_IN0:0
*                     ADC_IN1:8
*                     ADC_IN2:1
*                     ADC_IN9:9  //ͨ��9ֱ�������ڲ�1.1V��׼��ѹ
*                     //ADCCH_IN_x���ýӿ�
*                     ADCCH_IN_1:12
*                     ADCCH_IN_2:5
*                     ADCCH_IN_3:13
*                     ADCCH_IN_4:6
*                     ADCCH_IN_5:14
*                     ADCCH_IN_6:7
*                     ADCCH_IN_7:15
*
* Output         : None
* Return         : ��Ӧ����AD�ɼ���ֵ
******************************************************************************/
UINT32 ADC_GetConversionValue(UINT8 ADC_CHx)
{
	UINT32 adcisr = 0,i;

    ADC_ChannelSelEn(ADC_CHx);
	ADC_Init((ADC_SEQ_LEN_1|ADC_OVRMOD_LAST_CONV|ADC_CONTINUOUS_CONV_MODE|ADC_EXTERNAL_VREF|ADC_RIGHT_ALIGN|ADC_CONV_RESOLUTION_12BIT),
		     ((3<<8)|0x20),//1    0x20
				 CCW0(ADC_CHx),
				 0,
		     0x20);//0x20
	ADC_Cmd(ADC_EN);
	
	for (i = 0; i < 9; i++)
	{
		ADC_StartConv();
		while (EOSEQ !=(ADC->ISR & EOSEQ));
		ADC->ISR |= EOSEQ;
		ADC_StopConv();

		if(i == 0)
		{// ��һ��������
			adcisr = (ADC->uFIFO_DAT.FIFO)&0xFFFF;
			adcisr = 0;
			continue;
		}
		adcisr += (ADC->uFIFO_DAT.FIFO)&0xFFFF;

	}

	ADC_Cmd(ADC_DIS);
    
    ADC_ChannelSelDis(ADC_CHx);

	return ((adcisr>>3)&0x0fff);
}

/*******************************************************************************
* Function Name  : adc_module_dis
* Description    : ADCģ�����
* 				         ע�⣺
* 				    None
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
static void adc_module_dis(void)
{
	ADC->CR = ADDIS;
	while (0 != ADC->CR);

}
/*******************************************************************************
* Function Name  : adc_module_en
* Description    : ADCģ��ʹ��
* 				         ע�⣺
* 				    None
* Input          : None
* Output         : None
* Return         : None
******************************************************************************/
static void adc_module_en(void)
{
	ADC->CR = ADEN;
	while (ADRDY != (ADC->ISR& ADRDY));
}


/*******************************************************************************
* Function Name  : ADC_Cmd
* Description    : Enable or Disable ADC
* Input          :  - en��ADC_DIS----disable ADC   ADC_EN----enable ADC
*
* Output         : None
* Return         : None
******************************************************************************/
void ADC_Cmd(BOOL en)
{
	if (en == ADC_DIS)
	{
		adc_module_dis();
	}
	else
	{
		adc_module_en();
	}
}



/*******************************************************************************
* Function Name  : ADC_StartConv
* Description    : ADC ת������
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void ADC_StartConv(void)
{
	UINT32 adccr;
	adccr = ADC->CR;
	adccr |= ADSTART;
	ADC->CR = adccr;
}

/*******************************************************************************
* Function Name  : ADC_StopConv
* Description    : ADC ת��ֹͣ
* Input          : None
*
* Output         : None
* Return         : None
******************************************************************************/
void ADC_StopConv(void)
{
    volatile uint32_t flag;
	ADC->CR |= ADSTP;
    
    do
    {
        flag = ADC->CR;
    }while (ADSTP&flag);
}

/*******************************************************************************
* Function Name  : ADC_Get_Battery
* Description    : �ɼ�﮵�ص���
* Input          : - ADC_CHx��ADC����ͨ��  ��ӦоƬ���� ADC_INx (����xȡֵ0~7)
*
* Output         : None
* Return         : None
******************************************************************************/
unsigned int ADC_Get_Battery(unsigned char ADC_CHx)
{
	unsigned int resVC = 0;
	unsigned int res_battery = 0;
	unsigned int res_standard = 0;
	unsigned int reg_tmp;
	int i;

	for(i = 0; i < 10; i ++)
	{
		res_battery += ADC_GetConversionValue(ADC_CHx);
	}
	
	//input core test key
	CPM_Write_CoreTestKey(1);
	CPM->CPM_VCCCTMR |= (1<<21);
	
	reg_tmp = CPM->CPM_VCCVTRIMR;
	//CPM->CPM_VCCCTMR |= (1<<21);//OVERWR_VCC_TRIMλ����
	CPM->CPM_VCCVTRIMR |=  (1<<8);//��v_store reg
	CPM->CPM_VCCVTRIMR |=  (1<<10);//��trim en reg
	CPM->CPM_VCCVTRIMR |=  (1<<9);
	CPM->CPM_VCCCTMR &= ~(1<<21);
	
//	ADC_GetConversionValue(ADCCH_9);
//	for(i = 0; i < 10; i ++)
	{
		res_standard += ADC_GetConversionValue(ADCCH_9);
	}
	

	CPM->CPM_VCCVTRIMR &=  ~(1<<8);
	CPM->CPM_VCCVTRIMR &=  ~(1<<10);
	
	CPM->CPM_VCCCTMR |= (1<<21);
	CPM->CPM_VCCVTRIMR = reg_tmp;
    CPM->CPM_VCCCTMR &= ~(1<<21);

	CPM_Write_CoreTestKey(0);

	resVC = 9*res_battery/res_standard;

	return resVC;
}
