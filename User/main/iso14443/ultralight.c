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
 ** �� �� ���� ultralight.c
 **
 ** �ļ������� ����Mifare Ultralight�� Ultralight C�����������к����������ͱ���
 **
 ** �汾��ʷ:
 ** 2019-05-30 V1.00  EH   ��һ����ʽ�汾
 **
 ******************************************************************************/
#include "iso14443.h"
#include "ultralight.h"

/* ����ָ�����ȵ��ַ��� */
void encryptKey(uint8_t *keyData, uint8_t size)
{

}

/*����ָ�������ַ���*/
void discryptKey(uint8_t *keyData, uint8_t size)
{


}
/**
 ******************************************************************************
 ** \��  ��  ultralight c���ļ�����֤,
 **
 ** \��  ��
 **          uint8_t* Key 16�ֽ���Կ
 ** \����ֵ  ��֤״̬
 ******************************************************************************/
sta_result_t ULC_Authentication(uint8_t *Key)
{
    sta_result_t sta;
    uint16_t tmpSize;
    uint8_t i;
    uint8_t tmpBuf[20];
    uint8_t RndB[8], RndA[8]={0x78,0x98,0x01,0x83,0x46,0x76,0x31,0x52};
    sky1311TxCrcOn();
    sky1311RxCrcOn();
    tmpBuf[0] = ULC_AUTH;    // 0x1A
    tmpBuf[1] = 0;
    sta = ExchangeData(TYPE_A,RATE_OFF,tmpBuf,2,tmpBuf,&tmpSize);
    if(sta == Ok)
	{
	    if(tmpSize<9)
        {
            sta = Error;
            return sta;
        }
        else if(tmpBuf[0] == 0xAF)
        {
            discryptKey(&tmpBuf[1],8);
            for(i=0;i<8;i++)
            RndB[i] = tmpBuf[i+1];
        }
        else
        {
            sta = Error;
            return sta;
        }
        tmpBuf[0] = 0xAF;

        for(i=0;i<8;i++)
        {
            tmpBuf[i+1] = RndA[i];
            tmpBuf[i+9] =  RndB[i];
        }
        encryptKey(&tmpBuf[1],16); // ����RndA || RndB
        sta = ExchangeData(TYPE_A,RATE_OFF,tmpBuf,17,tmpBuf,&tmpSize);
        if(sta == Ok)
        {
            if(tmpSize==11)
            {
                encryptKey(RndA,8);
                for(i=0;i<8;i++)    //�Ƚϼ��ܺ��RndA�ͽ��յ�������
                {
                    if(RndA[i] != tmpBuf[i+1])
                        sta = Error;
                }
            }
        }
	}
	return sta;
}


/**
 ******************************************************************************
 ** \��  ��  ULC�������ݣ�
 **
 ** \��  ��  uint8_t address	Ҫ�����ĵ�ַ
 **          uint8_t *receBuff  ��ȡ�����ݴ洢��ַ��16�ֽڣ�
 ** \����ֵ  ����״̬
 ******************************************************************************/
sta_result_t ULC_Read(uint8_t address, uint8_t *receBuff)
{
	sta_result_t sta;
	uint16_t tmpSize;
	uint8_t tmpBuf[2];

	sky1311TxCrcOn();
	sky1311RxCrcOn();

	tmpBuf[0] = ULC_READ;    // 0x30
	tmpBuf[1] = address;

	sta = ExchangeData(TYPE_A,RATE_OFF,tmpBuf,2,receBuff,&tmpSize);
	if(sta == Ok)
    {
        if(tmpSize < 16)
        {
        	switch (receBuff[0]&0x0f)
            {
            case ULC_NAK:
                sta = NoResponse;
                break;
            case ULC_NAKCRC:
                sta = RxCheckCRC;
                break;
            case ULC_NAKOTH:
                sta = UnknowError;
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
 ** \��  ��  ultralight��д���ݡ�һ��д��1ҳ��4���ֽڣ�����
 **
 ** \��  ��  uint8_t address        Ҫ������ҳ��ַ
             uint8_t* data           Ҫд�������
 ** \����ֵ  ����״̬ Ok��ʾ�ɹ��� ������ʾ�д���
 ******************************************************************************/
sta_result_t ULC_Write(uint8_t address, uint8_t* data)
{
    sta_result_t sta;
    uint8_t tmpBuf[16];
    uint16_t tmpSize;

    sky1311TxCrcOn();
    sky1311RxCrcOff();

    tmpBuf[0] = ULC_WRITE;       // CMD = 0xA0
    tmpBuf[1] = address;      //
    tmpBuf[2] = data[0];
    tmpBuf[3] = data[1];
    tmpBuf[4] = data[2];
    tmpBuf[5] = data[3];
    sta = ExchangeData(TYPE_A,RATE_OFF,tmpBuf,6,tmpBuf,&tmpSize);
    if(sta == Ok)
    {
        if((tmpBuf[0] &0x0f) != ULC_ACK)
        {
            sta = NoResponse;
        }
    }
    return sta;
}

/**
 ******************************************************************************
 ** \��  ��  ultralight c ������д���ݡ�һ��д��16���ֽ����ݣ�ֻ�����4�ֽ���Ч
 **
 ** \��  ��  uint8_t pageAddress     Ҫ������ҳ��ַ��2--27��
             uint8_t* data           Ҫд�������
 ** \����ֵ  ����״̬ Ok��ʾ�ɹ��� ������ʾ�д���
 ******************************************************************************/
sta_result_t ULC_CompWrite(uint8_t pageAddress, uint8_t* data)
{
    sta_result_t sta;
    uint8_t tmpBuf[16];
    uint16_t tmpSize;

    sky1311TxCrcOn();
    sky1311RxCrcOff();

    tmpBuf[0] = ULC_COMWRITE;       // CMD = 0xA0
    tmpBuf[1] = pageAddress;       //
    sta = ExchangeData(TYPE_A,RATE_OFF,tmpBuf,2,tmpBuf,&tmpSize);
    if(Ok == sta)
    {
        if((tmpBuf[0] & 0x0f) != M1_ACK)
        {
            sta = Error;
        }
        else
        {
            sta = ExchangeData(TYPE_A,RATE_OFF,data,16,tmpBuf,&tmpSize);
            if(sta == Ok)
             {
                if((tmpBuf[0] &0x0f) != M1_ACK)
                    return Error;
             }
        }
    }
    return sta;
}

