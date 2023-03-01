/**
 * @file tsi_drv.h
 * @author System Application Team
 * @brief TSIģ��driver������ͷ�ļ�
  * @version V4.0.0
  * @date    2022.03.22
 * 
 * @copyright C*Core Copyright (c) 2020
 * 
 */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _TSI_DRV_H_
#define _TSI_DRV_H_
/* Includes ------------------------------------------------------------------*/
#include <stdint.h>
#include <string.h>
#include "tsi_reg.h"

#ifdef __cplusplus
extern "C"
{
#endif

    /** @addtogroup Driver
  * @{
  */

    /** @addtogroup TSI
  * @{
  */

    /*** �궨�� *******************************************************************/
    /** @addtogroup TSI Exported Macros
  * @{
  */
 
#define _CLEAR_CHANNELFINISHFLAG(TSIx) do{TSIx->TSI_ISR |= TSI_ISR_CHANNEL_FINISH;}while(0) /*!< ��ͨ��ɨ����ɱ�־λ */
#define _CLEAR_SCANFINISHFLAG(TSIx) do{TSIx->TSI_ISR |= TSI_ISR_SCANENDFINISH;}while(0) /*!< ���ɨ����ɱ�־λ */
#define _CLEAR_AWDFLAG(TSIx) do{TSIx->TSI_ISR |= TSI_ISR_AWD;}while(0) /*!< ���TSI�жϻ��ѱ�־λ */
#define _CLEAR_DELAY_CHECK_FLAG(TSIx) do{TSIx->TSI_ISR |= TSI_ISR_DELAY_CHECK_FINISH;}while(0) /*!< �����ʱ�ж�����жϱ�־λ */

#define _SET_IE_SCANFINISH(TSIx) do{TSIx->TSI_IER |= TSI_IER_SCANFINISH_IE;}while(0) /*!< ʹ��TSIͨ��ɨ������жϹ��� */
#define _CLEAR_IE_SCANFINISH(TSIx) do{TSIx->TSI_IER &= ~TSI_IER_SCANFINISH_IE;}while(0) /*!< ��ʹ��TSIͨ��ɨ������жϹ��� */
#define _SET_IE_AWD(TSIx) do{TSIx->TSI_IER |= TSI_IER_ANALOGWDT_IE;}while(0) /*!< ʹ��TSI�͹��Ĵ������ѹ��� */
#define _CLEAR_IE_AWD(TSIx) do{TSIx->TSI_IER &= ~TSI_IER_ANALOGWDT_IE;}while(0) /*!< ��ʹ��TSI�͹��Ĵ������ѹ��� */
#define _SET_IE_DELAY_CHECK(TSIx) do{TSIx->TSI_IER |= TSI_IER_DELAY_CHECK_IE;}while(0) /*!< ʹ��TSI�͹�����ʱ�жϴ������ѹ��� */
#define _CLEAR_IE_DELAY_CHECK(TSIx) do{TSIx->TSI_IER &= ~TSI_IER_DELAY_CHECK_IE;}while(0) /*!< ��ʹ��TSI�͹�����ʱ�жϴ������ѹ��� */

#define _SET_DEFAULT_CR(TSIx) do{TSIx->TSI_CR = 0x50000008;}while(0) /*!< ����TSI_WDCFGR�Ĵ���ΪĬ��ֵ */
#define _SET_TSI_EN(TSIx) do{TSIx->TSI_CR |= TSI_CR_SCAN_EN;}while(0) /*!< ʹ��TSI */
#define _SET_TSI_DISABLE(TSIx) do{TSIx->TSI_CR &= ~TSI_CR_SCAN_EN;}while(0) /*!< �ر�TSI */
// #define _SET_CR_DEFAULAT(TSIx) do{TSIx->TSI_CR |= TSI_CR_DEFAULT;}while(0) /*!< ����TSI_CR�Ĵ���ΪĬ��ֵ */
#define _SET_WORKMODE_SCAN(TSIx) do{TSIx->TSI_CR |= TSI_CR_SCAN_MODE;}while(0) /*!< ����TSIɨ��ģʽΪͨ��ѭ��ɨ��ģʽ */
#define _SET_WORKMODE_SINGLE(TSIx) do{TSIx->TSI_CR &= ~TSI_CR_SCAN_MODE;}while(0) /*!< ����TSIɨ��ģʽΪ��ͨ��ɨ��ģʽ */
#define _SET_SCANALWAYS(TSIx) do{TSIx->TSI_CR |= TSI_CR_AVALID_ALWAYS;}while(0) /*!< ʹ��TSI����ɨ�����ģʽ */
#define _SET_SCANCYCLICITY(TSIx) do{TSIx->TSI_CR &= ~TSI_CR_AVALID_ALWAYS;}while(0) /*!< ��ʹ��TSI����ɨ�����ģʽ */
#define _SET_SCAN_SEQUENCE_MODE_00001111(TSIx) do{TSIx->TSI_CR |= TSI_CR_SCAN_SEQUENCE_MODE;}while(0) /*!< ����ͨ��ɨ��ģʽΪ00001111... */
#define _SET_SCAN_SEQUENCE_MODE_01234567(TSIx) do{TSIx->TSI_CR &= ~TSI_CR_SCAN_SEQUENCE_MODE;}while(0) /*!< ����ͨ��ɨ��ģʽΪ012345678... */
#define _SET_TSI_EXP_ENABLE(TSIx) do{TSIx->TSI_CR &= ~TSI_CR_EXP_ENABLE;}while(0) /*!< �����쳣��⹦�� */
#define _SET_TSI_EXP_DISABLE(TSIx) do{TSIx->TSI_CR |= TSI_CR_EXP_ENABLE;}while(0) /*!< �������쳣��⹦�� */
#define _SET_TSI_SINGLE_FLAG_ENABLE(TSIx) do{TSIx->TSI_CR &= ~TSI_CR_SCAN_SINGLE_FLAG_EN;}while(0) /*!< �����쳣��⹦�� */
#define _SET_TSI_SINGLE_FLAG_DISABLE(TSIx) do{TSIx->TSI_CR |= TSI_CR_SCAN_SINGLE_FLAG_EN;}while(0) /*!< �������쳣��⹦�� */


#define _SET_TSTR_ANALOG(TSIx) do{TSIx->TSI_TSTR |= TSI_TSTR_ANALOG;}while(0) /*!< trim the tsi verf */
#define _SET_DISCARDMODE(TSIx) do{TSIx->TSI_TSTR |= TSI_TSTR_DISCARDMODE;}while(0) /*!< =1ͨ���л�ʱ�������� */
#define _SET_DISCARDMODE_NONE(TSIx) do{TSIx->TSI_TSTR &= ~TSI_TSTR_DISCARDMODE;}while(0) /*!< =1ͨ���л�ʱ�������� */
#define _SET_DISCARD_LEN(TSIx) do{TSIx->TSI_TSTR |= TSI_TSTR_DISCARDLEN;}while(0) /*!< ��ʧ���ݵĳ��� */
#define _SET_DISCARD_LEN_0(TSIx) do{TSIx->TSI_TSTR &= ~TSI_TSTR_DISCARDLEN;}while(0) /*!< ��ʧ���ݵĳ��� */
#define _SET_TSTR_IPS_FTRIM_FIXED(TSIx) do{TSIx->TSI_TSTR |= TSI_TSTR_IPS_FTRIM;}while(0) /*!< =1 ʹ�����ֵ�������� */
#define _SET_TSTR_IPS_FTRIM_RANDOM(TSIx) do{TSIx->TSI_TSTR &= ~TSI_TSTR_IPS_FTRIM;}while(0) /*!< =0 ʹ��λ[23:21]�̶�ֵ���� */
#define _SET_TSTR_filter_1(TSIx) do{TSIx->TSI_TSTR &=~0x0C;}while(0) /*!< ģ�� IP �˲�����Ϊ1 */
#define _SET_TSTR_filter_3(TSIx) do{TSIx->TSI_TSTR &=~0x0C;TSIx->TSI_TSTR |=0x04;}while(0) /*!<  ģ�� IP �˲�����Ϊ3 */
#define _SET_TSTR_filter_5(TSIx) do{TSIx->TSI_TSTR &=~0x0C;TSIx->TSI_TSTR |=0x08;}while(0) /*!<  ģ�� IP �˲�����Ϊ5 */
#define _SET_TSTR_filter_7(TSIx) do{TSIx->TSI_TSTR &=~0x0C;TSIx->TSI_TSTR |=0x0C;}while(0) /*!<  ģ�� IP �˲�����Ϊ7 */

#define _SET_DEFAULT_WDCFGR(TSIx) do{TSIx->TSI_WDCFGR = 0x00080100;}while(0) /*!< ����TSI_WDCFGR�Ĵ���ΪĬ��ֵ */
#define _SET_WDCFGR_LP(TSIx) do{TSIx->TSI_WDCFGR = 0x000002e0;}while(0) /*!< ����TSI_WDCFGR�Ĵ���Ϊ�͹������� */
#define _SET_SLEEP_STATE_CLR_EN(TSIx) do{TSIx->TSI_WDCFGR |= TSI_TWDCFGR_STATE_CLR;}while(0) /*!< ����TSI ����ģʽ�����״̬�� */
#define _SET_SLEEP_STATE_CLR_DIS(TSIx) do{TSIx->TSI_WDCFGR &= ~TSI_TWDCFGR_STATE_CLR;}while(0) /*!< ����TSI ����ģʽ�²����״̬��*/

#define _ENABLE_FINEMODE(TSIx) do{TSIx->TSI_CHCR &= ~TSI_CHCR_MODE_COARSE;}while(0) /*!< ���ù���ģʽΪFineģʽ */
#define _ENABLE_COARSEMODE(TSIx) do{TSIx->TSI_CHCR |= TSI_CHCR_MODE_COARSE;}while(0) /*!< ���ù���ģʽΪCoarseģʽ */
#define _SET_CURRENT(TSIx) do{TSIx->TSI_CHCR |= TSI_CHCR_CURRENT_MAX;}while(0) /*!< ���ó�ŵ���� */

#define _CLEAR_REGISTER_CHEN(TSIx) do{TSIx->TSI_CHEN = 0;}while(0) /*!< trim the tsi verf */

//TODO:CCM4202-E ADD  
#define _SET_IIR_FILTER_ENABLE(TSIx) do{TSIx->TSI_TCFLTCR |= TSI_TCFLTCR_IIR_EN;}while(0)/*!<ʹ��IIR�˲���>*/
#define _SET_IIR_FILTER_DISABLE(TSIx) do{TSIx->TSI_TCFLTCR &= ~TSI_TCFLTCR_IIR_EN;}while(0)/*!<�ر�IIR�˲���>*/
#define _SET_MEDIAN_FILTER_ENABLE(TSIx) do{TSIx->TSI_TCFLTCR |= TSI_TCFLTCR_MEDIAN_EN;}while(0)/*!<ʹ����ֵ�˲���>*/
#define _SET_MEDIAN_FILTER_DISABLE(TSIx) do{TSIx->TSI_TCFLTCR &= ~TSI_TCFLTCR_MEDIAN_EN;}while(0)/*!<�ر���ֵ�˲���>*/
#define _SET_MEDIAN_FLT_DATA_FROM_IIR(TSIx) do{TSIx->TSI_TCFLTCR |= TSI_TCFLTCR_MEDIAN_DATA_SEL;}while(0)/*!<������ֵ�˲���������ԴΪIIR���>*/
#define _SET_MEDIAN_FLT_DATA_FROM_ANA(TSIx) do{TSIx->TSI_TCFLTCR &= ~TSI_TCFLTCR_MEDIAN_DATA_SEL;}while(0)/*!<������ֵ�˲���������ԴΪģ��IP���>*/
#define _SET_MEAN_FILTER_ENABLE(TSIx) do{TSIx->TSI_TCFLTCR |= TSI_TCFLTCR_MEAN_EN;}while(0)/*!<ʹ�ܾ�ֵ�˲���>*/
#define _SET_MEAN_FILTER_DISABLE(TSIx) do{TSIx->TSI_TCFLTCR &= ~TSI_TCFLTCR_MEAN_EN;}while(0)/*!<�رվ�ֵ�˲���>*/
#define _SET_MEAN_FLT_DATA_FROM_ANA(TSIx) do{TSIx->TSI_TCFLTCR &= ~TSI_TCFLTCR_MEAN_DATA_SEL;}while(0)/*!<���þ�ֵ�˲���������ԴΪģ��IP���>*/
#define _SET_MEAN_FLT_DATA_FROM_IIR(TSIx) do{TSIx->TSI_TCFLTCR &= ~TSI_TCFLTCR_MEAN_DATA_SEL;TSIx->TSI_TCFLTCR |= 0x1U << 17;}while(0)/*!<���þ�ֵ�˲���������ԴΪIIR�˲������>*/
#define _SET_MEAN_FLT_DATA_FROM_MEDIAN(TSIx) do{TSIx->TSI_TCFLTCR &= ~TSI_TCFLTCR_MEAN_DATA_SEL;TSIx->TSI_TCFLTCR |= 0x1U << 18;}while(0)/*!<������ֵ�˲���������ԴΪ��ֵ�˲������>*/
#define _SET_LP_MEAN_DATA_CLR(TSIx) do{TSIx->TSI_TCFLTCR |= TSI_TCFLTCR_MEAN_DATA_CLR_EN;}while(0)/*!<LP��ֵ�˲����������ʹ��>*/
#define _SET_LP_MEAN_DATA_RSV(TSIx) do{TSIx->TSI_TCFLTCR &= ~TSI_TCFLTCR_MEAN_DATA_CLR_EN;}while(0)/*!<�ر�LP��ֵ�˲����������ʹ��>*/
#define _SET_MEAN_FLT_WIDTH_4(TSIx) do{TSIx->TSI_TCFLTCR &= ~TSI_TCFLTCR_MEAN_FLT_WIDTH;}while(0)/*!<���þ�ֵ�˲������Ϊ4byte>*/
#define _SET_MEAN_FLT_WIDTH_8(TSIx) do{TSIx->TSI_TCFLTCR &= ~TSI_TCFLTCR_MEAN_FLT_WIDTH;TSIx->TSI_TCFLTCR |= 0x1U << 20;}while(0)/*!<���þ�ֵ�˲������Ϊ8byte>*/
#define _SET_MEAN_FLT_WIDTH_16(TSIx) do{TSIx->TSI_TCFLTCR &= ~TSI_TCFLTCR_MEAN_FLT_WIDTH;TSIx->TSI_TCFLTCR |= 0x1U << 21;}while(0)/*!<���þ�ֵ�˲������Ϊ16byte>*/
#define _SET_MEAN_FLT_WIDTH_32(TSIx) do{TSIx->TSI_TCFLTCR &= ~TSI_TCFLTCR_MEAN_FLT_WIDTH;TSIx->TSI_TCFLTCR |=TSI_TCFLTCR_MEAN_FLT_WIDTH;}while(0)/*!<���þ�ֵ�˲������Ϊ32byte>*/
#define _SET_AWD_DATA_FROM_ANA(TSIx) do{TSIx->TSI_TCFLTCR &= ~TSI_TCFLTCR_AWD_DATA_SEL;}while(0)/*!<����AWD����ԴΪģ��IP>*/
#define _SET_AWD_DATA_FROM_MEAN(TSIx) do{TSIx->TSI_TCFLTCR &= ~TSI_TCFLTCR_AWD_DATA_SEL;TSIx->TSI_TCFLTCR |= 0x1U << 24;}while(0)/*!<����AWD����ԴΪ��ֵ�˲���>*/
#define _SET_AWD_DATA_FROM_IIR(TSIx) do{TSIx->TSI_TCFLTCR &= ~TSI_TCFLTCR_AWD_DATA_SEL;TSIx->TSI_TCFLTCR |= 0x1U << 25;}while(0)/*!<����AWD����ԴΪIIR�˲���>*/
#define _SET_AWD_DATA_FROM_MEDIAN(TSIx) do{TSIx->TSI_TCFLTCR &= ~TSI_TCFLTCR_AWD_DATA_SEL;TSIx->TSI_TCFLTCR |=TSI_TCFLTCR_AWD_DATA_SEL;}while(0)/*!<����AWD����ԴΪ��ֵ�˲���>*/
#define _MASK_DATA_FROM_ANA(TSIx) do{TSIx->TSI_TCFLTCR &= ~TSI_TCFLTCR_EXEPT_DATA_SEL;}while(0)/*!<�쳣��������ԴΪģ�� IP ���������>*/
#define _MASK_DATA_FROM_FILTER(TSIx) do{TSIx->TSI_TCFLTCR |= TSI_TCFLTCR_EXEPT_DATA_SEL;}while(0)/*!<�쳣��������ԴΪ�˲��������>*/
#define _SET_COARSE_FILTER_ENABLE(TSIx) do{TSIx->TSI_TCFLTCR |= TSI_TCFLTCR_COARSE_FILTER_EN;}while(0)/*!<TSI coarse ģʽ�¿����˲�����>*/
#define _SET_COARSE_FILTER_DISABLE(TSIx) do{TSIx->TSI_TCFLTCR &= ~TSI_TCFLTCR_COARSE_FILTER_EN;}while(0)/*!<TSI coarse ģʽ�¹ر��˲�����>*/

#define _SET_TRACK_ENABLE(TSIx) do{TSIx->TSI_ANACR |= TSI_ANACR_TRACK_ENABLE;}while(0)/*!<ʹ��TRACK����>*/
#define _SET_TRACK_DISABLE(TSIx) do{TSIx->TSI_ANACR &= ~TSI_ANACR_TRACK_ENABLE;}while(0)/*!<�ر�TRACK����>*/
#define _SET_HP_CMP_ENABLE(TSIx) do{TSIx->TSI_ANACR |= TSI_ANACR_CMP_HP_MODE;}while(0)/*!<ʹ�ܱȽ�����ѹģʽ>*/
#define _SET_HP_CMP_DISABLE(TSIx) do{TSIx->TSI_ANACR &= ~TSI_ANACR_CMP_HP_MODE;}while(0)/*!<�رձȽ�����ѹģʽ>*/
#define _SET_ICHARGE_ADD_ENABLE(TSIx) do{TSIx->TSI_ANACR |= TSI_ANACR_ICHARGE_ADD_ENABLE;}while(0)/*!<ʹ�ܸ�������>*/
#define _SET_ICHARGE_ADD_DISABLE(TSIx) do{TSIx->TSI_ANACR &= ~TSI_ANACR_ICHARGE_ADD_ENABLE;}while(0)/*!<�رո��ӵ���>*/
#define _SET_ICHE_SEL_ADD_CURRENT(TSIx) do{TSIx->TSI_ANACR |= TSI_ANACR_ICHE_SEL_ADD_MAX;}while(0) /*!< ���ø����ⲿ��ŵ���� */
#define _SET_ICHI_SEL_ADD_CURRENT(TSIx) do{TSIx->TSI_ANACR |= TSI_ANACR_ICHI_SEL_ADD_MAX;}while(0) /*!< ���ø����ڲ���ŵ���� */

#define _SET_DELAY_CHECK_ENABLE(TSIx) do{TSIx->TSI_DCCR |= TSI_DCCR_DELAY_CHECK_ENABLE;}while(0)/*!<������ʱ�жϹ���>*/
#define _SET_DELAY_CHECK_DISABLE(TSIx) do{TSIx->TSI_DCCR &= ~TSI_DCCR_DELAY_CHECK_ENABLE;}while(0)/*!<�ر���ʱ�жϹ���>*/
#define _SET_DELAY_CHECK_SRC_FROM_REG(TSIx) do{TSIx->TSI_DCCR |= TSI_DCCR_DELAY_CHECK_SEL;}while(0)/*!<���״��жϻ�׼ֵʹ�þ�ֵ�Ĵ�������ֵ�� 1 ��ǰ�Զ����µľ�ֵ>*/
#define _SET_DELAY_CHECK_SRC_FROM_MEAN(TSIx) do{TSIx->TSI_DCCR &= ~TSI_DCCR_DELAY_CHECK_SEL;}while(0)/*!<���״��жϻ�׼ֵʹ��ǰ���ֵľ�ֵ>*/
#define _SET_AVG_1S_UPDATE_ENABLE(TSIx) do{TSIx->TSI_DCCR |= TSI_DCCR_AVG_1S_UPDATE_ENABLE;}while(0)/*!<��ֵ�Ĵ���ÿ�� 1 ���Զ�����>*/
#define _SET_AVG_1S_UPDATE_DISABLE(TSIx) do{TSIx->TSI_DCCR &= ~TSI_DCCR_AVG_1S_UPDATE_ENABLE;}while(0)/*!<��ֵ�Ĵ������Զ�����>*/
#define _SET_FIRST_CHECK_SRC_FROM_REG(TSIx) do{TSIx->TSI_DCCR |= TSI_DCCR_REF_CHECK_SEL;}while(0)/*!<�״��жϻ�׼ֵʹ�þ�ֵ�Ĵ�������ֵ�� 1 ��ǰ�Զ����µľ�ֵ>*/
#define _SET_FIRST_CHECK_SRC_FROM_MEAN(TSIx) do{TSIx->TSI_DCCR &= ~TSI_DCCR_REF_CHECK_SEL;}while(0)/*!<�״��жϻ�׼ֵʹ��ǰ���ֵľ�ֵ>*/

#define _SET_1S_CNT_ENABLE(TSIx) do{TSIx->TSI_ENV_CR |= TSI_ENVCR_1S_CNT_ENABLE;}while(0)/*!<���� 1 ���������>*/
#define _SET_1S_CNT_DISABLE(TSIx) do{TSIx->TSI_ENV_CR &= ~TSI_ENVCR_1S_CNT_ENABLE;}while(0)/*!<�ر� 1 ���������>*/
#define _SET_DELAY_CMP_WITH_MEAN_AND_ENV(TSIx) do{TSIx->TSI_ENV_CR |= TSI_ENVCR_ENV_CMP_ENABLE;}while(0)/*!<ͬʱ�Ƚϻ��������;�ֵ>*/
#define _SET_DELAY_CMP_ONLY_WITH_MEAN(TSIx) do{TSIx->TSI_ENV_CR &= ~TSI_ENVCR_ENV_CMP_ENABLE;}while(0)/*!<ֻ�ȽϾ�ֵ>*/
#define _SET_ENV_1S_UPDATE_SRC_FROM_MEAN(TSIx) do{TSIx->TSI_ENV_CR |= TSI_ENVCR_UPDATE_SRC_SEL;}while(0)/*!<�������� 1 ���Զ���������ԴΪ��ֵ>*/
#define _SET_ENV_1S_UPDATE_SRC_FROM_ANA(TSIx) do{TSIx->TSI_ENV_CR &= ~TSI_ENVCR_UPDATE_SRC_SEL;}while(0)/*!<�������� 1 ���Զ���������ԴΪʵʱ����ֵ>*/
#define _SET_AWD_CHECK_ENV_ONLY(TSIx) do{TSIx->TSI_ENV_CR |= TSI_ENVCR_AWD_CHECK_SRC_SEL;}while(0)/*!<AWD ֻ�жϻ�������>*/
#define _SET_AWD_CHECK_ENV_AND_MEAN(TSIx) do{TSIx->TSI_ENV_CR &= ~TSI_ENVCR_AWD_CHECK_SRC_SEL;}while(0)/*!<AWD �жϾ�ֵ�ͻ�������>*/

#define _LOAD_AVG_DATA(TSIx) do{TSIx->TSI_AVG_LD |= TSI_AVG_LD_EN;}while(0)/*!<�Ѿ�ֵ���üĴ����е�ֵ���ص���ֵ���¼Ĵ���>*/
#define _LOAD_ENV_DATA(TSIx) do{TSIx->TSI_ENV_LD |= TSI_ENV_LD_EN;}while(0)/*!<�ѻ����������üĴ����е�ֵ���ص������������¼Ĵ���>*/


    /**
  * @}
  */

    /*** �ṹ�塢ö�ٱ������� *****************************************************/
    /** @addtogroup TSI Exported Types
  * @{
  */

    /**
  * @}
  */

    /*** ȫ�ֱ������� **************************************************************/
    /** @addtogroup TSI Exported Variables
  * @{
  */

    /**
  * @}
  */

    /*** �������� ******************************************************************/
    /** @addtogroup TSI Exported Functions
  * @{
  */
    extern inline void _SET_CHANNLENABLE(TSI_TypeDef *TSIx, const uint16_t channles);
    extern inline void _SET_SCANTIME(TSI_TypeDef *TSIx, const uint8_t time);
    extern inline void _SET_DISABLETIME(TSI_TypeDef *TSIx, const uint8_t time);
    extern inline uint8_t _GET_IF_SCANFINISH(TSI_TypeDef *TSIx);
    extern inline uint8_t _GET_IE_SCANFINISH(TSI_TypeDef *TSIx);
    extern inline uint8_t _GET_IF_AWD(TSI_TypeDef *TSIx);
    extern inline uint8_t _GET_IE_AWD(TSI_TypeDef *TSIx);
    extern void drv_tsi_reset(TSI_TypeDef *TSIx);
	extern inline uint8_t _GET_IF_SINGLE_SCANFINISH(TSI_TypeDef *TSIx);

    extern inline void _SET_DELAY_CHECK_CNT(TSI_TypeDef *TSIx, const uint8_t cnt);
    extern inline void _SET_Double_Check_DELAY_TIME(TSI_TypeDef *TSIx, const uint32_t time);
    extern inline void _SET_Env_Update_DELAY_TIME(TSI_TypeDef *TSIx, const uint32_t time);
    extern inline void _SET_IIR_FILTER_N(TSI_TypeDef *TSIx, const uint8_t IIR_N);
    extern inline void _SET_ICH_ADD_CURRENT(TSI_TypeDef *TSIx, const uint8_t ich);
    extern inline void _SET_TRACK_CH(TSI_TypeDef *TSIx);
    extern inline void _SET_MEAN_FILTER_WIDTH(TSI_TypeDef *TSIx, const uint8_t MEAN_WINDOWS_SIZE);
    extern inline void _SET_LP_AVERAGE_DATA(TSI_TypeDef *TSIx);
    extern inline void _SET_LP_ENV_DEALT(TSI_TypeDef *TSIx);
    /**
  * @}
  */

    /**
  * @}
  */

    /**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif //_TSI_DRV_H_

/************************ (C) COPYRIGHT C*Core *****END OF FILE****/
