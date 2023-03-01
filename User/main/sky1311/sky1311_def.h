/******************************************************************************/
/*               (C) ˹���������������Ƽ����޹�˾(SKYRELAY)                   */
/*                                                                            */
/* �˴�����˹���������������Ƽ����޹�˾Ϊ֧�ֿͻ���д��ʾ�������һ����       */
/* ����ʹ��˹���������оƬ���û������޳�ʹ�øô��룬���豣������������       */
/* �������Ϊ��ʾʹ�ã�����֤�������е�Ӧ��Ҫ�󣬵����ض��淶������ʱ��       */
/* ʹ����������ȷ�ϴ����Ƿ����Ҫ�󣬲���Ҫ��ʱ����������Ӧ���޸ġ�           */
/* http://www.skyrelay-ic.com                                                 */
/******************************************************************************/
/**
 ******************************************************************************
 ** �� �� ���� sky1311_def.h
 **
 ** �ļ������� �����б����Ļ������͡��ṹ����������Ͷ���
 **
 ** �汾��ʷ:
 ** 2015-10-05 V1.00  Mark   ��ʽ�汾
 ** 2020-04-27 V1.3.0 SZ     ��ʽ�汾
 ******************************************************************************/
#ifndef  __SKY1311_DEF_H_
#define  __SKY1311_DEF_H_
#include <stdio.h>
#include <limits.h>

/**
 ******************************************************************************
 ** \�������������Ͷ���
 **
 ******************************************************************************/
typedef enum sta_result
{
    Ok                      = 0,  ///< �޴������ɹ�
    Error                   = 1,  ///< ���ض�����
    Timeout                 = 3,  ///< ��ʱ����
    RxParity			    = 4,  ///< ������żУ���
    RxCheckCRC			    = 5,  ///< ����CRCУ���
    FifoFull                = 6,  ///< FIFO��
    FifoEmpty               = 7,  ///< FIFO��
    Collision               = 8,  ///< ����ͻ����
    Framing                 = 9,  ///< ����֡����
    UIDFormat               = 10, ///< UID��ʽ����
    M1Error                 = 11, ///< ����M1������

    ErrorRequest            = 12,  ///< ִ��REQA/WUPA ����ʱ����
    ErrorAnticollision      = 13,  ///< ִ�з���ͻ����ANTICOLLISIONʱ����
    ErrorSelect             = 14,  ///< ִ��SELECT ����ʱ����
    ErrorAts                = 15,  ///< ִ��ATS�������
    ErrorInvalidMode        = 16,  ///< Operation not allowed in current mode
    ErrorUninitialized      = 17,  ///< Module (or part of it) was not initialized properly
    ErrorBufferFull         = 18,  ///< Circular buffer can not be written because the buffer is full
    ErrorTimeout            = 19,  ///< Time Out error occurred (e.g. I2C arbitration lost, Flash time-out, etc.)
    ErrorNotReady           = 20,  ///< A requested final state is not reached
    OperationInProgress     = 21,  ///< Indicator for operation in progress
    
    NTag_InvalidAddr        = 0x30,
    NTag_CheckError         = 0x31,
    NTag_InvalidAuth        = 0x34,
    NTag_WriteError         = 0x35,

    UnknowError             = 0x7F,
    NoResponse              = 0xFF
}sta_result_t,en_result_t;


typedef enum sta_field
{
    NoObjectIn              = 0,    ///< �����������޿�������
    PiccIn                  = 1,    ///< �п��ڳ���
    OtherObjectIn           = 2
}sta_field_t;


/**
 ******************************************************************************
 ** \������ ���ÿ��ض���
 **
 ******************************************************************************/
#define CHECKCARD       1       // �Ƿ�Ҫѯ����1���ǣ�0������
#define READERCARD      1       // �Ƿ�ѯ��������1���ǣ�0������
#define USEDCMD         1       // �Ƿ�ʹ�ô������1���ǣ�0������
/******************************************************************************/
/* Demo�������������ѡ�� */
#define AUTO            0       // �Զ��жϿ����ͣ�����ȷ��
#define MIFARE1         1       // M1��
#define SMART           2       // CPU��
#define NTAG2           3       // NFC TAG Type2 (NTAG21x)

#define CARDTYPE        AUTO    // (���� SAK & ATQA �ж�)

/* �͹��ļ쿨AD�йز��� */
#define MINADVAL        0x30        // RCɨ��ʱ���ڴ�ֵ��Ϊ��Ч
#define MAXADVAL        0xFF
#define RSSI_DELTA      3           // �쿨ʱAD�仯�����ֵ
#define RSSI_ABS        20          // �쿨ʱAD�仯������ֵ

/* �͹��ļ쿨PA��ѹ */
#define CHK_PAVOL       PA_2P0V     // �쿨ʱʹ�õ�PA��ѹ
#define RD_PAVOL        PA_2P5V     // ����ʱʹ�õ�PA��ѹ��2.0��2.5��3.0��3.3��

/* SKY1311T �Ĵ��������йض��壬����ϸ����*/
#define IXTAL           (0)         //0���ⲿ������������300uA��2��������������500uA
#define ANA2_A          (0x00)      //��A�࿨ʱ Analog2�Ĵ���������
#define ANA2_B          (0x1C)      //��B�࿨ʱ Analog2�Ĵ���������
#define OSCFREQ         CLK_SEL3    //�ⲿ����27.12MHz( CLK_SEL2 = 13.56MHz)

/* ������Ϣ���� */
#define SKY_DEBUG             1//0      //uart debug
#define CARD_REMOVED_CHECK    1//0        //�Ƿ����˿�

#endif