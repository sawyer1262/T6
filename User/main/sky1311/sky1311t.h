/******************************************************************************/
/*               (C) ˹���������������Ƽ����޹�˾(SKYRELAY)                   */
/*                                                                            */
/* �˴�����˹���������������Ƽ����޹�˾Ϊ֧�ֿͻ���д��ʾ�������һ����       */
/* ����ʹ��˹���������оƬ���û������޳�ʹ�øô��룬���豣������������       */
/* �������Ϊ��ʾʹ�ã�����֤�������е�Ӧ��Ҫ�󣬵����ض��淶������ʱ��       */
/* ʹ����������ȷ�ϴ����Ƿ����Ҫ�󣬲���Ҫ��ʱ����������Ӧ���޸ġ�           */
/* (V1.10)                                                                    */
/******************************************************************************/
/**
 ******************************************************************************
 ** �� �� ���� sky1311s.h
 **
 ** �ļ������� ����sky1311s�����к����������ͱ���
 **
 ** �汾��ʷ:
 ** 2015-10-05 V1.00  EH   ��һ����ʽ�汾
 ** 2017-3-25  V.10  Release    ���´���ͺ����ṹ�����·���
 ** 2018-06-12 V1.1.2 Release   ���������շ��������޸Ĳ��ִ�������������
 ** 2018-09-02 V1.2.0 Release   �޸��˲��ֺ����Ľṹ��ɾ��һЩ���õĺ�������
 ******************************************************************************/
#ifndef  __SKY1311S_H_
#define  __SKY1311S_H_
#include "sky1311t_reg.h"
#include "Sky_typedef.h"

/*
typedef enum sta_result
{
    Ok                      = 0,  ///< ??,????
    Error                   = 1,  ///< ?????
    Timeout                 = 3,  ///< ????
    RxParity			    = 4,  ///< ???????
    RxCheckCRC			    = 5,  ///< ??CRC???
    FifoFull                = 6,  ///< FIFO?
    FifoEmpty               = 7,  ///< FIFO?
    Collision               = 8,  ///< ?????
    Framing                 = 9,  ///< ?????
    UIDFormat               = 10, ///< UID????
    M1Error                 = 11, ///< ??M1???

    ErrorAddressAlignment       = 12,  ///< Address alignment does not match
    ErrorAccessRights           = 13,  ///< Wrong mode (e.g. user/system) mode is set
    ErrorInvalidParameter       = 14,  ///< Provided parameter is not valid
    ErrorOperationInProgress    = 15,  ///< A conflicting or requested operation is still in progress
    ErrorInvalidMode            = 16,  ///< Operation not allowed in current mode
    ErrorUninitialized          = 17,  ///< Module (or part of it) was not initialized properly
    ErrorBufferFull             = 18,  ///< Circular buffer can not be written because the buffer is full
    ErrorTimeout                = 19,  ///< Time Out error occurred (e.g. I2C arbitration lost, Flash time-out, etc.)
    ErrorNotReady               = 20, ///< A requested final state is not reached
    OperationInProgress         = 21,  ///< Indicator for operation in progress
		DisConnect                  = 22,

	UnknowError				= 0x7F,
    NoResponse              = 0xFF
}sta_result_t,en_result_t;
*/
/**
 ******************************************************************************
 ** \������ȫ��Ԥ����
 **
 ******************************************************************************/

/**
 ******************************************************************************
 ** \������ ȫ�ֱ�������
 **
 ******************************************************************************/

extern uint8_t  irq_sta;

/**
 ******************************************************************************
 ** \������ ��������
 **
 ******************************************************************************/
void sky1311RxCrcOn(void);
void sky1311RxCrcOff(void);
void sky1311TxCrcOn(void);
void sky1311TxCrcOff(void);
void SetBitMask(uint8_t regAddr, uint8_t mask);
void ClearBitMask(uint8_t regAddr, uint8_t mask);
//void directModeInit(uint8_t mfout_sel);
void analogInit(void);
void pcdAntennaOn(uint8_t pcdType);
void pcdAntennaOff(void);
void irqClearAll(void);
void irqClear(uint8_t irq);
void bitRateSet(uint16_t high_rate);
void setPCDTimeOut(uint16_t dlyMs);
void typeAOperate(void);
void typeBOperate(void);
void sky1311Init(void);
void sky1311Reset(void);
void resetPicc(void);

sta_result_t bitCollisionTrans(uint8_t* txBuf, uint8_t txLen, uint8_t lastBitNum);
void sky1311_fifo_tx(uint8_t *txBuff, uint16_t txSize);
sta_result_t sky1311_fifo_rx(uint8_t *rxBuff, uint16_t *rxSize);
sta_result_t ExchangeData(uint8_t *pSendDataBuf, uint16_t ucSendLen,
                          uint8_t *pRcvDataBuf,  uint16_t *pRcvLen);

uint16_t sky1311RCFreqCali(void);
void checkCardInit(uint16_t rc_val);
void checkCardDeinit(void);
uint8_t checkCardRemove(uint16_t rc_val);


void RFID_PortInit(void);
#endif
