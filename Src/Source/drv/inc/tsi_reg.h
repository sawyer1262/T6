// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// File name    : tsi_reg.h
// Version      : V0.2
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifndef _TSI_REG_H_
#define _TSI_REG_H_

#include "type.h"
#include "memmap.h"

#define TSI ((TSI_TypeDef *)(TSI_BASE_ADDR))

typedef struct
{
	volatile unsigned int TSI_ISR;			/*!< TSI�ж�״̬�Ĵ���, ƫ�Ƶ�ַ: 0x0000 */
	volatile unsigned int TSI_IER;			/*!< TSI�ж����üĴ���, ƫ�Ƶ�ַ: 0x0004 */
	volatile unsigned int TSI_CR;			/*!< TSI���ƼĴ���, ƫ�Ƶ�ַ: 0x0008 */
	volatile unsigned int TSI_TSTR;			/*!< TSIģ���������üĴ���, ƫ�Ƶ�ַ: 0x000c */
	volatile unsigned int TSI_WDCFGR;		/*!< TSIģ�⿴�Ź����üĴ���, ƫ�Ƶ�ַ: 0x0010 */
	volatile unsigned int TSI_AWDTR;		/*!< TSIģ�⿴�Ź���ֵ�Ĵ���, ƫ�Ƶ�ַ: 0x0014 */
	volatile unsigned int TSI_CHCR;			/*!< TSIģʽ��ɨ��������üĴ���, ƫ�Ƶ�ַ: 0x0018 */
	volatile unsigned int TSI_CH0DR;		/*!< TSI-CH0���ݼĴ���, ƫ�Ƶ�ַ: 0x001c */
	volatile unsigned int TSI_CH1DR;		/*!< TSI-CH1���ݼĴ���, ƫ�Ƶ�ַ: 0x0020 */
	volatile unsigned int TSI_CH2DR;		/*!< TSI-CH2���ݼĴ���, ƫ�Ƶ�ַ: 0x0024 */
	volatile unsigned int TSI_CH3DR;		/*!< TSI-CH3���ݼĴ���, ƫ�Ƶ�ַ: 0x0028 */
	volatile unsigned int TSI_CH4DR;		/*!< TSI-CH4���ݼĴ���, ƫ�Ƶ�ַ: 0x002c */
	volatile unsigned int TSI_CH5DR;		/*!< TSI-CH5���ݼĴ���, ƫ�Ƶ�ַ: 0x0030 */
	volatile unsigned int TSI_CH6DR;		/*!< TSI-CH6���ݼĴ���, ƫ�Ƶ�ַ: 0x0034 */
	volatile unsigned int TSI_CH7DR;		/*!< TSI-CH7���ݼĴ���, ƫ�Ƶ�ַ: 0x0038 */
	volatile unsigned int TSI_CH8DR;		/*!< TSI-CH8���ݼĴ���, ƫ�Ƶ�ַ: 0x003c */
	volatile unsigned int TSI_CH9DR;		/*!< TSI-CH9���ݼĴ���, ƫ�Ƶ�ַ: 0x0040 */
	volatile unsigned int TSI_CHaDR;		/*!< TSI-CH10���ݼĴ���, ƫ�Ƶ�ַ: 0x0044 */
	volatile unsigned int TSI_CHbDR;		/*!< TSI-CH11���ݼĴ���, ƫ�Ƶ�ַ: 0x0048 */
	volatile unsigned int TSI_CHcDR;		/*!< TSI-CH12���ݼĴ���, ƫ�Ƶ�ַ: 0x004c */
	volatile unsigned int TSI_CHdDR;		/*!< TSI-CH13���ݼĴ���, ƫ�Ƶ�ַ: 0x0050 */
	volatile unsigned int TSI_CHeDR;		/*!< TSI-CH14���ݼĴ���, ƫ�Ƶ�ַ: 0x0054 */
	volatile unsigned int TSI_CHfDR;		/*!< TSI-CH15���ݼĴ���, ƫ�Ƶ�ַ: 0x0058 */
	volatile unsigned int TSI_RESERVED;		/*!< TSI �����Ĵ���, ƫ�Ƶ�ַ: 0x005c */
	volatile unsigned int TSI_RESERVED1;	/*!< TSI �����Ĵ���, ƫ�Ƶ�ַ: 0x0060 */
	volatile unsigned int TSI_BASE03;		/*!< TSI CH0~3 Cbase�Ĵ���, ƫ�Ƶ�ַ: 0x0064 */
	volatile unsigned int TSI_BASE47;		/*!< TSI CH4~7 Cbase�Ĵ���, ƫ�Ƶ�ַ: 0x0068 */
	volatile unsigned int TSI_BASE8b;		/*!< TSI CH8~b Cbase�Ĵ���, ƫ�Ƶ�ַ: 0x006c */
	volatile unsigned int TSI_BASEcf;		/*!< TSI CHc~f Cbase�Ĵ���, ƫ�Ƶ�ַ: 0x0070 */
	volatile unsigned int TSI_CHEN;			/*!< TSIͨ��ʹ�ܼĴ���, ƫ�Ƶ�ַ: 0x0074 */
	volatile unsigned int TSI_TCFLTCR;		/*!< TSI �˲������ƼĴ���, ƫ�Ƶ�ַ: 0x0078 */
	volatile unsigned int TSI_RESERVED3;	/*!< TSI �����Ĵ���, ƫ�Ƶ�ַ: 0x007c */
	volatile unsigned int TSI_DELTA03;		/*!< TSI ����������ֵ�����Ĵ���, ƫ�Ƶ�ַ: 0x0080 */
	volatile unsigned int TSI_DELTA47;		/*!< TSI ����������ֵ�����Ĵ���, ƫ�Ƶ�ַ: 0x0084 */
	volatile unsigned int TSI_DELTA8b;		/*!< TSI ����������ֵ�����Ĵ���, ƫ�Ƶ�ַ: 0x0088 */
	volatile unsigned int TSI_DELTAcf;		/*!< TSI ����������ֵ�����Ĵ���, ƫ�Ƶ�ַ: 0x008c */
	volatile unsigned int TSI_Cid_coarse03; /*!< TSI CH0~3 ����Coarse�Ĵ���, ƫ�Ƶ�ַ: 0x0090 */
	volatile unsigned int TSI_Cid_coarse47; /*!< TSI CH4~7 ����Coarse�Ĵ���, ƫ�Ƶ�ַ: 0x0094 */
	volatile unsigned int TSI_Cid_coarse8b; /*!< TSI CH8~b ����Coarse�Ĵ���, ƫ�Ƶ�ַ: 0x0098 */
	volatile unsigned int TSI_Cid_coarsecf; /*!< TSI CHc~f ����Coarse�Ĵ���, ƫ�Ƶ�ַ: 0x009c */
	volatile unsigned int TSI_NHB01;		/*!< TSI CH0��CH1 ����ͨ��Track���ƼĴ���, ƫ�Ƶ�ַ: 0x00a0 */
	volatile unsigned int TSI_NHB23;		/*!< TSI CH2��CH3 ����ͨ��Track���ƼĴ���, ƫ�Ƶ�ַ: 0x00a4 */
	volatile unsigned int TSI_NHB45;		/*!< TSI CH4��CH5 ����ͨ��Track���ƼĴ���, ƫ�Ƶ�ַ: 0x00a8 */
	volatile unsigned int TSI_NHB67;		/*!< TSI CH6��CH7 ����ͨ��Track���ƼĴ���, ƫ�Ƶ�ַ: 0x00ac */
	volatile unsigned int TSI_NHB89;		/*!< TSI CH8��CH9 ����ͨ��Track���ƼĴ���, ƫ�Ƶ�ַ: 0x00b0 */
	volatile unsigned int TSI_NHBab;		/*!< TSI CHa��CHb ����ͨ��Track���ƼĴ���, ƫ�Ƶ�ַ: 0x00b4 */
	volatile unsigned int TSI_NHBcd;		/*!< TSI CHc��CHd ����ͨ��Track���ƼĴ���, ƫ�Ƶ�ַ: 0x00b8 */
	volatile unsigned int TSI_NHBef;		/*!< TSI CHe��CHf ����ͨ��Track���ƼĴ���, ƫ�Ƶ�ַ: 0x00bc */
	volatile unsigned int TSI_RESERVED4;	/*!< TSI �����Ĵ���, ƫ�Ƶ�ַ: 0x00c0 */
	volatile unsigned int TSI_SEED0;		/*!< TSI ģ��IP Trim SEED0 �Ĵ���, ƫ�Ƶ�ַ: 0x00c4 */
	volatile unsigned int TSI_SEED1;		/*!< TSI ģ��IP Trim SEED0 �Ĵ���, ƫ�Ƶ�ַ: 0x00c8 */
	volatile unsigned int TSI_SEED2;		/*!< TSI ģ��IP Trim SEED0 �Ĵ���, ƫ�Ƶ�ַ: 0x00cc */
	volatile unsigned int TSI_UserData[8];	/*!< TSI �û����ݼĴ���, ƫ�Ƶ�ַ: 0x00d0~0x00ec */
	volatile unsigned int TSI_ANACR;		/*!< TSI ģ����ƼĴ���, ƫ�Ƶ�ַ: 0x00f0 */
	volatile unsigned int TSI_DCCR;			/*!< TSI ��ʱ�жϿ��ƼĴ���, ƫ�Ƶ�ַ: 0x00f4 */
	volatile unsigned int TSI_ENV_CR;		/*!< TSI �������ƼĴ���, ƫ�Ƶ�ַ: 0x00f8 */
	volatile unsigned int TSI_RESERVED5;	/*!< TSI �����Ĵ���, ƫ�Ƶ�ַ: 0x00fc */
	volatile unsigned int TSI_AVG_03;		/*!< TSI CH0~3 average�Ĵ���, ƫ�Ƶ�ַ: 0x0100 */
	volatile unsigned int TSI_AVG_47;		/*!< TSI CH4~7 average�Ĵ���, ƫ�Ƶ�ַ: 0x0104 */
	volatile unsigned int TSI_AVG_8b;		/*!< TSI CH8~b average�Ĵ���, ƫ�Ƶ�ַ: 0x0108 */
	volatile unsigned int TSI_AVG_cf;		/*!< TSI CHc~f average�Ĵ���, ƫ�Ƶ�ַ: 0x010c */
	volatile unsigned int TSI_ENV_DATA_03;	/*!< TSI CH0~3 �����������üĴ���, ƫ�Ƶ�ַ: 0x0110 */
	volatile unsigned int TSI_ENV_DATA_47;	/*!< TSI CH4~7 �����������üĴ���, ƫ�Ƶ�ַ: 0x0114 */
	volatile unsigned int TSI_ENV_DATA_8b;	/*!< TSI CH8~b �����������üĴ���, ƫ�Ƶ�ַ: 0x0118 */
	volatile unsigned int TSI_ENV_DATA_cf;	/*!< TSI CHc~f �����������üĴ���, ƫ�Ƶ�ַ: 0x011c */
	volatile unsigned int TSI_ENV_DLT_03;	/*!< TSI CH0~3 �����������üĴ���, ƫ�Ƶ�ַ: 0x0120 */
	volatile unsigned int TSI_ENV_DLT_47;	/*!< TSI CH4~7 �����������üĴ���, ƫ�Ƶ�ַ: 0x0124 */
	volatile unsigned int TSI_ENV_DLT_8b;	/*!< TSI CH8~b �����������üĴ���, ƫ�Ƶ�ַ: 0x0128 */
	volatile unsigned int TSI_ENV_DLT_cf;	/*!< TSI CHc~f �����������üĴ���, ƫ�Ƶ�ַ: 0x012c */
	volatile unsigned int TSI_AVG_SR_03;	/*!< TSI CH0~3 average״̬�Ĵ���, ƫ�Ƶ�ַ: 0x0130 */
	volatile unsigned int TSI_AVG_SR_47;	/*!< TSI CH4~7 average״̬�Ĵ���, ƫ�Ƶ�ַ: 0x0134 */
	volatile unsigned int TSI_AVG_SR_8b;	/*!< TSI CH8~b average״̬�Ĵ���, ƫ�Ƶ�ַ: 0x0138 */
	volatile unsigned int TSI_AVG_SR_cf;	/*!< TSI CHc~f average״̬�Ĵ���, ƫ�Ƶ�ַ: 0x013c */
	volatile unsigned int TSI_ENV_SR_03;	/*!< TSI CH0~3 ��������״̬�Ĵ���, ƫ�Ƶ�ַ: 0x0140 */
	volatile unsigned int TSI_ENV_SR_47;	/*!< TSI CH4~7 ��������״̬�Ĵ���, ƫ�Ƶ�ַ: 0x0144 */
	volatile unsigned int TSI_ENV_SR_8b;	/*!< TSI CH8~b ��������״̬�Ĵ���, ƫ�Ƶ�ַ: 0x0148 */
	volatile unsigned int TSI_ENV_SR_cf;	/*!< TSI CHc~f ��������״̬�Ĵ���, ƫ�Ƶ�ַ: 0x014c */
	volatile unsigned int TSI_AVG_LD;		/*!< TSI TSI ��ֵ���ؼĴ���, ƫ�Ƶ�ַ: 0x0150 */
	volatile unsigned int TSI_ENV_LD;		/*!< TSI TSI �����������ؼĴ���, ƫ�Ƶ�ַ: 0x0154 */
} TSI_TypeDef;

