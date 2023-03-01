/******************************************************************************/
/*               (C) ˹���������������Ƽ����޹�˾(SKYRELAY)                     */
/*                                                                            */
/* �˴�����˹���������������Ƽ����޹�˾Ϊ֧�ֿͻ���д��ʾ�������һ����           */
/* ����ʹ��˹���������оƬ���û������޳�ʹ�øô��룬���豣������������           */
/* �������Ϊ��ʾʹ�ã�����֤�������е�Ӧ��Ҫ�󣬵����ض��淶������ʱ��           */
/* ʹ����������ȷ�ϴ����Ƿ����Ҫ�󣬲���Ҫ��ʱ����������Ӧ���޸ġ�               */
/* (V1.10)                                                                    */
/******************************************************************************/
/**
 ******************************************************************************
 ** �� �� ���� iso14443_3b.c
 **
 ** �ļ������� ����ISO14443-3 type B�Ĳ��������������ͱ���
 **
 ** �汾��ʷ:
 ** 2015-10-05 V1.00  EH        ��һ����ʽ�汾
 ** 2017-3-25  V.10  Release    ���´���ͺ����ṹ�����·���
 ** 2018-06-12 V1.1.2 Release   �޸Ĳ��ִ�������������
 **
 ******************************************************************************/
#include "iso14443.h"
/**
 ******************************************************************************
 ** \��  ��  type B ��������REQB, ����ֵ�� ATQB��
 **
 **
 ** \��  ��  uint8_t ucReqCode:������� ISO14443_3B_REQIDL 0x00 -- ���У������Ŀ�
 **								  ISO14443_3B_REQALL 0x08 -- ���У��������˳��Ŀ�
 **			 uint8_t ucAFI ��Ӧ�ñ�ʶ����0x00��ȫѡ
 **			 uint8_t N��ʱ϶����,ȡֵ��Χ0--4��
 **          nAQTB_t *pATQB ����Ӧ�𷵻�����ָ�룬12�ֽ�
 ** \����ֵ  ����״̬��Ok���ɹ��� ����ֵ��ʧ��
 ******************************************************************************/
sta_result_t piccRequestB(uint8_t ucReqCode, uint8_t ucAFI, uint8_t N, nAQTB_t *pATQB)
{
    sta_result_t sta;
    uint8_t tmpBuf[16];
    uint16_t tmpSize;

    tmpBuf[0] = APF_CODE;         // APf = 0x05
    tmpBuf[1] = ucAFI;            // AFI, 00,ѡ������PICC
    tmpBuf[2] = (ucReqCode & 0x08)|(N & 0x07);
    sta = ExchangeData(tmpBuf,3,tmpBuf,&tmpSize);
    if(sta == Ok && tmpSize>11)
    {
        if(tmpSize<12)
            return Error;
        pATQB->PUPI[0] = tmpBuf[1];
        pATQB->PUPI[1] = tmpBuf[2];
        pATQB->PUPI[2] = tmpBuf[3];
        pATQB->PUPI[3] = tmpBuf[4];             // 4 Bytes PUPI
        pATQB->AppDat[0] = tmpBuf[5];
        pATQB->AppDat[1] = tmpBuf[6];
        pATQB->AppDat[2] = tmpBuf[7];
        pATQB->AppDat[3] = tmpBuf[8];           // 4 Bytes Application data
        pATQB->ProtInf[0] = tmpBuf[9];
        pATQB->ProtInf[1] = tmpBuf[10];
        pATQB->ProtInf[2] = tmpBuf[11];         // 3 bytes protocol info
    }
    return sta;
}
/**
 ******************************************************************************
 ** \��  ��  type B ʱ϶��Ƿ���ײ Slot-Marker������ֵ��ATQB
 **
 ** \��  ��  uint8_t ʱ϶��Ŀ1--15,��piccRequestB�е�N�����й�
 **          nAQTB_t *pATQB ����Ӧ�𷵻�����ָ�룬12�ֽ�
 ** \����ֵ  ״̬��Ok����ȷ�� ����ֵ������
 ******************************************************************************/
