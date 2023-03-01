/******************************************************************************/
/*               (C) ˹���������������Ƽ����޹�˾(SKYRELAY)                   */
/*                                                                            */
/* �˴�����˹���������������Ƽ����޹�˾Ϊ֧�ֿͻ���д��ʾ�������һ����       */
/* ����ʹ��˹���������оƬ���û������޳�ʹ�øô��룬���豣������������       */
/* �������Ϊ��ʾʹ�ã�����֤�������е�Ӧ��Ҫ�󣬵����ض��淶������ʱ��       */
/* ʹ����������ȷ�ϴ����Ƿ����Ҫ�󣬲���Ҫ��ʱ����������Ӧ���޸ġ�           */
/* (V1.00)                                                                    */
/******************************************************************************/
/**
 ******************************************************************************
 ** �� �� ���� ntag.c
 **
 ** �ļ������� ����NTAG2�����������к����������ͱ���
 **
 ** �汾��ʷ:
 ** 2019-05-05 V1.00  EH   ��һ����ʽ�汾
 ******************************************************************************/
#include "iso14443.h"
#include "ntag.h"
void sky1311RxCrcOn(void);
void sky1311RxCrcOff(void);
void sky1311TxCrcOn(void);
void sky1311TxCrcOff(void);


/**
 ******************************************************************************
 ** \��  ��  ��ȡNTAG21X/20X�İ汾��
 **
 ** \��  ��  8�ֽڰ汾�ŵ�����
 ** \����ֵ  ״̬
 ******************************************************************************/
sta_result_t NTag_GetVision(uint8_t* data)
{
    sta_result_t sta;
    uint16_t tmpSize;
    uint8_t tmpBuf[16];

    sky1311TxCrcOn();
    sky1311RxCrcOn();

    tmpBuf[0] = NTAG_GETVER;        // 0x60
    sta = ExchangeData(tmpBuf,1,data,&tmpSize);
    if(sta == Ok)
    {
        if(tmpSize < 8)
        {
            switch (data[0]&0x0f)
            {
            case NTAG_INVALIDADDR:
                sta = NTag_InvalidAddr;
                break;
            case NTAG_CHECKERROR:
                sta = NTag_CheckError;
                break;
            case NTAG_INVALIDAUTH:
                sta = NTag_InvalidAuth;
                break;
            case NTAG_WRITEERROR:
                sta = NTag_WriteError;
                break;
            default:
                sta = UnknowError;
                break;
            }
        }
    }
    return sta;
}

/**
 ******************************************************************************
 ** \��  ��  ��ȡָ����ַ������
 **
 ** \��  ��  ָ���ĵ�ַ��16�ֽ���������ָ��
 ** \����ֵ  ״̬
 ******************************************************************************/
sta_result_t NTag_Read(uint8_t addr, uint8_t* data)
{
    sta_result_t sta;
    uint16_t tmpSize;
    uint8_t tmpBuf[4];

    sky1311TxCrcOn();
    sky1311RxCrcOn();
    tmpBuf[0] = NTAG_READ;        // 0x30
    tmpBuf[1] = addr;
    sta = ExchangeData(tmpBuf,2,data,&tmpSize);
    if(sta == Ok)
    {
        if(tmpSize < 16)
        {
            switch (data[0]&0x0f)
            {
            case NTAG_INVALIDADDR:
                sta = NTag_InvalidAddr;
                break;
            case NTAG_CHECKERROR:
                sta = NTag_CheckError;
                break;
            case NTAG_INVALIDAUTH:
                sta = NTag_InvalidAuth;
                break;
            case NTAG_WRITEERROR:
                sta = NTag_WriteError;
                break;
            default:
                sta = UnknowError;
                break;
            }
        }
    }
    return sta;
}

/**
 ******************************************************************************
 ** \��  ��  ��ȡָ����ַ����
 **
 ** \��  ��  ��ʼ��ַ�ͽ�����ַ���������ݵ�����ָ�롣���ݴ�С��n*4�ֽ�
 ** \����ֵ  ״̬
 ******************************************************************************/
sta_result_t NTag_FastRead(uint8_t starAddr,uint8_t endAddr, uint8_t* data)
{
    sta_result_t sta;
    uint16_t tmpSize;
    uint8_t tmpBuf[4];

    sky1311TxCrcOn();
    sky1311RxCrcOn();
    tmpBuf[0] = NTAG_FASTREAD;        // 0x3A
    tmpBuf[1] = starAddr;
    tmpBuf[2] = endAddr;
    sta = ExchangeData(tmpBuf,3,data,&tmpSize);
    if(sta == Ok)
    {
        if(tmpSize != (endAddr-starAddr+1)*4)
        {
            switch (data[0]&0x0f)
            {
            case NTAG_INVALIDADDR:
                sta = NTag_InvalidAddr;
                break;
            case NTAG_CHECKERROR:
                sta = NTag_CheckError;
                break;
            case NTAG_INVALIDAUTH:
                sta = NTag_InvalidAuth;
                break;
            case NTAG_WRITEERROR:
                sta = NTag_WriteError;
                break;
            default:
                sta = UnknowError;
                break;
            }
        }
    }
    return sta;
}

/**
 ******************************************************************************
 ** \��  ��  NTAGд���ݡ�һ��д��4���ֽ�����
 **
 ** \��  ��  uint8_t addr          Ҫ�����ĵ�ַ
             uint8_t* data         Ҫд�������ָ��
 ** \����ֵ  ����״̬ Ok��ʾ�ɹ��� ������ʾ�д���
 ******************************************************************************/