/******************************************************************************/
/*                                                                            */
/*                                  TSI                                       */
/*                                                                            */
/******************************************************************************/

/*****************  Bit definition for TSI_ISR register  *****************/
// TSI_ISR, TSI interrupt and status register, 0x00
#define TSI_ISR_CHANNEL_FINISH (uint32_t)(1U << 19)		  /*!< TSI ��ͨ��ɨ����ɱ�־λ, д1���� */
#define TSI_ISR_SCANENDFINISH (uint32_t)(1U << 17)		  /*!< TSI����ʹ��ͨ��ɨ����ɱ�־, д1���� */
#define TSI_ISR_SINGLE_SCANENDFINISH (uint32_t)(1U << 16) /*!< TSIͨ��ɨ����ɱ�־, д1���� */
#define TSI_ISR_AWD (uint32_t)(1U << 7)					  /*!< TSIģ�⿴�Ź��жϱ�־λ, д1���� */
#define TSI_ISR_DELAY_CHECK_FINISH (uint32_t)(1U << 6)	  /*!< ��ʱ�ж�����жϱ�־λ, д1���� */
/*****************  Bit definition for TSI_IER register  *****************/
// TSI_IER, TSI interrupt and status register, 0x04
#define TSI_IER_SCANFINISH_IE (uint32_t)(1U << 30)	/*!< TSIͨ��ɨ������ж�ʹ�ܼĴ��� */
#define TSI_IER_ANALOGWDT_IE (uint32_t)(1U << 29)	/*!< TSIģ�⿴�Ź��ж�ʹ�ܼĴ��� */
#define TSI_IER_DELAY_CHECK_IE (uint32_t)(1U << 24) /*!< ��ʱ�ж��ж�ʹ��λ */
/*****************  Bit definition for TSI_CR register  *****************/
// TSI_CR, TSI control register, 0x08
//#define TSI_CR_DEFAULT (uint32_t)(1U << 30)
#define TSI_CR_CH_SW_TIME_MAX (uint32_t)(0xFU << 28) /*!< ͨ���л�ʱ������*/
#define TSI_CR_BASE_SW_MODE (uint32_t)(0x1U << 27)	 /*!< CBase �� Cid_Coarse �л�ģʽ*/
#define TSI_CR_EXP_ENABLE (uint32_t)(1U << 24)		 /*!< 0 = �����쳣��⹦��;1 = �������쳣��⹦�� */
#define TSI_CR_SCAN_SINGLE_FLAG_EN (uint32_t)(1U << 16)
#define TSI_CR_SCAN_MODE (uint32_t)(1U << 7)		  /*!< =1ͨ��ѭ��ɨ��ģʽ; =0��ͨ��ɨ��ģʽ */
#define TSI_CR_SCAN_SEQUENCE_MODE (uint32_t)(1U << 5) /*!< =1 Channel switch sequence is 000011112222; =0 Channel switch sequence is 0123456.... */
#define TSI_CR_DATA_CLR (uint32_t)(1U << 4)			  /*!< �� TSI ����ģʽ��������ݼĴ��� */
#define TSI_CR_AVALID_ALWAYS (uint32_t)(1U << 3)	  /*!< TSI������ */
#define TSI_CR_SCAN_EN (uint32_t)(1U << 0)			  /*!< ʹ��TSI */
/*****************  Bit definition for TSI_TSTR register  *****************/
//  TSI_TSTR, TSI test register, 0x0c
#define TSI_TSTR_ANALOG (1U)						   /*!< TSIģ�����trimֵ */
#define TSI_TSTR_DISCARDLEN (uint32_t)(3U << 29)	   /*!< ��ʧ���ݵĳ���*/
#define TSI_TSTR_DISCARDMODE (uint32_t)(1U << 31)	   /*!< =1ͨ���л�ʱ�������� */
#define TSI_TSTR_FTRIM_LV (uint32_t)(7U << 21)		   /*!< ģ�� IP trim ���� */
#define TSI_TSTR_IPS_FTRIM (uint32_t)(1U << 4)		   /*!<ģ�� IP trim ���÷�ʽѡ��,0 = ʹ��λ[23:21]�̶�ֵ����,1 = ʹ�����ֵ��������*/
#define TSI_TSTR_IPS_FILTER_SEL_LV (uint32_t)(3U << 2) /*!<ģ�� IP �˲�����*/
/*****************  Bit definition for TSI_TWDCFGR register  *****************/
//  TSI_TWDCFGR, TSI watchdog config register, 0x10
#define TSI_TWDCFGR_STATE_CLR (uint32_t)(1U << 30) /*!< TSI ����ģʽ�����״̬�� */