sta_result_t piccSlotMarker(uint8_t N, nAQTB_t *pATQB)
{
    sta_result_t sta;
    uint8_t tmpBuf[16];
    uint16_t tmpSize;
    if(N>16)
        return Error;
    N--;

    tmpBuf[0] = ( (N<<4)|APN_CODE );      // APn = xxxx0101b
    sta = ExchangeData(tmpBuf,1,tmpBuf,&tmpSize);
    if(sta == Ok){
        if(tmpSize<12)
            return Error;
        pATQB->PUPI[0] = tmpBuf[1];
        pATQB->PUPI[1] = tmpBuf[2];
        pATQB->PUPI[2] = tmpBuf[3];
        pATQB->PUPI[3] = tmpBuf[4];             // 4 Bytes PUPI
        pATQB->AppDat[0] = tmpBuf[5];
        pATQB->AppDat[1] = tmpBuf[6];
        pATQB->AppDat[2] = tmpBuf[7];
        pATQB->AppDat[3] = tmpBuf[8];           // 4 Bytes Application data
        pATQB->ProtInf[0] = tmpBuf[9];
        pATQB->ProtInf[1] = tmpBuf[10];
        pATQB->ProtInf[2] = tmpBuf[11];         // 3 bytes protocol info
    }
    return sta;
}
/**
 ******************************************************************************
 ** \��  ��  ѡ��type B PICC
 **
 ** \��  ��     uint8_t *pPUPI					    // 4�ֽ�PICC��ʶ��
//				uint8_t ucDSI_DRI					// PCD<-->PICC ����ѡ��
//				uint8_t MAX_FSDI				    // PCD�����ջ�������С
//				uint8_t ucCID						// 0 - 14,����֧��CID��������Ϊ0000
//				uint8_t ucProType					// ֧�ֵ�Э�飬�������Ӧ�е�ProtocolTypeָ��
//				uint8_t *pHigherLayerINF			// �߲�������Ϣ
//				uint8_t ucINFLen					// �߲������ֽ���
// ���ڲ���:    uint8_t *pAATTRIB					// ATTRIB�����Ӧ
//				uint8_t *pRLen					    // ATTRIB�����Ӧ���ֽ���
 ** \����ֵ  ״̬ Ok�ɹ��� ����ʧ��
 ******************************************************************************/
sta_result_t piccAttrib(uint8_t *pPUPI, uint8_t ucDSI_DRI,uint8_t MAX_FSDI,
				 uint8_t ucCID, uint8_t ucProType, uint8_t *pHigherLayerINF, uint8_t ucINFLen,
				 uint8_t *pAATTRIB, uint8_t *pRLen)
{
    sta_result_t sta;
    uint8_t tmpBuf[32];
    uint16_t tmpSize;
    tmpBuf[0] = APC_CODE;     // 0x1D
    tmpBuf[1] = pPUPI[0];
    tmpBuf[2] = pPUPI[1];
    tmpBuf[3] = pPUPI[2];
    tmpBuf[4] = pPUPI[3];                             // 4 Bytes PUPI
    tmpBuf[5] = 0x00;                               // param 1
    tmpBuf[6] = ((ucDSI_DRI << 4) | (MAX_FSDI & 0x0F)) & 0xFF;// param 2, rate and length
    tmpBuf[7] =  ucProType & 0x0f;
    tmpBuf[8] = ucCID & 0x0f;
    if (ucINFLen>0)
		{
        for(tmpSize=0;tmpSize<ucINFLen;tmpSize++)
            tmpBuf[9+tmpSize] = *pHigherLayerINF++;
		}
    sta = ExchangeData(tmpBuf,9+ucINFLen,tmpBuf,&tmpSize);
    if(sta == Ok){
        if(tmpSize==0)
            return Error;
        *pRLen = tmpSize;
        while(tmpSize--)
        {
            pAATTRIB[tmpSize] = tmpBuf[tmpSize];
        }

    }
    return sta;
}
/**
 ******************************************************************************
 ** \��  ��  ��ָ����type B PICC��������
 **
 ** \��  ��  uint8_t* pPUPI // 4�ֽ�PICC��ʶ��
 ** \����ֵ  ״̬
 ******************************************************************************/
sta_result_t piccHaltB(uint8_t *pPUPI)
{
    sta_result_t sta;
    uint8_t tmpBuf[5];
    uint16_t tmpSize;
    tmpBuf[0] = HALTB_CODE;      // 0x50;
    tmpBuf[1] = pPUPI[0];
    tmpBuf[2] = pPUPI[1];
    tmpBuf[3] = pPUPI[2];
    tmpBuf[4] = pPUPI[3];
    sta = ExchangeData(tmpBuf,5,tmpBuf,&tmpSize);
    return sta;
}
/**
 ******************************************************************************
 ** \��  ��  ȡ��type B PICCѡ��
 **
 ** \��  ��  uint8_t* �����׵�ַ
 ** \����ֵ  ״̬
 ******************************************************************************/
sta_result_t piccDeselectB(uint8_t *param)
{
    sta_result_t sta;
    uint8_t tmpBuf[5];
    uint16_t tmpSize;
    tmpBuf[0] = 0xCA;
    tmpBuf[1] = param[1];
    sta = ExchangeData(tmpBuf,2,tmpBuf,&tmpSize);
    return sta;
}