sta_result_t NTag_PageWrite(uint8_t addr, uint8_t* data)
{
    sta_result_t sta;
    uint8_t tmpBuf[16];
    uint16_t tmpSize;

    sky1311TxCrcOn();
    sky1311RxCrcOff();

    tmpBuf[0] = NTAG_WRITE;       // CMD = 0xA2
    tmpBuf[1] = addr;
    tmpBuf[2] = data[0];
    tmpBuf[3] = data[1];
    tmpBuf[4] = data[2];
    tmpBuf[5] = data[3];
    sta = ExchangeData(tmpBuf,6,tmpBuf,&tmpSize);
    if(sta == Ok)
    {
        if((tmpBuf[0] &0x0f) != NTAG_ACK)
        {
            switch (tmpBuf[0]&0x0f)
            {
            case NTAG_INVALIDADDR:
                sta = NTag_InvalidAddr;
                break;
            case NTAG_CHECKERROR:
                sta = NTag_CheckError;
                break;
            case NTAG_INVALIDAUTH:
                sta = NTag_InvalidAuth;
                break;
            case NTAG_WRITEERROR:
                sta = NTag_WriteError;
                break;
            default:
                sta = UnknowError;
                break;
            }
        }
    }
    return sta;
}

/**
 ******************************************************************************
 ** \��  ��  ��ȡNTAG���� counter ��ֵ
 **
 ** \��  ��  3�ֽ���������ָ��
 ** \����ֵ  ״̬
 ******************************************************************************/
sta_result_t NTag_ReadCounter(uint8_t* data)
{
    sta_result_t sta;
    uint16_t tmpSize;
    uint8_t tmpBuf[4];

    sky1311TxCrcOn();
    sky1311RxCrcOn();
    tmpBuf[0] = NTAG_READCNT;        // 0x39
    tmpBuf[1] = 0x02;
    sta = ExchangeData(tmpBuf,2,data,&tmpSize);
    if(sta == Ok)
    {
        if(tmpSize < 3)
        {
            switch (data[0]&0x0f)
            {
            case NTAG_INVALIDADDR:
                sta = NTag_InvalidAddr;
                break;
            case NTAG_CHECKERROR:
                sta = NTag_CheckError;
                break;
            case NTAG_INVALIDAUTH:
                sta = NTag_InvalidAuth;
                break;
            case NTAG_WRITEERROR:
                sta = NTag_WriteError;
                break;
            default:
                sta = UnknowError;
                break;
            }
        }
    }
    return sta;
}

/**
 ******************************************************************************
 ** \��  ��  NTAG��������֤
 **
 ** \��  ��  uint8_t* password      4�ֽ�����

 ** \����ֵ  ����״̬ Ok��ʾ�ɹ��� ������ʾ�д���
 ******************************************************************************/
sta_result_t NTag_Authenticate(uint8_t* password)
{
    sta_result_t sta;
    uint8_t tmpBuf[16];
    uint16_t tmpSize;

    sky1311TxCrcOn();
    sky1311RxCrcOn();

    tmpBuf[0] = NTAG_PWDAUTH;       // CMD = 0x1B
    tmpBuf[1] = password[0];      //
    tmpBuf[2] = password[1];
    tmpBuf[3] = password[2];
    tmpBuf[4] = password[3];
    sta = ExchangeData(tmpBuf,5,tmpBuf,&tmpSize);
    if(sta == Ok)
    {
        if(tmpSize<2)
        {
            switch (tmpBuf[0]&0x0f)
            {
            case NTAG_INVALIDADDR:
                sta = NTag_InvalidAddr;
                break;
            case NTAG_CHECKERROR:
                sta = NTag_CheckError;
                break;
            case NTAG_INVALIDAUTH:
                sta = NTag_InvalidAuth;
                break;
            case NTAG_WRITEERROR:
                sta = NTag_WriteError;
                break;
            default:
                sta = UnknowError;
                break;
            }
        }
    }
    return sta;
}

/**
 ******************************************************************************
 ** \��  ��  ��ȡNTAG���� ECC signature ��ֵ
 **
 ** \��  ��  4�ֽ���������ָ��
 ** \����ֵ  ״̬
 ******************************************************************************/
sta_result_t NTag_ReadSign(uint8_t* data)
{
    sta_result_t sta;
    uint16_t tmpSize;
    uint8_t tmpBuf[4];

    sky1311TxCrcOn();
    sky1311RxCrcOn();
    tmpBuf[0] = NTAG_READSIG;        // 0x39
    tmpBuf[1] = 0x00;
    sta = ExchangeData(tmpBuf,2,data,&tmpSize);
    if(sta == Ok)
    {
        if(tmpSize < 4)
        {
            switch (data[0]&0x0f)
            {
            case NTAG_INVALIDADDR:
                sta = NTag_InvalidAddr;
                break;
            case NTAG_CHECKERROR:
                sta = NTag_CheckError;
                break;
            case NTAG_INVALIDAUTH:
                sta = NTag_InvalidAuth;
                break;
            case NTAG_WRITEERROR:
                sta = NTag_WriteError;
                break;
            default:
                sta = UnknowError;
                break;
            }
        }
    }
    return sta;
}