/*****************  Bit definition for TSI_CHCR register  *****************/
// TSI_CHCR, TSI channel config register, 0x18
#define TSI_CHCR_MODE_COARSE (uint32_t)(1U << 30)
#define TSI_CHCR_CURRENT_MAX 0xF0U /*!< ���ó�ŵ��ٶ� */
/*****************  Bit definition for TSI_CHEN register  *****************/
// TSI_CHEN, TSI Channel Enable register, 0x74
#define TSI_CHEN_SCANTIME (uint32_t)(0xFFU << 24)	 /*!< TSI scan times */
#define TSI_CHEN_DISABLETIME (uint32_t)(0x00U << 16) /*!< TSI disable time = (disable len+1) * 500us */

// TODO:����CCM4202-E�����Ĵ���
/*****************  Bit definition for TSI_TCFLTCR register  *****************/
// TSI_TCFLTCR, TSI �˲������ƼĴ���, 0x78
#define TSI_TCFLTCR_IIR_EN (uint32_t)(0x1U << 0)			/*!<IIR�˲���ʹ��>*/
#define TSI_TCFLTCR_MEDIAN_EN (uint32_t)(0x1U << 4)			/*!<��ֵ�˲���ʹ��>*/
#define TSI_TCFLTCR_MEDIAN_DATA_SEL (uint32_t)(0x1U << 5)	/*!<0=ģ��IP�����1=IIR�˲������>*/
#define TSI_TCFLTCR_IIR_FILTER_N_MAX (uint32_t)(0xFFU << 8) /*!< IIR�˲���ϵ��N */
#define TSI_TCFLTCR_MEAN_EN (uint32_t)(0x1U << 16)			/*!<��ֵ�˲���ʹ��>*/
#define TSI_TCFLTCR_MEAN_DATA_SEL (uint32_t)(0x3U << 17)	/*!< ��ֵ�˲�������Դ��00=ģ��IP��01=IIR�����10=��ֵ�����11=ģ��IP*/
#define TSI_TCFLTCR_MEAN_DATA_CLR_EN (uint32_t)(0x1U << 19) /*!<��ֵ�˲����������ʹ��>*/
#define TSI_TCFLTCR_MEAN_FLT_WIDTH (uint32_t)(0x3U << 20)	/*!< ��ֵ�˲���ȣ�00=4B��01=8B��10=16B��11=32B*/
#define TSI_TCFLTCR_AWD_DATA_SEL (uint32_t)(0x3U << 24)		/*!< ��ֵ�˲���ȣ�00=ģ��IP��01=��ֵ�����10=IIR�����11=��ֵ���*/
#define TSI_TCFLTCR_EXEPT_DATA_SEL (uint32_t)(0x1U << 28)	/*!<0=�쳣��������ԴΪģ��IP��������ݣ�1=�쳣��������ԴΪ�˲��������>*/
#define TSI_TCFLTCR_COARSE_FILTER_EN (uint32_t)(0x1U << 31) /*!<COARSEģʽ���˲���ʹ��>*/

/*****************  Bit definition for TSI_ANACR register  *****************/
// TSI_ANACR, TSI Analog Control register, 0xF0
#define TSI_ANACR_TRACK_ENABLE (uint32_t)(0x1U << 0)	   /*!< ����TRACK���� */
#define TSI_ANACR_CMP_HP_MODE (uint32_t)(0x1U << 3)		   /*!< �Ƚ�����ѹģʽ> */
#define TSI_ANACR_ICHARGE_ADD_ENABLE (uint32_t)(0x1U << 4) /*!< ��������ʹ��> */
#define TSI_ANACR_ICHE_SEL_ADD_MAX (uint32_t)(0xFU << 8)   /*!<�ⲿ��������> */
#define TSI_ANACR_ICHI_SEL_ADD_MAX (uint32_t)(0xFU << 12)  /*!<�ڲ���������> */

/*****************  Bit definition for TSI_DCCR register  *****************/
// TSI_DCCR, TSI ��ʱ�жϿ��ƼĴ���, 0xF4
#define TSI_DCCR_DELAY_CHECK_ENABLE (uint32_t)(0x1U << 0)	   /*!< ������ʱ�жϹ��� */
#define TSI_DCCR_DELAY_CHECK_SEL (uint32_t)(0x1U << 1)		   /*!< ��ʱ�ж�ģʽ�£����״��ж�ʱ��׼ֵѡ�� */
#define TSI_DCCR_AVG_1S_UPDATE_ENABLE (uint32_t)(0x1U << 4)	   /*!< ��ֵ�Ĵ��� 1 ���Զ�����ʹ�� */
#define TSI_DCCR_REF_CHECK_SEL (uint32_t)(0x1U << 7)		   /*!< ��ʱ�ж�ģʽ�£��״��ж�ʱ��׼ֵѡ�� */
#define TSI_DCCR_CHECK_TIMES_MAX (uint32_t)(0x7U << 8)		   /*!<��ʱ�жϵĴ���> */
#define TSI_DCCR_CHECK_DELAY_CNT_MAX (uint32_t)(0xFFFFU << 16) /*!<��ʱ�жϵ�ʱ����> */

/*****************  Bit definition for TSI_ENV_CR register  *****************/
// TSI_ENV_CR, TSI �������ƼĴ���, 0xF8
#define TSI_ENVCR_1S_CNT_ENABLE (uint32_t)(0x1U << 0)			/*!< ����1 ��������� */
#define TSI_ENVCR_ENV_CMP_ENABLE (uint32_t)(0x1U << 1)			/*!< ��ʱ�ж�ģʽ�£� ͬʱ�Ƚϻ��������;�ֵ */
#define TSI_ENVCR_UPDATE_SRC_SEL (uint32_t)(0x1U << 3)			/*!< �������� 1 ���Զ���������Դѡ�� */
#define TSI_ENVCR_AWD_CHECK_SRC_SEL (uint32_t)(0x1U << 4)		/*!< AWD �жϻ�������ѡ�� */
#define TSI_ENVCR_CHECK_DELAY_CNT_MAX (uint32_t)(0xFFFFU << 16) /*!<1 ���ʱ��������> */

/*****************  Bit definition for TSI_AVG_LD register  *****************/
// TSI_AVG_LD, TSI �������ƼĴ���, 0x150
#define TSI_AVG_LD_EN (uint32_t)(0x1U << 0) /*!< �Դ�λд 1���Ѿ�ֵ���üĴ����е�ֵ���ص���ֵ���¼Ĵ����� */
/*****************  Bit definition for TSI_ENV_LD register  *****************/
// TSI_ENV_LD, TSI �������ƼĴ���, 0x154
#define TSI_ENV_LD_EN (uint32_t)(0x1U << 0) /*!< �Դ�λд 1���ѻ����������üĴ����е�ֵ���ص������������¼Ĵ����� */

#endif /*_TSI_REG_H_*/
